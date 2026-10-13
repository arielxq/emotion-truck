const API_BASE = '';

async function apiPost(path, data) {
    const res = await fetch(API_BASE + path, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(data)
    });
    if (!res.ok) throw new Error(`API error: ${res.status}`);
    return res.json();
}

async function apiGet(path) {
    const res = await fetch(API_BASE + path);
    if (!res.ok) throw new Error(`API error: ${res.status}`);
    return res.json();
}

function log(message, isError = false) {
    const output = document.getElementById('log-output');
    if (!output) return;
    const time = new Date().toLocaleTimeString('zh-TW', { hour12: false });
    const entry = document.createElement('div');
    entry.className = 'log-entry' + (isError ? ' error' : '');
    entry.textContent = `[${time}] ${message}`;
    output.appendChild(entry);
    output.scrollTop = output.scrollHeight;
}