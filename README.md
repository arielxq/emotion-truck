# Emotion Garbage Truck｜情緒垃圾車

用搖桿選擇情緒，交給一台有脾氣的垃圾車。結合 ESP32 互動硬體、Three.js 3D 展示、Web 互動模擬與 FastAPI/SQL 統計的情緒宣洩體驗原型。

> 本專題是象徵性的互動設計，不是心理治療工具，也不提供醫療建議。

## 主要入口

| 入口 | 檔案 | 你會看到 |
|---|---|---|
| 3D 互動展示 | [`truck/showcase.html`](truck/showcase.html) | Three.js 車體、爆炸視圖、互動圖表、Section 10 硬體操作台 |
| 實驗性 3D 簡報 | [`web/`](web/) | Vite＋Three.js、3D hero、互動狀態展示、簡報模式 |
| 硬體預覽 | [Wokwi](https://wokwi.com/projects/477775315279623169) | ESP32 接線與硬體互動展示 |

## 專案亮點

- **13-state 互動狀態機**：從休息、待機、累積、滿袋、垃圾車前來、抵達、丟出，到三種工作人員回應、清運完成與每日上限。
- **3D 車體展示**：GLB 車體＋電子零件、OrbitControls、三點照明、爆炸／組回視圖與響應式畫布。
- **互動模擬**：showcase Section 10 的 LCD1602／搖桿／LED／蜂鳴器操作台，以及 `web/` 的 Vite 3D 互動簡報。
- **資料層**：FastAPI＋SQLAlchemy／SQL，提供模擬流程、裝置狀態與統計 API。
- **實機硬體**：ESP32、HW504 搖桿、LCD1602、單色 LED、無源蜂鳴器，共 7 個 GPIO。

## 技術架構

| 層 | 技術 | 責任 |
|---|---|---|
| 互動硬體 | ESP32、PlatformIO、HW504、LCD1602、LED、蜂鳴器 | 13-state 狀態機、GPIO 輸入輸出、實機音效 |
| 3D 展示 | Three.js r160、GLTFLoader、GLB、OrbitControls | 車體與電子零件、爆炸視圖、互動圖表 |
| 互動模擬 | Vanilla JS、Web Audio、Vite＋Three.js | Section 10 操作台、`web/` 3D 互動簡報 |
| 資料層 | FastAPI、SQLAlchemy、SQL | 模擬流程、裝置狀態、統計 API |
| 硬體預覽 | Wokwi | 外部電路與硬體展示 |

`showcase.html` 的 Section 10 與 `web/` 目前都是純前端 demo；FastAPI／SQL 後端保留模擬流程、裝置狀態與統計 API，尚未與前端做即時同步。

## 3D 展示

### Showcase（主要入口）

`truck/showcase.html` 是主要 3D 展示：

- Three.js r160＋GLTFLoader，載入 `truck/models/01_Truck_Body.glb` 與 `02_Electronics.glb`。
- OrbitControls：可旋轉、縮放，限制極角與距離；三點照明與柔和陰影。
- 「拆解／組回」爆炸視圖：輪子、車廂、LED、喇叭、蜂鳴器、車體、LCD、ESP32、垃圾桶、搖桿等零件平滑位移。
- 材質 fallback 使用大地色票，模型缺材質時仍能維持一致的視覺。
- ResizeObserver 自動調整畫布，桌機與手機都能瀏覽。

Showcase 其他展示：

- 6 張品牌化 SVG 圖表：狀態機、時間時序、工作人員回應機率、bags Sankey、使用者旅程、系統架構；支援拖曳、滾輪縮放與 Lightbox。
- Section 10 全寬硬體操作台：LCD1602 16×2、搖桿方向／SW、LED、蜂鳴器、8 個流程按鈕；展示節奏 3 秒（實機 10 秒），1100px 以下兩欄、620px 以下單欄。

> 3D 頁面需用 HTTP server 開啟；直接雙擊 `file://` 可能無法載入 GLB。Three.js 由 CDN 載入，首次開啟需連網。

### Web（實驗性）

`web/` 是 Vite＋Three.js 的獨立實驗頁，適合展示與簡報：

- `web/src/main.js`：OBJ/MTL 3D hero、情緒選擇、旋轉與狀態流程、簡報模式。
- 使用 `web/public/models/` 的車體模型；`npm run dev` 啟動。
- 目前是純前端互動展示，尚未接 FastAPI，也不是主要硬體操作入口。

## 互動模擬

### Showcase Section 10

- LCD1602 16×2：第一行狀態、第二行第 7 欄（0-indexed）5×8 表情。
- 搖桿：上／左／右／下＋中央 SW；同方向 3 次後才能確認。
- `ARRIVED` 時旋轉三圈（1080°）進入 `THROW_READY`，再按 SW 丟出。
- LED 節奏與蜂鳴器狀態同步；「啟用音效」預設關閉。
- 8 個流程按鈕：`REST_CLOSED`、`進入 IDLE`、`垃圾車前來`、`直接到 ARRIVED`、`旋轉一圈`、`reset day`、`SW 確認`、`重設`。
- 展示節奏 3 秒（實機 10 秒）；手機版自動換行、單欄排列。

### Showcase vs Web

| | Showcase Section 10 | Web |
|---|---|---|
| 定位 | 硬體操作模擬 | 3D 互動簡報 |
| 3D | GLB 車體＋爆炸視圖 | OBJ/MTL 3D hero |
| 操作 | LCD／搖桿／LED／蜂鳴器 | 情緒選擇、旋轉、狀態流程 |
| 資料 | 純前端 | 純前端 |
| 節奏 | 3 秒展示（實機 10 秒） | 即時互動 |

## 硬體與互動規則

狀態流程：

```text
REST_CLOSED → IDLE → ACCUMULATING → FULL → APPROACHING
→ ARRIVED → THROW_READY → THROWING
→ RESPONDING_CAREFUL / RESPONDING_THANKS / RESPONDING_RECYCLE
→ COLLECTED → IDLE 或 REST_DAILY_LIMIT
```

| 元件 | 接點 |
|---|---|
| HW504 VRx / VRy / SW | GPIO34 / GPIO35 / GPIO32 |
| LCD1602 I2C SDA / SCL | GPIO21 / GPIO22，位址通常為 `0x27` |
| 無源蜂鳴器 | GPIO25 |
| 單色 LED | GPIO27，串 220Ω 電阻 |

互動規則：

- 同方向推 3 次後按 SW 確認，bags +1；bags 上限 5。
- `ARRIVED` 時旋轉三圈（累積 1080°）進入 `THROW_READY`，再按 SW 丟出。
- `CAREFUL`／`THANKS` 收走全部；`RECYCLE` 隨機保留 1～原有 bags（原有 0 包時維持 0）。
- 每日清運上限 2 次；`reset day` 只清除每日次數，`重設` 才將 bags 歸零。
- LCD1602 第一行顯示狀態，第二行第 7 欄（0-indexed）顯示 5×8 表情。
- 單色 LED 與蜂鳴器依狀態切換節奏與音效；詳細對照見 `docs/state-and-hardware-spec.md`。

## 快速開始

### 3D 展示

```bash
python3 -m http.server 8080
```

開啟 <http://127.0.0.1:8080/truck/showcase.html>。

### Web 實驗頁

```bash
cd web
npm install
npm run dev
```

依 Vite 顯示的網址開啟。

### 啟動 FastAPI

```bash
cd emotion-truck-backend
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
uvicorn app.main:app --reload
```

模擬資料使用 `device_id=SIM-001`、`source=SIMULATION`。主要 API：

| 類型 | 端點 |
|---|---|
| 模擬流程 | `POST /api/sim/state`、`POST /api/sim/emotion`、`POST /api/sim/collection`、`POST /api/sim/reset` |
| 裝置狀態 | `GET /api/device/status?device_id=SIM-001` |
| 統計 | `GET /api/stats/emotion`、`GET /api/stats/weekly`、`GET /api/stats/summary` |
| 即時狀態 | `WS /ws/status` |

目前前端 demo 不即時呼叫這些 API；API 與 SQL 仍可獨立測試。

### ESP32 韌體

```bash
pio run -d platformio
pio run -d platformio -t upload
```

實機燒錄、旋轉、LCD、LED 與蜂鳴器結果請填寫在 `docs/hardware-test-record.md`；Web 模擬通過不等於 ESP32 實機通過。

## 專案結構

```text
truck/showcase.html        主要 3D 展示 + 圖表 + Section 10 硬體操作台
truck/models/              GLB / OBJ / MTL 模型
web/                       Vite + Three.js 實驗性 3D 簡報
web/src/main.js            3D hero、互動狀態、簡報模式
platformio/src/            ESP32 韌體與音效
sounds/                    原始音效音符來源
emotion-truck-backend/     FastAPI、SQL schema 與服務
diagrams/                  狀態機、時序、Sankey 等 SVG 來源
scripts/sync-diagrams.py   將 diagrams/ 同步到 showcase
docs/                      規格、驗證、實作與硬體測試紀錄
```

## 限制與下一步

- `showcase.html` 的 Section 10 與 `web/` 都是純前端 demo，不寫入 API 或 SQL。
- FastAPI／SQL 後端已具備模擬、裝置與統計 API，但尚未與前端做即時雙向同步。
- Wokwi 是外部硬體展示連結，不嵌入主要操作頁，也不會自動同步 Web 狀態。
- 3D showcase 需要 HTTP server 與 CDN；`web/` 需以 Vite 啟動。
- FastAPI／SQL 正式部署、ESP32 正式上報與 worker response 分布統計仍待驗證。
- 詳細規格請見 [`docs/state-and-hardware-spec.md`](docs/state-and-hardware-spec.md)、[`docs/verification-matrix.md`](docs/verification-matrix.md)、[`docs/implementation-log.md`](docs/implementation-log.md) 與 [`docs/hardware-test-record.md`](docs/hardware-test-record.md)。
