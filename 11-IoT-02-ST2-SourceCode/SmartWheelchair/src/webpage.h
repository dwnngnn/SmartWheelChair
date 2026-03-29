#pragma once

#include <Arduino.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
    <title>ESP32 Car Controller</title>
    <!-- Modern Fonts -->
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@400;500;600&display=swap" rel="stylesheet">
    <link rel="stylesheet" href="style.css">
</head>
<body>
    <div class="overlay">
        <!-- Info + Controls Column -->
        <div class="main-controls">
            <div class="glass-panel info-panel">
                <div class="header">
                    <div class="status-indicator" id="conn-indicator"></div>
                    <h1>Car Controller</h1>
                </div>
                <p>Connected to ESP32 car module</p>
                <div class="status-metrics">
                    <div class="metric">
                        <span class="metric-label">Head</span>
                        <div class="head-visual blur-val" id="val-head-container">
                            <div class="head-turret" id="val-head-turret">
                                <div class="head-eyes"></div>
                            </div>
                        </div>
                        <span style="font-size: 0.65rem; color:#8b949e;" id="val-head-text">90°</span>
                    </div>
                    <div class="metric">
                        <span class="metric-label">Distance</span>
                        <span class="metric-value blur-val" id="val-dist">--</span>
                        <span class="metric-unit">cm</span>
                    </div>
                    <div class="metric">
                        <span class="metric-label">Mode</span>
                        <span class="metric-value blur-val" id="val-mode">Manual</span>
                    </div>
                    <div class="metric">
                        <span class="metric-label">Ping</span>
                        <span class="metric-value blur-val" id="val-ping">--</span>
                        <span class="metric-unit">ms</span>
                    </div>
                </div>
            </div>

            <div class="glass-panel controls-panel">
                <h2 class="controls-title">Movement</h2>
                <div class="dpad">
                    <div class="dpad-row">
                        <button id="btn-head-left" class="dpad-btn head-btn" data-cmd="C">↺</button>
                        <button id="btn-up" class="dpad-btn" data-cmd="F">W</button>
                        <button id="btn-head-right" class="dpad-btn head-btn" data-cmd="D">↻</button>
                    </div>
                    <div class="dpad-row">
                        <button id="btn-left" class="dpad-btn" data-cmd="L">A</button>
                        <button id="btn-down" class="dpad-btn" data-cmd="B">S</button>
                        <button id="btn-right" class="dpad-btn" data-cmd="R">D</button>
                    </div>
                </div>
            </div>

            <div class="glass-panel actions-panel">
                <h2 class="controls-title">Actions</h2>
                <div class="action-grid">
                    <button id="btn-auto-turn" class="action-btn" data-cmd="1">
                        <span class="action-icon">⟳</span>
                        <span class="action-label">Auto Turn</span>
                    </button>
                    <button id="btn-auto-on" class="action-btn btn-auto-on" data-cmd="V">
                        <span class="action-icon">⚡</span>
                        <span class="action-label">Auto ON</span>
                    </button>
                    <button id="btn-auto-off" class="action-btn btn-auto-off" data-cmd="v">
                        <span class="action-icon">⏸</span>
                        <span class="action-label">Auto OFF</span>
                    </button>
                    <button id="btn-led" class="action-btn btn-led" data-cmd="I">
                        <span class="action-icon">💡</span>
                        <span class="action-label">LED Control</span>
                    </button>
                    <button id="btn-mute" class="action-btn" data-cmd="M">
                        <span class="action-icon" id="mute-icon">🔔</span>
                        <span class="action-label" id="mute-label">Buzzer ON</span>
                    </button>
                    <button id="btn-reset" class="action-btn btn-reset" data-cmd="Z">
                        <span class="action-icon">🔄</span>
                        <span class="action-label">Reset</span>
                    </button>
                </div>
            </div>

            <div class="glass-panel sounds-panel">
                <h2 class="controls-title">Sound Effects</h2>
                <div class="sound-controls">
                    <select id="sound-select" class="sound-select"></select>
                    <div class="sound-actions">
                        <button id="btn-play-sound" class="action-btn">
                            <span class="action-icon">▶️</span>
                            <span class="action-label">Play</span>
                        </button>
                        <button id="btn-delete-sound" class="action-btn">
                            <span class="action-icon">🗑️</span>
                            <span class="action-label">Delete</span>
                        </button>
                    </div>
                    <div class="upload-controls">
                        <input type="file" id="sound-upload" accept=".mp3" hidden>
                        <button id="btn-upload-sound" class="action-btn btn-upload">
                            <span class="action-icon">📤</span>
                            <span class="action-label">Upload .mp3</span>
                        </button>
                    </div>
                </div>
            </div>
        </div>

        <!-- Log Panel -->
        <div class="glass-panel log-panel">
            <div class="log-header">
                <h2 class="controls-title">Command Log</h2>
                <button id="btn-clear-log" class="clear-log-btn">Clear</button>
            </div>
            <div id="log-container" class="log-container">
                <div class="log-entry log-info">System ready...</div>
            </div>
        </div>
    </div>

    <script src="script.js"></script>
</body>
</html>
)rawliteral";

const char style_css[] PROGMEM = R"rawliteral(
* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
    font-family: 'Inter', -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
}

body {
    height: 100vh;
    width: 100vw;
    overflow: hidden;
    position: relative;
    background-color: #0d1117;
    display: flex;
    align-items: center;
    justify-content: center;
}

/* ===== Overlay Layout ===== */
.overlay {
    position: absolute;
    inset: 0;
    z-index: 1000;
    display: flex;
    flex-direction: column;
    padding: 12px;
    gap: 12px;
    overflow-y: auto;
    -webkit-overflow-scrolling: touch;
}

/* ===== Glass Panel Base ===== */
.glass-panel {
    background: rgba(13, 17, 23, 0.85);
    backdrop-filter: blur(16px);
    -webkit-backdrop-filter: blur(16px);
    border: 1px solid rgba(255, 255, 255, 0.08);
    border-radius: 16px;
    padding: 16px;
    color: #e6edf3;
    box-shadow: 0 8px 32px rgba(0, 0, 0, 0.4);
    flex-shrink: 0;
}

/* ===== Info Panel ===== */
.info-panel {
    padding: 14px 16px;
}

.header {
    display: flex;
    align-items: center;
    gap: 10px;
    margin-bottom: 4px;
}

.status-indicator {
    width: 8px;
    height: 8px;
    background-color: #00ff88;
    border-radius: 50%;
    box-shadow: 0 0 10px #00ff88;
    animation: pulse-status 2s infinite ease-in-out;
}

@keyframes pulse-status {
    0%, 100% { opacity: 1; box-shadow: 0 0 10px #00ff88; }
    50% { opacity: 0.6; box-shadow: 0 0 4px #00ff88; }
}

.glass-panel h1 {
    font-size: 1.2rem;
    font-weight: 600;
    background: linear-gradient(135deg, #00f2fe, #4facfe);
    -webkit-background-clip: text;
    background-clip: text;
    -webkit-text-fill-color: transparent;
    letter-spacing: -0.3px;
}

.glass-panel p {
    font-size: 0.75rem;
    color: #8b949e;
    line-height: 1.4;
    margin-bottom: 12px;
}

/* ===== Status Metrics ===== */
.status-metrics {
    display: flex;
    gap: 12px;
    background: rgba(255, 255, 255, 0.03);
    border: 1px solid rgba(255, 255, 255, 0.05);
    border-radius: 10px;
    padding: 10px;
}

.metric {
    flex: 1;
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
}

.metric-label {
    font-size: 0.65rem;
    text-transform: uppercase;
    color: #8b949e;
    letter-spacing: 0.5px;
    margin-bottom: 4px;
}

.metric-value {
    font-size: 1.1rem;
    font-weight: 600;
    color: #c9d1d9;
    font-family: 'SF Mono', 'Fira Code', 'Cascadia Code', monospace;
    transition: filter 0.3s ease;
}

.blur-val {
    filter: blur(4px);
    opacity: 0.5;
}

.metric-unit {
    font-size: 0.6rem;
    color: #8b949e;
    margin-left: 2px;
}

/* ===== Visual Robot Head Data ===== */
.head-visual {
    width: 38px;
    height: 38px;
    border-radius: 50%;
    background: rgba(255, 255, 255, 0.03);
    border: 2px solid rgba(255, 255, 255, 0.1);
    position: relative;
    margin: 4px 0;
    display: flex;
    justify-content: center;
    align-items: center;
    box-shadow: inset 0 2px 5px rgba(0,0,0,0.4);
}

.head-turret {
    width: 26px;
    height: 26px;
    border-radius: 50%;
    background: #161b22;
    border: 1px solid #30363d;
    position: relative;
    display: flex;
    justify-content: center;
    transition: transform 0.2s cubic-bezier(0.2, 0.8, 0.2, 1);
}

.head-eyes {
    width: 14px;
    height: 5px;
    background: #4facfe;
    border-radius: 3px;
    position: absolute;
    top: 2px;
    box-shadow: 0 0 8px #4facfe;
}

/* ===== Section Titles ===== */
.controls-title {
    font-size: 0.75rem;
    color: #8b949e;
    margin-bottom: 12px;
    text-align: center;
    text-transform: uppercase;
    letter-spacing: 1px;
    font-weight: 500;
}

/* ===== D-Pad ===== */
.dpad {
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 6px;
}

.dpad-row {
    display: flex;
    justify-content: center;
    gap: 6px;
}

.dpad-btn {
    width: 72px;
    height: 72px;
    background: rgba(255, 255, 255, 0.05);
    border: 1px solid rgba(255, 255, 255, 0.1);
    color: #e6edf3;
    font-size: 1.5rem;
    font-weight: bold;
    border-radius: 16px;
    cursor: pointer;
    transition: all 0.15s ease;
    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.3);
    display: flex;
    justify-content: center;
    align-items: center;
    user-select: none;
    -webkit-user-select: none;
    touch-action: none;
}

.head-btn {
    color: #4facfe;
    font-size: 1.8rem;
    font-weight: normal;
}

.dpad-btn:active, .dpad-btn.active {
    background: rgba(79, 172, 254, 0.35);
    border-color: #4facfe;
    box-shadow: 0 0 12px rgba(79, 172, 254, 0.4);
    transform: scale(0.95);
}

.btn-stop {
    background: rgba(255, 70, 70, 0.1);
    border-color: rgba(255, 70, 70, 0.2);
    color: #ff6b6b;
    font-size: 0.9rem;
}

.btn-stop:active, .btn-stop.active {
    background: rgba(255, 70, 70, 0.35);
    border-color: #ff6b6b;
    box-shadow: 0 0 12px rgba(255, 70, 70, 0.4);
}

/* ===== Action Buttons Grid ===== */
.action-grid {
    display: grid;
    grid-template-columns: repeat(2, 1fr);
    gap: 8px;
}

.action-btn {
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    gap: 4px;
    padding: 10px 6px;
    background: rgba(255, 255, 255, 0.04);
    border: 1px solid rgba(255, 255, 255, 0.08);
    border-radius: 12px;
    color: #c9d1d9;
    cursor: pointer;
    transition: all 0.15s ease;
    user-select: none;
    -webkit-user-select: none;
    touch-action: manipulation;
}

.action-icon {
    font-size: 1.3rem;
}

.action-label {
    font-size: 0.65rem;
    text-transform: uppercase;
    letter-spacing: 0.5px;
    opacity: 0.7;
}

.action-btn:active {
    background: rgba(79, 172, 254, 0.25);
    border-color: #4facfe;
    transform: scale(0.95);
}

.btn-auto-on:active {
    background: rgba(0, 255, 136, 0.25);
    border-color: #00ff88;
}

.btn-auto-off:active {
    background: rgba(255, 70, 70, 0.25);
    border-color: #ff6b6b;
}

.btn-led:active {
    background: rgba(255, 215, 0, 0.25);
    border-color: #ffd700;
    box-shadow: 0 0 15px rgba(255, 215, 0, 0.4);
}

.btn-led span.action-icon {
    color: #ffd700;
    text-shadow: 0 0 8px rgba(255, 215, 0, 0.4);
}

.btn-reset:active {
    background: rgba(255, 70, 70, 0.25);
    border-color: #ff6b6b;
    box-shadow: 0 0 15px rgba(255, 70, 70, 0.4);
}
.btn-reset span.action-icon {
    color: #ff6b6b;
    text-shadow: 0 0 8px rgba(255, 70, 70, 0.4);
}

.sound-controls {
    display: flex;
    flex-direction: column;
    gap: 8px;
}
.sound-select {
    width: 100%;
    padding: 8px;
    background: rgba(255, 255, 255, 0.05);
    border: 1px solid rgba(255, 255, 255, 0.1);
    color: #e6edf3;
    border-radius: 8px;
    font-size: 0.8rem;
    outline: none;
}
.sound-actions {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 8px;
}
.btn-upload {
    width: 100%;
}

/* ===== Log Panel ===== */
.log-panel {
    flex: 1;
    min-height: 100px;
    display: flex;
    flex-direction: column;
    overflow: hidden;
}

.log-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 8px;
}

.log-header .controls-title {
    margin-bottom: 0;
    text-align: left;
}

.clear-log-btn {
    background: rgba(255, 255, 255, 0.05);
    border: 1px solid rgba(255, 255, 255, 0.1);
    color: #8b949e;
    font-size: 0.65rem;
    padding: 4px 10px;
    border-radius: 8px;
    cursor: pointer;
    text-transform: uppercase;
    letter-spacing: 0.5px;
    transition: all 0.15s ease;
}

.clear-log-btn:active {
    background: rgba(255, 70, 70, 0.2);
    border-color: rgba(255, 70, 70, 0.3);
    color: #ff6b6b;
}

.log-container {
    flex: 1;
    overflow-y: auto;
    display: flex;
    flex-direction: column;
    gap: 3px;
    font-family: 'SF Mono', 'Fira Code', 'Cascadia Code', monospace;
    font-size: 0.7rem;
    padding-right: 4px;
}

.log-container::-webkit-scrollbar {
    width: 4px;
}

.log-container::-webkit-scrollbar-track {
    background: transparent;
}

.log-container::-webkit-scrollbar-thumb {
    background: rgba(255, 255, 255, 0.1);
    border-radius: 2px;
}

.log-entry {
    padding: 4px 8px;
    border-radius: 6px;
    background: rgba(255, 255, 255, 0.02);
    color: #8b949e;
    white-space: nowrap;
    overflow: hidden;
    text-overflow: ellipsis;
    animation: fadeIn 0.2s ease;
}

.log-entry .log-time {
    color: #484f58;
    margin-right: 6px;
}

.log-entry .log-cmd {
    color: #4facfe;
    font-weight: 600;
}

.log-info {
    color: #00ff88;
}

.log-error {
    color: #ff6b6b;
}

@keyframes fadeIn {
    from { opacity: 0; transform: translateY(-4px); }
    to   { opacity: 1; transform: translateY(0); }
}

/* =======================================
   PORTRAIT (default, phones upright)
   ======================================= */
.main-controls {
    display: flex;
    flex-direction: column;
    gap: 12px;
    flex-shrink: 0;
}

/* =======================================
   LANDSCAPE (phone rotated)
   ======================================= */
@media (orientation: landscape) and (max-height: 500px) {
    .overlay {
        flex-direction: row;
        padding: 8px;
        gap: 8px;
        overflow: hidden;
    }

    .main-controls {
        flex-direction: column;
        gap: 8px;
        width: 55%;
        max-width: 320px;
        overflow-y: auto;
        flex-shrink: 0;
    }

    .info-panel {
        padding: 10px 14px;
    }

    .glass-panel h1 {
        font-size: 1rem;
    }

    .glass-panel p {
        display: none;
    }

    .controls-panel, .actions-panel {
        padding: 10px 14px;
    }

    .controls-title {
        margin-bottom: 8px;
        font-size: 0.65rem;
    }

    .dpad-btn {
        width: 56px;
        height: 56px;
        font-size: 1.2rem;
        border-radius: 12px;
    }

    .dpad {
        gap: 4px;
    }

    .dpad-row {
        gap: 4px;
    }

    .action-grid {
        grid-template-columns: repeat(2, 1fr);
        gap: 6px;
    }

    .action-btn {
        padding: 8px 4px;
    }

    .action-icon {
        font-size: 1.1rem;
    }

    .action-label {
        font-size: 0.55rem;
    }

    .log-panel {
        flex: 1;
        min-height: unset;
        max-height: 100%;
    }
}

/* =======================================
   SMALL PORTRAIT SCREENS
   ======================================= */
@media (max-width: 380px) {
    .overlay {
        padding: 8px;
        gap: 8px;
    }

    .glass-panel {
        padding: 12px;
        border-radius: 14px;
    }

    .dpad-btn {
        width: 60px;
        height: 60px;
    }

    .action-grid {
        grid-template-columns: repeat(2, 1fr);
        gap: 6px;
    }
}

)rawliteral";

const char script_js[] PROGMEM = R"rawliteral(
// --- IoT Car WebSocket Controls ---
const isLocal = (location.hostname === '192.168.4.1');
const wsUrl = isLocal ? `ws://${location.hostname}:81` : `ws://${location.host}/ws/car`;
const ws = new WebSocket(wsUrl);

// --- Log System ---
const logContainer = document.getElementById('log-container');
const MAX_LOG_ENTRIES = 5;

const CMD_LABELS = {
    'F': 'Forward',
    'B': 'Backward',
    'L': 'Turn Left',
    'R': 'Turn Right',
    'S': 'Stop',
    'C': 'Head Left',
    'D': 'Head Right',
    '1': 'Auto Turn',
    'V': 'Auto ON',
    'v': 'Auto OFF',
    'I': 'LED Toggle',
    'Z': 'Reset ESP32',
};

function getTimeStr() {
    const now = new Date();
    return now.toLocaleTimeString('en-US', { hour12: false, hour: '2-digit', minute: '2-digit', second: '2-digit' });
}

function addLog(cmd, type = 'cmd') {
    const entry = document.createElement('div');
    entry.className = `log-entry${type === 'info' ? ' log-info' : type === 'error' ? ' log-error' : ''}`;

    if (type === 'cmd') {
        const label = CMD_LABELS[cmd] || cmd;
        entry.innerHTML = `<span class="log-time">${getTimeStr()}</span><span class="log-cmd">[${cmd}]</span> ${label}`;
    } else {
        entry.innerHTML = `<span class="log-time">${getTimeStr()}</span>${cmd}`;
    }

    logContainer.appendChild(entry);

    // Trim old entries
    while (logContainer.children.length > MAX_LOG_ENTRIES) {
        logContainer.removeChild(logContainer.firstChild);
    }

    logContainer.scrollTop = logContainer.scrollHeight;
}

// --- UI Status Updater ---
function updateStatusUI(status) {
    // Update Head Degree
    const headContainer = document.getElementById('val-head-container');
    const headTurret = document.getElementById('val-head-turret');
    const headText = document.getElementById('val-head-text');
    if (status.head !== undefined) {
        headText.textContent = status.head + '°';
        // Map ESP servo degree to CSS rotation. 
        // 90 - status.head -> Inverted logical degree mapped to CSS (90=M, 0=L/R flipped, 180=L/R flipped)
        const cssRot = 90 - status.head;
        headTurret.style.transform = `rotate(${cssRot}deg)`;
        headContainer.classList.remove('blur-val');
    }

    // Update Distance
    const distEl = document.getElementById('val-dist');
    if (status.dist !== undefined) {
        distEl.textContent = typeof status.dist === 'number' ? status.dist.toFixed(1) : status.dist;
        distEl.classList.remove('blur-val');
        
        // Color code distance (closer = redder)
        if (typeof status.dist === 'number') {
            if (status.dist < 15) distEl.style.color = '#ff6b6b';      // Danger: Too close (Red)
            else if (status.dist < 40) distEl.style.color = '#f1c40f'; // Warning: Getting close (Yellow)
            else distEl.style.color = '#00ff88';                       // Safe: Far away (Green)
        }
    }

    // Update Mode
    const modeEl = document.getElementById('val-mode');
    if (status.auto !== undefined) {
        modeEl.textContent = status.auto ? 'Auto' : 'Manual';
        modeEl.classList.remove('blur-val');
        if (status.auto) {
            modeEl.style.color = '#00ff88';
        } else {
            modeEl.style.color = '#c9d1d9';
        }
    }

    // Update Buzzer status
    const muteIcon = document.getElementById('mute-icon');
    const muteLabel = document.getElementById('mute-label');
    if (status.buzzer !== undefined) {
        if (status.buzzer) {
            muteIcon.textContent = '🔔';
            muteLabel.textContent = 'Buzzer ON';
            muteIcon.style.color = '#00ff88';
        } else {
            muteIcon.textContent = '🔕';
            muteLabel.textContent = 'Buzzer OFF';
            muteIcon.style.color = '#ff6b6b';
        }
    }
}

// --- Fetch Initial Status ---
fetch('/api/status')
    .then(res => res.json())
    .then(status => updateStatusUI(status))
    .catch(err => console.error("Initial status err:", err));

// --- Status handling now moved to WebSocket ---

// --- Ping System ---
let pingStart = 0;
setInterval(() => {
    if (ws && ws.readyState === WebSocket.OPEN && !isLocal) {
        pingStart = Date.now();
        ws.send('PING\n');
    }
}, 2000);

if (isLocal) {
    setTimeout(() => {
        const pingEl = document.getElementById('val-ping');
        if (pingEl) {
            pingEl.textContent = '<1ms';
            pingEl.classList.remove('blur-val');
            pingEl.style.color = '#00ff88';
        }
    }, 500);
}

// --- WebSocket / Polling ---
ws.onopen = () => {
    console.log("🔌 Connected to IoT Car WebSocket!");
    addLog('WebSocket connected', 'info');
    document.getElementById('conn-indicator').style.backgroundColor = '#00ff88';
    document.getElementById('conn-indicator').style.boxShadow = '0 0 10px #00ff88';
};
ws.onerror = (err) => {
    console.error("WebSocket Error:", err);
    addLog('WebSocket error!', 'error');
};
ws.onclose = () => {
    addLog('WebSocket disconnected', 'error');
    document.getElementById('conn-indicator').style.backgroundColor = '#ff6b6b';
    document.getElementById('conn-indicator').style.boxShadow = '0 0 10px #ff6b6b';
    
    // Blur values on disconnect
    document.getElementById('val-dist').classList.add('blur-val');
    document.getElementById('val-mode').classList.add('blur-val');
    document.getElementById('val-head-container')?.classList.add('blur-val');
};

ws.onmessage = (event) => {
    const data = event.data.trim();
    
    if (data === "PONG") {
        const ping = Date.now() - pingStart;
        const pingEl = document.getElementById('val-ping');
        if (pingEl) {
            pingEl.textContent = ping;
            pingEl.classList.remove('blur-val');
            if (ping < 50) pingEl.style.color = '#00ff88';
            else if (ping < 150) pingEl.style.color = '#f1c40f';
            else pingEl.style.color = '#ff6b6b';
        }
        return;
    }
    
    // Check if it's a JSON status update
    if (data.startsWith('{') && data.endsWith('}')) {
        try {
            const status = JSON.parse(data);
            updateStatusUI(status);
            return; // Exit after processing status so it doesn't log
        } catch (e) {
            console.error("Failed to parse status JSON:", e);
        }
    }
};

function sendCommand(cmd) {
    if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send(cmd + '\n');
        addLog(cmd);
    }
}

// --- Continuous Hold (for movement buttons) ---
let commandInterval = null;
let activeCommand = null;

function startCommand(cmd) {
    if (activeCommand === cmd) return;
    stopCommand(false);

    activeCommand = cmd;
    sendCommand(cmd);
    commandInterval = setInterval(() => sendCommand(cmd), 100);
}

function stopCommand(shouldSendStop = true) {
    if (commandInterval) {
        clearInterval(commandInterval);
        commandInterval = null;
    }
    if (activeCommand) {
        activeCommand = null;
        if (shouldSendStop) sendCommand('S');
    }
}

// --- Movement D-Pad (hold to repeat) ---
const holdButtons = ['btn-up', 'btn-down', 'btn-left', 'btn-right', 'btn-stop', 'btn-head-left', 'btn-head-right'];

holdButtons.forEach(id => {
    const btn = document.getElementById(id);
    if (!btn) return;
    const cmd = btn.dataset.cmd;

    btn.addEventListener('pointerdown', (e) => {
        e.preventDefault();
        btn.classList.add('active');
        if (cmd === 'S') {
            stopCommand();
        } else {
            startCommand(cmd);
        }
    });

    btn.addEventListener('pointerup', () => {
        btn.classList.remove('active');
        if (cmd !== 'S') stopCommand();
    });
    btn.addEventListener('pointerleave', () => {
        btn.classList.remove('active');
        if (cmd !== 'S') stopCommand();
    });
    btn.addEventListener('pointercancel', () => {
        btn.classList.remove('active');
        if (cmd !== 'S') stopCommand();
    });
    btn.addEventListener('contextmenu', (e) => e.preventDefault());
});

// --- Action Buttons (single press) ---
const actionButtons = ['btn-auto-turn', 'btn-auto-on', 'btn-auto-off', 'btn-led', 'btn-mute', 'btn-reset'];

actionButtons.forEach(id => {
    const btn = document.getElementById(id);
    if (!btn) return;
    const cmd = btn.dataset.cmd;

    btn.addEventListener('pointerdown', (e) => {
        e.preventDefault();
        sendCommand(cmd);
    });
    btn.addEventListener('contextmenu', (e) => e.preventDefault());
});

// --- Keyboard Controls ---
const KEY_MAP = {
    'w': 'F', 'W': 'F',
    'a': 'L', 'A': 'L',
    's': 'B', 'S': 'B',
    'd': 'R', 'D': 'R',
    'c': 'C',
    'e': 'D',
    '1': '1',
    'i': 'I',
    'I': 'I',
    'm': 'M',
    'M': 'M',
};

const keysDown = new Set();

document.addEventListener('keydown', (e) => {
    if (keysDown.has(e.key)) return;
    keysDown.add(e.key);

    const cmd = KEY_MAP[e.key];
    if (cmd) {
        if (['F', 'B', 'L', 'R', 'C', 'D'].includes(cmd)) {
            startCommand(cmd);
        } else {
            sendCommand(cmd);
        }
    }
});

document.addEventListener('keyup', (e) => {
    keysDown.delete(e.key);
    const cmd = KEY_MAP[e.key];
    if (cmd && ['F', 'B', 'L', 'R', 'C', 'D'].includes(cmd)) {
        stopCommand();
    }
});

// --- Clear Log ---
document.getElementById('btn-clear-log')?.addEventListener('click', () => {
    logContainer.innerHTML = '';
    addLog('Log cleared', 'info');
});

// --- Sound System ---
const soundSelect = document.getElementById('sound-select');

function loadSounds() {
    fetch('/api/sounds')
        .then(res => res.json())
        .then(data => {
            if (!soundSelect) return;
            soundSelect.innerHTML = '';
            if (data.sounds.length === 0) {
                const opt = document.createElement('option');
                opt.textContent = 'No sounds available';
                opt.value = '';
                soundSelect.appendChild(opt);
            } else {
                data.sounds.forEach(sound => {
                    const opt = document.createElement('option');
                    opt.textContent = sound;
                    opt.value = sound;
                    soundSelect.appendChild(opt);
                });
            }
        })
        .catch(err => console.error("Error loading sounds", err));
}

if (soundSelect) loadSounds();

document.getElementById('btn-play-sound')?.addEventListener('click', () => {
    const sound = soundSelect?.value;
    if (sound) {
        sendCommand(`play?sound=${sound}`);
    }
});

document.getElementById('btn-delete-sound')?.addEventListener('click', () => {
    const sound = soundSelect?.value;
    if (sound && confirm(`Delete ${sound}?`)) {
        fetch(`/api/sounds/${sound}`, { method: 'DELETE' })
            .then(res => res.json())
            .then(() => loadSounds())
            .catch(err => console.error("Error deleting", err));
    }
});

const uploadInput = document.getElementById('sound-upload');
document.getElementById('btn-upload-sound')?.addEventListener('click', () => {
    uploadInput?.click();
});

uploadInput?.addEventListener('change', (e) => {
    const file = e.target.files[0];
    if (file && file.name.endsWith('.mp3')) {
        const formData = new FormData();
        formData.append('file', file);
        fetch('/api/sounds/upload', {
            method: 'POST',
            body: formData
        }).then(res => res.json())
          .then(() => {
              loadSounds();
              uploadInput.value = ''; // clear
          })
          .catch(err => console.error("Error uploading", err));
    } else if (file) {
        alert("Only MP3 files are allowed.");
    }
});

)rawliteral";

