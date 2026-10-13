# 硬體、Web 與後端驗證矩陣

## 狀態定義

| 標記 | 意義 |
|---|---|
| ✅ | 已完成程式或已觀察到的測試結果 |
| 🧪 | 程式已建立，尚待對應平台驗證 |
| ⏳ | 尚未開始測試 |
| ⚠️ | 已知限制或需額外確認 |

> Web 模擬器通過不等於 ESP32 實機通過。Wokwi、Web 與實體硬體分開記錄。

## ESP32 硬體與韌體

| 項目 | Build | 已燒錄 | 實機 | 備註 |
|---|---:|---:|---:|---|
| LCD I2C 掃描與 `0x27` | ✅ | ✅ | ✅ | 位址已確認 |
| LCD 背光與文字 | ✅ | ✅ | ✅ | 對比度可能需調整 |
| HW504 VRx / VRy / SW | ✅ | ⏳ | ⏳ | GPIO34 / GPIO35 / GPIO32 |
| 單色 LED | ✅ | ⏳ | ⏳ | GPIO27，220Ω；取代早期 RGB LED |
| 無源蜂鳴器 | ✅ | ⏳ | ⏳ | GPIO25，Core 3.x LEDC |
| LCD、LED、搖桿、蜂鳴器整合 | ✅ | ⏳ | ⏳ | 待完整接線測試 |
| 13-state 本地狀態機 | ✅ | ⏳ | ⏳ | 需逐狀態實機確認 |
| 情緒累積三次與 bags 上限 5 | ✅ | ⏳ | ⏳ | `ACCUMULATING` → `IDLE`/`FULL` |
| `atan2()` 360°／720°／1080° | ✅ | ⏳ | ⏳ | 三圈後 `THROW_READY` |
| `THROWING` 不可取消 | ✅ | ⏳ | ⏳ | 約 1 秒後回應 |
| 三種工作人員回應與結算 | ✅ | ⏳ | ⏳ | 50%／25%／25% |
| `dailyCollectCount` 上限 2 | ✅ | ⏳ | ⏳ | 超過後阻擋正式流程 |
| `reset day` 保留 bags | ✅ | ⏳ | ⏳ | 清除每日次數 |
| APPROACHING 非阻塞音樂與 LED | 🧪 | ⏳ | ⏳ | Fur Elise 約 10 秒 |
| COLLECTED 漸弱與離開流程 | 🧪 | ⏳ | ⏳ | 待實機確認時序 |

## Web simulator

| 項目 | 狀態 | 備註 |
|---|---:|---|
| `truck/simulator.html` 載入 | ✅ | 獨立靜態頁 |
| Game Boy 虛擬搖桿與 A/B | ✅ | 方向、確認、重設 |
| 13-state 流程 | ✅ | 含三種 `RESPONDING_*` |
| 固定 LCD／控制區尺寸 | ✅ | 狀態文字變化不造成版面跳動 |
| 1080°／三次旋轉展示 | ✅ | Web 以「旋轉一圈」測試按鈕展示 |
| Web Audio 硬體音符 | ✅ | 優先使用 `sounds.h`／`sounds/` 對應音符 |
| Fur Elise 接近／離開 | ✅ | 約 10 秒；瀏覽器需先啟用音效 |
| 音效失敗不阻斷狀態機 | ✅ | AudioContext 例外隔離 |
| Chart.js 四類統計 | 🧪 | API 可用時使用後端，否則 demo fallback |
| FastAPI simulation API | 🧪 | 需啟動後端驗證寫入結果 |
| 手機響應式操作 | 🧪 | 待不同瀏覽器人工測試 |
| Wokwi 外部連結 | ✅ | 不嵌入、不與 Web 雙向同步 |

## Backend / SQL

| 項目 | 狀態 | 備註 |
|---|---:|---|
| `/api/sim/state` | 🧪 | `SIM-001`、`source=SIMULATION` |
| `/api/sim/emotion` | 🧪 | 寫入情緒事件 |
| `/api/sim/collection` | 🧪 | `returned_bags` 為相容欄位，語意為收走數量 |
| `/api/device/status` | 🧪 | 裝置狀態查詢 |
| `/api/stats/emotion` | 🧪 | 情緒分布 |
| `/api/stats/weekly` | 🧪 | 每週清運 |
| `/api/stats/summary` | 🧪 | 摘要與目前統計 |
| demo-data fallback | ✅ | API 失敗時保留展示能力 |

## 更新規則

- 只有實際燒錄並觀察硬體結果後，才將 ESP32 實機欄改為 ✅。
- 只有 Build 成功，不代表實機功能完成。
- Web 測試結果不能直接移植到硬體欄位。
- 每次狀態或接線變更，應同步 `docs/state-and-hardware-spec.md` 與 `docs/implementation-log.md`。
