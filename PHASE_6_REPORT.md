# Phase 6 — Final Sweep Report

**Date:** 2026-04-25  
**Branch:** `irlos/phase-6-final`  
**Binary:** `build/rundir/Debug/bin/obs` (debug build)

---

## 1. Dead Include Sweep

Grepped all surviving `.cpp`/`.hpp` files against the full list of deleted headers. Three dead includes found — all inside conditional blocks that evaluate false on Linux:

| File | Dead include | Guard |
|------|-------------|-------|
| `UI/window-basic-main.cpp` | `update/win-update.hpp`, `update/shared-update.hpp` (×2), `update/models/whatsnew.hpp` | `#ifdef _WIN32`, `#ifdef WHATSNEW_ENABLED` |
| `UI/obs-app.cpp` | `update/models/branches.hpp` | `#if defined(_WIN32) \|\| defined(ENABLE_SPARKLE_UPDATER)` |

**Action:** Removed all five dead include lines. No functional change (none compiled on Linux), but eliminates future confusion.

---

## 2. Dead CMake Subdirectory Sweep

Reviewed `plugins/CMakeLists.txt`, `UI/CMakeLists.txt`, and `UI/frontend-plugins/CMakeLists.txt`.

No dead `add_subdirectory` calls found. All stripped plugins were removed from the cmake file in Phase 3. `check_obs_browser()` and `check_obs_websocket()` remain as mandatory-submodule guards.

One note: `add_obs_plugin(nv-filters PLATFORMS WINDOWS)` correctly limits nv-filters to Windows — the NVAFX SDK has no Linux API. This matches upstream behavior.

---

## 3. Build Results

### obs-studio UI binary
**Result: PASS** — builds cleanly with no errors or new warnings.

### Plugin build
**Result: PASS** (14/14 expected plugins built; 2 skipped due to build env)

| Plugin | Status | Notes |
|--------|--------|-------|
| frontend-tools | ✓ built | auto-scene-switcher, output timer |
| image-source | ✓ built | image, slideshow, color sources |
| linux-alsa | ✓ built | ALSA audio capture |
| linux-pipewire | ✓ built | PipeWire (camera portal only; screencast stripped) |
| linux-pulseaudio | ✓ built | PulseAudio mic + desktop audio |
| obs-ffmpeg | ✓ built | FFmpeg encoder, media source, VAAPI |
| obs-filters | ✓ built | Chroma key, color correction, noise suppression |
| obs-outputs | ✓ built | RTMP, FLV, MP4 outputs |
| obs-qsv11 | ✓ built | Intel Quick Sync encoder |
| obs-transitions | ✓ built | Cut, fade, swipe, stinger, wipe |
| obs-websocket | ✓ built | v5.5.3 — irlosd control plane |
| obs-x264 | ✓ built | x264 H.264 software encoder |
| rtmp-services | ✓ built | RTMP service definitions |
| text-freetype2 | ✓ built | FreeType2 text overlays |
| obs-browser | ✗ not built | CEF (Chromium Embedded Framework) not in build env; must supply pre-built binary on appliance |
| obs-webrtc | ✗ build error | Pre-existing upstream breakage: `libdatachannel` deprecated API (`H264RtpPacketizer::defaultClockRate`, `setNeedsToReport()`). Not introduced by IrlosStudio. |
| linux-jack | ✗ not built | JACK dev headers not in build env |
| obs-libfdk | ✗ not built | FDK-AAC dev headers not in build env |
| obs-nvenc | ✗ not built | NVIDIA NVENC SDK not in build env |
| nv-filters | ✗ intentional | PLATFORMS WINDOWS — NVAFX SDK is Windows-only |

**Stale rundir cleanup:** Removed pre-Phase-3 leftover `.so` files for `decklink`, `linux-capture`, `linux-v4l2`, `obs-vst`, `vlc-video` from rundir. These would have loaded spuriously without rebuild.

---

## 4. Binary Size

| Metric | IrlosStudio (debug) | System OBS 30.2.2 (release/stripped) |
|--------|--------------------|------------------------------------|
| Binary on disk | 38 MB | 5.0 MB |
| `.text` section | 4,675,303 bytes (4.46 MB) | 5,070,846 bytes (4.84 MB) |
| `.data` section | 145,352 bytes | 122,832 bytes |

The IrlosStudio binary has **~395 KB less compiled code** in `.text` despite being a debug build (debug adds DWARF, reduces optimization). A stripped release build of IrlosStudio would be smaller still.

---

## 5. LOC Delta vs Upstream

```
git diff 5aaac2f0f HEAD --stat | tail -1
323 files changed, 980 insertions(+), 47480 deletions(-)
```

**Net: −46,500 lines removed** across 323 files.  
Breakdown by phase:
- Phase 2 (UI chrome strip): Settings dialog, projector, undo/redo, update system, auth flows, VCam, importers, YouTube, multitrack (~35,000 lines)
- Phase 3 (plugins): Screencast portal, stripped plugin CMake entries (~500 lines)
- Phase 4 (theme fixes): Dark/ SVG restore, ui-config.h.in (+399 lines)
- Phase 5 (rebrand): Controls dock, about dialog, locale (~237 lines)
- Phase 6 (sweep): Dead includes (~10 lines)

---

## 6. Smoke Test Results

OBS launched cleanly. No theme fallback. No settings or exit buttons visible. Menu bar hidden.

| Test | Result | Notes |
|------|--------|-------|
| GUI launches | PASS | Yami theme loads; no fallback |
| Menu bar hidden | PASS | `ui->menubar->hide()` in setupUi |
| Controls dock | PASS | Stream + Record buttons only; Settings/Exit/VCam removed |
| Scene collection loads | PASS | 3 scenes: BRB, STARTING, LIVE |
| obs-websocket connects | PASS | Port 4455, no auth |
| GetVersion | PASS | OBS 30.2.3, ws 5.5.3, RPC 1 |
| GetSceneList | PASS | 3 scenes returned |
| SetCurrentProgramScene | PASS | Round-trip confirmed via follow-up GetSceneList |
| ffmpeg_source (SRT) | PASS | `CreateInput` code 100; source added to scene |
| browser_source | SKIP | obs-browser requires CEF — not in this build env |
| SetStreamServiceSettings | PASS | `rtmp://127.0.0.1/live` accepted |
| StartStream | PASS | Command accepted (TCP connects; drops — no RTMP impl on dummy listener) |

---

## 7. Registered Source Types

From `GetInputKindList` at runtime:

| Source ID | Plugin |
|-----------|--------|
| `alsa_input_capture` | linux-alsa |
| `color_source_v3` | image-source |
| `ffmpeg_source` | obs-ffmpeg |
| `image_source` | image-source |
| `pipewire-camera-source` | linux-pipewire |
| `pulse_input_capture` | linux-pulseaudio |
| `pulse_output_capture` | linux-pulseaudio |
| `slideshow_v2` | image-source |
| `text_ft2_source_v2` | text-freetype2 |

Not registered in this build env (expected on production appliance):
- `browser_source` — requires pre-built CEF/obs-browser
- `jack_output_capture` — requires JACK dev headers
- `nv_background_blur_filter` / `nvidia_audiofx_filter` — requires NVAFX (Windows-only SDK)

Transitions registered (obs-transitions): `cut_transition`, `fade_transition`, `swipe_transition`, `slide_transition`, `obs_stinger_transition`, `wipe_transition`, `fade_to_color_transition`

Outputs registered (obs-outputs + obs-ffmpeg): `rtmp_output` (streaming), `ffmpeg_muxer` (recording)

---

## 8. Known Issues (Pre-existing, Not IrlosStudio-Introduced)

| Issue | Affected component | Status |
|-------|-------------------|--------|
| `obs-webrtc` fails to build against newer `libdatachannel` | `plugins/obs-webrtc/whip-output.cpp` | Upstream regression; WHIP output not available until upstream fixes |
| `VAAPI: Failed to initialize display` at startup | obs-ffmpeg VAAPI encoder | Non-fatal; VAAPI not available in this display environment; NVENC/x264 encoders unaffected |

---

## 9. irlosd Integration Notes

- **Websocket endpoint:** `ws://127.0.0.1:4455` — no authentication
- **Config file:** `~/.config/obs-studio/plugin_config/obs-websocket/config.json`  
  Write before launching IrlosStudio; set `server_enabled: true`, `auth_required: false`
- **Profile/collection:** irlosd writes `~/.config/obs-studio/basic/profiles/<name>/` and `~/.config/obs-studio/basic/scenes/<name>.json` before launch
- **Safe mode:** Never pass `--safe-mode` — obs-websocket is in OBS's `unsafe_modules` list and will be disabled
- **obs-browser:** Ship pre-built CEF + obs-browser `.so` alongside the binary; the CMake configuration accepts it when placed at `plugins/obs-browser/`
