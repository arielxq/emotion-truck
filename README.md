# Emotion Garbage Truck｜情緒垃圾車

用搖桿選擇情緒，交給一台有脾氣的垃圾車。這是一個結合 ESP32 互動硬體、Web 模擬器與 FastAPI/SQL 統計的情緒宣洩體驗原型。

> 本專題是象徵性的互動設計，不是心理治療工具，也不提供醫療建議。

## 目前完成內容

- ESP32 13-state 狀態機：`REST_CLOSED`、`IDLE`、`ACCUMULATING`、`FULL`、`APPROACHING`、`ARRIVED`、`THROW_READY`、`THROWING`、三種 `RESPONDING_*`、`COLLECTED`、`REST_DAILY_LIMIT`
- HW504 兩軸搖桿：情緒累積使用上／左／右，清運前使用 `atan2()` 累積 1080°（三圈）
- LCD1602 I2C、單色 LED、無源蜂鳴器
- 原硬體音符：累積、FULL、throw、工作人員回應與 Fur Elise 接近／離開音樂
- bags 上限 5；`CAREFUL`／`THANKS` 收走最多 5 包，`RECYCLE` 收走 1–4 包並保留剩餘
- 每日清運上限 2 次；`reset day` 只清除每日次數，不清除剩餘 bags
- `truck/simulator.html` Game Boy 風格 Web 模擬器、虛擬搖桿、操作說明、測試控制、Web Audio 與 Chart.js 統計
- FastAPI simulation/device/stats API；API 不可用時，Web 頁面會使用明確標示的 demo fallback
- Wokwi 硬體展示：<https://wokwi.com/projects/477775315279623169>

## 狀態流程

```text
REST_CLOSED → IDLE → ACCUMULATING → FULL → APPROACHING
→ ARRIVED → THROW_READY → THROWING → RESPONDING_CAREFUL / RESPONDING_THANKS / RESPONDING_RECYCLE
→ COLLECTED → IDLE 或 REST_DAILY_LIMIT
```

## 專案結構

```text
truck/index.html                 原始 3D 展示頁（保留）
truck/showcase.html              主要 3D 展示頁與硬體入口
truck/simulator.html             Web 模擬器與統計頁
platformio/src/main.cpp          ESP32 韌體
platformio/src/sounds.h          硬體音符與音效規格
sounds/                          原始音效音符來源
emotion-truck-backend/            FastAPI、SQL schema 與服務
  app/routers/sim.py             模擬狀態、情緒與清運 API
  app/routers/stats.py           統計 API
  sql/schema.sql                 資料表定義
docs/                            狀態、測試與實作紀錄
```

## 啟動 Web 與後端

### 只預覽前端

```bash
python3 -m http.server 8080
```

開啟 <http://127.0.0.1:8080/truck/simulator.html>。

### 啟動 FastAPI

```bash
cd emotion-truck-backend
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
uvicorn app.main:app --reload
```

Web 模擬器會優先呼叫 API；若 API 或資料庫不可用，頁面會切換到 demo-data fallback，不代表資料已寫入 SQL。

主要 API：

```text
GET  /api/stats/emotion
GET  /api/stats/weekly
GET  /api/stats/summary
GET  /api/device/status?device_id=SIM-001
POST /api/sim/state
POST /api/sim/emotion
POST /api/sim/collection
```

模擬資料使用 `device_id=SIM-001`、`source=SIMULATION`。後端仍保留 SQL 相容欄位 `returned_bags`，前端與文件以「collected bags／收走數量」理解。

## ESP32 硬體

| 元件 | 接點 |
|---|---|
| HW504 VRx / VRy / SW | GPIO34 / GPIO35 / GPIO32 |
| LCD1602 I2C SDA / SCL | GPIO21 / GPIO22，位址通常為 `0x27` |
| 無源蜂鳴器 | GPIO25 |
| 單色 LED | GPIO27，串 220Ω 電阻 |

PlatformIO 建置：

```bash
pio run -d platformio
pio run -d platformio -t upload
```

實機燒錄、旋轉、LCD、LED 與蜂鳴器結果請填寫在 `docs/hardware-test-record.md`；Web 模擬器通過不等於 ESP32 實機通過。

## 目前限制與下一步

- Web 模擬器與 ESP32 使用相同狀態、bags、每日上限與音效規格，但目前是可獨立展示的模擬流程，不是與硬體的雙向即時同步。
- Wokwi 是外部硬體展示連結，不嵌入為主要操作頁，也不會自動同步 Web 模擬器狀態。
- FastAPI/SQL 的正式部署、ESP32 正式上報與 worker response 分布統計仍需依實際資料與環境完成驗證。
- 詳細規格請見 `docs/state-and-hardware-spec.md`、`docs/verification-matrix.md` 與 `docs/implementation-log.md`。
