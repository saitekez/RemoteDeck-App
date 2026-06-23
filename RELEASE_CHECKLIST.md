# RemoteDeck Release Checklist

Use this before tagging or publishing a new RemoteDeck build.

## Local Checks

- Run the manifest validator:

  ```powershell
  node tools/validate-remotedeck-project.js
  ```

- Run the embedded UI validator:

  ```powershell
  node tools/validate-embedded-ui.js
  ```

- Compile the firmware:

  ```powershell
  arduino-cli compile --fqbn esp32:esp32:adafruit_qtpy_esp32s3_nopsram RemoteDeck
  ```

## Firmware Smoke Test

- Flash a known-good ESP32-S3 board.
- Confirm the `RemoteDeck` WiFi access point appears after boot.
- Join with the documented AP password or a private password from `remotedeck_secrets.h`.
- Open `http://192.168.4.1`.
- Confirm the status line shows the expected firmware version.

## UI Test Matrix

- iPhone Safari in portrait.
- iPhone Safari in landscape.
- Android Chrome in portrait.
- Desktop browser for setup sanity.

Check:

- Keyboard layout fits without overlap.
- Touchpad movement, left click, right click, and scrolling work.
- Settings persist after refresh.
- Tools page shows output without layout breakage.
- Macros page renders each profile.

## Host Profile Smoke Test

- Windows: copy, paste, Task Manager, CMD, a read-only diagnostic, and one admin macro with UAC confirmation.
- Linux: terminal, network, disk, memory, process list.
- macOS: Spotlight, Terminal, Activity Monitor, copy, paste.
- Android TV: D-pad, OK, Back, Home, Search, volume, mute, and play/pause.

## Release Notes

- Mention the firmware version.
- Mention board targets tested.
- Mention any known device or browser limitations.
- Mention whether AP password or control PIN behavior changed.
