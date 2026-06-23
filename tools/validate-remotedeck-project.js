#!/usr/bin/env node
"use strict";

const fs = require("fs");
const path = require("path");

const args = process.argv.slice(2);
const jsonMode = args.includes("--json");
const manifestArg = args.find((arg) => !arg.startsWith("--")) || "remotedeck.project.json";
const root = path.dirname(path.resolve(manifestArg));
const manifestPath = path.resolve(manifestArg);

const errors = [];
const warnings = [];
const info = [];

const MAX_MANIFEST_BYTES = 1024 * 1024;
const MAX_TEXT_FILE_BYTES = 1024 * 1024;

const ISSUE_GUIDE = {
  INVALID_JSON: {
    title: "The project manifest is not valid JSON",
    fix: "Open remotedeck.project.json, fix the JSON syntax, and try the import again."
  },
  UNSUPPORTED_SCHEMA: {
    title: "This Studio project schema is not supported",
    fix: "Use schema remotedeck.project.v1 or update Studio to a version that supports this schema."
  },
  MISSING_FILE: {
    title: "A required project file is missing",
    fix: "Put the missing file back in the project folder or update the manifest to point to the right file."
  },
  PATH_TRAVERSAL: {
    title: "A manifest path points outside the project folder",
    fix: "Use a relative path that stays inside this RemoteDeck project folder."
  },
  ABSOLUTE_PATH: {
    title: "A manifest path is absolute",
    fix: "Use project-relative paths so the import works on other machines."
  },
  SYMLINK_NOT_ALLOWED: {
    title: "A referenced file is a link",
    fix: "Replace the linked file with a normal file inside the project folder before importing."
  },
  UI_NOT_INCLUDED: {
    title: "The firmware does not include the declared UI file",
    fix: "Add the declared UI header include to the firmware or correct firmware.embeddedUi in the manifest."
  },
  TOOL_NOT_HANDLED: {
    title: "A declared tool is missing a firmware handler",
    fix: "Add a matching tool handler in RemoteDeck/RemoteDeck.ino or remove the tool action from the manifest."
  },
  SHORTCUT_NOT_HANDLED: {
    title: "A declared macro is missing a firmware handler",
    fix: "Add a matching shortcut handler in RemoteDeck/RemoteDeck.ino or remove the action from the manifest profile."
  },
  UI_SHORTCUT_NOT_DECLARED: {
    title: "The UI sends a macro that is not declared in the manifest",
    fix: "Add the action to webSocketContract.shortcutProfiles or remove it from the UI."
  },
  UI_TOOL_NOT_DECLARED: {
    title: "The UI sends a tool action that is not declared in the manifest",
    fix: "Add the action to webSocketContract.toolActions or remove it from the UI."
  },
  MISSING_CORE_UI: {
    title: "The embedded UI is missing a core control area",
    fix: "Make sure index_html.h contains both keyboardGrid and touchpad elements."
  },
  PROFILE_NOT_SENT: {
    title: "The UI does not send the selected OS profile",
    fix: "Send the active profile with shortcut and tool WebSocket messages."
  },
  PORT_CONFLICT: {
    title: "Two device services use the same port",
    fix: "Use different values for HTTP, WebSocket, and captive DNS ports."
  },
  DEVICE_NAME_MISMATCH: {
    title: "Manifest device name does not match firmware",
    fix: "Update remotedeck.project.json or the firmware DEVICE_NAME constant so they match."
  },
  MDNS_NAME_MISMATCH: {
    title: "Manifest mDNS name does not match firmware",
    fix: "Update remotedeck.project.json or the firmware MDNS_NAME constant so they match."
  },
  HTTP_PORT_MISMATCH: {
    title: "Manifest HTTP port does not match firmware",
    fix: "Update the manifest or the WebServer port in RemoteDeck/RemoteDeck.ino."
  },
  WEBSOCKET_PORT_MISMATCH: {
    title: "Manifest WebSocket port does not match firmware",
    fix: "Update the manifest or the WebSocketsServer port in RemoteDeck/RemoteDeck.ino."
  },
  DNS_PORT_MISMATCH: {
    title: "Manifest captive DNS port does not match firmware",
    fix: "Update the manifest or DNS_PORT in RemoteDeck/RemoteDeck.ino."
  },
  MANIFEST_LIBRARY_MISSING: {
    title: "Firmware uses a library that is not listed in the manifest",
    fix: "Add the library to firmware.libraries so Studio can install or check it."
  },
  INVALID_SAFE_REMOVAL: {
    title: "Rollback removal policy is unsafe",
    fix: "Use safeRemoval: studio-workspace-only so rollback cannot touch user folders."
  },
  INVALID_ROLLBACK_STRATEGY: {
    title: "Rollback strategy is not supported",
    fix: "Use strategy: transactional-workspace."
  }
};

function guideFor(code) {
  return ISSUE_GUIDE[code] || {
    title: code.replace(/_/g, " ").toLowerCase().replace(/^\w/, (char) => char.toUpperCase()),
    fix: "Review the referenced field or file, correct it, and run validation again."
  };
}

function makeIssue(severity, code, message, options = {}) {
  const guide = guideFor(code);
  return {
    severity,
    code,
    title: options.title || guide.title,
    message,
    location: options.location || undefined,
    value: options.value,
    fix: options.fix || guide.fix,
    detail: options.detail || options.hint || undefined
  };
}

function addError(code, message, hintOrOptions) {
  const options = typeof hintOrOptions === "object" && hintOrOptions !== null
    ? hintOrOptions
    : { hint: hintOrOptions };
  errors.push(makeIssue("error", code, message, options));
}

function addWarning(code, message, hintOrOptions) {
  const options = typeof hintOrOptions === "object" && hintOrOptions !== null
    ? hintOrOptions
    : { hint: hintOrOptions };
  warnings.push(makeIssue("warning", code, message, options));
}

function addInfo(code, message, options = {}) {
  info.push({
    severity: "info",
    code,
    title: options.title || code.replace(/_/g, " ").toLowerCase(),
    message,
    location: options.location || undefined
  });
}

function displayPath(filePath) {
  if (!filePath) {
    return "";
  }
  const relative = path.relative(root, filePath);
  return relative && !relative.startsWith("..") && !path.isAbsolute(relative)
    ? relative
    : filePath;
}

function readText(filePath, codeName) {
  try {
    return fs.readFileSync(filePath, "utf8");
  } catch (err) {
    addError(codeName, `Unable to read ${displayPath(filePath)}.`, {
      location: displayPath(filePath),
      detail: err.message
    });
    return "";
  }
}

function parseJson(filePath) {
  try {
    if (fs.existsSync(filePath) && fs.statSync(filePath).size > MAX_MANIFEST_BYTES) {
      addError("MANIFEST_TOO_LARGE", "Project manifest is too large to import safely.", {
        location: displayPath(filePath),
        value: fs.statSync(filePath).size,
        fix: "Keep remotedeck.project.json under 1 MB and move large data into normal project files."
      });
      return null;
    }
    return JSON.parse(readText(filePath, "READ_MANIFEST"));
  } catch (err) {
    addError("INVALID_JSON", "Project manifest is not valid JSON.", {
      location: displayPath(filePath),
      detail: err.message
    });
    return null;
  }
}

function isPlainObject(value) {
  return value !== null && typeof value === "object" && !Array.isArray(value);
}

function requireString(obj, key, location) {
  if (typeof obj?.[key] !== "string" || obj[key].trim() === "") {
    addError("REQUIRED_STRING", `${location}.${key} must be a non-empty string.`, {
      location: `${location}.${key}`
    });
    return "";
  }
  return obj[key];
}

function requireBoolean(obj, key, location) {
  if (typeof obj?.[key] !== "boolean") {
    addError("REQUIRED_BOOLEAN", `${location}.${key} must be true or false.`, {
      location: `${location}.${key}`
    });
    return false;
  }
  return obj[key];
}

function requirePort(obj, key, location) {
  const value = obj?.[key];
  if (!Number.isInteger(value) || value < 1 || value > 65535) {
    addError("INVALID_PORT", `${location}.${key} must be a port from 1-65535.`, {
      location: `${location}.${key}`,
      value
    });
    return 0;
  }
  return value;
}

function requireStringArray(obj, key, location) {
  const value = obj?.[key];
  if (!Array.isArray(value) || value.length === 0 || value.some((item) => typeof item !== "string" || item.trim() === "")) {
    addError("REQUIRED_STRING_ARRAY", `${location}.${key} must be a non-empty array of strings.`, {
      location: `${location}.${key}`
    });
    return [];
  }

  const duplicates = value.filter((item, index) => value.indexOf(item) !== index);
  if (duplicates.length > 0) {
    addError("DUPLICATE_VALUES", `${location}.${key} contains duplicates: ${[...new Set(duplicates)].join(", ")}.`, {
      location: `${location}.${key}`,
      value: [...new Set(duplicates)]
    });
  }

  return value;
}

function checkAllowedKeys(obj, allowedKeys, location) {
  if (!isPlainObject(obj)) {
    return;
  }

  const extras = Object.keys(obj).filter((key) => !allowedKeys.includes(key));
  if (extras.length > 0) {
    addError("UNKNOWN_FIELD", `${location} contains unsupported field(s): ${extras.join(", ")}.`, {
      location,
      value: extras,
      fix: "Remove unsupported fields or update the schema/importer before importing."
    });
  }
}

function safeProjectPath(relativePath, location) {
  if (typeof relativePath !== "string" || relativePath.trim() === "") {
    addError("INVALID_PATH", `${location} must be a relative path.`, {
      location
    });
    return "";
  }

  if (path.isAbsolute(relativePath) || /^[A-Za-z]:/.test(relativePath)) {
    addError("ABSOLUTE_PATH", `${location} must not be absolute.`, {
      location,
      value: relativePath
    });
    return "";
  }

  const resolved = path.resolve(root, relativePath);
  const relative = path.relative(root, resolved);
  if (relative.startsWith("..") || path.isAbsolute(relative)) {
    addError("PATH_TRAVERSAL", `${location} must stay inside the project folder.`, {
      location,
      value: relativePath
    });
    return "";
  }

  return resolved;
}

function checkFile(relativePath, location) {
  const resolved = safeProjectPath(relativePath, location);
  if (!resolved) {
    return "";
  }

  if (!fs.existsSync(resolved)) {
    addError("MISSING_FILE", `${location} points to a missing file.`, {
      location,
      value: relativePath
    });
    return resolved;
  }

  const lstat = fs.lstatSync(resolved);
  if (lstat.isSymbolicLink()) {
    addError("SYMLINK_NOT_ALLOWED", `${location} points to a symbolic link.`, {
      location,
      value: relativePath
    });
    return resolved;
  }

  const stat = fs.statSync(resolved);
  if (!stat.isFile()) {
    addError("NOT_A_FILE", `${location} must point to a file.`, {
      location,
      value: relativePath
    });
  }

  if (stat.size > MAX_TEXT_FILE_BYTES) {
    addWarning("LARGE_FILE", `${location} is larger than expected for a RemoteDeck import.`, {
      location,
      value: `${relativePath} (${stat.size} bytes)`,
      fix: "Confirm this file is intentional before importing."
    });
  }

  return resolved;
}

function extractRawLiteralHtml(headerText) {
  const match = headerText.match(/R"rawliteral\(([\s\S]*)\)rawliteral";/);
  if (!match) {
    addError("MISSING_EMBEDDED_HTML", "Embedded UI header must contain an R\"rawliteral(...)rawliteral\" HTML block.");
    return "";
  }
  return match[1];
}

function extractProfileActions(uiText) {
  const configStart = uiText.indexOf("const profileConfig");
  if (configStart < 0) {
    addError("MISSING_PROFILE_CONFIG", "UI must define const profileConfig for Studio import.");
    return { profiles: [], actions: new Set() };
  }

  const braceStart = uiText.indexOf("{", configStart);
  if (braceStart < 0) {
    addError("BROKEN_PROFILE_CONFIG", "profileConfig is missing an opening brace.");
    return { profiles: [], actions: new Set() };
  }

  let depth = 0;
  let end = -1;
  for (let i = braceStart; i < uiText.length; i++) {
    const char = uiText[i];
    if (char === "{") {
      depth++;
    } else if (char === "}") {
      depth--;
      if (depth === 0) {
        end = i + 1;
        break;
      }
    }
  }

  if (end < 0) {
    addError("BROKEN_PROFILE_CONFIG", "profileConfig is missing a closing brace.");
    return { profiles: [], actions: new Set() };
  }

  const body = uiText.slice(braceStart, end);
  const profiles = [...body.matchAll(/^\s*([a-z0-9-]+):\s*\{/gm)].map((match) => match[1]);
  const actions = new Set([...body.matchAll(/\[\s*"[^"]+"\s*,\s*"([^"]+)"\s*\]/g)].map((match) => match[1]));
  return { profiles, actions };
}

function extractToolActions(uiText) {
  return new Set([...uiText.matchAll(/requestTool\(\s*["']([^"']+)["']/g)].map((match) => match[1]));
}

function extractFirmwareActions(firmwareText) {
  return new Set([...firmwareText.matchAll(/strcmp\(action,\s*"([^"]+)"\)/g)].map((match) => match[1]));
}

function extractFirmwareConstant(firmwareText, name) {
  const match = firmwareText.match(new RegExp(`const\\s+char\\s*\\*\\s*${name}\\s*=\\s*"([^"]+)"`));
  return match ? match[1] : "";
}

function extractFirmwarePort(firmwareText, pattern) {
  const match = firmwareText.match(pattern);
  return match ? Number(match[1]) : 0;
}

function checkPortConsistency(manifest, firmwareText, html) {
  const ports = [
    ["device.httpPort", manifest.device?.httpPort],
    ["device.webSocketPort", manifest.device?.webSocketPort],
    ["device.captiveDnsPort", manifest.device?.captiveDnsPort]
  ];
  const seen = new Map();

  for (const [field, port] of ports) {
    if (!Number.isInteger(port)) {
      continue;
    }
    if (seen.has(port)) {
      addError("PORT_CONFLICT", `${field} uses port ${port}, which is already used by ${seen.get(port)}.`, {
        location: field,
        value: port
      });
    } else {
      seen.set(port, field);
    }
  }

  const firmwareHttpPort = extractFirmwarePort(firmwareText, /WebServer\s+server\((\d+)\)/);
  if (firmwareHttpPort && firmwareHttpPort !== manifest.device?.httpPort) {
    addError("HTTP_PORT_MISMATCH", `Manifest HTTP port is ${manifest.device?.httpPort}, but firmware uses ${firmwareHttpPort}.`, {
      location: "device.httpPort",
      value: manifest.device?.httpPort
    });
  }

  const firmwareWebSocketPort = extractFirmwarePort(firmwareText, /WebSocketsServer\s+webSocket\s*=\s*WebSocketsServer\((\d+)\)/);
  if (firmwareWebSocketPort && firmwareWebSocketPort !== manifest.device?.webSocketPort) {
    addError("WEBSOCKET_PORT_MISMATCH", `Manifest WebSocket port is ${manifest.device?.webSocketPort}, but firmware uses ${firmwareWebSocketPort}.`, {
      location: "device.webSocketPort",
      value: manifest.device?.webSocketPort
    });
  }

  const firmwareDnsPort = extractFirmwarePort(firmwareText, /DNS_PORT\s*=\s*(\d+)/);
  if (firmwareDnsPort && firmwareDnsPort !== manifest.device?.captiveDnsPort) {
    addError("DNS_PORT_MISMATCH", `Manifest captive DNS port is ${manifest.device?.captiveDnsPort}, but firmware uses ${firmwareDnsPort}.`, {
      location: "device.captiveDnsPort",
      value: manifest.device?.captiveDnsPort
    });
  }

  if (html && manifest.device?.webSocketPort && !html.includes(`:${manifest.device.webSocketPort}/`)) {
    addWarning("UI_WEBSOCKET_PORT_NOT_FOUND", `UI does not appear to connect to WebSocket port ${manifest.device.webSocketPort}.`, {
      location: "ui.source",
      fix: "Confirm the WebSocket URL in index_html.h uses the manifest WebSocket port."
    });
  }
}

function checkDeviceIdentity(manifest, firmwareText) {
  const firmwareDeviceName = extractFirmwareConstant(firmwareText, "DEVICE_NAME");
  if (firmwareDeviceName && firmwareDeviceName !== manifest.device?.wifiAccessPoint) {
    addError("DEVICE_NAME_MISMATCH", `Manifest WiFi access point is '${manifest.device?.wifiAccessPoint}', but firmware uses '${firmwareDeviceName}'.`, {
      location: "device.wifiAccessPoint",
      value: manifest.device?.wifiAccessPoint
    });
  }

  const firmwareMdnsName = extractFirmwareConstant(firmwareText, "MDNS_NAME");
  if (firmwareMdnsName && firmwareMdnsName !== manifest.device?.mdnsName) {
    addError("MDNS_NAME_MISMATCH", `Manifest mDNS name is '${manifest.device?.mdnsName}', but firmware uses '${firmwareMdnsName}'.`, {
      location: "device.mdnsName",
      value: manifest.device?.mdnsName
    });
  }

  if (manifest.device?.wifiAccessPoint && Buffer.byteLength(manifest.device.wifiAccessPoint, "utf8") > 32) {
    addError("SSID_TOO_LONG", "WiFi access point name must be 32 bytes or fewer.", {
      location: "device.wifiAccessPoint",
      value: manifest.device.wifiAccessPoint
    });
  }
}

function checkLibraryManifest(manifest, firmwareText) {
  const libraries = new Set(manifest.firmware?.libraries || []);
  const requiredLibraries = [
    ["WebSockets", /#include\s+<WebSocketsServer\.h>/],
    ["ArduinoJson", /#include\s+<ArduinoJson\.h>/],
    ["WiFiManager", /#include\s+<WiFiManager\.h>/]
  ];

  for (const [library, includePattern] of requiredLibraries) {
    if (includePattern.test(firmwareText) && !libraries.has(library)) {
      addError("MANIFEST_LIBRARY_MISSING", `Firmware includes ${library}, but firmware.libraries does not list it.`, {
        location: "firmware.libraries",
        value: library
      });
    }
  }
}

function checkUiDocument(html) {
  if (!html) {
    return;
  }

  const ids = [...html.matchAll(/\bid="([^"]+)"/g)].map((match) => match[1]);
  const duplicates = ids.filter((id, index) => ids.indexOf(id) !== index);
  if (duplicates.length > 0) {
    addError("DUPLICATE_UI_IDS", `Embedded UI contains duplicate id(s): ${[...new Set(duplicates)].join(", ")}.`, {
      location: "ui.source",
      value: [...new Set(duplicates)],
      fix: "Make every HTML id unique so buttons and result panels target the right elements."
    });
  }

  const requiredIds = [
    "profileWindows",
    "profileLinux",
    "profileMacos",
    "quickGrid",
    "keyboardGrid",
    "touchpad",
    "toolStatus",
    "networkResults",
    "macroContent"
  ];
  const missingIds = requiredIds.filter((id) => !ids.includes(id));
  if (missingIds.length > 0) {
    addError("MISSING_UI_ID", `Embedded UI is missing required id(s): ${missingIds.join(", ")}.`, {
      location: "ui.source",
      value: missingIds,
      fix: "Restore the missing UI elements before importing."
    });
  }
}

function validateManifestShape(manifest) {
  if (!isPlainObject(manifest)) {
    addError("INVALID_MANIFEST", "Project manifest must be a JSON object.");
    return;
  }

    checkAllowedKeys(manifest, ["schema", "name", "description", "device", "firmware", "ui", "webSocketContract", "docs", "studioImport"], "manifest");

  if (manifest.schema !== "remotedeck.project.v1") {
    addError("UNSUPPORTED_SCHEMA", "Project schema must be remotedeck.project.v1.", {
      location: "schema",
      value: manifest.schema,
      detail: "Importers should refuse unknown schemas unless a migration is available."
    });
  }

  requireString(manifest, "name", "manifest");
  requireString(manifest, "description", "manifest");

  for (const section of ["device", "firmware", "ui", "webSocketContract", "docs"]) {
    if (!isPlainObject(manifest[section])) {
      addError("MISSING_SECTION", `manifest.${section} must be present.`);
    }
  }

  if (isPlainObject(manifest.device)) {
    checkAllowedKeys(manifest.device, ["family", "recommendedBoard", "requiresNativeUsb", "wifiAccessPoint", "mdnsName", "httpPort", "webSocketPort", "captiveDnsPort"], "device");
    requireString(manifest.device, "family", "device");
    requireString(manifest.device, "recommendedBoard", "device");
    requireBoolean(manifest.device, "requiresNativeUsb", "device");
    requireString(manifest.device, "wifiAccessPoint", "device");
    requireString(manifest.device, "mdnsName", "device");
    requirePort(manifest.device, "httpPort", "device");
    requirePort(manifest.device, "webSocketPort", "device");
    requirePort(manifest.device, "captiveDnsPort", "device");
  }

  if (isPlainObject(manifest.firmware)) {
    checkAllowedKeys(manifest.firmware, ["entry", "embeddedUi", "framework", "libraries"], "firmware");
    requireString(manifest.firmware, "entry", "firmware");
    requireString(manifest.firmware, "embeddedUi", "firmware");
    if (manifest.firmware.framework !== "arduino") {
      addError("UNSUPPORTED_FRAMEWORK", "firmware.framework must be arduino for this importer.", {
        location: "firmware.framework",
        value: manifest.firmware.framework
      });
    }
    requireStringArray(manifest.firmware, "libraries", "firmware");
  }

  if (isPlainObject(manifest.ui)) {
    checkAllowedKeys(manifest.ui, ["type", "source", "profiles"], "ui");
    if (manifest.ui.type !== "embedded-html-header") {
      addError("UNSUPPORTED_UI_TYPE", "ui.type must be embedded-html-header for this importer.", {
        location: "ui.type",
        value: manifest.ui.type
      });
    }
    requireString(manifest.ui, "source", "ui");
    requireStringArray(manifest.ui, "profiles", "ui");
  }

  if (isPlainObject(manifest.webSocketContract)) {
    checkAllowedKeys(manifest.webSocketContract, ["clientEvents", "toolActions", "shortcutProfiles"], "webSocketContract");
    requireStringArray(manifest.webSocketContract, "clientEvents", "webSocketContract");
    requireStringArray(manifest.webSocketContract, "toolActions", "webSocketContract");
    if (!isPlainObject(manifest.webSocketContract.shortcutProfiles)) {
      addError("MISSING_SHORTCUT_PROFILES", "webSocketContract.shortcutProfiles must be an object keyed by profile.", {
        location: "webSocketContract.shortcutProfiles"
      });
    } else {
      for (const [profile, actions] of Object.entries(manifest.webSocketContract.shortcutProfiles)) {
        if (!/^[a-z0-9-]+$/.test(profile)) {
          addError("INVALID_PROFILE_NAME", `Profile name '${profile}' must use lowercase letters, numbers, or hyphens.`, {
            location: `webSocketContract.shortcutProfiles.${profile}`,
            value: profile
          });
        }
        requireStringArray({ actions }, "actions", `webSocketContract.shortcutProfiles.${profile}`);
      }
    }
  }

  if (isPlainObject(manifest.docs)) {
    checkAllowedKeys(manifest.docs, ["readme", "studioNotes"], "docs");
    requireString(manifest.docs, "readme", "docs");
    if (manifest.docs.studioNotes !== undefined) {
      requireString(manifest.docs, "studioNotes", "docs");
    }
  }

  if (manifest.studioImport !== undefined) {
    if (!isPlainObject(manifest.studioImport)) {
      addError("INVALID_STUDIO_IMPORT", "manifest.studioImport must be an object when present.");
    } else {
      checkAllowedKeys(manifest.studioImport, ["schemaFile", "validator", "rollback", "testFixture"], "studioImport");
      for (const key of ["schemaFile", "validator"]) {
        if (manifest.studioImport[key] !== undefined) {
          requireString(manifest.studioImport, key, "studioImport");
        }
      }
      if (manifest.studioImport.testFixture !== undefined && typeof manifest.studioImport.testFixture !== "boolean") {
        addError("INVALID_TEST_FIXTURE_FLAG", "studioImport.testFixture must be true or false when present.");
      }
      if (manifest.studioImport.rollback !== undefined) {
        const rollback = manifest.studioImport.rollback;
        if (!isPlainObject(rollback)) {
          addError("INVALID_ROLLBACK_POLICY", "studioImport.rollback must be an object when present.");
        } else {
          if (rollback.strategy !== "transactional-workspace") {
            addError("INVALID_ROLLBACK_STRATEGY", "studioImport.rollback.strategy must be transactional-workspace.", {
              location: "studioImport.rollback.strategy",
              value: rollback.strategy
            });
          }
          for (const key of ["recordImportReceipt", "snapshotBeforeUpdate", "archiveBeforeDelete"]) {
            if (typeof rollback[key] !== "boolean") {
              addError("INVALID_ROLLBACK_FLAG", `studioImport.rollback.${key} must be true or false.`, {
                location: `studioImport.rollback.${key}`,
                value: rollback[key]
              });
            }
          }
          if (rollback.safeRemoval !== "studio-workspace-only") {
            addError("INVALID_SAFE_REMOVAL", "studioImport.rollback.safeRemoval must be studio-workspace-only.", {
              location: "studioImport.rollback.safeRemoval",
              value: rollback.safeRemoval
            });
          }
          if (!Number.isInteger(rollback.retainSnapshots) || rollback.retainSnapshots < 1 || rollback.retainSnapshots > 50) {
            addError("INVALID_SNAPSHOT_RETENTION", "studioImport.rollback.retainSnapshots must be from 1-50.", {
              location: "studioImport.rollback.retainSnapshots",
              value: rollback.retainSnapshots
            });
          }
        }
      }
    }
  }
}

function compareSets(expected, actual, code, label, location) {
  const missing = expected.filter((item) => !actual.has(item));
  if (missing.length > 0) {
    addError(code, `${label} missing from implementation: ${missing.join(", ")}.`, {
      location,
      value: missing
    });
  }
}

function validateProject(manifest) {
  validateManifestShape(manifest);

  if (!manifest || errors.some((error) => ["INVALID_MANIFEST", "MISSING_SECTION"].includes(error.code))) {
    return;
  }

  const firmwarePath = checkFile(manifest.firmware?.entry, "firmware.entry");
  const uiPath = checkFile(manifest.firmware?.embeddedUi, "firmware.embeddedUi");
  const uiSourcePath = checkFile(manifest.ui?.source, "ui.source");
  checkFile(manifest.docs?.readme, "docs.readme");
  if (manifest.docs?.studioNotes) {
    checkFile(manifest.docs.studioNotes, "docs.studioNotes");
  }

  if (manifest.studioImport?.schemaFile) {
    checkFile(manifest.studioImport.schemaFile, "studioImport.schemaFile");
  }
  if (manifest.studioImport?.validator) {
    checkFile(manifest.studioImport.validator, "studioImport.validator");
  }

  if (uiPath && uiSourcePath && path.resolve(uiPath) !== path.resolve(uiSourcePath)) {
    addWarning("UI_PATH_MISMATCH", "firmware.embeddedUi and ui.source point to different files.", "This is allowed only if Studio knows which source is canonical.");
  }

  const firmwareText = firmwarePath ? readText(firmwarePath, "READ_FIRMWARE") : "";
  const headerText = uiPath ? readText(uiPath, "READ_UI") : "";
  const html = extractRawLiteralHtml(headerText);

  if (firmwareText && uiPath) {
    const includeName = path.basename(uiPath).replace(/[.*+?^${}()|[\]\\]/g, "\\$&");
    if (!new RegExp(`#include\\s+["<]${includeName}[">]`).test(firmwareText)) {
      addError("UI_NOT_INCLUDED", `Firmware does not include ${path.basename(uiPath)}.`, {
        location: "firmware.entry",
        value: path.basename(uiPath)
      });
    }
  }

  if (firmwareText) {
    checkDeviceIdentity(manifest, firmwareText);
    checkLibraryManifest(manifest, firmwareText);
  }

  checkPortConsistency(manifest, firmwareText, html);

  const requiredEvents = ["shortcut", "tool", "mouse_move", "mouse_left", "mouse_right", "mouse_scroll", "touch start", "touch end"];
  const clientEvents = manifest.webSocketContract?.clientEvents || [];
  for (const event of requiredEvents) {
    if (!clientEvents.includes(event)) {
      addError("MISSING_CLIENT_EVENT", `webSocketContract.clientEvents should include '${event}'.`, {
        location: "webSocketContract.clientEvents",
        value: event
      });
    }
  }

  const manifestProfiles = manifest.ui?.profiles || [];
  const shortcutProfiles = Object.keys(manifest.webSocketContract?.shortcutProfiles || {});
  for (const profile of manifestProfiles) {
    if (!shortcutProfiles.includes(profile)) {
      addError("PROFILE_CONTRACT_MISSING", `ui profile '${profile}' has no shortcutProfiles entry.`, {
        location: "webSocketContract.shortcutProfiles",
        value: profile
      });
    }
  }
  for (const profile of shortcutProfiles) {
    if (!manifestProfiles.includes(profile)) {
      addWarning("UNUSED_SHORTCUT_PROFILE", `shortcut profile '${profile}' is not listed in ui.profiles.`, {
        location: "webSocketContract.shortcutProfiles",
        value: profile
      });
    }
  }

  const firmwareActions = extractFirmwareActions(firmwareText);
  const manifestToolActions = manifest.webSocketContract?.toolActions || [];
  const manifestShortcutActions = Object.values(manifest.webSocketContract?.shortcutProfiles || {}).flat();
  compareSets(manifestToolActions, firmwareActions, "TOOL_NOT_HANDLED", "Tool actions", "webSocketContract.toolActions");
  compareSets([...new Set(manifestShortcutActions)], firmwareActions, "SHORTCUT_NOT_HANDLED", "Shortcut actions", "webSocketContract.shortcutProfiles");

  if (html) {
    checkUiDocument(html);
    const uiToolActions = extractToolActions(html);
    const uiProfiles = extractProfileActions(html);
    compareSets([...uiToolActions], new Set(manifestToolActions), "UI_TOOL_NOT_DECLARED", "UI tool actions", "ui.source");
    compareSets(manifestToolActions.filter((action) => !["dnslookup", "portcheck", "wol"].includes(action)), uiToolActions, "DECLARED_TOOL_NOT_IN_UI", "Manifest tool actions", "webSocketContract.toolActions");
    compareSets(manifestProfiles, new Set(uiProfiles.profiles), "PROFILE_NOT_IN_UI", "Manifest profiles", "ui.profiles");
    compareSets([...uiProfiles.actions], new Set(manifestShortcutActions), "UI_SHORTCUT_NOT_DECLARED", "UI shortcut actions", "ui.source");

    if (!html.includes('profile: activeProfile')) {
      addError("PROFILE_NOT_SENT", "UI should send the active profile with shortcut and tool messages.", {
        location: "ui.source"
      });
    }

    if (!html.includes('id="keyboardGrid"') || !html.includes('id="touchpad"')) {
      addError("MISSING_CORE_UI", "UI must include keyboardGrid and touchpad targets.", {
        location: "ui.source"
      });
    }
  }

  if (manifest.device?.requiresNativeUsb && !/USBHIDKeyboard/.test(firmwareText)) {
    addError("MISSING_USB_HID", "Manifest requires native USB but firmware does not include USBHIDKeyboard.", {
      location: "firmware.entry"
    });
  }

  if (!/DNSServer/.test(firmwareText) || !/dnsServer\.start/.test(firmwareText)) {
    addWarning("CAPTIVE_DNS_NOT_FOUND", "Captive DNS helper was not found in firmware.", {
      location: "firmware.entry"
    });
  }

  if (!/maintainWifi/.test(firmwareText)) {
    addWarning("WIFI_WATCHDOG_NOT_FOUND", "WiFi/AP recovery helper was not found in firmware.", {
      location: "firmware.entry"
    });
  }

  addInfo("PROJECT_ROOT", root);
  addInfo("MANIFEST", path.relative(root, manifestPath) || path.basename(manifestPath), {
    location: displayPath(manifestPath)
  });
}

const manifest = parseJson(manifestPath);
if (manifest) {
  validateProject(manifest);
}

const result = {
  ok: errors.length === 0,
  errors,
  warnings,
  info
};

if (jsonMode) {
  console.log(JSON.stringify(result, null, 2));
} else {
  console.log(result.ok ? "RemoteDeck project validation passed." : "RemoteDeck project validation failed.");
  for (const item of errors) {
    console.log("");
    console.log(`ERROR ${item.code}: ${item.title}`);
    console.log(`  Problem: ${item.message}`);
    if (item.location) {
      console.log(`  Where: ${item.location}`);
    }
    if (item.value !== undefined) {
      console.log(`  Value: ${Array.isArray(item.value) ? item.value.join(", ") : item.value}`);
    }
    if (item.detail) {
      console.log(`  Detail: ${item.detail}`);
    }
    console.log(`  Fix: ${item.fix}`);
  }
  for (const item of warnings) {
    console.log("");
    console.log(`WARN ${item.code}: ${item.title}`);
    console.log(`  Note: ${item.message}`);
    if (item.location) {
      console.log(`  Where: ${item.location}`);
    }
    if (item.value !== undefined) {
      console.log(`  Value: ${Array.isArray(item.value) ? item.value.join(", ") : item.value}`);
    }
    if (item.detail) {
      console.log(`  Detail: ${item.detail}`);
    }
    console.log(`  Suggested fix: ${item.fix}`);
  }
  for (const item of info) {
    console.log(`INFO ${item.code}: ${item.message}`);
  }
}

process.exit(result.ok ? 0 : 1);
