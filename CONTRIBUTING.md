# Contributing To RemoteDeck

Thanks for helping improve RemoteDeck.

## Good First Areas

- Add and test IT-focused macros.
- Improve tools that run from the ESP32.
- Test more ESP32-S3 boards with native USB.
- Improve Android TV, Linux, and macOS profile coverage.
- Improve docs and screenshots.

## Before Opening A Pull Request

Run:

```powershell
node tools/validate-remotedeck-project.js
node tools/validate-embedded-ui.js
arduino-cli compile --fqbn esp32:esp32:adafruit_qtpy_esp32s3_nopsram RemoteDeck
```

If you cannot run the Arduino compile, say so in the pull request and include what you did test.

## Firmware Notes

- Keep private credentials out of commits.
- Use `RemoteDeck/remotedeck_secrets.h` for local AP password and optional control PIN overrides.
- Keep the embedded UI self-contained in `RemoteDeck/index_html.h`.
- Prefer profile-specific actions over fragile one-size-fits-all shortcuts.

## Safety Notes

RemoteDeck can send keystrokes and mouse actions to a connected host. Treat new macros like automation scripts: keep them clear, intentional, and easy to cancel or understand.
