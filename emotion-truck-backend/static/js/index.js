async function loadStatus() {
    try {
        const data = await apiGet('/ws/status');
        // WebSocket 會即時更新，這裡作為初始載入
    } catch (e) {
        // 改用 REST API
    }
}

async function loadHardwareStatus() {
    try {
        const data = await apiGet('/api/device/status?device_id=ESP32-001');
        document.getElementById('hw-state').textContent = data.current_state || '-';
        document.getElementById('hw-bags').textContent = `${data.bags_count || 0}/5`;
        document.getElementById('hw-daily').textContent = `${data.daily_count || 0}/2`;
        document.getElementById('hw-heartbeat').textContent = 
            data.last_heartbeat ? new Date(data.last_heartbeat).toLocaleTimeString() : '-';
    } catch (e) {
        document.getElementById('hw-state').textContent = '離線';
    }
}

async function loadSimStatus() {
    try {
        const data = await apiGet('/api/device/status?device_id=SIM-001');
        document.getElementById('sim-state').textContent = data.current_state || '-';
        document.getElementById('sim-bags').textContent = `${data.bags_count || 0}/5`;
        document.getElementById('sim-daily').textContent = `${data.daily_count || 0}/2`;
        document.getElementById('sim-heartbeat').textContent = 
            data.last_heartbeat ? new Date(data.last_heartbeat).toLocaleTimeString() : '-';
    } catch (e) {
        document.getElementById('sim-state').textContent = '離線';
    }
}

async function loadSummary() {
    try {
        const hw = await apiGet('/api/stats/summary?source=HARDWARE');
        const sim = await apiGet('/api/stats/summary?source=SIMULATION');
        document.getElementById('hw-total').textContent = hw[0]?.total_collections || 0;
        document.getElementById('sim-total').textContent = sim[0]?.total_collections || 0;
        document.getElementById('total-returned').textContent = 
            (hw[0]?.total_returned || 0) + (sim[0]?.total_returned || 0);
    } catch (e) {
        log(`Summary error: ${e.message}`, true);
    }
}

async function loadSchedule() {
    try {
        const data = await apiGet('/api/schedule/today');
        document.getElementById('schedule-1').textContent = data.play_time_1 || '-';
        document.getElementById('schedule-2').textContent = data.play_time_2 || '-';
        document.getElementById('schedule-holiday').textContent = 
            data.is_holiday ? '是' : '否';
    } catch (e) {
        log(`Schedule error: ${e.message}`, true);
    }
}

// 定期更新
setInterval(() => {
    loadHardwareStatus();
    loadSimStatus();
    loadSummary();
}, 3000);

// 初始載入
loadHardwareStatus();
loadSimStatus();
loadSummary();
loadSchedule();