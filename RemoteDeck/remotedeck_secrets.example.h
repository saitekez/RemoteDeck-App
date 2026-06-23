/*
 * Optional private RemoteDeck settings.
 *
 * To use:
 * 1. Copy this file to remotedeck_secrets.h in the same folder.
 * 2. Change the values below.
 * 3. Keep remotedeck_secrets.h private. It is ignored by git.
 */

#pragma once

// WPA2 password for the RemoteDeck WiFi access point. Use 8-63 characters.
#define REMOTEDECK_AP_PASSWORD "ChangeMe123"

// Optional browser control PIN. Leave blank to disable the extra web unlock.
#define REMOTEDECK_CONTROL_PIN ""
