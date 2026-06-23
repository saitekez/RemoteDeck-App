# Security Policy

## Supported Version

RemoteDeck is alpha software. Security fixes should target the current `main` branch unless a release branch exists.

## Reporting A Vulnerability

Please do not publish a working exploit for a RemoteDeck security issue before it can be fixed.

Open a private GitHub security advisory if available, or contact the project maintainer directly. Include:

- Firmware version.
- Board model.
- Whether a custom `remotedeck_secrets.h` was used.
- Steps to reproduce.
- Expected impact.

## Security Model

RemoteDeck serves a local HTTP page from the ESP32. Browser-trusted HTTPS is not practical for the default self-hosted hotspot at `192.168.4.1`.

The intended field security model is:

- WPA2-protected RemoteDeck access point.
- Private AP password changed before deployment.
- Optional browser control PIN for HID actions.
- Local-only control surface.
- Physical access awareness, because USB HID devices can type and click on the connected host.
