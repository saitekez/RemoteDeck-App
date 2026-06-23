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

body.theme-slate {
    --bg: #0a0f12;
    --panel: #172126;
    --panel-strong: #263238;
    --key: #27343a;
    --key-active: #256b71;
    --accent: #42d68b;
    --accent-blue: #4cc9f0;
    --border: #38464d;
    --border-bright: #607079;
    --muted: #b7c5ca;
}

body.theme-contrast {
    --bg: #000000;
    --panel: #111111;
    --panel-strong: #222222;
    --key: #303030;
    --key-active: #0d7a42;
    --accent: #6cff9a;
    --accent-blue: #7cc7ff;
    --border: #676767;
    --border-bright: #8a8a8a;
    --text: #ffffff;
    --muted: #d5d5d5;
}

body.theme-matrix {
    --bg: #020603;
    --panel: #07140b;
    --panel-strong: #0d2113;
    --key: #102716;
    --key-active: #008f45;
    --accent: #00ff76;
    --accent-blue: #8bffc4;
    --border: #194d2b;
    --border-bright: #2adb71;
    --text: #ecfff2;
    --muted: #91cfa4;
    --danger: #ff5b6a;
    --shadow: rgba(0, 0, 0, 0.74);
    --inner-light: rgba(139, 255, 196, 0.13);
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

body.theme-slate {
    background:
        linear-gradient(180deg, #1e3137 0%, #0f171b 34%, #050708 100%);
}

body.theme-contrast {
    background:
        linear-gradient(180deg, #151515 0%, #050505 36%, #000000 100%);
}

body.theme-matrix {
    background:
        repeating-linear-gradient(90deg, rgba(0, 255, 118, 0.055) 0 1px, transparent 1px 28px),
        repeating-linear-gradient(180deg, rgba(0, 255, 118, 0.040) 0 1px, transparent 1px 22px),
        linear-gradient(180deg, #041109 0%, #020603 58%, #000000 100%);
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
    grid-template-columns: repeat(3, minmax(0, 1fr));
    gap: 8px;
}

.connectionArea {
    display: grid;
    gap: 6px;
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

.securityBar {
    display: grid;
    grid-template-columns: minmax(0, 1fr) auto auto;
    gap: 6px;
    align-items: center;
}

.securityBar input {
    min-width: 0;
    min-height: 36px;
    padding: 0 12px;
    border: 1px solid var(--border);
    border-radius: 8px;
    background: rgba(8, 9, 11, 0.82);
    color: var(--text);
    font: inherit;
    font-size: 14px;
    font-weight: 700;
}

.securityBar input:focus {
    border-color: var(--accent-blue);
    outline: none;
}

.securityBar button {
    min-height: 36px;
    padding: 0 14px;
    border: 1px solid var(--border-bright);
    border-radius: 8px;
    background:
        linear-gradient(180deg, rgba(63, 185, 80, 0.30), rgba(27, 93, 49, 0.35)),
        #203126;
    color: var(--text);
    font-size: 14px;
    font-weight: 800;
}

.securityState {
    color: var(--muted);
    font-size: 12px;
    font-weight: 800;
    text-transform: uppercase;
}

.securityState.unlocked {
    color: #b7f5c4;
}

.profileBar {
    display: grid;
    grid-template-columns: repeat(4, minmax(0, 1fr));
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

#settingsPage.active {
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

.settingsGrid {
    display: grid;
    gap: 10px;
}

.settingPanel {
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

.settingHeader,
.rangeHeader {
    display: flex;
    align-items: center;
    justify-content: space-between;
    gap: 10px;
    color: var(--text);
    font-size: 14px;
    font-weight: 900;
}

.settingValue {
    color: var(--muted);
    font-size: 12px;
    text-transform: uppercase;
}

.segmented {
    display: grid;
    grid-template-columns: repeat(4, minmax(0, 1fr));
    gap: 6px;
    margin-top: 10px;
}

.settingsBtn {
    min-height: 42px;
    padding: 0 10px;
    border: 1px solid var(--border);
    border-radius: 8px;
    background:
        linear-gradient(180deg, rgba(255, 255, 255, 0.08), rgba(0, 0, 0, 0.16)),
        var(--key);
    color: var(--muted);
    font-size: 13px;
    font-weight: 900;
}

.settingsBtn.active {
    border-color: var(--accent-blue);
    color: var(--text);
    background:
        linear-gradient(180deg, rgba(88, 166, 255, 0.24), rgba(0, 0, 0, 0.10)),
        var(--panel-strong);
}

.settingsBtn.dangerBtn {
    border-color: rgba(248, 81, 73, 0.72);
    color: #ffd0cd;
    background:
        linear-gradient(180deg, rgba(248, 81, 73, 0.18), rgba(0, 0, 0, 0.12)),
        var(--key);
}

.rangeControl {
    display: grid;
    gap: 8px;
    margin-top: 12px;
}

.rangeControl input {
    width: 100%;
    accent-color: var(--accent-blue);
}

.toggleRow {
    display: grid;
    grid-template-columns: minmax(0, 1fr) auto;
    gap: 10px;
    align-items: center;
    margin-top: 10px;
}

.toggleBtn {
    min-width: 112px;
    min-height: 42px;
    padding: 0 12px;
    border: 1px solid rgba(63, 185, 80, 0.62);
    border-radius: 8px;
    background:
        linear-gradient(180deg, rgba(63, 185, 80, 0.26), rgba(0, 0, 0, 0.10)),
        var(--key);
    color: var(--text);
    font-size: 13px;
    font-weight: 900;
}

.toggleBtn.off {
    border-color: rgba(248, 81, 73, 0.72);
    background:
        linear-gradient(180deg, rgba(248, 81, 73, 0.22), rgba(0, 0, 0, 0.12)),
        var(--key);
}

.settingStatus {
    margin: 8px 0 0;
    color: var(--muted);
    font-size: 12px;
    font-weight: 800;
    line-height: 1.35;
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

    .toggleRow {
        grid-template-columns: 1fr;
    }

    .segmented {
        grid-template-columns: repeat(4, minmax(0, 1fr));
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
        font-size: 13px;
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
    .toolBtn,
    .settingsBtn {
        min-height: 44px;
        padding: 7px;
        font-size: 13px;
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
        <button id="centerNav" class="panelBtn" type="button" onclick="navCenter()">Settings</button>
        <button id="rightNav" class="panelBtn" type="button" onclick="navRight()">Macros</button>
    </nav>

    <div class="connectionArea">
        <div id="statusBar" class="offline">Disconnected</div>
        <div id="securityBar" class="securityBar" hidden>
            <input id="controlPinInput" type="password" inputmode="numeric" autocomplete="off" placeholder="Control PIN" aria-label="Control PIN">
            <button id="controlPinButton" type="button" onclick="saveControlPin()">Unlock</button>
            <span id="securityState" class="securityState">Locked</span>
        </div>
    </div>

    <div class="profileBar" aria-label="Target operating system">
        <button id="profileWindows" class="profileBtn active" type="button" onclick="setProfile('windows')">Windows</button>
        <button id="profileLinux" class="profileBtn" type="button" onclick="setProfile('linux')">Linux</button>
        <button id="profileMacos" class="profileBtn" type="button" onclick="setProfile('macos')">macOS</button>
        <button id="profileAndroidTv" class="profileBtn" type="button" onclick="setProfile('androidtv')">Android</button>
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

    <section id="settingsPage" class="page">
        <div class="settingsGrid">
            <section class="settingPanel">
                <div class="settingHeader">
                    <span>Theme</span>
                    <span id="themeValue" class="settingValue">Deep</span>
                </div>
                <div class="segmented" aria-label="Theme">
                    <button id="themeDeep" class="settingsBtn" type="button" onclick="setTheme('deep')">Deep</button>
                    <button id="themeSlate" class="settingsBtn" type="button" onclick="setTheme('slate')">Slate</button>
                    <button id="themeContrast" class="settingsBtn" type="button" onclick="setTheme('contrast')">Contrast</button>
                    <button id="themeMatrix" class="settingsBtn" type="button" onclick="setTheme('matrix')">Matrix</button>
                </div>
            </section>

            <section class="settingPanel">
                <div class="settingHeader">
                    <span>Pointer</span>
                    <span id="pointerValue" class="settingValue">Balanced</span>
                </div>
                <label class="rangeControl">
                    <span class="rangeHeader">
                        <span>Sensitivity</span>
                        <output id="mouseSensitivityValue">100%</output>
                    </span>
                    <input id="mouseSensitivityInput" type="range" min="40" max="180" step="5" value="100">
                </label>
                <label class="rangeControl">
                    <span class="rangeHeader">
                        <span>Acceleration</span>
                        <output id="mouseAccelerationValue">65%</output>
                    </span>
                    <input id="mouseAccelerationInput" type="range" min="0" max="120" step="5" value="65">
                </label>
                <div class="toggleRow">
                    <p class="settingStatus">Single tap on the touchpad sends a left click.</p>
                    <button id="tapToClickButton" class="toggleBtn" type="button" onclick="toggleBooleanSetting('tapToClick')">Tap On</button>
                </div>
                <div class="toggleRow">
                    <p class="settingStatus">Two-finger scroll follows touch direction when enabled.</p>
                    <button id="naturalScrollButton" class="toggleBtn off" type="button" onclick="toggleBooleanSetting('naturalScroll')">Natural Off</button>
                </div>
            </section>

            <section class="settingPanel">
                <div class="settingHeader">
                    <span>Startup</span>
                    <span id="startupValue" class="settingValue">Deck</span>
                </div>
                <div class="segmented" aria-label="Startup page">
                    <button id="startupDeck" class="settingsBtn" type="button" onclick="setStartupPage('deck')">Deck</button>
                    <button id="startupTools" class="settingsBtn" type="button" onclick="setStartupPage('tools')">Tools</button>
                    <button id="startupMacros" class="settingsBtn" type="button" onclick="setStartupPage('macros')">Macros</button>
                    <button id="startupSettings" class="settingsBtn" type="button" onclick="setStartupPage('settings')">Settings</button>
                </div>
                <div class="toggleRow">
                    <p class="settingStatus">Ask before sending administrator-level macros.</p>
                    <button id="adminConfirmButton" class="toggleBtn" type="button" onclick="toggleBooleanSetting('confirmAdmin')">Confirm On</button>
                </div>
            </section>

            <section class="settingPanel">
                <div class="settingHeader">
                    <span>HTTP Server</span>
                    <span id="httpValue" class="settingValue">On</span>
                </div>
                <div class="toggleRow">
                    <p id="httpSettingStatus" class="settingStatus">Local page loading is enabled.</p>
                    <button id="httpToggleButton" class="toggleBtn" type="button" onclick="toggleHttpServer()">HTTP On</button>
                </div>
            </section>

            <section class="settingPanel">
                <div class="settingHeader">
                    <span>Reset</span>
                    <span id="resetValue" class="settingValue">Local</span>
                </div>
                <div class="toggleRow">
                    <p class="settingStatus">Restore theme, pointer, startup, and safety settings for this browser.</p>
                    <button class="settingsBtn dangerBtn" type="button" onclick="resetSettings()">Reset Settings</button>
                </div>
            </section>
        </div>
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
let controlPinRequired = false;
let controlPin = loadControlPin();
let httpServerEnabled = true;

const defaultSettings = {
    theme: "deep",
    mouseSensitivity: 100,
    mouseAcceleration: 65,
    tapToClick: true,
    naturalScroll: false,
    startupPage: "deck",
    confirmAdmin: true
};

const themeNames = {
    deep: "Deep",
    slate: "Slate",
    contrast: "Contrast",
    matrix: "Matrix"
};

let settings = loadSettings();

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
    },
    androidtv: {
        buttonId: "profileAndroidTv",
        quick: [
            ["Up", "tvup"],
            ["Left", "tvleft"],
            ["OK", "tvselect"],
            ["Right", "tvright"],
            ["Down", "tvdown"],
            ["Back", "tvback"],
            ["Home", "tvhome"],
            ["Play", "tvplay"]
        ],
        groups: [
            {
                title: "Android TV Remote",
                actions: [
                    ["Up", "tvup"],
                    ["Left", "tvleft"],
                    ["OK", "tvselect"],
                    ["Right", "tvright"],
                    ["Down", "tvdown"],
                    ["Back", "tvback"],
                    ["Home", "tvhome"],
                    ["Menu", "tvmenu"]
                ]
            },
            {
                title: "Media And Search",
                actions: [
                    ["Search", "tvsearch"],
                    ["Play/Pause", "tvplay"],
                    ["Previous", "tvprev"],
                    ["Next", "tvnext"],
                    ["Volume Down", "tvvoldown"],
                    ["Volume Up", "tvvolup"],
                    ["Mute", "tvmute"],
                    ["Settings", "tvsettings"]
                ]
            }
        ]
    }
};

function byId(id) {
    return document.getElementById(id);
}

function loadControlPin() {
    try {
        return window.localStorage.getItem("remotedeckControlPin") || "";
    } catch (err) {
        return "";
    }
}

function persistControlPin(pin) {
    try {
        if (pin) {
            window.localStorage.setItem("remotedeckControlPin", pin);
        } else {
            window.localStorage.removeItem("remotedeckControlPin");
        }
    } catch (err) {
        return;
    }
}

function updateSecurityBar() {
    const bar = byId("securityBar");
    const input = byId("controlPinInput");
    const state = byId("securityState");

    if (!bar || !input || !state) {
        return;
    }

    bar.hidden = !controlPinRequired;

    if (controlPinRequired && input.value !== controlPin) {
        input.value = controlPin;
    }

    state.textContent = controlPin ? "Unlocked" : "Locked";
    state.classList.toggle("unlocked", !!controlPin);
}

function saveControlPin() {
    const input = byId("controlPinInput");
    controlPin = input.value.trim();
    persistControlPin(controlPin);
    updateSecurityBar();
    flashStatus(controlPin ? "Unlocked" : "PIN Cleared");
}

function loadSettings() {
    try {
        const stored = JSON.parse(window.localStorage.getItem("remotedeckSettings") || "{}");
        return {
            theme: themeNames[stored.theme] ? stored.theme : defaultSettings.theme,
            mouseSensitivity: clampNumber(stored.mouseSensitivity, 40, 180, defaultSettings.mouseSensitivity),
            mouseAcceleration: clampNumber(stored.mouseAcceleration, 0, 120, defaultSettings.mouseAcceleration),
            tapToClick: stored.tapToClick !== false,
            naturalScroll: stored.naturalScroll === true,
            startupPage: normalizePageName(stored.startupPage || defaultSettings.startupPage),
            confirmAdmin: stored.confirmAdmin !== false
        };
    } catch (err) {
        return Object.assign({}, defaultSettings);
    }
}

function saveSettings() {
    try {
        window.localStorage.setItem("remotedeckSettings", JSON.stringify(settings));
    } catch (err) {
        return;
    }
}

function clampNumber(value, min, max, fallback) {
    const number = Number(value);

    if (!Number.isFinite(number)) {
        return fallback;
    }

    return Math.max(min, Math.min(max, number));
}

function setTheme(theme) {
    settings.theme = themeNames[theme] ? theme : defaultSettings.theme;
    saveSettings();
    applyTheme();
    updateSettingsPage();
}

function applyTheme() {
    document.body.classList.remove("theme-slate", "theme-contrast", "theme-matrix");

    if (settings.theme !== "deep") {
        document.body.classList.add("theme-" + settings.theme);
    }
}

function updatePointerSetting(name, value) {
    settings[name] = name === "mouseSensitivity"
        ? clampNumber(value, 40, 180, defaultSettings.mouseSensitivity)
        : clampNumber(value, 0, 120, defaultSettings.mouseAcceleration);

    saveSettings();
    updateSettingsPage();
}

function toggleBooleanSetting(name) {
    if (typeof settings[name] !== "boolean") {
        return;
    }

    settings[name] = !settings[name];
    saveSettings();
    updateSettingsPage();
    flashStatus(settings[name] ? "Setting On" : "Setting Off");
}

function resetSettings() {
    if (!window.confirm("Reset RemoteDeck settings on this browser?")) {
        return;
    }

    settings = Object.assign({}, defaultSettings);
    saveSettings();
    applyTheme();
    updateSettingsPage();
    flashStatus("Settings Reset");
}

function setStartupPage(pageName) {
    settings.startupPage = normalizePageName(pageName);
    saveSettings();
    updateSettingsPage();
    flashStatus("Startup: " + pageLabel(settings.startupPage));
}

function pageLabel(pageName) {
    const labels = {
        deck: "Deck",
        tools: "Tools",
        macros: "Macros",
        settings: "Settings"
    };

    return labels[pageName] || labels.deck;
}

function normalizePageName(pageName) {
    return ["deck", "tools", "macros", "settings"].indexOf(pageName) >= 0
        ? pageName
        : "deck";
}

function pointerLabel() {
    if (settings.mouseSensitivity >= 135 || settings.mouseAcceleration >= 95) {
        return "Fast";
    }

    if (settings.mouseSensitivity <= 70 || settings.mouseAcceleration <= 25) {
        return "Precise";
    }

    return "Balanced";
}

function updateSettingsPage() {
    const theme = settings.theme;
    const sensitivity = settings.mouseSensitivity;
    const acceleration = settings.mouseAcceleration;
    const startupPage = normalizePageName(settings.startupPage);

    ["deep", "slate", "contrast", "matrix"].forEach(function(name) {
        byId("theme" + name.charAt(0).toUpperCase() + name.slice(1)).classList.toggle("active", theme === name);
    });

    ["deck", "tools", "macros", "settings"].forEach(function(name) {
        byId("startup" + name.charAt(0).toUpperCase() + name.slice(1)).classList.toggle("active", startupPage === name);
    });

    byId("themeValue").textContent = themeNames[theme] || themeNames.deep;
    byId("pointerValue").textContent = pointerLabel();
    byId("startupValue").textContent = pageLabel(startupPage);
    byId("mouseSensitivityInput").value = sensitivity;
    byId("mouseSensitivityValue").textContent = sensitivity + "%";
    byId("mouseAccelerationInput").value = acceleration;
    byId("mouseAccelerationValue").textContent = acceleration + "%";
    updateToggleButton("tapToClickButton", settings.tapToClick, "Tap On", "Tap Off");
    updateToggleButton("naturalScrollButton", settings.naturalScroll, "Natural On", "Natural Off");
    updateToggleButton("adminConfirmButton", settings.confirmAdmin, "Confirm On", "Confirm Off");
    updateHttpSettingUi();
}

function updateToggleButton(id, enabled, onText, offText) {
    const button = byId(id);

    if (!button) {
        return;
    }

    button.textContent = enabled ? onText : offText;
    button.classList.toggle("off", !enabled);
}

function updateHttpSettingUi(message) {
    const value = byId("httpValue");
    const status = byId("httpSettingStatus");
    const button = byId("httpToggleButton");

    if (!value || !status || !button) {
        return;
    }

    value.textContent = httpServerEnabled ? "On" : "Off";
    button.textContent = httpServerEnabled ? "HTTP On" : "HTTP Off";
    button.classList.toggle("off", !httpServerEnabled);

    status.textContent = message || (
        httpServerEnabled
            ? "Local page loading is enabled."
            : "Current page can still restore it. Reboot also turns it back on."
    );
}

function toggleHttpServer() {
    const nextEnabled = !httpServerEnabled;

    if (!sendMessage({
        event: "settings",
        httpEnabled: nextEnabled
    })) {
        updateHttpSettingUi("Connect to RemoteDeck first.");
        return;
    }

    updateHttpSettingUi(nextEnabled ? "Turning HTTP on..." : "Turning HTTP off...");
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
        if (!confirmShortcutSend(label, action)) {
            return;
        }

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

function confirmShortcutSend(label, action) {
    if (!settings.confirmAdmin || !adminActions.has(action)) {
        return true;
    }

    return window.confirm("Send admin macro: " + label + "?");
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
    const center = byId("centerNav");
    const right = byId("rightNav");

    if (currentPage === "deck") {
        left.textContent = "Tools";
        center.textContent = "Settings";
        right.textContent = "Macros";
        left.classList.remove("active");
        center.classList.remove("active");
        right.classList.remove("active");
        return;
    }

    left.textContent = "Back";
    center.textContent = "Settings";
    right.textContent = "Home";
    left.classList.toggle("active", currentPage !== "settings");
    center.classList.toggle("active", currentPage === "settings");
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

function navCenter() {
    showPage(currentPage === "settings" ? "deck" : "settings");
}

function showPage(pageName) {
    pageName = normalizePageName(pageName);
    previousPage = currentPage;
    currentPage = pageName;

    ["deck", "tools", "macros", "settings"].forEach(function(name) {
        byId(name + "Page").classList.toggle("active", name === pageName);
    });

    updateNav();
}

function applyStartupPage() {
    currentPage = normalizePageName(settings.startupPage);
    previousPage = "deck";

    ["deck", "tools", "macros", "settings"].forEach(function(name) {
        byId(name + "Page").classList.toggle("active", name === currentPage);
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
        const payload = Object.assign({}, message);

        if (controlPin) {
            payload.pin = controlPin;
        }

        websock.send(JSON.stringify(payload));
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
                sendScroll(settings.naturalScroll ? -wheel : wheel);
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
            const sensitivity = settings.mouseSensitivity / 100;
            const acceleration = settings.mouseAcceleration / 100;
            const speed = Math.max(
                1,
                Math.min(36, (6 + distance * acceleration) * sensitivity)
            );
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

        if (!moved && settings.tapToClick) {
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
        if (data.error.indexOf("Unlock RemoteDeck") === 0) {
            controlPinRequired = true;
            updateSecurityBar();
            flashStatus("Unlock Required");
        }

        byId("toolStatus").textContent = data.error;
        return;
    }

    if (data.action === "status") {
        controlPinRequired = !!data.controlPinRequired;
        httpServerEnabled = data.httpServerEnabled !== false;
        updateSecurityBar();
        updateHttpSettingUi();

        if (data.version) {
            setStatus("Connected v" + data.version, "online");
        }
        return;
    }

    if (data.action === "settings") {
        if (data.httpEnabled !== undefined) {
            httpServerEnabled = !!data.httpEnabled;
        }

        updateHttpSettingUi(data.message);
        flashStatus(data.message || "Settings Updated");
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

function setupSecurityInput() {
    byId("controlPinInput").addEventListener("keydown", function(e) {
        if (e.key === "Enter") {
            e.preventDefault();
            saveControlPin();
        }
    });
}

function setupSettingsControls() {
    byId("mouseSensitivityInput").addEventListener("input", function(e) {
        updatePointerSetting("mouseSensitivity", e.target.value);
    });

    byId("mouseAccelerationInput").addEventListener("input", function(e) {
        updatePointerSetting("mouseAcceleration", e.target.value);
    });
}

function start() {
    applyTheme();
    activeProfile = initialProfile();
    updateProfileButtons();
    renderQuickGrid();
    renderMacros();
    applyStartupPage();
    updateSettingsPage();
    updateSecurityBar();
    createKeyboard();
    setupTouchpad();
    setupToolInputs();
    setupSecurityInput();
    setupSettingsControls();
    connectSocket();
}

window.addEventListener("load", start);
</script>
</body>
</html>
)rawliteral";
