# 硬體與功能驗證矩陣

## 狀態定義

| 狀態 | 意義 |
|---|---|
| ✅ | 實機或完整流程已驗證成功 |
| 🧪 | 程式已建立或編譯成功，尚未完成實機驗證 |
| ⏳ | 尚未開始測試 |
| ⚠️ | 有已知限制或需要額外確認 |

## 硬體驗證

| 項目 | 程式編譯 | 已燒錄 | 實機成功 | 備註 |
|---|---:|---:|---:|---|
| LCD I2C 掃描 | ✅ | ✅ | ✅ | 找到 `0x27` |
| LCD 背光 | ✅ | ✅ | ✅ | 正常 |
| LCD 文字 | ✅ | ✅ | ✅ | 需要調整背面對比度 |
| 蜂鳴器 440 Hz | ✅ | ⏳ | ⏳ | Core 3.x GPIO-based LEDC |
| 蜂鳴器 880 Hz | ✅ | ⏳ | ⏳ | Core 3.x GPIO-based LEDC |
| 搖桿 VRx | ⏳ | ⏳ | ⏳ | GPIO 34，ADC |
| 搖桿 VRy | ⏳ | ⏳ | ⏳ | GPIO 35，ADC |
| 搖桿 SW | ⏳ | ⏳ | ⏳ | GPIO 32，INPUT_PULLUP |
| 單色 LED | ✅ | ⏳ | ⏳ | GPIO 27 → 220Ω → LED → GND |

## 硬體整合驗證

| 項目 | 程式編譯 | 已燒錄 | 實機成功 | 備註 |
|---|---:|---:|---:|---|
| LCD + 搖桿同步顯示 | ✅ | ⏳ | ⏳ | 整合程式已建立 |
| 搖桿旋轉偵測 | ✅ | ⏳ | ⏳ | GPIO34/35，順／逆時針 3 圈 |
| 搖桿按鍵觸發蜂鳴器 | ✅ | ⏳ | ⏳ | SW GPIO 32，確認音 880 Hz |
| LCD + 蜂鳴器 + 單色 LED + 搖桿 | ✅ | ⏳ | ⏳ | 等待實機整合測試 |
| 搖桿選擇狀態 | ✅ | ⏳ | ⏳ | `SELECTING` |
| 按鍵確認狀態 | ✅ | ⏳ | ⏳ | `CONFIRMED`，維持 1.5 秒 |
| 方向對應提示音 | ✅ | ⏳ | ⏳ | 程式內重新設計短音型 |
| `ACCUMULATING` 音效 | ✅ | ⏳ | ⏳ | `sounds.h`，已接入按鍵確認流程 |
| `FULL` 音效 | ✅ | ⏳ | ⏳ | `sounds/full.ino` 旋律整理 |
| `THROWING` 音效 | ✅ | ⏳ | ⏳ | `sounds/throw.ino` 旋律整理 |
| 工作人員「小心」 | ✅ | ⏳ | ⏳ | `achievement-bling.ino` 旋律整理 |
| 工作人員「謝謝」 | ✅ | ⏳ | ⏳ | `correct.ino` 旋律整理 |
| 工作人員「這是回收」 | ✅ | ⏳ | ⏳ | `error.ino` 旋律整理 |
| Toelise 接近 10 秒漸強 | ✅ | ⏳ | ⏳ | Fur Elise 片段循環，duty cycle 近似音量 |
| Toelise 離開 10 秒漸弱 | ✅ | ⏳ | ⏳ | Fur Elise 片段循環，duty cycle 近似音量 |
| 音效測試韌體 Build | ✅ | ⏳ | ⏳ | `main.cpp` 已改為 Serial `m0`～`m9` 測試版 |
| REST_CLOSED 狀態顯示 | ✅ | ⏳ | ⏳ | `state closed`，LCD 休息文字、LED 熄滅 |
| REST_DAILY_LIMIT 狀態顯示 | ✅ | ⏳ | ⏳ | `state daily limit`，不顯示 bags、LED 熄滅 |
| IDLE 狀態顯示 | ✅ | ⏳ | ⏳ | `state idle`，LCD 待機文字、LED 熄滅 |
| 情緒累積 1/5～4/5 | ✅ | ⏳ | ⏳ | 搖桿確認，單色 LED 短閃與累積音效 |
| 情緒累積 5/5 | ✅ | ⏳ | ⏳ | 搖桿確認，進入 FULL、單色 LED 快閃 |
| 情緒累積上限保護 | ✅ | ⏳ | ⏳ | 第 6 次 `add` 不增加數量 |
| 累積重設 | ✅ | ⏳ | ⏳ | `reset` 回到 IDLE 並清零 |
| 單色 LED GPIO27 | ✅ | ⏳ | ⏳ | 220Ω 串聯，取代 RGB LED |
| LCD 自訂表情 8 字元 | ✅ | ⏳ | ⏳ | 第一行文字、第二行置中表情 |
| 同方向推 1/3 | ✅ | ⏳ | ⏳ | LCD 顯示方向進度 |
| 同方向推 2/3 | ✅ | ⏳ | ⏳ | LCD 顯示方向進度 |
| 同方向推 3/3 | ✅ | ⏳ | ⏳ | 顯示情緒選項，等待 SW |
| SW 確認累積 | ✅ | ⏳ | ⏳ | 按下邊緣觸發，避免重複累積 |
| 推下取消 | ✅ | ⏳ | ⏳ | 清除方向選擇並回到 IDLE |
| 累積第 5 包進入 FULL | ✅ | ⏳ | ⏳ | 單 LED 200ms 閃爍 |
| ARRIVED 旋轉偵測 | 🧪 | ⏳ | ⏳ | `atan2()` 累積 360°／720°／1080°＝3 圈 |
| THROW_READY 確認 | ✅ | ⏳ | ⏳ | 旋轉完成後按 SW |
| THROWING 自動回應 | ✅ | ⏳ | ⏳ | 1 秒後隨機進入三種回應 |
| RESPONDING bags 結算 | ✅ | ⏳ | ⏳ | CAREFUL/THANKS -5；RECYCLE -1～4 |
| COLLECTED 清運次數 | ✅ | ⏳ | ⏳ | dailyCollectCount +1，第二次進 REST_DAILY_LIMIT |
| REST_DAILY_LIMIT | ✅ | ⏳ | ⏳ | bags 保留但不顯示、不操作 |
| `reset day` | ✅ | ⏳ | ⏳ | 清零 dailyCollectCount，保留 bags 並進 IDLE |
| APPROACHING LED 持續閃爍 | 🧪 | ⏳ | ⏳ | 非阻塞 Fur Elise 播放期間 300/300 ms |
| atan2 角度三圈 | 🧪 | ⏳ | ⏳ | 360°／720°／1080°，待第二輪實機確認 |
| dailyCollectCount 上限 | 🧪 | ⏳ | ⏳ | 不超過 2，REST_DAILY_LIMIT 阻擋正式流程 |
| RECYCLE collected 命名 | 🧪 | ⏳ | ⏳ | Serial 顯示收走數量與剩餘數量 |
| ARRIVED 單 LED 長亮 | ✅ | ⏳ | ⏳ | 不啟用零間隔閃爍 |

## 軟體功能驗證

| 功能 | 狀態 | 備註 |
|---|---|---|
| FastAPI `/api/status` | ✅ | Python 測試通過 |
| FastAPI `/api/arrive` | ✅ | Python 測試通過 |
| FastAPI `/api/joystick` | ✅ | Python 測試通過 |
| Web 虛擬搖桿 | 🧪 | MVP 已建立 |
| Web 垃圾袋動畫 | 🧪 | MVP 已建立 |
| ESP32 本地狀態機 | 🧪 | 已整合 13 狀態、bags 結算與每日清運次數，待實機確認 |
| ESP32 與 FastAPI 串接 | ⏳ | 尚未實作 |
| Toelise 接近音樂 | 🧪 | `sounds.h` 已建立，尚未接入完整狀態機 |
| 工作人員三種提示音 | 🧪 | `sounds.h` 已建立，尚未接入完整狀態機 |

## 更新規則

- 只有實際燒錄並觀察硬體結果後，才將「實機成功」改為 ✅。
- 只有 Build 成功，不代表硬體功能已完成。
- 每次狀態變更都應同步在 `docs/implementation-log.md` 新增日期紀錄。
- 企劃書只在一個階段驗證完成後，更新最終設計與功能標記。
