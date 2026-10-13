let currentSource = 'HARDWARE';
let emotionChart = null;
let weeklyChart = null;

const EMOTION_LABELS = {
    'GAN': 'ㄍㄢˋ · 憤怒',
    'KAO': 'ㄎㄠˋ · 不爽',
    'CAO': 'ㄘㄠˋ · 煩躁'
};

const EMOTION_COLORS = {
    'GAN': '#e74c3c',
    'KAO': '#e67e22',
    'CAO': '#f1c40f'
};

async function fetchEmotionStats() {
    const sources = currentSource === 'ALL'
        ? ['HARDWARE', 'SIMULATION']
        : [currentSource];

    const totals = {};
    for (const src of sources) {
        const data = await apiGet(`/api/stats/emotion?source=${src}`);
        data.forEach(row => {
            const key = row.emotion_type || 'UNKNOWN';
            totals[key] = (totals[key] || 0) + row.count;
        });
    }
    return totals;
}

async function fetchWeeklyStats() {
    const sources = currentSource === 'ALL'
        ? ['HARDWARE', 'SIMULATION']
        : [currentSource];

    const totals = {};
    for (const src of sources) {
        const data = await apiGet(`/api/stats/weekly?source=${src}`);
        data.forEach(row => {
            const key = String(row.date).slice(0, 10);
            totals[key] = (totals[key] || 0) + row.count;
        });
    }
    return totals;
}

async function fetchSummary() {
    const sources = currentSource === 'ALL'
        ? ['HARDWARE', 'SIMULATION']
        : [currentSource];

    let total = 0, returned = 0, sumBags = 0, count = 0;
    for (const src of sources) {
        const data = await apiGet(`/api/stats/summary?source=${src}`);
        const row = data[0] || {};
        total += row.total_collections || 0;
        returned += row.total_returned || 0;
        if (row.avg_bags) {
            sumBags += row.avg_bags * (row.total_collections || 0);
            count += row.total_collections || 0;
        }
    }
    return {
        total,
        returned,
        avg: count > 0 ? (sumBags / count).toFixed(2) : '0'
    };
}

function renderEmotionChart(totals) {
    const labels = Object.keys(totals).map(k => EMOTION_LABELS[k] || k);
    const values = Object.values(totals);
    const colors = Object.keys(totals).map(k => EMOTION_COLORS[k] || '#888');

    if (emotionChart) emotionChart.destroy();

    const ctx = document.getElementById('emotionChart').getContext('2d');
    emotionChart = new Chart(ctx, {
        type: 'doughnut',
        data: {
            labels,
            datasets: [{
                data: values,
                backgroundColor: colors,
                borderColor: '#16213e',
                borderWidth: 2
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            plugins: {
                legend: {
                    labels: { color: '#eee' }
                }
            }
        }
    });
}

function renderWeeklyChart(totals) {
    const dates = Object.keys(totals).sort();
    const values = dates.map(d => totals[d]);

    if (weeklyChart) weeklyChart.destroy();

    const ctx = document.getElementById('weeklyChart').getContext('2d');
    weeklyChart = new Chart(ctx, {
        type: 'bar',
        data: {
            labels: dates,
            datasets: [{
                label: '清運次數',
                data: values,
                backgroundColor: '#e94560',
                borderRadius: 4
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            plugins: {
                legend: {
                    labels: { color: '#eee' }
                }
            },
            scales: {
                x: {
                    ticks: { color: '#eee' },
                    grid: { color: '#0f3460' }
                },
                y: {
                    beginAtZero: true,
                    ticks: { color: '#eee', stepSize: 1 },
                    grid: { color: '#0f3460' }
                }
            }
        }
    });
}

async function loadAll() {
    try {
        const emotions = await fetchEmotionStats();
        renderEmotionChart(emotions);

        const weekly = await fetchWeeklyStats();
        renderWeeklyChart(weekly);

        const summary = await fetchSummary();
        document.getElementById('stat-total').textContent = summary.total;
        document.getElementById('stat-returned').textContent = summary.returned;
        document.getElementById('stat-avg').textContent = summary.avg;
    } catch (e) {
        console.error('Load stats failed:', e);
    }
}

function bindFilterButtons() {
    document.querySelectorAll('.filter-btn').forEach(btn => {
        btn.addEventListener('click', () => {
            document.querySelectorAll('.filter-btn').forEach(b =>
                b.classList.remove('active')
            );
            btn.classList.add('active');
            currentSource = btn.dataset.source;
            loadAll();
        });
    });
}

document.addEventListener('DOMContentLoaded', () => {
    bindFilterButtons();
    loadAll();
    setInterval(loadAll, 10000);
});