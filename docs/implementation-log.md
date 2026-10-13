# 情緒垃圾車 — 施行日誌

---

## 2026-10-12｜13 狀態正式規格、bags 繼承與旋轉測試整合

---

## 2026-10-13｜依第一輪實機測試修正旋轉、LED 與清運上限

### 實機發現

- `APPROACHING` 的 LED 只閃爍一次，原因是阻塞式 Fur Elise 播放期間無法執行 LED 更新。
- 四區段 12 次旋轉會產生 `Invalid rotation step`，與實際「旋轉 3 圈」操作不一致。
- 測試指令可在每日上限後繼續觸發清運，造成 `dailyCollectCount` 超過 2。
- `returnedBagCount` 實際代表垃圾車收走數量，命名容易與退回數量混淆。

### 修正

- 加入非阻塞 `startTruckMusic()`／`updateTruckMusic()`，APPROACHING 期間可持續更新 LED，並在音樂完成後進入 ARRIVED。
- 使用 `atan2()` 取得搖桿角度，累積同方向角度；360°、720°、1080° 分別代表 1、2、3 圈。
- `dailyCollectCount` 增加時加上 2 次上限保護。
- 將 `returnedBagCount` 改為 `collectedBagCount`，Serial 改顯示 `Collected X bags, remaining=Y`。
- REST_DAILY_LIMIT 下的 `state careful/thanks/recycle/collected` 受到限制；需要單獨測試時使用 `test careful` 等指令。
- `COLLECTED` 使用非阻塞 Fur Elise 漸弱，完成音樂或最短顯示時間後才進入下一狀態。

### 後續實機測試

- 重新確認 APPROACHING LED 是否持續 300/300 ms 閃爍。
- 確認 LCD 顯示 `Rotate 1/3`、`Rotate 2/3`、`Rotate 3/3`。
- 確認 1080° 後進入 THROW_READY，不再出現 `Invalid rotation step`。
- 確認 dailyCollectCount 永遠不超過 2。
- 確認回收 Serial 使用 collected／remaining 語意。

### 正式狀態

ESP32 韌體與對照文件統一使用：

```text
REST_CLOSED → IDLE → ACCUMULATING → FULL → APPROACHING
→ ARRIVED → THROW_READY → THROWING → RESPONDING_*
→ COLLECTED → IDLE 或 REST_DAILY_LIMIT
```

### bags 與每日清運次數

- `RESPONDING_CAREFUL`／`RESPONDING_THANKS`：`bags = max(0, bags - 5)`。
- `RESPONDING_RECYCLE`：隨機減少 1～4 包，但不超過目前 bags。
- `COLLECTED` 不再清零 bags，只增加 `dailyCollectCount`。
- `dailyCollectCount < 2`：回到 `IDLE`，繼承剩餘 bags。
- `dailyCollectCount >= 2`：進入 `REST_DAILY_LIMIT`；bags 內部保留，但不顯示、不接受輸入、不累積。
- `reset day` 只清除每日清運次數並進入 `IDLE`，不清除 bags。
- `reset` 清除 bags 與每日清運次數，供完整測試重置使用。

### 搖桿旋轉流程

- `ARRIVED` 不再使用左／右／上固定計數。
- 任意方向開始，之後以相鄰方向判斷順時針或逆時針。
- UP、RIGHT、DOWN、LEFT 四個區段完成 12 段，即旋轉 3 圈。
- 每一段需回到中央；中途反向的步驟忽略。
- 12 段完成後進入 `THROW_READY`，播放提示音並等待 SW。
- SW 後進入不可取消的 `THROWING`。

### LCD／LED／音效

- LCD 使用 8 個自訂字元：第一行文字、第二行置中表情。
- `REST_CLOSED`、`IDLE`、`REST_DAILY_LIMIT` LED 熄滅。
- `APPROACHING` 播放 Fur Elise 完整接近段，完成後進入 `ARRIVED`。
- `THROW_READY` 播放丟垃圾提示音；`THROWING` 不接受搖桿或按鍵。

### 文件同步

- 新增 `docs/state-and-hardware-spec.md`，集中保存系統架構、接線、狀態、bags、LED、搖桿、音效與 LCD 對照表。
- 更新 `docs/verification-matrix.md`，將旋轉 3 圈、每日清運次數與 `reset day` 納入待實機驗證項目。
- Proposal 的正式狀態與硬體圖表待依此規格同步，避免舊版 REST／RETURNED／RGB 描述混用。

### Build

- PlatformIO Build：✅ 成功
- 實體旋轉、LCD 表情、蜂鳴器與 LED：⏳ 待燒錄驗證

## 專案資訊

- 專案名稱：Emotion Garbage Truck
- 開始日期：2026-10-10
- 目標：完成可由 ESP32、蜂鳴器、LCD、搖桿與 RGB LED 操作的互動原型
- 硬體平台：ESP32 Dev Module
- 開發工具：VS Code + PlatformIO
- Arduino 框架：Arduino-ESP32 Core 3.x（目前 Build log 為 3.3.12）

> 本日誌記錄實際操作、測試結果、問題與解法。企劃書只在階段驗證完成後更新最終設計。

---

## 2026-10-10｜Day 1：專案結構與硬體接線

### 任務

- 建立 FastAPI、前端與 PlatformIO 專案結構
- 確認 ESP32 硬體接線
- 建立硬體測試程式的初始版本

### 實際接線

| 元件 | 腳位 | ESP32 |
|---|---|---:|
| HW504 | VRx | GPIO 34 |
| HW504 | VRy | GPIO 35 |
| HW504 | SW | GPIO 32 |
| 無源蜂鳴器 | + | GPIO 25 |
| 無源蜂鳴器 | - | GND |
| LCD1602 I2C | SDA | GPIO 21 |
| LCD1602 I2C | SCL | GPIO 22 |
| RGB LED 紅色 | R | GPIO 27，串 220Ω |
| RGB LED 綠色 | G | GPIO 14，串 220Ω |
| RGB LED 藍色 | B | GPIO 26，串 220Ω |
| RGB LED 公共腳 | GND | GND |

### 結果

- 接線表已確認。
- RGB LED 確認為四腳共陰極設計。
- LCD I2C 位址初始以 `0x27` 測試。

### 下一步

- 以單一元件方式測試 LCD。

---

## 2026-10-10｜Day 1：LCD1602 個別測試

### 任務

- 測試 LCD1602 I2C 通訊與文字顯示。

### 操作

1. 使用 PlatformIO 建立 LCD 專用測試程式。
2. 初始化 SDA `GPIO 21`、SCL `GPIO 22`。
3. 掃描 I2C 位址。
4. 使用 `LiquidCrystal_I2C` 初始化 LCD。
5. 顯示 `Emotion Truck` 與 LCD 測試文字。

### 結果

- PlatformIO Build：✅ 成功
- LCD 背光：✅ 正常
- LCD 文字：✅ 已確認可顯示
- LCD I2C 位址：`0x27`

### 問題與解法

| 問題 | 解法 |
|---|---|
| LCD 有背光但沒有文字 | 調整 LCD 背面的對比度電位器 |
| LCD 位址不確定 | 使用 I2C scanner；本次確認為 `0x27` |

### 最終確認

目前 LCD 測試程式使用：

```cpp
const uint8_t LCD_ADDRESS = 0x27;
```

### 下一步

- 測試 GPIO 25 的無源蜂鳴器。

---

## 2026-10-11｜Day 2：Arduino-ESP32 Core 3.x 與蜂鳴器程式

### 任務

- 統一所有硬體測試使用 Arduino-ESP32 Core 3.x API。
- 建立蜂鳴器單獨測試程式。

### 操作

1. 先以官方 `espressif32` 平台嘗試指定 Arduino Core 3.x。
2. 由 Build log 檢查實際 framework 版本。
3. 發現官方 PlatformIO 平台版本與 Core 3.x 整合不完整。
4. 改用 pioarduino `platform-espressif32` 平台。
5. 使用 Core 3.x GPIO-based LEDC API：

```cpp
ledcAttach(BUZZER_PIN, frequency, resolution);
ledcWriteTone(BUZZER_PIN, frequency);
```

6. 建立 440 Hz 與 880 Hz 兩段測試音。

### 結果

- pioarduino 平台安裝：✅
- Build log 確認 `framework-arduinoespressif32 @ 3.3.12`：✅
- 蜂鳴器測試程式編譯：✅
- 440 Hz 實機聲音：⏳ 尚待燒錄確認
- 880 Hz 實機聲音：⏳ 尚待燒錄確認

### 問題與解法

| 問題 | 解法 |
|---|---|
| Core 2.x 的 `ledcSetup()` / `ledcAttachPin()` 與目標版本不一致 | 改用 Core 3.x 的 `ledcAttach(pin, frequency, resolution)` |
| `ledcWriteTone()` 參數使用方式不同 | Core 3.x 使用 GPIO pin，而不是 LEDC channel |
| 官方 PlatformIO 平台直接接 Core 3.0 framework 缺少 libs | 改用 pioarduino 平台，取得配套 framework 與 ESP-IDF libraries |

### 下一步

- 燒錄蜂鳴器測試程式。
- 以 Serial Monitor 115200 baud 確認播放流程。
- 實際確認 440 Hz 與 880 Hz 是否有聲音及音高差異。

---

## 2026-10-11｜Day 2：四元件硬體整合測試程式

### 任務

- 將已個別驗證的 LCD、蜂鳴器、搖桿與四腳 RGB LED 放入同一個韌體。
- 維持 Arduino-ESP32 Core 3.x 的 GPIO-based LEDC API。

### 操作

1. 將 `main.cpp` 改為整合測試版本。
2. 初始化 LCD1602 I2C，使用已確認的 `0x27`。
3. 初始化 HW504：VRx `GPIO 34`、VRy `GPIO 35`、SW `GPIO 32`。
4. 初始化蜂鳴器 `GPIO 25`。
5. 初始化共陰極 RGB LED：R `GPIO 27`、G `GPIO 14`、B `GPIO 26`。
6. 使用 Core 3.x `ledcAttach(pin, frequency, resolution)` 與 `ledcWrite(pin, value)`。
7. 方向改變時同步更新 Serial、LCD 與 RGB 顏色。
8. 按下搖桿 SW 時播放 880 Hz 確認音。

### 結果

- 整合程式 Build：✅ 成功
- Framework：✅ Arduino-ESP32 Core 3.3.12
- LCD 實機整合：⏳ 待燒錄確認
- 蜂鳴器實機整合：⏳ 待燒錄確認
- 搖桿實機整合：⏳ 待燒錄確認
- RGB LED 實機整合：⏳ 待燒錄確認

### 預期測試流程

```text
搖桿中心 → LCD CENTER、RGB 藍色
推上     → LCD UP、RGB 紅色、Serial 顯示 GAN
推左     → LCD LEFT、RGB 紫色、Serial 顯示 KAO
推右     → LCD RIGHT、RGB 黃色、Serial 顯示 CAO
按下 SW  → 蜂鳴器播放 880 Hz
```

### 下一步

- 燒錄整合測試程式。
- 用 Serial Monitor 115200 baud 觀察 X/Y/SW、方向與情緒。
- 確認 LCD、RGB 顏色與蜂鳴器能否在同一支韌體中同時工作。

---

## 目前進度摘要

---

## 2026-10-11｜Day 2：垃圾車到達後的丟垃圾驗證流程

### 修正內容

- `ARRIVED` 狀態加入左、右、上三方向各 3 次的丟垃圾手勢計數。
- 完成條件改為 `L3 + R3 + U3`，不是單純總數達 9 次。
- 按鍵確認在方向判斷前處理，避免搖桿停留在最後方向時無法送出。
- 按鍵沿用 debounce 與按下邊緣觸發，避免持續按住造成重複進入。
- `ARRIVED` 使用單色 LED 長亮，不再啟動零間隔閃爍。
- `THROWING` 結束後以 `millis()` 自動隨機進入三種工作人員回應。
- 「這是回收」回應時隨機退回 1～4 包，但不超過目前累積數量，LCD 顯示實際退回數量。
- `RESPONDING` 與 `COLLECTED` 的流程改為非阻塞時間轉移。

### 新增測試指令

```text
state arrived
throw reset
status
```

### 預期丟垃圾測試

```text
state arrived
左 → 中央，重複 3 次
右 → 中央，重複 3 次
上 → 中央，重複 3 次
按下 SW
```

預期狀態：

```text
ARRIVED → THROWING → RESPONDING_* → COLLECTED 或 IDLE
```

### 驗證狀態

- 程式 Build：✅ 成功
- 實體丟垃圾手勢：⏳ 待燒錄確認
- 三方向各 3 次計數：⏳ 待燒錄確認
- 自動工作人員回應：⏳ 待燒錄確認
- 回收退回數量與 LCD：⏳ 待燒錄確認

---

## 2026-10-11｜Day 2：丟垃圾測試的 DOWN 行為與音樂入口修正

### 問題

- `ARRIVED` 狀態進行丟垃圾手勢時，推下會被誤當成取消。
- 直接使用 `state arrived` 時，本來就會跳過接近音樂，容易誤以為音樂沒有播放。

### 修正

- 在 `ARRIVED` 的丟垃圾流程中，DOWN 改為忽略，不清除 L/R/U 計數。
- 情緒選擇階段仍保留 DOWN 取消功能。
- Serial Help 明確區分：
  - `state approaching`：播放 Fur Elise 接近測試，再進入 ARRIVED。
  - `state arrived`：跳過音樂，直接測試丟垃圾手勢。

### 結果

- DOWN 不取消丟垃圾手勢：✅ Build 成功
- `state approaching`／`state arrived` 測試語意：✅ 已補充說明
- 實機行為：⏳ 待燒錄確認

---

## 2026-10-11｜Day 2：顏色調整與第一版互動狀態機

### 任務

- 解決橙色與黃色在 RGB LED 上辨識度不足的問題。
- 將四元件整合測試推進為本地互動原型。

### 操作

1. 將左方向 RGB 顏色由原本的橙色／先前測試色調整為紫色：`R=180, G=0, B=255`。
2. 保留右方向黃色：`R=255, G=180, B=0`。
3. 建立兩個本地狀態：`SELECTING` 與 `CONFIRMED`。
4. 搖桿移動時進行情緒選擇，LCD 與 RGB LED 顯示目前選項。
5. 按下 SW 後確認選項，播放對應的短提示音，LCD 顯示確認結果。
6. 確認畫面維持 1.5 秒後回到下一次選擇。
7. 提示音採用程式內重新設計的短音型，不直接複製 `sounds/` 參考檔。

### 結果

- 紫色／黃色配色程式：✅ 已完成
- 本地互動狀態機程式：✅ 已完成
- Arduino-ESP32 Core 3.3.12 Build：✅ 成功
- 紫色與黃色實機辨識度：⏳ 待燒錄確認
- 搖桿選擇與按鍵確認流程：⏳ 待燒錄確認
- LCD、RGB、蜂鳴器同步運作：⏳ 待燒錄確認

### 預期流程

```text
搖桿移動 → SELECTING 顯示方向與顏色
按下 SW  → CONFIRMED 顯示情緒並播放提示音
等待 1.5 秒 → 回到 SELECTING
```

### 下一步

- 燒錄並測試紫色／黃色的實際顯色差異。
- 測試三個有效方向的選擇、確認、提示音與回復流程。
- 實機通過後，再加入垃圾車接近／離開的完整情境狀態機。

---

## 2026-10-11｜Day 2：音效標頭與垃圾車音樂淡入淡出

### 音效檔案檢查

實際 `sounds/` 目錄已確認包含 7 個參考檔案：

```text
ACCUMULATING.ino
full.ino
correct.ino
error.ino
achievement-bling.ino
throw.ino
Fur Elise.ino
```

專案實際檔名是 `Fur Elise.ino`，不是 `toelise.ino`；
`achievement-bling.ino` 的拼法也以實際檔案為準。

### 操作

1. 新增 `platformio/src/sounds.h`。
2. 將累積、滿額、丟垃圾、三種工作人員回覆整理成音符陣列與函式。
3. 將原本 `main.cpp` 的臨時確認音改為 `playAccumulating()`。
4. 將 Fur Elise 片段循環到完整 10 秒。
5. 以 LEDC duty cycle 近似控制無源蜂鳴器的音量。
6. 建立 `playApproaching()` 漸強與 `playCollected()` 漸弱介面。

### 結果

- 音效檔案缺漏：✅ 未發現缺漏
- `sounds.h` 建立：✅
- `main.cpp` 引入音效模組：✅
- Core 3.x API 編譯：✅ 成功
- 累積音效接入目前按鍵流程：✅
- Toelise 10 秒漸強／漸弱函式：✅ 已建立，待實機確認
- 七種音效實際聲音：⏳ 待實機逐項驗證

### 設計限制

無源蜂鳴器沒有真正的音量控制端。`ledcWrite()` 的 duty cycle 只能作為音量近似值，
實際淡入淡出效果會依蜂鳴器型號、供電與麵包板接線而不同。

### 下一步

- 新增 Serial 手動音效測試指令，逐項測試 `m-accumulate`、`m-full`、`m-throw`、
  `m-careful`、`m-thanks`、`m-recycle`、`m-approach`、`m-leave`。
- 測試 Toelise 10 秒播放長度與音量變化。
- 再把音效函式接入 `REST`、`IDLE`、`ACCUMULATING`、`FULL`、`APPROACHING`、
  `THROWING`、`RESPONDING`、`COLLECTED`、`RETURNING` 狀態機。

---

| 項目 | 狀態 |
|---|---|
| 專案結構 | ✅ 已建立 |
| LCD 背光 | ✅ 已驗證 |
| LCD 文字顯示 | ✅ 已驗證 |
| LCD I2C 位址 | ✅ `0x27` |
| 蜂鳴器程式編譯 | ✅ 已驗證 |
| 蜂鳴器實機聲音 | ⏳ 待確認 |
| HW504 搖桿 | ⏳ 待測試 |
| RGB LED | ⏳ 待測試 |
| 四元件整合 | 🧪 已編譯，待實機確認 |
| 本地互動狀態機 | 🧪 已編譯，待實機確認 |
| 音效模組 `sounds.h` | 🧪 已編譯，待實機確認 |
| Toelise 10 秒漸強／漸弱 | 🧪 已建立，待實機確認 |
| FastAPI Web 原型 | ✅ 軟體測試通過 |

---

## 2026-10-11｜Day 2：搖桿三次選擇與單色 LED 改造

### 任務

- 將 RGB LED 控制改為單色 LED GPIO27。
- 實作同方向推動三次後選出情緒，再按下 SW 確認送出。
- 實作推下取消與按鍵 debounce／邊緣觸發。
- 保留 REST、IDLE、FULL 及垃圾車相關狀態的 Serial 測試入口。

### 硬體變更

```text
GPIO27 → 220Ω → 單色 LED 長腳
單色 LED 短腳 → GND
```

已移除 RGB LED 的 GPIO14、GPIO26 控制；GPIO27 現在只控制單色 LED。

### 搖桿流程

```text
IDLE
  → 同方向推 1 次：方向 1/3
  → 同方向推 2 次：方向 2/3
  → 同方向推 3 次：顯示情緒選項
  → 按下 SW：累積 1 包，回到 IDLE
  → 推下：取消目前選擇
```

切換到另一方向時，會重新開始該方向的 `1/3` 計數；不保留多個方向同時累積，避免操作歧義。

### 結果

- 單色 LED GPIO27 Core 3.x PWM：✅ Build 成功
- 三次方向選擇邏輯：✅ Build 成功
- 按鍵邊緣觸發與 debounce：✅ Build 成功
- 推下取消：✅ Build 成功
- 固定亮燈狀態不再使用閃爍函式：✅ 已修正
- LCD／搖桿／LED／蜂鳴器實機整合：⏳ 待燒錄確認

### 實作限制

目前 LCD 顯示採用英文，因一般 LCD1602 HD44780 不一定支援直接顯示 UTF-8 中文；企劃書中的中文文案保留作為介面設計，實機文字先以穩定的英文字串驗證。

### 下一步

- 接線改為單色 LED 後燒錄。
- 以 `state idle` 進入待機。
- 分別測試左、右、上方向三次與按鍵確認。
- 測試推下取消、改變方向重算，以及第 5 包進入 FULL。

---

## 2026-10-11｜Day 2：REST、IDLE 與情緒累積第一階段測試韌體

### 任務

- 先驗證 REST／IDLE 切換。
- 驗證情緒累積 `1/5` 到 `5/5`。
- 驗證第 6 次累積不會超過上限。
- 使用非阻塞 `millis()` 更新 RGB LED 閃爍。

### Serial 指令

| 指令 | 功能 |
|---|---|
| `state rest` | 進入 REST，灰色常亮 |
| `state idle` | 進入 IDLE，藍色常亮 |
| `add` | 累積一包固定測試情緒，最多 5 包 |
| `reset` | 清除累積並回到 IDLE |
| `status` | 顯示狀態與累積數量 |
| `help` | 顯示指令說明 |

### 預期流程

```text
state rest → REST，LCD 顯示 Truck resting，RGB 灰色
state idle → IDLE，LCD 顯示 Truck not here，RGB 藍色
add        → ACCUMULATING，1/5～4/5，紅色短閃並播放累積音
add        → FULL，5/5，紫色慢閃並播放滿額音
add        → 維持 FULL，不超過 5/5
reset      → 回到 IDLE，累積歸零
```

### 結果

- 第一階段測試版 `main.cpp`：✅ 建立
- REST／IDLE 指令切換：✅ 已編譯
- 累積上限 5 包：✅ 已編譯
- 第 6 次累積保護：✅ 已編譯
- 非阻塞 RGB 更新架構：✅ 已建立
- LCD／RGB／音效實機結果：⏳ 待燒錄確認

### 下一步

- 燒錄後依序執行 `state rest`、`state idle`、`add` 五次與第六次 `add`。
- 確認 REST 不會累積，IDLE 可以開始累積。
- 確認 5/5 後 RGB 紫色慢閃、`FULL` 音效只在進入滿額時播放。

---

## 2026-10-11｜Day 2：音樂與音效專用測試韌體

### 任務

- 將 `main.cpp` 改為可逐項測試所有音效與 Fur Elise 音樂的測試版本。
- 暫時移除搖桿互動狀態機，避免測試音效時受到其他流程干擾。

### 操作

1. 保留 LCD 作為目前測試項目提示。
2. 透過 Serial Monitor 接收 `m0`～`m9` 指令。
3. 以 GPIO 25 的 Core 3.x LEDC 播放 `sounds.h` 中的音效函式。
4. 建立停止音效指令，確認蜂鳴器可以關閉。

### 測試指令

| 指令 | 測試內容 |
|---|---|
| `m1` | 情緒累積音效 |
| `m2` | 情緒已滿音效 |
| `m3` | 丟垃圾音效 |
| `m4` | 工作人員「小心」 |
| `m5` | 工作人員「謝謝」 |
| `m6` | 工作人員「這是回收」 |
| `m7` | Fur Elise 10 秒漸強 |
| `m8` | Fur Elise 10 秒漸弱 |
| `m9` | 停止音效 |
| `m0` | 顯示指令說明 |

### 結果

- 音效測試版 `main.cpp`：✅ 已建立
- Serial 指令入口：✅ 已建立
- LCD 測試提示：✅ 已保留
- PlatformIO Build：✅ 成功
- 各音效實際聲音：⏳ 待燒錄逐項確認
- Fur Elise 漸強／漸弱效果：⏳ 待燒錄確認

### 下一步

- 燒錄此版本並開啟 115200 baud Serial Monitor。
- 依序執行 `m1`～`m8`，每次記錄聲音是否正確。
- 若要停止 10 秒音樂，輸入 `m9`；目前播放函式為阻塞式，播放期間需等該段播放結束後才能處理新的序列埠指令。
- 音效確認完成後，再將音效接入正式狀態機。

---

## 2026-10-14｜Web 模擬器、硬體音效與文件同步

### 已完成

- 建立 `truck/simulator.html`，提供 Game Boy 風格虛擬搖桿、A/B 按鍵、狀態展示、流程測試控制與 Chart.js 統計。
- Web 模擬器完成 13-state 展示流程，包含 `REST_CLOSED`、`THROW_READY`、三種 `RESPONDING_*`、`COLLECTED` 與 `REST_DAILY_LIMIT`。
- 使用 `platformio/src/sounds.h` 與 `sounds/` 的原始音符建立 Web Audio；加入 Fur Elise 約 10 秒接近／離開段落。
- 音效啟用受瀏覽器使用者手勢限制；AudioContext 或 oscillator 失敗時不阻斷狀態機。
- Web 模擬器加入 FastAPI/SQL 呼叫與 demo-data fallback，使用 `SIM-001` 與 `source=SIMULATION`。
- 建立 `truck/showcase.html` 作為 3D 展示與硬體入口，Wokwi 使用外部連結，不作 Web 即時同步。
- README、proposal 與 docs 改以 13-state、單色 LED GPIO27、1080° 三圈及 Web/Backend 分層為現行描述。

### 邊界與待驗證

- ESP32 實機燒錄、旋轉、LED、蜂鳴器與完整狀態仍須依 `docs/verification-matrix.md` 逐項確認。
- Web simulator 的流程測試通過，不等於 ESP32 實機驗證通過。
- SQL 的 `returned_bags` 是相容欄位，文件以 collected bags 表達收走數量。
- 早期 RGB LED、單一 REST、RETURNED 與 12 段旋轉內容保留在歷史日誌，只代表開發過程，不是現行規格。
