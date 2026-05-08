# Irlos Control Protocol
#### GPL-3.0
#### Author: Ethan Manners
#### Version: 1.0
#### Status: Draft
#### Transport: Unix domain socket
#### Encoding: Length-prefixed UTF-8 JSON

## 1. Overview

The IRLOS Control Protocol (ICP) is the wire protocol spoken between `irlosd` (the IRLOS control daemon) and the `irlos-control` plugin loaded into IrlosStudio. ICP exists to give `irlosd` programmatic control over IrlosStudio's scene state and streaming state without requiring a network-facing remote-control interface.

ICP replaces the use of obs-websocket in this deployment. Where obs-websocket exposes the entire OBS feature set over an authenticated WebSocket server suitable for remote clients, ICP exposes a fixed set of 8 operations over a local Unix socket between two cooperating processes on the same host. The protocol is intentionally minimal: there is no event subsystem, no authentication handshake (filesystem permissions on the socket are the access control), no request multiplexing, and no support for clients other than `irlosd`.

ICP is not a general-purpose OBS remote control. Programs that need broader OBS access should use obs-websocket.

## 2. Transport

ICP runs over a Unix domain socket of type `SOCK_STREAM` (TCP-like)
The socket is created by the `irlos-control` plugin when it loads inside IrlosStudio, and is removed when the plugin unloads.

#### Unix socket location: /run/irlos/obs-control.sock
Parent directory `/run/irlos/` is created by systemd via a tmpfiles.d configuration shipped with IRLOS
The default socket path is `/run/irlos/obs-control.sock`. The plugin reads its socket path from its OBS plugin configuration; the client reads its target socket path from `irlosd`'s configuration. Deployments running inside multiple IrlosStudio instances on a single host MUST configure each instance with a distinct path. The recommended convention for multi-instance deployments is `/run/irlos/<instance-name>/obs-control.sock`


#### Permissions:
Socket file is owned by `root:irlos` with mode `0660`
Owner: `root` (read/write)
Group: `irlos` (read/write)
Other: no access

The `irlos` group is created by the IRLOS package install scripts. Both `irlosd` and `IrlosStudio` run as members of this group. `irlosd` runs as root, `IrlosStudio` runs as the underprivileged user `irlosstudio`

No process outside the `irlos` group can connect to the socket. The protocol does not implement application-layer authentication. The kernel's filesystem permissions are the sole access control mechanism.


#### Connection Model:
The plugin accepts at most one client connection at a time. While a client is connected, additional connection attempts are rejected at the protocol layer (See S6 Handshake) with an error response, after which the plugin closes the new connection.

Clients (`irlosd`) are expected to maintain a single long-lived connection for the lifetime of the daemon. Reconnection on failure is the client's responsibility.

The plugin does not initiate connections. All connections are established by the client (`irlosd`) and accepted by the plugin.


#### Socket lifecycle:
Socket gets created on plugin load and removed on plugin unload. If the plugin crashes or is killed, a stale socket file may remain. The plugin must `unlink()` any pre-existing socket file at the configured path before `bind()` on plugin load.

The plugin does not attempt to verify whether a stale socket has an active listener before removing it. This is safe under the single-instance assumption: only one IrlosStudio runs at a time per socket.



## 3. Framing

ICP messages are framed using a 4-byte length prefix followed by a UTF-8 JSON payload.

#### Wire format
Each message on the wire consists of:
`[4 bytes: length]  [N bytes: payload]`

The length field is an unsigned 32-bit integer in network byte order (big-endian). It specifies the size of the payload in bytes, not including the length field itself.

The payload is a UTF-8 encoded JSON document. The payload MUST be JSON object (not an array, string, number, or other JSON type at the top level).

#### Length constraints
The maximum payload size is 16384 bytes (16KiB). A length value greater than this MUST cause the receiver to close the connection without reading further. The receiver SHOULD log the violation, but MUST NOT send a response. By definition, a sender that violates the length cap is either malfunctioning or hostile, and engaging in further is unsafe.

The minimum payload size is 2 bytes (the smallest valid JSON object: `{}`). A length value less than 2 MUST cause the receiver to close the connection.


#### Reading a message
To read one message, the receiver:

1. Reads exactly 4 bytes from the socket. These are the length prefix.
2. Interprets the 4 bytes as a big-endian uint32. Call this value `N`.
3. If `N > 16384` or `N < 2`, closes the connection.
4. Reads exactly `N` bytes from the socket. These are the payload.
5. Validates the payload as UTF-8 and parses it as JSON. If either validation fails, closes the connection.
6. Validates that the parsed JSON is an object. If not, closes the connection.

The receiver MUST NOT begin processing the next message until the current message has been fully read and parsed. There is no read-ahead, no buffered parsing, no message coalescing.


#### Writing a message
To write one message, the sender:

1. Serializes the message body to UTF-8 JSON.
2. Verifies the byte length of the JSON does not exceed 16384. If it does, the sender has a bug. It MUST NOT truncate or split the message. It MUST log the error and refuse to send.
3. Writes 4 bytes containing the byte length as a big-endian uint32.
4. Write the JSON bytes.

The sender SHOULD use a single `write(2)` syscall for the combined length+payload buffer where possible, but the protocol does not require it. Receivers MUST handle messages that arrive split across multiple TCP-level (i.e. socket-level) reads, since the kernel may deliver bytes in arbitrary chunks regardless of how they were sent.

#### What ICP does not support
ICP framing has no message types at the framing layer, no checksums, no compression, and no chunking. These are deliberate omissions:

- No message types: All messages are JSON objects. The type/operation is encoded inside the JSON (the `op` field, defined in S8). The framing layer is uniform

- No checksums: The transport is a Unix socket on the same host; the kernel's IPC mechanisms do not introduce bit errors. Checksumming would be wasted work.

- No compression: Messages are small (<16KiB) and on the same host. Compression overhead exceeds streaming

- No chunking: Messages either fit in 16KiB or they don't. If a future operation needs to transfer more data than fits, it should be redesigned (or use a different mechanism entirely) rather than bolted onto the framing layer.


## 4. Encoding

ICP message payloads are UTF-8 encoded JSON documents conforming to RFC 8259.

#### JSON dialect
ICP uses strict JSON. Specifically:

- No comments. Neither `//` line comments nor `/* */` block comments are permitted.
- No trailing commas in objects or arrays.
- No unquoted keys. All object keys MUST be double-quoted strings.
- No single-quoted strings. All strings MUST use double quotes.
- No hex, octal, or other non-decimal numeric literals.
- No `NaN`, `Infinity`, or `-Infinity`. These are not valid JSON values.

Implementations MUST reject payloads that violate these rules. JSON parsers like nlohmann/json default to strict mode; do not enable lenient parsing modes.

#### Character encoding
Payloads MUST be valid UTF-8. Any payload containing invalid UTF-8 byte sequences MUST be rejected and the connection closed.

JSON strings MAY contain any Unicode codepoint, including non-ASCII characters (ex: scene names with emoji, Cyrillic, Chinese characters, etc.) Recievers MUST handle the full Unicode range.

#### Numeric types
JSON does not distinguish between integers and floating-point numbers at the syntactic level. Both are written as decimal numbers. ICP imposes the following type rules at the application layer.

- Fields documented as **integer** MUST contain whole numbers within the range of a 64-bit signed integer. Receivers MUST reject non-integer values.
- Fields documented as **boolean** MUST contain `true` or `false`. The strings `"true"`, `"false"`, the integers `0` or `1`, and `null` are not acceptable substitutes.
- Fields documented as string MUST contain JSON strings. Number, booleans, and null are not acceptable substitutes.

ICP does not use floating-point numbers anywhere in the protocol. Any field that might naively be expressed as a float (bitrates, latency, durations) is expressed as an integer in a fixed unit (kbps, milliseconds, seconds).

#### Key naming convention
Object keys use `snake_case`, all lowercase, ASCII letters and underscores only. Examples: `op`, `scene_name`, `current_bitrate_kbps`.

Keys MUST NOT contain hyphens, dots, spaces, or non-ASCII characters. This convention is enforced for consistency between client and server implementations and to avoid quoting hazards in different languages.

#### Field ordering
Implementations MUST NOT depend on the order of keys within JSON objects. JSON does not specify a canonical key order, and serializers may emit keys in any order. Parsers must accept keys in any order.

#### Unknown Fields
Receivers MUST ignore unknown fields in messages they otherwise understand. This rule allows the protocol to be extended in backward-compatible ways: e.g. a v1.1 server can include new fields in responses to v1.0 clients without breaking them.

The exception is the top-level `op` field in requests: an unknown `op` value MUST be rejected with an error response (see S7)

## 5. Handshake

When a client connects to the plugin's sockets, the two endpoints perform a handshake before any other messages are exchanged. The handshake establishes protocol version compatibility and confirms exclusive client access.

#### Sequence
The handshake consists of two messages:

1. The client sends a `hello` request immediately after the TCP-level connection is established.
2. The plugin sends a `hello` response (success or failure).

No other messages are valid until the handshake completes. The plugin MUST NOT process any other request from a client that has not completed a successful handshake. The client MUST NOT send any other request until it has received a successful handshake response.

**Client `hello` request**
```
{
  "op": "hello",
  "protocol_version": 1.
}
```

Fields:

- `op` (string, required) - MUST be the literal string `"hello"`.
- `protocol_version` (integer, require) - the major version of ICP the client implements. v1 of this specification uses `1`

**Plugin `hello` response (success)**
```
{
  "ok": true,
  "result": {
    "protocol_version": 1,
    "plugin_version": "0.1.0",
    "irlosstudio_version": "0.1.0"
  }
}
```

Fields:

- `ok` (boolean, required) - `true` for successful handshake.
- `result.protocol_version` (integer, required) - The major version of ICP the plugin implements.
- `result.plugin_version` (string, required) - the version of the `irlos-control` plugin itself, as a semver string.
- `result.irlosstudio_version` (string, required) - the version of IrlosStudio the plugin is loaded into.

**Plugin `hello` response (failure)**
```
{
  "ok": false,
  "error": {
    "code": "version_mismatch",
    "message": "Client protocol version 2 not supported; this plugin implements version 1."
  }
}
```

Or
```
{
  "ok": false,
  "error": {
    "code": "client_already_attached",
    "message": "Another client is currently connected."
  }
}
```

After sending a failure response, the plugin MUST close the connection.

#### Defined error codes for the handshake
- `version_mismatch` - the client's `protocol_version` is not supported by the plugin.
- `client_already_attached` - another client holds the active connection.
- `malformed_hello` - the `hello` request was missing required fields, had wrong types, or was not a `hello` op.

#### Version compatibility
ICP uses major-version numbers only. A plugin implementing protocol version `N` MUST accept only clients reporting `protocol_version: N`. There is no minor-version negotiation.

This is a deliberate simplification. Within a major version, the protocol is extended only by:

- Adding new optional fields to existing requests or responses
- Adding new operations
- Adding new fields to error responses

These changes are backward-compatible by the rules in S4 (unknown fields ignored, unknown operations rejected). Any change that would break an older implementation requires bumping the major version.

#### Behavior on handshake failure
If the client sends a request other than `hello` as its first message, the plugin MUST respond with a `malformed_hello` error and close the connection.

If the client sends a `hello` with an unsupported version, the plugin MUST respond with `version_mismatch` and close the connection.

If a second client connects while the plugin already has an active session, the plugin MUST accept the connection (so it can send a structured error), respond to the second client's `hello` with `client_already_attached`, and close the second connection. The original client's session is unaffected.

#### Why the handshake exists
The handshake serves three purposes:

1. Version negotiation - Client and plugin agree on protocol version before exchanging any state-changing messages. This catches mismatches at connection time rather than partway through a session when a message is misparsed.

2. Concurrent client rejection - The single-client invariant from S2 is enforced here. The second client gets a structured, debuggable error instead of mysteriously failing.

3. Implementation diagnostics - The plugin's response includes plugin version and IrlosStudio version, which the client can log. When debugging, knowing exactly which versions are in play matters.



## 6. Request/Response model

After a successful handshake, the connection enters the request/response phase. All subsequent communication on the connection follows a strict request/response pattern.

#### Strict synchronous exchange
For each request the client sends, the plugin sends exactly one response. The client MUST NOT send a second request until the response to the first request has been received. The plugin MUST NOT process a second request until the response to the first has been written to the socket.

This rule aplies to the connection as a whole, not per-operation. There are no parallel requests, no streaming responses, no out of order responses.

#### Request shape
Every request is a JSON object with at least these fields:
```
{
  "op": "<operation_name>",
  ...
}
```

- `op` (string, required) - the operation name. MUST be one of the defined operations in S8 (or `hello`, defined in S5)

Additional fields are operation-specific and defined per-operation in S8

#### Response shape
Every response is a JSON object with one of two shapes.

**Success response:**
```
{
  "ok": true,
  "result": { ... }
}
```

- `ok` (boolean, required) - `true` for success.
- `result` (object, required) - operation-specific result data. The shape of `result` is defined per-operation in S8. For operations that return no meaningful data, `result` is the empty object `{}`. Even when empty, the `result` field MUST be present.

**Error response:**
```
{
  "ok": false,
  "error": {
    "code": "<error_code>",
    "message": "<human-readable description>"
  }
}
```

- `ok` (boolean, required) - `false` for error.
- `error.code` (string, required) - a snake_case error identifier from the defined set (see S7)
- `error.message` (string, required) - human-readable English description of the error.

A response MUST contain exactly one of `result` or `error`, never both, never neither.

#### Request-response pairing
ICP does not use request IDs. Pairing of requests to responses is by position: response `N` on a connection corresponds to request `N`.

This is sufficient because requests are strictly serialized. The client knows which request it just sent; the next response it receives is the response to that request.

#### Operation determinism
For a given plugin state, an operation MUST behave deterministically: the same request, sent twice in succession, MUST produce responses that differ only in fields documented as time-varying or state-dependent. Operations MUST NOT have hidden side effects beyond those documented in S8.

This rule is for implementer discipline. It rules out a category of "spooky" bugs where an operation works the first time and fails the second time for reasons not visible to the caller.

#### Connection state
The connection itself maintains minimal state:

- Whether the handshake has completed (after a successful `hello` exchange)
- Nothing else

Operations do not establish per-operation state on the connection. Each request is a complete description of what the client wants; each response is a complete description of what happened.

This is a deliberate simplicity choice. ICP is not session-stateful in the way HTTP cookies or SSH channels are. The plugin's state is OBS (IrlosStudio) state (which scene is current, whether streaming is active, etc.), and that state is queried or modified by individual operations. The connection itself is just a transport.

## 7. Error format



## 8. Operations

1. Switch to a named scene
2. Get current scene
3. Get list of scenes (for validation at startup)
4. Start streaming
5. Stop streaming
6. Get streaming state
7. Start recording
8. Stop recording



## 9. Conection lifecycle



## 10. Versioning Policy




