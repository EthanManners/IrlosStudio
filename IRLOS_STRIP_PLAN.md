# IrlosStudio — Phase 1 Strip Plan

Discovery pass against OBS Studio `master` (tag 30.2.3).
All verdicts are for Phase 2 unless noted "Phase 3".
**READ-ONLY PASS — no files were modified.**

---

## 1. UI/ File Inventory

### KEEP — Core composition, main window, essential docks

#### Main window / application shell
| File | Description |
|------|-------------|
| `obs-app.cpp/.hpp` | Application entry, global init, command-line parsing |
| `obs-app-theming.cpp/.hpp` | Theme loading (keep for hardcoded dark theme) |
| `window-main.hpp` | Base class for main window |
| `window-basic-main.cpp/.hpp` | Core main window — scene/source lists, docks, output controls |
| `window-basic-main-outputs.cpp/.hpp` | Output management logic (streaming/recording state) |
| `window-basic-main-transitions.cpp` | Transition bar and logic |
| `window-basic-main-scene-collections.cpp` | Scene-collection load/switch logic (irlosd writes collections) |
| `window-basic-main-profiles.cpp` | Profile load/switch logic (irlosd writes profiles) |
| `window-basic-main-browser.cpp` | Browser dock integration wiring |
| `window-basic-main-icons.cpp` | Source-type icon loading |
| `window-basic-main-screenshot.cpp` | Screenshot-to-file functionality |
| `window-basic-preview.cpp/.hpp` | Preview display widget |
| `window-basic-status-bar.cpp/.hpp` | Status bar (bitrate, dropped frames, CPU) |
| `window-basic-stats.cpp/.hpp` | Statistics dock |
| `basic-controls.cpp/.hpp` | Controls dock (Start/Stop stream, record, studio mode) |

#### Scene / source tree
| File | Description |
|------|-------------|
| `scene-tree.cpp/.hpp` | Scene list widget |
| `source-tree.cpp/.hpp` | Source list widget |
| `item-widget-helpers.cpp/.hpp` | Helper widgets for scene/source list items |
| `visibility-item-widget.cpp/.hpp` | Eye-icon visibility toggle widget |
| `source-label.cpp/.hpp` | Source name label widget |

#### Dock base + browser docks
| File | Description |
|------|-------------|
| `window-dock.cpp/.hpp` | OBSDock base class (all docks inherit this) |
| `window-dock-browser.cpp/.hpp` | BrowserDock widget (base for chat/dashboard docks) |
| `window-extra-browsers.cpp/.hpp` | **UNSURE** — see note below |

#### Properties / filters / transform
| File | Description |
|------|-------------|
| `window-basic-properties.cpp/.hpp` | Source properties dialog |
| `window-basic-filters.cpp/.hpp` | Filters dialog |
| `window-basic-transform.cpp/.hpp` | Transform editor dialog |
| `window-basic-interaction.cpp/.hpp` | Source interaction window (browser clicks, gamepad) |
| `window-basic-adv-audio.cpp/.hpp` | Advanced audio mixer dialog |

#### Media / display
| File | Description |
|------|-------------|
| `media-controls.cpp/.hpp` | Playback controls toolbar for media sources |
| `qt-display.cpp/.hpp` | Qt wrapper around libobs display |
| `display-helpers.hpp` | Display utility helpers |
| `multiview.cpp/.hpp` | Multiview rendering (UNSURE — see note) |

#### Source addition
| File | Description |
|------|-------------|
| `window-basic-source-select.cpp/.hpp` | "Add Source" type picker dialog |
| `context-bar-controls.cpp/.hpp` | Source context toolbar (per-source toolbar buttons) |

#### Audio controls
| File | Description |
|------|-------------|
| `volume-control.cpp/.hpp` | Volume slider + fader widget |
| `adv-audio-control.cpp/.hpp` | Per-channel controls in advanced audio dialog |
| `mute-checkbox.hpp` | Mute toggle checkbox |
| `balance-slider.hpp` | Audio balance slider |
| `audio-encoders.cpp/.hpp` | Audio encoder enumeration helpers |

#### Widget utilities (all KEEP — used by core UI)
| File | Description |
|------|-------------|
| `absolute-slider.cpp/.hpp` | Click-to-set slider |
| `clickable-label.hpp` | Clickable QLabel |
| `focus-list.cpp/.hpp` | List widget that tracks focus |
| `horizontal-scroll-area.cpp/.hpp` | Horizontal scroll area widget |
| `lineedit-autoresize.cpp/.hpp` | Auto-resizing line edit |
| `menu-button.cpp/.hpp` | Drop-down menu button widget |
| `noncheckable-button.hpp` | QPushButton subclass that can't be checked |
| `obs-proxy-style.cpp/.hpp` | Qt style proxy (custom look tweaks) |
| `preview-controls.cpp/.hpp` | Preview zoom/fit controls |
| `url-push-button.cpp/.hpp` | Hyperlink-style push button |

#### Platform / system
| File | Description |
|------|-------------|
| `platform.hpp` | Platform abstraction header |
| `platform-x11.cpp` | Linux/X11 platform helpers |
| `platform-windows.cpp` | Windows platform helpers (can rot, not deleted) |
| `system-info.hpp` | System info interface |
| `system-info-posix.cpp` | POSIX system info (memory, CPU) |
| `system-info-windows.cpp` | Windows system info (can rot) |

#### Misc utilities
| File | Description |
|------|-------------|
| `ffmpeg-utils.cpp/.hpp` | FFmpeg utility wrappers |
| `remote-text.cpp/.hpp` | Background HTTP text fetcher |
| `screenshot-obj.hpp` | Screenshot capture helper |
| `ui-validation.cpp/.hpp` | Input validation helpers |
| `log-viewer.cpp/.hpp` | Log viewer dock |
| `window-missing-files.cpp/.hpp` | Missing-files dialog (source paths) |
| `window-namedialog.cpp/.hpp` | Generic name-entry dialog (used for scene/source rename) |

#### OBS frontend API
| File | Description |
|------|-------------|
| `obs-frontend-api/obs-frontend-api.cpp` | Frontend API implementation (plugins call this) |
| `obs-frontend-api/obs-frontend-internal.hpp` | Internal frontend API header |

---

### STRIP — Settings, auth, update, platform-specific chrome

#### Settings dialog
| File | Reason |
|------|--------|
| `window-basic-settings.cpp/.hpp` | Entire settings UI — irlosd manages config |
| `window-basic-settings-a11y.cpp` | Accessibility settings tab |
| `window-basic-settings-appearance.cpp` | Appearance settings tab |
| `window-basic-settings-stream.cpp` | Stream key / service selector tab |

#### Auto-config wizard
| File | Reason |
|------|--------|
| `window-basic-auto-config.cpp/.hpp` | First-run wizard — Irlos handles first-boot |
| `window-basic-auto-config-test.cpp` | Bandwidth/encoder test wizard page |

#### Update system (`UI/update/` — entire directory)
| File | Reason |
|------|--------|
| `update/update-window.cpp/.hpp` | Update UI |
| `update/update-helpers.cpp/.hpp` | Update download helpers |
| `update/shared-update.cpp/.hpp` | Shared update logic |
| `update/win-update.cpp/.hpp` | Windows updater |
| `update/mac-update.cpp/.hpp` | macOS updater |
| `update/crypto-helpers.hpp` | Crypto for update verification |
| `update/crypto-helpers-mbedtls.cpp` | mbedTLS crypto impl |
| `update/models/branches.hpp` | Update branch model |
| `update/models/whatsnew.hpp` | What's New model |

#### Windows updater binary (`UI/win-update/` — entire directory)
| File                             | Reason                                 |
| -------------------------------- | -------------------------------------- |
| `win-update/updater/updater.cpp` | Windows-only standalone updater binary |
| `win-update/updater/patch.cpp`   | Binary patcher                         |
| `win-update/updater/helpers.cpp` | Updater helpers                        |
| `win-update/updater/hash.cpp`    | Hash verification                      |
| `win-update/updater/http.cpp`    | HTTP download                          |

#### Projector
| File | Reason |
|------|--------|
| `window-projector.cpp/.hpp` | Fullscreen/windowed projector — no displays on appliance |

#### Undo/redo
| File | Reason |
|------|--------|
| `undo-stack-obs.cpp/.hpp` | Undo/redo stack — complexity without value on appliance |

#### Drag-and-drop source addition
| File | Reason |
|------|--------|
| `window-basic-main-dropfiles.cpp` | Desktop drag-and-drop — VNC workflow doesn't use this |

#### Virtual camera UI
| File | Reason |
|------|--------|
| `window-basic-vcam-config.cpp/.hpp` | VCam config dialog |
| `window-basic-vcam.hpp` | VCam header |

#### YouTube integration
| File | Reason |
|------|--------|
| `window-dock-youtube-app.cpp/.hpp` | YouTube Live Dashboard dock |
| `window-youtube-actions.cpp/.hpp` | YouTube "Go Live" actions dialog |
| `youtube-api-wrappers.cpp/.hpp` | YouTube API client |
| `goliveapi-network.cpp/.hpp` | YouTube Go Live API networking |
| `goliveapi-postdata.cpp/.hpp` | YouTube Go Live API post data |
| `goliveapi-censoredjson.cpp/.hpp` | YouTube Go Live API log sanitizer |

#### Auth / OAuth (all platforms)
| File | Reason |
|------|--------|
| `auth-base.cpp/.hpp` | Base OAuth class — **KEEP** (Twitch auth kept) |
| `auth-oauth.cpp/.hpp` | OAuth2 flow — **KEEP** (Twitch auth kept) |
| `auth-listener.cpp/.hpp` | OAuth callback listener — **KEEP** (Twitch auth kept) |
| `auth-twitch.cpp/.hpp` | Twitch OAuth + dock creation — **KEEP** (decision: keep full auth flow) |
| `auth-restream.cpp/.hpp` | Restream auth — STRIP (Restream not used) |
| `auth-youtube.cpp/.hpp` | YouTube auth — STRIP (YouTube stripped) |

#### Hotkey editor
| File | Reason |
|------|--------|
| `hotkey-edit.cpp/.hpp` | Hotkey assignment widget (lives in settings dialog) |

#### Scene collection importer UI
| File | Reason |
|------|--------|
| `window-importer.cpp/.hpp` | Importer dialog |
| `importers/importers.cpp/.hpp` | Multi-format importer logic |
| `importers/classic.cpp` | OBS Classic importer |
| `importers/sl.cpp` | Streamlabs importer |
| `importers/studio.cpp` | OBS Studio importer |
| `importers/xsplit.cpp` | XSplit importer |

#### Multitrack video output UI
| File | Reason |
|------|--------|
| `multitrack-video-output.cpp/.hpp` | Enhanced RTMP multitrack output UI |
| `multitrack-video-error.cpp/.hpp` | Multitrack error dialog |
| `models/multitrack-video.hpp` | Multitrack data model |

#### Log reporter / crash handler
| File | Reason |
|------|--------|
| `window-log-reply.cpp/.hpp` | Log upload to pastebin |

#### macOS permissions
| File | Reason |
|------|--------|
| `window-permissions.cpp/.hpp` | macOS screen recording permissions dialog |

#### Remux tool
| File | Reason |
|------|--------|
| `window-remux.cpp/.hpp` | Post-recording remux dialog — no recordings on appliance |

#### API interface (scripts/plugin bridge)
| File | Reason |
|------|--------|
| `api-interface.cpp` | Frontend API plugin bridge for scripts — scripting is stripped |

---

### RESOLVED — Previously UNSURE

| File | Decision |
|------|----------|
| `window-extra-browsers.cpp/.hpp` | **KEEP** — useful for adding ad-hoc chat overlay URLs via VNC without a rebuild. |
| `window-basic-about.cpp/.hpp` | **KEEP** — rebrand in Phase 5. |
| `multiview.cpp/.hpp` | **KEEP** — revisit if it pulls in projector code during Phase 2 build. |
| `auth-twitch.cpp/.hpp` | **KEEP** — full OAuth flow and Twitch docks kept as-is. |
| `window-dock-browser.cpp/.hpp` | **KEEP** — BrowserDock base needed by Twitch docks and extra browsers. |
| `screenshot-obj.hpp` / `window-basic-main-screenshot.cpp` | **KEEP** — harmless, context-menu accessible. |

---

### KEEP — `.ui` Form Files

| File | Description |
|------|-------------|
| `forms/OBSBasic.ui` | Main window layout |
| `forms/OBSBasicControls.ui` | Controls dock |
| `forms/OBSBasicProperties.ui` | Source properties dialog |
| `forms/OBSBasicFilters.ui` | Filters dialog |
| `forms/OBSAdvAudio.ui` | Advanced audio mixer |
| `forms/OBSBasicTransform.ui` | Transform editor |
| `forms/OBSBasicInteraction.ui` | Interaction window |
| `forms/OBSBasicSourceSelect.ui` | Add source dialog |
| `forms/OBSLogViewer.ui` | Log viewer |
| `forms/StatusBarWidget.ui` | Status bar widget |
| `forms/ColorSelect.ui` | Color picker |
| `forms/OBSPermissions.ui` | (macOS only — can rot) |
| `forms/OBSBasicVCamConfig.ui` | **STRIP** (VCam) |
| `forms/source-toolbar/browser-source-toolbar.ui` | Browser source toolbar |
| `forms/source-toolbar/color-source-toolbar.ui` | Color source toolbar |
| `forms/source-toolbar/image-source-toolbar.ui` | Image source toolbar |
| `forms/source-toolbar/text-source-toolbar.ui` | Text source toolbar |
| `forms/source-toolbar/device-select-toolbar.ui` | Device source toolbar |
| `forms/source-toolbar/game-capture-toolbar.ui` | **STRIP** (game capture gone) |
| `forms/source-toolbar/media-controls.ui` | Media playback toolbar |

### STRIP — `.ui` Form Files

| File | Reason |
|------|--------|
| `forms/OBSBasicSettings.ui` | Settings dialog |
| `forms/OBSAbout.ui` | About dialog — replace with IrlosStudio about in Phase 5 |
| `forms/AutoConfigStartPage.ui` | Wizard page 1 |
| `forms/AutoConfigStreamPage.ui` | Wizard page 2 |
| `forms/AutoConfigVideoPage.ui` | Wizard page 3 |
| `forms/AutoConfigTestPage.ui` | Wizard page 4 |
| `forms/AutoConfigFinishPage.ui` | Wizard page 5 |
| `forms/OBSImporter.ui` | Scene importer dialog |
| `forms/OBSMissingFiles.ui` | Missing files dialog — KEEP: still needed for broken sources |
| `forms/OBSRemux.ui` | Remux dialog |
| `forms/OBSUpdate.ui` | Update dialog |
| `forms/OBSExtraBrowsers.ui` | Extra browser docks dialog |
| `forms/OBSYoutubeActions.ui` | YouTube actions dialog |
| `forms/OBSLogReply.ui` | Log upload dialog |

**Correction**: `forms/OBSMissingFiles.ui` and `window-missing-files.cpp/.hpp` should be **KEEP** — users will have broken source paths if they move files and need to repath them.

### STRIP — `UI/frontend-plugins/`

| File | Reason |
|------|--------|
| `frontend-plugins/aja-output-ui/` (all) | AJA I/O — stripped in Phase 3 |
| `frontend-plugins/decklink-captions/` (all) | DeckLink — stripped in Phase 3 |
| `frontend-plugins/decklink-output-ui/` (all) | DeckLink — stripped in Phase 3 |
| `frontend-plugins/frontend-tools/captions.cpp/.hpp` | Closed captions (MSSAPI, Windows-only) |
| `frontend-plugins/frontend-tools/captions-handler.cpp/.hpp` | Captions handler |
| `frontend-plugins/frontend-tools/captions-mssapi.cpp/.hpp` | MS Speech API |
| `frontend-plugins/frontend-tools/captions-mssapi-stream.cpp/.hpp` | MSSAPI stream |
| `frontend-plugins/frontend-tools/scripts.cpp/.hpp` | Script editor UI |
| `frontend-plugins/frontend-tools/forms/scripts.ui` | Script editor form |
| `frontend-plugins/frontend-tools/forms/captions.ui` | Captions form |

### KEEP — `UI/frontend-plugins/`

| File | Reason |
|------|--------|
| `frontend-plugins/frontend-tools/auto-scene-switcher.cpp/.hpp` | Auto scene switcher (useful for BRB scenes) |
| `frontend-plugins/frontend-tools/auto-scene-switcher-nix.cpp` | Linux implementation |
| `frontend-plugins/frontend-tools/auto-scene-switcher-win.cpp` | Windows (can rot) |
| `frontend-plugins/frontend-tools/output-timer.cpp/.hpp` | Output timer |
| `frontend-plugins/frontend-tools/tool-helpers.hpp` | Shared helpers |
| `frontend-plugins/frontend-tools/forms/auto-scene-switcher.ui` | Switcher form |
| `frontend-plugins/frontend-tools/forms/output-timer.ui` | Timer form |

### Themes

Keep only `UI/data/themes/Yami/` (complete dark theme) and `UI/data/themes/Yami.obt`.
Strip: `Acri/`, `Dark/`, `Light/`, `Rachni/`, all `.ovt` variant files.
Hardcode the Yami theme in `obs-app-theming.cpp` to skip the theme selector.

---

## 2. Plugin Inventory

| Plugin | Path | Purpose | Verdict |
|--------|------|---------|---------|
| obs-ffmpeg | `plugins/obs-ffmpeg/` | FFmpeg encoder (HEVC, AV1, VAAPI, nvenc via ffmpeg), media source, SRT/RTMP input | **KEEP** |
| obs-x264 | `plugins/obs-x264/` | x264 H.264 software encoder | **KEEP** |
| obs-outputs | `plugins/obs-outputs/` | RTMP, FLV, MP4, null output implementations | **KEEP** |
| rtmp-services | `plugins/rtmp-services/` | Service definitions (Twitch, YouTube etc.) — irlosd uses this via profile | **KEEP** |
| obs-websocket | `plugins/obs-websocket/` | WebSocket RPC server — irlosd control plane, **CRITICAL** | **KEEP** |
| obs-browser | `plugins/obs-browser/` | CEF browser source — chat overlays, dashboards | **KEEP** |
| obs-filters | `plugins/obs-filters/` | Color correction, chroma key, noise suppression, etc. | **KEEP** |
| image-source | `plugins/image-source/` | Image, slideshow, and color sources | **KEEP** |
| text-freetype2 | `plugins/text-freetype2/` | FreeType2 text overlay (Linux) | **KEEP** |
| obs-transitions | `plugins/obs-transitions/` | Cut, fade, swipe, stinger transitions | **KEEP** |
| obs-nvenc | `plugins/obs-nvenc/` | NVIDIA NVENC H.264/HEVC/AV1 via native API | **KEEP** (GPU encoding on server) |
| obs-qsv11 | `plugins/obs-qsv11/` | Intel Quick Sync encoder | **KEEP** (conditional, server may have Intel iGPU) |
| obs-libfdk | `plugins/obs-libfdk/` | FDK-AAC encoder (better quality than ffmpeg AAC) | **KEEP** |
| nv-filters | `plugins/nv-filters/` | NVIDIA AI noise suppression, background blur | **KEEP** (all servers have a GPU) |
| obs-webrtc | `plugins/obs-webrtc/` | WHIP WebRTC output | **KEEP** |
| linux-pulseaudio | `plugins/linux-pulseaudio/` | PulseAudio mic and desktop audio capture | **KEEP** (retained in case) |
| linux-alsa | `plugins/linux-alsa/` | ALSA audio capture | **KEEP** (retained in case) |
| linux-pipewire | `plugins/linux-pipewire/` | PipeWire screen/camera/audio capture | **KEEP** (audio portion retained in case; screen/camera sources still strip in Phase 3) |
| linux-jack | `plugins/linux-jack/` | JACK audio | **KEEP** (retained in case) |
| linux-v4l2 | `plugins/linux-v4l2/` | Video4Linux2 webcam capture + v4l2loopback virtual camera | **STRIP** |
| linux-capture | `plugins/linux-capture/` | X11/XComposite/XShm screen capture | **STRIP** (headless server) |
| oss-audio | `plugins/oss-audio/` | OSS audio capture (FreeBSD) | **STRIP** |
| sndio | `plugins/sndio/` | sndio audio (BSD) | **STRIP** |
| coreaudio-encoder | `plugins/coreaudio-encoder/` | macOS CoreAudio AAC encoder | **STRIP** (Linux target) |
| mac-avcapture | `plugins/mac-avcapture/` | macOS AV Foundation camera | **STRIP** |
| mac-capture | `plugins/mac-capture/` | macOS screen capture | **STRIP** |
| mac-syphon | `plugins/mac-syphon/` | macOS Syphon inter-app video | **STRIP** |
| mac-videotoolbox | `plugins/mac-videotoolbox/` | macOS VideoToolbox encoder | **STRIP** |
| mac-virtualcam | `plugins/mac-virtualcam/` | macOS virtual camera | **STRIP** |
| win-capture | `plugins/win-capture/` | Windows screen/game/window capture | **STRIP** |
| win-dshow | `plugins/win-dshow/` | Windows DirectShow audio/video | **STRIP** |
| win-wasapi | `plugins/win-wasapi/` | Windows WASAPI audio | **STRIP** |
| aja | `plugins/aja/` | AJA I/O card input/output | **STRIP** |
| decklink | `plugins/decklink/` | Blackmagic DeckLink I/O | **STRIP** |
| obs-vst | `plugins/obs-vst/` | VST2 plugin host for audio | **STRIP** |
| obs-text | `plugins/obs-text/` | GDI+ text source (Windows-only) | **STRIP** (text-freetype2 covers Linux) |
| vlc-video | `plugins/vlc-video/` | VLC-backed video source | **STRIP** (ffmpeg_source covers all cases) |

---

## 3. Source Type Registrations

Grepped `obs_register_source` and `obs_source_info` across all `.c`/`.cpp` files.

### KEEP (allowlist)

| Source ID | Plugin / File | Notes |
|-----------|---------------|-------|
| `ffmpeg_source` | `plugins/obs-ffmpeg/obs-ffmpeg-source.c` | SRT/RTMP/file media input — core IRL source |
| `browser_source` | `plugins/obs-browser/` | Chat overlays, dashboards |
| `image_source` | `plugins/image-source/image-source.c` | BRB/starting-soon graphics |
| `slideshow` | `plugins/image-source/obs-slideshow.c` | Rotating graphics (also `obs-slideshow-mk2.c`) |
| `text_ft2_source` | `plugins/text-freetype2/text-freetype2.c` | Text overlays on Linux |
| `scene` | `libobs/obs-scene.c` | Nested scenes (registered by libobs, never touch) |
| `group` | `libobs/obs-scene.c` | Source grouping (registered by libobs) |
| `color_source` (v1/v2/v3) | `plugins/image-source/color-source.c` | Solid color backgrounds |
| `pulse_input_capture` | `plugins/linux-pulseaudio/` | Microphone input (retained in case) |
| `pulse_output_capture` | `plugins/linux-pulseaudio/` | Desktop audio monitor (retained in case) |
| `alsa_input_capture` | `plugins/linux-alsa/` | ALSA audio (retained in case) |
| `jack_output_capture` | `plugins/linux-jack/` | JACK audio (retained in case) |
| `nv_background_blur_filter` | `plugins/nv-filters/` | NVIDIA AI background blur |
| `nvidia_audiofx_filter` | `plugins/nv-filters/` | NVIDIA AI audio noise suppression |

### STRIP (removed with their plugins)

| Source ID | Plugin | Reason |
|-----------|--------|--------|
| `monitor_capture` | `plugins/win-capture/` | Windows display capture |
| `window_capture` | `plugins/win-capture/` | Windows window capture |
| `game_capture` | `plugins/win-capture/` | Windows game capture |
| `xcomposite_input` | `plugins/linux-capture/` | X11 window capture |
| `xshm_input` / `xshm_input_v2` | `plugins/linux-capture/` | X11 screen capture |
| `pipewire-desktop-capture-source` | `plugins/linux-pipewire/` | PipeWire screen capture |
| `pipewire-screen-capture-source` | `plugins/linux-pipewire/` | PipeWire screen capture |
| `pipewire-window-capture-source` | `plugins/linux-pipewire/` | PipeWire window capture |
| `pipewire-camera-source` | `plugins/linux-pipewire/` | PipeWire camera |
| `v4l2_input` | `plugins/linux-v4l2/` | V4L2 webcam |
| `virtualcam_output` | `plugins/linux-v4l2/` | V4L2 virtual camera output |
| `oss_input_capture` | `plugins/oss-audio/` | OSS audio (FreeBSD-only, strip) |
| `sndio_output_capture` | `plugins/sndio/` | sndio audio |
| `wasapi_input_capture` | `plugins/win-wasapi/` | Windows mic |
| `wasapi_output_capture` | `plugins/win-wasapi/` | Windows desktop audio |
| `wasapi_process_output_capture` | `plugins/win-wasapi/` | Windows per-app audio |
| `coreaudio_input_capture` | `plugins/mac-capture/` | macOS mic |
| `coreaudio_output_capture` | `plugins/mac-capture/` | macOS desktop audio |
| `av_capture_input` / `av_capture_input_v2` | `plugins/mac-avcapture/` | macOS camera |
| `screen_capture` | `plugins/mac-capture/` | macOS screen capture |
| `syphon-input` | `plugins/mac-syphon/` | macOS Syphon |
| `dshow_input` | `plugins/win-dshow/` | Windows DirectShow |
| `decklink_input` / `decklink_output` | `plugins/decklink/` | DeckLink cards |
| `aja_source` / `aja_output` | `plugins/aja/` | AJA cards |
| `vlc_source` | `plugins/vlc-video/` | VLC media source |
| `text_gdiplus` / `text_gdiplus_v2` | `plugins/obs-text/` | Windows GDI+ text |
| `vst_filter` | `plugins/obs-vst/` | VST plugin |

### Transitions (registered by obs-transitions — KEEP plugin, KEEP all)

| Source ID | Notes |
|-----------|-------|
| `cut_transition` | Cut |
| `fade_transition` | Fade |
| `swipe_transition` | Swipe |
| `slide_transition` | Slide |
| `obs_stinger_transition` | Stinger |
| `wipe_transition` | Wipe (with mask) |
| `fade_to_color_transition` | Fade to color |

### Encoders/Outputs (registered separately from sources — KEEP all from surviving plugins)

These are not "sources" but are registered similarly. All from KEEP plugins survive.

---

## 4. Menu Bar Construction

Source: `UI/forms/OBSBasic.ui` + `UI/window-basic-main.cpp`.

The full menu bar to be **removed entirely** in Phase 2:

### File
- Show Recordings
- Remux Recording…
- Settings…
- Show Settings Folder
- Show Profile Folder
- Exit

### Edit
- Undo / Redo
- Copy Source / Paste Reference / Paste Duplicate
- Copy Filters / Paste Filters
- Transform →
  - Edit Transform…
  - Copy Transform
  - Paste Transform
  - Reset Transform
  - Rotate 90° CW / 90° CCW / 180°
  - Flip Horizontal / Flip Vertical
  - Fit to Screen / Stretch to Screen / Center to Screen
  - Reset to Original Size
- Reorder →
  - Move Up / Move Down / Move to Top / Move to Bottom
- Scale →
  - Scale to Window / Scale to Canvas / Scale to Output
- Lock Preview
- Advanced Audio Properties…

### View
- Reset UI
- Fullscreen Interface (F11)
- Scene List Mode → (List / Grid)
- Toolbars → (toggle context bar, source toolbars, etc.)
- Toggle Status Bar
- Statistics
- Multiview Projector → (populated at runtime)
- Multiview Windowed
- Always on Top

### Docks
- Lock Docks
- Side Docks → (left/right placement)
- Reset Docks
- ─────
- (runtime dock list, populated dynamically)

### Profile
- New Profile…
- Duplicate Profile…
- Rename Profile…
- Remove Profile
- Import Profile…
- Export Profile…
- ─────
- (runtime profile list)

### Scene Collection
- New Scene Collection…
- Duplicate Scene Collection…
- Rename Scene Collection…
- Remove Scene Collection
- Import Scene Collection…
- Export Scene Collection…
- Show Missing Files…
- Remigrate Scene Collection
- ─────
- (runtime collection list)

### Tools
- Auto-Configure Wizard…
- ─────
- (runtime tools, populated by plugins e.g. `frontend-tools` output timer, scene switcher)

### Help
- Help Portal
- OBS Website
- OBS Discord
- Logs → (Show/Upload Current Log / Upload Last Log / View Current Log File)
- Crash Logs → (Show/Upload Last Crash Log)
- Check for Updates…
- Restart in Safe Mode…
- Show macOS Permissions (macOS only)
- What's New
- Release Notes
- About OBS Studio…

**Phase 2 action:** Remove the QMenuBar instantiation. All entries above disappear.
Right-click context menus on scene and source lists are **separate** and survive.

---

## 5. Settings Dialog Tabs

Source: `UI/forms/OBSBasicSettings.ui`, `UI/window-basic-settings.cpp`, companion files.

All 9 tabs are **STRIP** (the entire settings dialog is removed):

| Tab | File(s) | What it configures |
|-----|---------|-------------------|
| General | `window-basic-settings.cpp` | Language, theme, system tray, update checks, general UI options |
| Appearance | `window-basic-settings-appearance.cpp` | Theme picker, custom CSS |
| Stream | `window-basic-settings-stream.cpp` | Service selector, stream key, OAuth connect |
| Output | `window-basic-settings.cpp` | Encoder, bitrate, recording path (irlosd manages via profile) |
| Audio | `window-basic-settings.cpp` | Sample rate, channels, audio devices |
| Video | `window-basic-settings.cpp` | Canvas resolution, output resolution, FPS |
| Hotkeys | `window-basic-settings.cpp` + `hotkey-edit.cpp` | All hotkey bindings |
| Accessibility | `window-basic-settings-a11y.cpp` | Color vision modes, high contrast |
| Advanced | `window-basic-settings.cpp` | Process priority, filename format, delay, network, browser sources |

**Profile compatibility note:** OBS reads Video/Audio/Output settings at startup from profile `.ini` files (written by irlosd). The settings *dialog* is the UI for editing those files. Deleting the dialog does not affect startup profile loading — only the editing UI is removed.

---

## 6. obs-websocket Integration Points

Source: grep of `UI/**/*.cpp` for `websocket`, `WebSocket`, `obs-websocket`.

### Direct references in UI code

**`UI/window-basic-main.cpp:244`**
```cpp
static const unordered_set<string> unsafe_modules = {
    "frontend-tools", // Scripting
    "obs-websocket",  // Allows outside modifications
};
```
Purpose: marks obs-websocket as "unsafe" so it can be disabled by Safe Mode (`--safe-mode` flag). This is benign for IrlosStudio — we will never run in safe mode. **Do not touch.**

**`UI/window-basic-main.cpp:2384`**
```cpp
/* Modules can access frontend information ... some modules (e.g. obs-websockets)
   are known to use the filesystem location of the current profile ... */
RefreshSceneCollections(true);
obs_load_all_modules2(&mfi);
```
Purpose: comment explaining why profile/collection discovery happens before module load. This ordering ensures obs-websocket has a valid profile path when it initializes. **Critical — do not reorder. Do not touch.**

**`UI/obs-app.cpp:2813`**
```cpp
"--safe-mode: Run in Safe Mode (disables third-party plugins, scripting, and WebSockets).\n"
```
Purpose: help text for `--safe-mode` CLI flag. Informational only. No functional impact.

### What is NOT in the UI

- There is **no settings panel** for obs-websocket in the OBS UI code. The WebSocket plugin ships its own settings page which it registers via the OBS frontend API (`obs_frontend_add_tools_menu_item` or similar). That settings page lives entirely inside `plugins/obs-websocket/` — not in the UI source.
- The port (4455), password, and auth-required flag are stored inside the obs-websocket plugin's own config file (`obs-studio/plugin_config/obs-websocket/config.json`), written by irlosd at first boot. The UI never reads or writes that file.
- **Conclusion:** Stripping the OBS settings dialog, the menu bar, and any UI chrome does **not** touch obs-websocket configuration or its control plane. The plugin loads via `obs_load_all_modules2` and self-configures from its own config file, entirely independent of the OBS UI.

### Safe Mode caution

If IrlosStudio is ever launched with `--safe-mode`, obs-websocket will be disabled (it is in `unsafe_modules`). irlosd would then be unable to control IrlosStudio. Ensure the systemd unit file for IrlosStudio does **not** pass `--safe-mode`.

---

## 7. Pre-Phase-2 Decisions — RESOLVED

All questions answered. Phase 2 may proceed.

| # | Question | Decision |
|---|----------|----------|
| Q1 | Twitch docks — keep full auth flow or add fixed browser docks? | **KEEP full `auth-twitch.cpp`** (OAuth + docks kept as-is) |
| Q2 | `window-extra-browsers` — strip or keep? | **KEEP** (useful for adding ad-hoc chat overlays via VNC) |
| Q3 | Audio capture plugins — keep linux-pulseaudio? | **KEEP all Linux audio plugins** (linux-pulseaudio, linux-alsa, linux-pipewire, linux-jack) — retained in case; oss-audio and sndio still stripped (BSD-only) |
| Q4 | `nv-filters` — keep? | **KEEP** (all servers have a GPU) |
| Q5 | `obs-webrtc` (WHIP) — keep? | **KEEP** |
| Q6 | Undo/redo — strip with call site cleanup? | **STRIP** (`undo-stack-obs.cpp/.hpp` + all `undo_s.add_action(...)` call sites removed in Phase 2) |

---

*End of Phase 1 discovery. No source files were modified.*
*Updated: decisions recorded after owner review. Ready for Phase 2.*
