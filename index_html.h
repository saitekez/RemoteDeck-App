static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, viewport-fit=cover">
<meta name="theme-color" content="#101010">
<meta name="mobile-web-app-capable" content="yes">
<meta name="apple-mobile-web-app-capable" content="yes">
<meta name="apple-mobile-web-app-title" content="RemoteDeck">
<title>RemoteDeck</title>
<style>
:root {
    color-scheme: dark;
    --bg: #090a0b;
    --panel: #1b1c1f;
    --panel-strong: #292b30;
    --key: #2d2f34;
    --key-active: #2f8f5b;
    --accent: #3fb950;
    --accent-blue: #58a6ff;
    --border: #3a3a3d;
    --border-bright: #5a5d65;
    --text: #f5f5f7;
    --muted: #b0b0b5;
    --danger: #f85149;
    --shadow: rgba(0, 0, 0, 0.62);
    --inner-light: rgba(255, 255, 255, 0.12);
    --key-gap: clamp(5px, 1.2vw, 8px);
    --key-height: clamp(44px, 11.3vw, 56px);
    --keyboard-max: 780px;
}

* {
    box-sizing: border-box;
    -webkit-tap-highlight-color: transparent;
}

html,
body {
    margin: 0;
    min-height: 100%;
    background:
        linear-gradient(180deg, #202124 0%, #0f1012 28%, #050506 100%);
    color: var(--text);
    font-family: Arial, Helvetica, sans-serif;
    overscroll-behavior: none;
    -webkit-text-size-adjust: 100%;
}

body {
    min-height: 100svh;
    padding:
        env(safe-area-inset-top)
        env(safe-area-inset-right)
        env(safe-area-inset-bottom)
        env(safe-area-inset-left);
    overflow-x: hidden;
}

button {
    border: 0;
    color: inherit;
    font: inherit;
    touch-action: manipulation;
    transition:
        background 120ms ease,
        border-color 120ms ease,
        box-shadow 120ms ease,
        transform 120ms ease;
}

button:focus-visible {
    outline: 3px solid var(--accent-blue);
    outline-offset: 2px;
}

[hidden] {
    display: none !important;
}

#app {
    width: min(980px, 100%);
    min-height: 100svh;
    margin: 0 auto;
    padding: clamp(8px, 2vw, 12px);
    display: grid;
    grid-template-rows: auto auto auto 1fr;
    gap: clamp(8px, 2vw, 12px);
    background:
        linear-gradient(180deg, rgba(255, 255, 255, 0.045), transparent 80px),
        linear-gradient(180deg, rgba(255, 255, 255, 0.025), rgba(0, 0, 0, 0.18));
    border-left: 1px solid rgba(255, 255, 255, 0.05);
    border-right: 1px solid rgba(255, 255, 255, 0.05);
    box-shadow:
        0 22px 55px var(--shadow),
        inset 0 1px 0 rgba(255, 255, 255, 0.08);
}

.topNav {
    display: grid;
    grid-template-columns: repeat(2, minmax(0, 1fr));
    gap: 8px;
}

#statusBar {
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 10px;
    min-height: 36px;
    color: var(--muted);
    font-size: clamp(15px, 4vw, 20px);
    font-weight: 700;
    text-align: center;
}

#statusBar::before {
    content: "";
    width: 18px;
    height: 18px;
    border-radius: 50%;
    background: var(--danger);
    box-shadow: 0 0 12px rgba(248, 81, 73, 0.65);
}

#statusBar.online {
    color: #b7f5c4;
}

#statusBar.online::before {
    background: var(--accent);
    box-shadow: 0 0 12px rgba(63, 185, 80, 0.75);
}

#statusBar.offline {
    color: #ffb8b4;
}

.profileBar {
    display: grid;
    grid-template-columns: repeat(3, minmax(0, 1fr));
    gap: 8px;
}

.profileBtn {
    min-height: 38px;
    padding: 0 8px;
    border: 1px solid var(--border);
    border-radius: 8px;
    background:
        linear-gradient(180deg, rgba(255, 255, 255, 0.08), rgba(0, 0, 0, 0.18)),
        #1a1c20;
    color: var(--muted);
    font-size: clamp(12px, 3.4vw, 15px);
    font-weight: 800;
    box-shadow:
        0 6px 12px rgba(0, 0, 0, 0.34),
        inset 0 1px 0 rgba(255, 255, 255, 0.08);
}

.profileBtn.active {
    border-color: var(--accent-blue);
    color: var(--text);
    background:
        linear-gradient(180deg, rgba(88, 166, 255, 0.28), rgba(35, 68, 105, 0.40)),
        #202733;
}

.panelBtn,
.actionBtn,
.toolBtn,
.kbKey {
    border: 1px solid var(--border-bright);
    border-radius: 8px;
    background:
        linear-gradient(180deg, rgba(255, 255, 255, 0.10), rgba(255, 255, 255, 0.015) 45%, rgba(0, 0, 0, 0.22)),
        var(--key);
    box-shadow:
        0 8px 16px rgba(0, 0, 0, 0.42),
        inset 0 1px 0 var(--inner-light),
        inset 0 -1px 0 rgba(0, 0, 0, 0.45);
    text-shadow: 0 1px 1px rgba(0, 0, 0, 0.65);
}

.panelBtn {
    min-height: clamp(54px, 11vw, 74px);
    background:
        linear-gradient(180deg, rgba(255, 255, 255, 0.22), rgba(255, 255, 255, 0.04) 48%, rgba(0, 0, 0, 0.26)),
        var(--key);
    color: var(--text);
    font-size: clamp(19px, 5vw, 25px);
    font-weight: 800;
    box-shadow:
        0 12px 20px rgba(0, 0, 0, 0.48),
        inset 0 1px 0 rgba(255, 255, 255, 0.16),
        inset 0 -1px 0 rgba(0, 0, 0, 0.55);
}

.panelBtn.active {
    border-color: var(--accent-blue);
}

.page {
    min-height: 0;
    display: none;
}

.page.active {
    display: flex;
    flex-direction: column;
    gap: 10px;
}

#touchpad {
    min-height: 178px;
    height: clamp(178px, 32svh, 330px);
    border: 2px solid #55575c;
    border-radius: 8px;
    background:
        linear-gradient(135deg, rgba(88, 166, 255, 0.13), transparent 44%),
        linear-gradient(180deg, rgba(255, 255, 255, 0.045), rgba(0, 0, 0, 0.16)),
        var(--panel);
    display: flex;
    align-items: center;
    justify-content: center;
    color: var(--muted);
    font-weight: 800;
    user-select: none;
    touch-action: none;
    box-shadow:
        inset 0 2px 12px rgba(0, 0, 0, 0.82),
        inset 0 1px 0 rgba(255, 255, 255, 0.09),
        0 10px 24px rgba(0, 0, 0, 0.45);
}

.quickGrid,
.shortcutGrid,
.toolGrid,
.macroGroupGrid {
    display: grid;
    gap: 8px;
}

.quickGrid {
    grid-template-columns: repeat(4, minmax(0, 1fr));
}

.shortcutGrid {
    grid-template-columns: repeat(4, minmax(0, 1fr));
}

.toolGrid {
    grid-template-columns: repeat(3, minmax(0, 1fr));
}

.actionBtn,
.toolBtn {
    min-height: 52px;
    padding: 10px;
    font-weight: 800;
}

.actionBtn.adminAction {
    border-color: rgba(250, 204, 21, 0.66);
    color: #fff5bd;
    background:
        linear-gradient(180deg, rgba(250, 204, 21, 0.20), rgba(0, 0, 0, 0.16)),
        var(--key);
}

.toolBtn {
    min-height: 72px;
}

.macroGroup {
    display: flex;
    flex-direction: column;
    gap: 8px;
}

.sectionTitle {
    margin: 4px 2px 0;
    color: var(--muted);
    font-size: 13px;
    font-weight: 800;
    text-transform: uppercase;
}

.macroGroupGrid {
    grid-template-columns: repeat(3, minmax(0, 1fr));
}

#toolsPage.active {
    display: grid;
    grid-template-rows: minmax(180px, auto) 1fr;
    gap: 10px;
}

#macrosPage.active {
    overflow-y: auto;
    padding-bottom: 4px;
}

.actionBtn:active,
.toolBtn:active,
.kbKey.pressed {
    transform: scale(0.98);
    background: var(--key-active);
}

.keyboard {
    width: min(100%, var(--keyboard-max));
    margin: 0 auto;
    display: flex;
    flex-direction: column;
    gap: var(--key-gap);
    padding: clamp(8px, 2vw, 12px);
    border: 1px solid var(--border);
    border-radius: 8px;
    background: var(--panel);
    user-select: none;
    -webkit-user-select: none;
}

.kbRow {
    width: 100%;
    display: flex;
    justify-content: center;
    gap: var(--key-gap);
}

.kbRow.homeRow {
    padding-left: 5%;
    padding-right: 5%;
}

.kbRow.shiftRow {
    padding-left: 0;
    padding-right: 0;
}

.kbRow.bottomRow {
    padding-top: 2px;
}

.kbKey {
    height: var(--key-height);
    min-width: 0;
    flex: 1 1 0;
    padding: 0 6px;
    font-size: clamp(16px, 4.6vw, 22px);
    font-weight: 800;
    line-height: 1;
    white-space: nowrap;
    text-align: center;
    letter-spacing: 0;
    touch-action: none;
}

.kbKey.wide {
    flex: 1.35 1 0;
    font-size: clamp(13px, 3.5vw, 16px);
}

.kbKey.systemKey,
.kbKey.modeKey,
.kbKey.returnKey {
    background: #46474b;
    color: var(--text);
}

.kbKey.space {
    flex: 5.4 1 0;
    font-size: clamp(13px, 3.5vw, 16px);
}

.kbKey.returnKey {
    flex: 2 1 0;
}

.kbKey.modeKey {
    flex: 1.35 1 0;
}

.kbKey.shiftActive {
    border-color: var(--accent-blue);
    background: #28527a;
}

.kbKey.capsLockActive {
    border-color: var(--accent);
    background: #27633c;
}

.resultPanel {
    min-height: 174px;
    padding: 12px;
    border: 1px solid var(--border-bright);
    border-radius: 8px;
    background:
        linear-gradient(180deg, rgba(255, 255, 255, 0.055), rgba(255, 255, 255, 0.01)),
        var(--panel);
    box-shadow:
        0 10px 22px rgba(0, 0, 0, 0.38),
        inset 0 1px 0 rgba(255, 255, 255, 0.08);
}

#toolStatus {
    margin: 0 0 10px;
    color: var(--muted);
    font-weight: 800;
}

.resultGrid {
    display: grid;
    grid-template-columns: 130px 1fr;
    gap: 8px 10px;
    align-items: center;
}

.resultGrid dt {
    color: var(--muted);
    font-weight: 800;
}

.resultGrid dd {
    margin: 0;
    min-width: 0;
    overflow-wrap: anywhere;
}

.toolInputPanel {
    display: grid;
    grid-template-columns: minmax(0, 1fr) 88px;
    gap: 8px;
    margin-bottom: 10px;
}

.toolInputPanel.twoFields {
    grid-template-columns: minmax(0, 1fr) 82px 88px;
}

.toolInput {
    width: 100%;
    min-height: 42px;
    border: 1px solid var(--border-bright);
    border-radius: 8px;
    background:
        linear-gradient(180deg, rgba(255, 255, 255, 0.055), rgba(0, 0, 0, 0.12)),
        #101114;
    color: var(--text);
    padding: 0 10px;
    font: inherit;
    font-weight: 700;
    box-shadow:
        inset 0 2px 8px rgba(0, 0, 0, 0.55),
        inset 0 1px 0 rgba(255, 255, 255, 0.06);
}

.toolRunBtn {
    min-height: 42px;
    padding: 0 10px;
}

#networkResults {
    width: 100%;
    min-height: 104px;
    max-height: 34svh;
    margin: 0;
    padding: 10px;
    border: 1px solid var(--border);
    border-radius: 8px;
    background: #0b0f14;
    color: var(--text);
    white-space: pre-wrap;
    overflow: auto;
}

@media (max-width: 720px) {
    #app {
        padding: 8px;
    }

    .shortcutGrid {
        grid-template-columns: repeat(2, minmax(0, 1fr));
    }

    .quickGrid {
        grid-template-columns: repeat(4, minmax(0, 1fr));
    }

    .quickGrid .actionBtn {
        min-height: 46px;
        padding: 6px 4px;
        font-size: 12px;
    }

    .profileBtn {
        min-height: 34px;
        font-size: 12px;
    }

    .toolGrid {
        grid-template-columns: repeat(2, minmax(0, 1fr));
    }

    .toolBtn {
        min-height: 56px;
        padding: 8px;
        font-size: 14px;
    }

    .macroGroupGrid {
        grid-template-columns: repeat(2, minmax(0, 1fr));
    }

    .shortcutGrid .actionBtn,
    .macroGroupGrid .actionBtn {
        min-height: 48px;
        padding: 7px;
        font-size: 13px;
    }

    .kbKey {
        padding: 0 3px;
    }

    .resultGrid {
        grid-template-columns: 92px 1fr;
    }

    .resultPanel {
        min-height: 150px;
    }

    .toolInputPanel,
    .toolInputPanel.twoFields {
        grid-template-columns: 1fr;
    }

    #networkResults {
        min-height: 82px;
        max-height: 28svh;
    }
}

@media (orientation: landscape) and (max-height: 520px) {
    :root {
        --key-gap: clamp(4px, 1vw, 7px);
        --key-height: clamp(34px, 11svh, 46px);
        --keyboard-max: 980px;
    }

    #app {
        min-height: 100svh;
        gap: 6px;
        padding: 6px;
    }

    #statusBar {
        min-height: 28px;
        font-size: 13px;
    }

    .profileBar {
        gap: 6px;
    }

    .profileBtn {
        min-height: 30px;
        font-size: 12px;
    }

    .panelBtn {
        min-height: 38px;
        font-size: 14px;
    }

    #deckPage.active {
        display: grid;
        grid-template-columns: minmax(220px, 0.85fr) minmax(390px, 1.15fr);
        grid-template-rows: minmax(0, 1fr) auto;
        grid-template-areas:
            "touchpad keyboard"
            "quick keyboard";
        align-items: stretch;
        gap: 6px;
    }

    #touchpad {
        grid-area: touchpad;
        min-height: 0;
        height: auto;
    }

    #deckPage .quickGrid {
        grid-area: quick;
        grid-template-columns: repeat(4, minmax(0, 1fr));
    }

    #deckPage .keyboard {
        grid-area: keyboard;
        align-self: end;
        width: 100%;
    }

    .actionBtn,
    .toolBtn {
        min-height: 44px;
        padding: 7px;
        font-size: 14px;
    }

    .keyboard {
        padding: 7px;
    }

    .kbKey {
        font-size: clamp(15px, 5.2svh, 20px);
    }

    .kbKey.wide,
    .kbKey.space,
    .kbKey.returnKey,
    .kbKey.modeKey {
        font-size: clamp(12px, 3.7svh, 15px);
    }
}
</style>
</head>
<body>
<div id="app">
    <nav class="topNav" aria-label="RemoteDeck pages">
        <button id="leftNav" class="panelBtn" type="button" onclick="navLeft()">Tools</button>
        <button id="rightNav" class="panelBtn" type="button" onclick="navRight()">Macros</button>
    </nav>

    <div id="statusBar" class="offline">Disconnected</div>

    <div class="profileBar" aria-label="Target operating system">
        <button id="profileWindows" class="profileBtn active" type="button" onclick="setProfile('windows')">Windows</button>
        <button id="profileLinux" class="profileBtn" type="button" onclick="setProfile('linux')">Linux</button>
        <button id="profileMacos" class="profileBtn" type="button" onclick="setProfile('macos')">macOS</button>
    </div>

    <main id="deckPage" class="page active">
        <div id="touchpad" aria-label="Touchpad">Touchpad</div>

        <div id="quickGrid" class="quickGrid"></div>

        <div id="keyboardGrid" class="keyboard"></div>
    </main>

    <section id="toolsPage" class="page">
        <section class="resultPanel" aria-live="polite">
            <p id="toolStatus">Choose a tool.</p>

            <div id="toolInputPanel" class="toolInputPanel" hidden>
                <input id="toolHostInput" class="toolInput" type="text" autocapitalize="none" autocomplete="off" spellcheck="false">
                <input id="toolPortInput" class="toolInput" type="number" inputmode="numeric" min="1" max="65535" hidden>
                <button id="toolRunButton" class="actionBtn toolRunBtn" type="button" onclick="runSelectedTool()">Run</button>
            </div>

            <dl id="ipInfoResults" class="resultGrid" hidden>
                <dt>WiFi</dt><dd id="wifiName">---</dd>
                <dt>IP</dt><dd id="ipAddress">---</dd>
                <dt>AP IP</dt><dd id="apAddress">---</dd>
                <dt>Hostname</dt><dd id="hostName">---</dd>
                <dt>Gateway</dt><dd id="gatewayAddress">---</dd>
                <dt>Signal</dt><dd id="wifiRSSI">---</dd>
            </dl>

            <dl id="publicIpResults" class="resultGrid" hidden>
                <dt>Public IP</dt><dd id="publicIpAddress">---</dd>
                <dt>ISP</dt><dd id="publicIsp">---</dd>
                <dt>City</dt><dd id="publicCity">---</dd>
                <dt>Region</dt><dd id="publicRegion">---</dd>
                <dt>Country</dt><dd id="publicCountry">---</dd>
            </dl>

            <pre id="networkResults" hidden>No snapshot yet.</pre>
        </section>

        <div class="toolGrid">
            <button class="toolBtn" type="button" onclick="requestTool('devicehealth')">Device Health</button>
            <button class="toolBtn" type="button" onclick="requestTool('ipinfo')">IP Info</button>
            <button class="toolBtn" type="button" onclick="requestTool('publicip')">Public IP</button>
            <button class="toolBtn" type="button" onclick="requestTool('networkscan')">Network Snapshot</button>
            <button class="toolBtn" type="button" onclick="requestTool('ticketinfo')">Ticket Info</button>
            <button class="toolBtn" type="button" onclick="requestTool('pcspecs')">System Specs</button>
            <button class="toolBtn" type="button" onclick="runDnsLookup()">DNS Lookup</button>
            <button class="toolBtn" type="button" onclick="runPortCheck()">Port Check</button>
            <button class="toolBtn" type="button" onclick="runWakeOnLan()">Wake LAN</button>
        </div>
    </section>

    <section id="macrosPage" class="page">
        <div id="macroContent" class="macroGroup"></div>
    </section>
</div>

<script>
const alphaRows = [
    { className: "topRow", keys: ["q", "w", "e", "r", "t", "y", "u", "i", "o", "p"] },
    { className: "homeRow", keys: ["a", "s", "d", "f", "g", "h", "j", "k", "l"] },
    { className: "shiftRow", keys: ["SHIFT", "z", "x", "c", "v", "b", "n", "m", "BKSP"] },
    { className: "bottomRow", keys: ["123", "SPACE", "ENTER"] }
];

const symbolRows = [
    { className: "topRow", keys: ["1", "2", "3", "4", "5", "6", "7", "8", "9", "0"] },
    { className: "topRow", keys: ["-", "/", ":", ";", "(", ")", "$", "&", "@", "\""] },
    { className: "shiftRow", keys: ["#+=", ".", ",", "?", "!", "'", "BKSP"] },
    { className: "bottomRow", keys: ["ABC", "SPACE", "ENTER"] }
];

const alternateSymbolRows = [
    { className: "topRow", keys: ["[", "]", "{", "}", "#", "%", "^", "*", "+", "="] },
    { className: "topRow", keys: ["_", "\\", "|", "~", "<", ">", "`"] },
    { className: "shiftRow", keys: ["#+=", ".", ",", "?", "!", "'", "BKSP"] },
    { className: "bottomRow", keys: ["ABC", "SPACE", "ENTER"] }
];

let websock;
let connected = false;
let shiftEnabled = false;
let numberMode = false;
let symbolAltMode = false;
let capsLock = false;
let lastShiftTap = 0;
let reconnectTimer = 0;
let currentPage = "deck";
let previousPage = "deck";
let selectedTool = "";
let activeProfile = "windows";
let statusRestoreTimer = 0;

const adminActions = new Set([
    "cmdadmin",
    "powershelladmin",
    "sfcscan",
    "dismrepair",
    "flushdns",
    "winsockreset",
    "gpupdate"
]);

const profileConfig = {
    windows: {
        buttonId: "profileWindows",
        quick: [
            ["Copy", "copy"],
            ["Paste", "paste"],
            ["Select All", "selectall"],
            ["Ctrl Alt Del", "cad"],
            ["Win X", "winx"],
            ["Lock", "lock"],
            ["TaskMgr", "taskmgr"],
            ["CMD", "cmd"]
        ],
        groups: [
            {
                title: "Quick",
                actions: [
                    ["Run", "run"],
                    ["Copy", "copy"],
                    ["Paste", "paste"],
                    ["Select All", "selectall"],
                    ["Ctrl Alt Del", "cad"],
                    ["Win X", "winx"],
                    ["Lock", "lock"],
                    ["Task Manager", "taskmgr"]
                ]
            },
            {
                title: "Windows Admin",
                actions: [
                    ["Command Prompt", "cmd"],
                    ["CMD Admin", "cmdadmin"],
                    ["PowerShell", "powershell"],
                    ["PS Admin", "powershelladmin"],
                    ["Device Manager", "devmgmt"],
                    ["Services", "services"],
                    ["Event Viewer", "eventvwr"],
                    ["Network Adapters", "ncpa"]
                ]
            },
            {
                title: "Troubleshooting",
                actions: [
                    ["System Info", "sysinfo"],
                    ["IP Config", "netinfo"],
                    ["Routes", "routes"],
                    ["DNS Test", "dnscheck"],
                    ["Event Logs", "logs"],
                    ["Ping 8.8.8.8", "ping8"],
                    ["Disk Info", "diskinfo"],
                    ["Memory", "memoryinfo"],
                    ["CPU", "cpuinfo"],
                    ["Uptime", "uptimeinfo"],
                    ["Processes", "processes"],
                    ["SFC Admin", "sfcscan"],
                    ["DISM Admin", "dismrepair"],
                    ["Flush DNS Admin", "flushdns"],
                    ["Winsock Admin", "winsockreset"],
                    ["GPUpdate Admin", "gpupdate"]
                ]
            }
        ]
    },
    linux: {
        buttonId: "profileLinux",
        quick: [
            ["Terminal", "terminal"],
            ["System Info", "sysinfo"],
            ["Network", "netinfo"],
            ["Disk", "diskinfo"],
            ["Memory", "memoryinfo"],
            ["CPU", "cpuinfo"],
            ["Uptime", "uptimeinfo"],
            ["Processes", "processes"]
        ],
        groups: [
            {
                title: "Linux Server",
                actions: [
                    ["Terminal", "terminal"],
                    ["System Info", "sysinfo"],
                    ["IP Address", "netinfo"],
                    ["Routes", "routes"],
                    ["Disk Free", "diskinfo"],
                    ["Memory", "memoryinfo"],
                    ["CPU", "cpuinfo"],
                    ["Uptime", "uptimeinfo"]
                ]
            },
            {
                title: "Services And Logs",
                actions: [
                    ["Processes", "processes"],
                    ["Services", "services"],
                    ["Journal", "logs"],
                    ["Kernel Log", "dmesg"],
                    ["Ping 8.8.8.8", "ping8"],
                    ["DNS Test", "dnscheck"]
                ]
            }
        ]
    },
    macos: {
        buttonId: "profileMacos",
        quick: [
            ["Terminal", "terminal"],
            ["System Info", "sysinfo"],
            ["Network", "netinfo"],
            ["Disk", "diskinfo"],
            ["Memory", "memoryinfo"],
            ["CPU", "cpuinfo"],
            ["Uptime", "uptimeinfo"],
            ["Activity", "activity"]
        ],
        groups: [
            {
                title: "macOS",
                actions: [
                    ["Spotlight", "spotlight"],
                    ["Terminal", "terminal"],
                    ["Activity Monitor", "activity"],
                    ["Force Quit", "forcequit"],
                    ["Lock", "lock"],
                    ["Copy", "copy"],
                    ["Paste", "paste"],
                    ["Select All", "selectall"]
                ]
            },
            {
                title: "Diagnostics",
                actions: [
                    ["System Info", "sysinfo"],
                    ["Network", "netinfo"],
                    ["Routes", "routes"],
                    ["Disk", "diskinfo"],
                    ["Memory", "memoryinfo"],
                    ["CPU", "cpuinfo"],
                    ["Uptime", "uptimeinfo"],
                    ["Processes", "processes"],
                    ["Ping 8.8.8.8", "ping8"],
                    ["DNS Test", "dnscheck"]
                ]
            }
        ]
    }
};

function byId(id) {
    return document.getElementById(id);
}

function savedProfile() {
    try {
        return window.localStorage.getItem("remotedeckProfile");
    } catch (err) {
        return "";
    }
}

function persistProfile(profile) {
    try {
        window.localStorage.setItem("remotedeckProfile", profile);
    } catch (err) {
        return;
    }
}

function normalizeProfile(profile) {
    return profileConfig[profile] ? profile : "windows";
}

function detectedBrowserProfile() {
    const nav = window.navigator || {};
    const userAgent = (nav.userAgent || "").toLowerCase();
    const platform = [
        nav.userAgentData && nav.userAgentData.platform,
        nav.platform,
        nav.vendor,
        nav.userAgent
    ].filter(Boolean).join(" ").toLowerCase();
    const mobileController = /android|iphone|ipad|ipod|mobile/.test(userAgent)
        || (/mac/.test(platform) && nav.maxTouchPoints > 1);

    if (mobileController) {
        return "windows";
    }

    if (/win/.test(platform)) {
        return "windows";
    }

    if (/mac/.test(platform)) {
        return "macos";
    }

    if (/linux|x11|cros/.test(platform)) {
        return "linux";
    }

    return "windows";
}

function initialProfile() {
    const stored = savedProfile();
    return stored && profileConfig[stored]
        ? stored
        : detectedBrowserProfile();
}

function createActionButton(label, action) {
    const btn = document.createElement("button");
    btn.className = "actionBtn";
    btn.type = "button";
    btn.textContent = label;
    if (adminActions.has(action)) {
        btn.classList.add("adminAction");
        btn.title = "Requires administrator approval on the connected PC.";
    }
    btn.addEventListener("click", function() {
        if (sendShortcut(action)) {
            flashStatus(
                adminActions.has(action)
                    ? "Approve UAC on PC"
                    : label + " sent"
            );
        }
    });
    return btn;
}

function renderQuickGrid() {
    const grid = byId("quickGrid");
    grid.innerHTML = "";

    profileConfig[activeProfile].quick.forEach(function(item) {
        grid.appendChild(createActionButton(item[0], item[1]));
    });
}

function renderMacros() {
    const content = byId("macroContent");
    content.innerHTML = "";

    profileConfig[activeProfile].groups.forEach(function(group) {
        const title = document.createElement("div");
        title.className = "sectionTitle";
        title.textContent = group.title;
        content.appendChild(title);

        const grid = document.createElement("div");
        grid.className = "macroGroupGrid";
        group.actions.forEach(function(item) {
            grid.appendChild(createActionButton(item[0], item[1]));
        });
        content.appendChild(grid);
    });
}

function updateProfileButtons() {
    Object.keys(profileConfig).forEach(function(profile) {
        byId(profileConfig[profile].buttonId).classList.toggle("active", profile === activeProfile);
    });
}

function setProfile(profile) {
    activeProfile = normalizeProfile(profile);
    persistProfile(activeProfile);
    updateProfileButtons();
    renderQuickGrid();
    renderMacros();
}

function labelForKey(key) {
    if (key === "BKSP") {
        return "delete";
    }
    if (key === "SPACE") {
        return "space";
    }
    if (key === "ENTER") {
        return "return";
    }
    if (key === "SHIFT") {
        return capsLock ? "caps" : "shift";
    }
    if (key === "#+=") {
        return symbolAltMode ? "123" : "#+=";
    }
    if (key === "123" || key === "ABC") {
        return key;
    }
    if (!numberMode && key.length === 1 && /[a-z]/.test(key)) {
        return shiftEnabled || capsLock
            ? key.toUpperCase()
            : key;
    }
    return key;
}

function keyClassFor(key) {
    if (key === "SHIFT" || key === "BKSP") {
        return "wide systemKey";
    }
    if (key === "123" || key === "ABC" || key === "#+=") {
        return "modeKey";
    }
    if (key === "SPACE") {
        return "space";
    }
    if (key === "ENTER") {
        return "returnKey";
    }
    return "";
}

function setStatus(text, state) {
    const bar = byId("statusBar");
    bar.textContent = text;
    bar.className = state;
}

function flashStatus(text) {
    clearTimeout(statusRestoreTimer);
    setStatus(text, connected ? "online" : "offline");

    if (connected) {
        statusRestoreTimer = setTimeout(function() {
            setStatus("Connected", "online");
        }, 2600);
    }
}

function updateNav() {
    const left = byId("leftNav");
    const right = byId("rightNav");

    if (currentPage === "deck") {
        left.textContent = "Tools";
        right.textContent = "Macros";
        left.classList.remove("active");
        right.classList.remove("active");
        return;
    }

    left.textContent = "Back";
    right.textContent = "Home";
    left.classList.add("active");
    right.classList.add("active");
}

function navLeft() {
    if (currentPage === "deck") {
        showPage("tools");
        return;
    }

    goBack();
}

function navRight() {
    if (currentPage === "deck") {
        showPage("macros");
        return;
    }

    showPage("deck");
}

function showPage(pageName) {
    previousPage = currentPage;
    currentPage = pageName;

    ["deck", "tools", "macros"].forEach(function(name) {
        byId(name + "Page").classList.toggle("active", name === pageName);
    });

    updateNav();
}

function goBack() {
    const target = previousPage && previousPage !== currentPage
        ? previousPage
        : "deck";

    showPage(target);
}

function sendMessage(message) {
    if (websock && websock.readyState === WebSocket.OPEN) {
        websock.send(JSON.stringify(message));
        return true;
    }

    setStatus("Disconnected", "offline");
    return false;
}

function connectSocket() {
    clearTimeout(reconnectTimer);

    const host = window.location.hostname || "192.168.4.1";

    try {
        websock = new WebSocket("ws://" + host + ":81/");
    } catch (err) {
        connected = false;
        setStatus("Disconnected", "offline");
        reconnectTimer = setTimeout(connectSocket, 1500);
        return;
    }

    websock.onopen = function() {
        connected = true;
        setStatus("Connected", "online");
    };

    websock.onclose = function() {
        connected = false;
        setStatus("Disconnected", "offline");
        reconnectTimer = setTimeout(connectSocket, 1500);
    };

    websock.onerror = function() {
        connected = false;
        setStatus("Disconnected", "offline");
    };

    websock.onmessage = function(evt) {
        let data;

        try {
            data = JSON.parse(evt.data);
        } catch (err) {
            return;
        }

        handleMessage(data);
    };
}

function createKeyboard() {
    const container = byId("keyboardGrid");
    const rows = numberMode
        ? (symbolAltMode ? alternateSymbolRows : symbolRows)
        : alphaRows;

    container.innerHTML = "";

    rows.forEach(function(row) {
        const rowDiv = document.createElement("div");
        rowDiv.className = "kbRow " + row.className;

        row.keys.forEach(function(key) {
            const btn = document.createElement("button");
            btn.type = "button";
            btn.className = "kbKey";
            btn.textContent = labelForKey(key);
            btn.setAttribute("aria-label", labelForKey(key));

            const extraClass = keyClassFor(key);
            if (extraClass) {
                extraClass.split(" ").forEach(function(className) {
                    btn.classList.add(className);
                });
            }

            if (key === "SHIFT") {
                if (capsLock) {
                    btn.classList.add("capsLockActive");
                } else if (shiftEnabled) {
                    btn.classList.add("shiftActive");
                }
            }

            let isDown = false;

            btn.addEventListener("pointerdown", function(e) {
                if (e.button !== 0) {
                    return;
                }

                e.preventDefault();
                isDown = true;
                if (btn.setPointerCapture) {
                    btn.setPointerCapture(e.pointerId);
                }
                btn.classList.add("pressed");
                sendKey(key, true);
            });

            function releaseKey(e) {
                if (!isDown) {
                    return;
                }

                e.preventDefault();
                isDown = false;
                btn.classList.remove("pressed");
                sendKey(key, false);
            }

            btn.addEventListener("pointerup", releaseKey);
            btn.addEventListener("pointercancel", releaseKey);
            btn.addEventListener("lostpointercapture", function() {
                if (isDown) {
                    isDown = false;
                    btn.classList.remove("pressed");
                    sendKey(key, false);
                }
            });

            rowDiv.appendChild(btn);
        });

        container.appendChild(rowDiv);
    });
}

function sendKey(key, pressed) {
    if (key === "SHIFT") {
        if (pressed) {
            toggleShift();
        }
        return;
    }

    if (key === "#+=") {
        if (pressed) {
            symbolAltMode = !symbolAltMode;
            createKeyboard();
        }
        return;
    }

    if (key === "123") {
        if (pressed) {
            numberMode = true;
            symbolAltMode = false;
            createKeyboard();
        }
        return;
    }

    if (key === "ABC") {
        if (pressed) {
            numberMode = false;
            symbolAltMode = false;
            createKeyboard();
        }
        return;
    }

    sendMessage({
        event: pressed ? "touch start" : "touch end",
        key: key,
        shift: shiftEnabled
    });

    if (pressed && shiftEnabled && !capsLock && key.length === 1 && /[a-z]/.test(key)) {
        shiftEnabled = false;
        createKeyboard();
    }
}

function toggleShift() {
    const now = Date.now();

    if (now - lastShiftTap < 500) {
        capsLock = !capsLock;
        shiftEnabled = capsLock;
    } else {
        shiftEnabled = !shiftEnabled;
    }

    lastShiftTap = now;
    createKeyboard();
}

function sendShortcut(action) {
    return sendMessage({
        event: "shortcut",
        action: action,
        profile: activeProfile
    });
}

function sendMouseClick(eventName) {
    sendMessage({
        event: eventName
    });
}

function sendScroll(delta) {
    sendMessage({
        event: "mouse_scroll",
        delta: delta
    });
}

function setupTouchpad() {
    const pad = byId("touchpad");
    let active = false;
    let moved = false;
    let lastX = 0;
    let lastY = 0;
    let ignoreClick = false;
    let twoFingerMode = false;
    let twoFingerMoved = false;
    let lastTouchY = 0;

    function averageTouchY(touches) {
        return (touches[0].clientY + touches[1].clientY) / 2;
    }

    pad.addEventListener("touchstart", function(e) {
        if (e.touches.length >= 2) {
            twoFingerMode = true;
            twoFingerMoved = false;
            lastTouchY = averageTouchY(e.touches);
            ignoreClick = true;
            active = false;
            e.preventDefault();
        }
    }, { passive: false });

    pad.addEventListener("touchmove", function(e) {
        if (!twoFingerMode || e.touches.length < 2) {
            return;
        }

        const y = averageTouchY(e.touches);
        const delta = y - lastTouchY;

        if (Math.abs(delta) > 3) {
            const wheel = Math.max(
                -8,
                Math.min(8, Math.round(delta / 6))
            );

            if (wheel !== 0) {
                twoFingerMoved = true;
                sendScroll(wheel);
                lastTouchY = y;
            }
        }

        e.preventDefault();
    }, { passive: false });

    pad.addEventListener("touchend", function(e) {
        if (!twoFingerMode || e.touches.length >= 2) {
            return;
        }

        if (!twoFingerMoved) {
            sendMouseClick("mouse_right");
        }

        twoFingerMode = false;
        ignoreClick = false;
        e.preventDefault();
    }, { passive: false });

    pad.addEventListener("pointerdown", function(e) {
        if (ignoreClick || twoFingerMode) {
            return;
        }

        if (e.pointerType === "mouse" && e.button === 2) {
            sendMouseClick("mouse_right");
            return;
        }

        active = true;
        moved = false;
        lastX = e.clientX;
        lastY = e.clientY;
        if (pad.setPointerCapture) {
            pad.setPointerCapture(e.pointerId);
        }
        e.preventDefault();
    });

    pad.addEventListener("pointermove", function(e) {
        if (!active) {
            return;
        }

        const dx = e.clientX - lastX;
        const dy = e.clientY - lastY;
        const distance = Math.max(Math.abs(dx), Math.abs(dy));

        if (distance > 2) {
            moved = true;
        }

        if (distance > 0) {
            const speed = Math.min(6 + distance * 0.65, 28);
            sendMessage({
                event: "mouse_move",
                dx: Math.round(dx * speed),
                dy: Math.round(dy * speed)
            });
        }

        lastX = e.clientX;
        lastY = e.clientY;
        e.preventDefault();
    });

    function endPointer(e) {
        if (!active) {
            ignoreClick = false;
            return;
        }

        if (!moved) {
            sendMouseClick("mouse_left");
        }

        active = false;
        ignoreClick = false;
        e.preventDefault();
    }

    pad.addEventListener("pointerup", endPointer);
    pad.addEventListener("pointercancel", endPointer);
    pad.addEventListener("contextmenu", function(e) {
        e.preventDefault();
    });
}

function requestTool(action, params) {
    byId("toolStatus").textContent = "Running " + toolLabel(action) + "...";
    byId("toolInputPanel").hidden = true;
    byId("ipInfoResults").hidden = true;
    byId("publicIpResults").hidden = true;
    byId("networkResults").hidden = true;

    const message = Object.assign({
        event: "tool",
        action: action,
        profile: activeProfile
    }, params || {});

    if (!sendMessage(message)) {
        byId("toolStatus").textContent = "Connect to RemoteDeck first.";
    }
}

function toolLabel(action) {
    const labels = {
        ipinfo: "IP Info",
        devicehealth: "Device Health",
        publicip: "Public IP",
        networkscan: "Network Snapshot",
        ticketinfo: "Ticket Info",
        pcspecs: "System Specs",
        dnslookup: "DNS Lookup",
        portcheck: "Port Check",
        wol: "Wake LAN"
    };

    return labels[action] || "Tool";
}

function showToolInput(action) {
    selectedTool = action;

    const panel = byId("toolInputPanel");
    const host = byId("toolHostInput");
    const port = byId("toolPortInput");

    panel.hidden = false;
    panel.classList.toggle("twoFields", action === "portcheck");
    port.hidden = action !== "portcheck";
    host.value = "";
    port.value = "";

    if (action === "dnslookup") {
        byId("toolStatus").textContent = "DNS Lookup";
        host.placeholder = "Host name";
        host.value = "google.com";
    } else if (action === "portcheck") {
        byId("toolStatus").textContent = "Port Check";
        host.placeholder = "Host or IP";
        host.value = "google.com";
        port.value = "443";
    } else if (action === "wol") {
        byId("toolStatus").textContent = "Wake LAN";
        host.placeholder = "MAC address";
        host.value = "00:11:22:33:44:55";
    }

    byId("ipInfoResults").hidden = true;
    byId("publicIpResults").hidden = true;
    byId("networkResults").hidden = true;
    host.focus();
}

function runSelectedTool() {
    const host = byId("toolHostInput").value.trim();
    const portValue = byId("toolPortInput").value.trim();

    if (selectedTool === "dnslookup") {
        if (host) {
            requestTool("dnslookup", {
                host: host
            });
        } else {
            byId("toolStatus").textContent = "Enter a host name.";
        }
        return;
    }

    if (selectedTool === "portcheck") {
        const portNumber = parseInt(portValue, 10);

        if (!host) {
            byId("toolStatus").textContent = "Enter a host or IP.";
            return;
        }

        if (!Number.isInteger(portNumber) || portNumber < 1 || portNumber > 65535) {
            byId("toolStatus").textContent = "Use a port from 1-65535.";
            return;
        }

        if (host && portValue) {
            requestTool("portcheck", {
                host: host,
                port: portNumber
            });
        }
        return;
    }

    if (selectedTool === "wol") {
        if (host) {
            requestTool("wol", {
                mac: host
            });
        } else {
            byId("toolStatus").textContent = "Enter the device MAC address.";
        }
    }
}

function runDnsLookup() {
    showToolInput("dnslookup");
}

function runPortCheck() {
    showToolInput("portcheck");
}

function runWakeOnLan() {
    showToolInput("wol");
}

function handleMessage(data) {
    if (data.error) {
        byId("toolStatus").textContent = data.error;
        return;
    }

    if (data.action === "status") {
        if (data.version) {
            setStatus("Connected v" + data.version, "online");
        }
        return;
    }

    if (data.output) {
        byId("toolStatus").textContent = toolLabel(data.action);
        byId("networkResults").hidden = false;
        byId("networkResults").textContent = data.output;
        return;
    }

    if (data.action === "ipinfo" || data.wifi !== undefined) {
        byId("toolStatus").textContent = "IP info";
        byId("ipInfoResults").hidden = false;
        byId("wifiName").textContent = data.wifi || "---";
        byId("ipAddress").textContent = data.ip || "---";
        byId("apAddress").textContent = data.apip || "---";
        byId("hostName").textContent = data.host || "---";
        byId("gatewayAddress").textContent = data.gateway || "---";
        byId("wifiRSSI").textContent = data.rssi !== undefined ? data.rssi + " dBm" : "---";
        return;
    }

    if (data.action === "publicip" || data.publicip !== undefined) {
        byId("toolStatus").textContent = "Public IP";
        byId("publicIpResults").hidden = false;
        byId("publicIpAddress").textContent = data.publicip || "---";
        byId("publicIsp").textContent = data.isp || "---";
        byId("publicCity").textContent = data.city || "---";
        byId("publicRegion").textContent = data.region || "---";
        byId("publicCountry").textContent = data.country || "---";
        return;
    }

    if (data.action === "networkscan" || data.scanresults !== undefined) {
        byId("toolStatus").textContent = "Network snapshot";
        byId("networkResults").hidden = false;
        byId("networkResults").textContent = data.scanresults || "No data returned.";
    }
}

function setupToolInputs() {
    [byId("toolHostInput"), byId("toolPortInput")].forEach(function(input) {
        input.addEventListener("keydown", function(e) {
            if (e.key === "Enter") {
                e.preventDefault();
                runSelectedTool();
            }
        });
    });
}

function start() {
    activeProfile = initialProfile();
    updateProfileButtons();
    renderQuickGrid();
    renderMacros();
    updateNav();
    createKeyboard();
    setupTouchpad();
    setupToolInputs();
    connectSocket();
}

window.addEventListener("load", start);
</script>
</body>
</html>
)rawliteral";
