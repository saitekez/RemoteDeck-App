#!/usr/bin/env node

const fs = require("fs");
const path = require("path");

const projectRoot = path.resolve(__dirname, "..");
const uiPath = path.join(projectRoot, "RemoteDeck", "index_html.h");
const source = fs.readFileSync(uiPath, "utf8");
const scriptMatch = source.match(/<script>([\s\S]*?)<\/script>/);

if (!scriptMatch) {
  throw new Error("RemoteDeck embedded UI script block was not found.");
}

new Function(scriptMatch[1]);

class FakeClassList {
  constructor() {
    this.values = new Set();
  }

  add(...names) {
    names.forEach((name) => this.values.add(name));
  }

  remove(...names) {
    names.forEach((name) => this.values.delete(name));
  }

  toggle(name, force) {
    const enabled = force === undefined ? !this.values.has(name) : force;

    if (enabled) {
      this.add(name);
    } else {
      this.remove(name);
    }
  }
}

class FakeElement {
  constructor(id = "") {
    this.id = id;
    this.value = "";
    this.textContent = "";
    this.className = "";
    this.hidden = false;
    this.children = [];
    this.style = {};
    this.classList = new FakeClassList();
  }

  appendChild(child) {
    this.children.push(child);
    return child;
  }

  setAttribute() {}
  addEventListener() {}
  removeEventListener() {}
}

const requiredIds = [
  "app",
  "leftNav",
  "centerNav",
  "rightNav",
  "statusBar",
  "securityBar",
  "controlPinInput",
  "controlPinButton",
  "securityState",
  "profileWindows",
  "profileLinux",
  "profileMacos",
  "profileAndroidTv",
  "deckPage",
  "toolsPage",
  "macrosPage",
  "settingsPage",
  "touchpad",
  "quickGrid",
  "keyboardGrid",
  "toolStatus",
  "toolInputPanel",
  "toolHostInput",
  "toolPortInput",
  "toolRunButton",
  "ipInfoResults",
  "wifiName",
  "ipAddress",
  "apAddress",
  "hostName",
  "gatewayAddress",
  "wifiRSSI",
  "publicIpResults",
  "publicIpAddress",
  "publicIsp",
  "publicCity",
  "publicRegion",
  "publicCountry",
  "networkResults",
  "macroContent",
  "themeValue",
  "themeDeep",
  "themeSlate",
  "themeContrast",
  "themeMatrix",
  "pointerValue",
  "mouseSensitivityInput",
  "mouseSensitivityValue",
  "mouseAccelerationInput",
  "mouseAccelerationValue",
  "tapToClickButton",
  "naturalScrollButton",
  "startupValue",
  "startupDeck",
  "startupTools",
  "startupMacros",
  "startupSettings",
  "adminConfirmButton",
  "httpValue",
  "httpSettingStatus",
  "httpToggleButton",
  "resetValue"
];

const elements = Object.fromEntries(
  requiredIds.map((id) => [id, new FakeElement(id)])
);

elements.mouseSensitivityInput.value = "100";
elements.mouseAccelerationInput.value = "65";

global.document = {
  body: new FakeElement("body"),
  getElementById(id) {
    if (!elements[id]) {
      elements[id] = new FakeElement(id);
    }

    return elements[id];
  },
  createElement(tag) {
    return new FakeElement(tag);
  },
  querySelector() {
    return null;
  }
};

global.window = {
  localStorage: {
    getItem() {
      return null;
    },
    setItem() {},
    removeItem() {}
  },
  navigator: {
    userAgent: "Mozilla/5.0 (iPhone)",
    platform: "iPhone",
    maxTouchPoints: 5
  },
  location: {
    hostname: "192.168.4.1"
  },
  confirm() {
    return true;
  },
  addEventListener(event, callback) {
    if (event === "load") {
      callback();
    }
  }
};

global.WebSocket = function WebSocket() {
  this.readyState = 0;
};
global.WebSocket.OPEN = 1;
global.setTimeout = () => 0;
global.clearTimeout = () => {};

new Function(scriptMatch[1])();

console.log("RemoteDeck embedded UI validation passed.");
