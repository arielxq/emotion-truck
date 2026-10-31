# Emotion Truck Backend

FastAPI + SQLAlchemy／MySQL backend for simulation events, device status, and statistics.

## Setup

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
uvicorn app.main:app --reload
```

Database connection is configured with `DB_HOST`, `DB_PORT`, `DB_USER`, `DB_PASSWORD`, and `DB_NAME` in `.env`. The schema and demo rows are in `sql/schema.sql` and `sql/seed.sql`.

## Pages

- `/`：主頁
- `/sim`：模擬控制台
- `/history`：歷史記錄
- `/stats`：統計圖表（含情緒、每週清運、工作人員回應）

## API

| Method | Path | Description |
|---|---|---|
| POST | `/api/sim/state` | 寫入模擬狀態 |
| POST | `/api/sim/emotion` | 寫入模擬情緒事件 |
| POST | `/api/sim/collection` | 寫入模擬清運事件 |
| POST | `/api/sim/reset` | 重設模擬資料 |
| GET | `/api/device/status?device_id=SIM-001` | 查詢裝置狀態 |
| GET | `/api/stats/emotion?source=HARDWARE` | 情緒分布 |
| GET | `/api/stats/weekly?source=HARDWARE` | 最近 7 天清運次數 |
| GET | `/api/stats/summary?source=HARDWARE` | 清運摘要 |
| GET | `/api/stats/responses?source=HARDWARE` | 工作人員回應分布（CAREFUL／THANKS／RECYCLE） |
| WS | `/ws/status` | 即時狀態 |

`source` 一般為 `HARDWARE` 或 `SIMULATION`；`/api/stats/responses` 另直接支援 `ALL`。統計頁的「全部」由前端合併兩個來源。`truck/showcase.html` 的 Section 10b 目前使用示範資料，不即時呼叫這些端點。
