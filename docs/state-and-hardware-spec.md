# 情緒垃圾車：狀態與硬體整合規格

> 本文件是目前 ESP32 韌體、LCD、LED、搖桿與蜂鳴器測試的對照基準。
> 只有實機測試通過後，才將驗證矩陣中的實機欄位改為通過。

## 1. 系統架構圖

```text
                         ┌──────────────────────────────┐
                         │ ESP32 狀態機                  │
                         │ bags / dailyCollectCount     │
                         │ millis() / Serial test       │
                         └──────────┬───────────────────┘
                                    │
          ┌─────────────────────────┼─────────────────────────┐
          │                         │                         │
   ┌──────▼──────┐           ┌──────▼──────┐           ┌──────▼──────┐
   │ 兩軸搖桿     │           │ LCD1602     │           │ 單色 LED     │
   │ VRx/VRy/SW  │           │ I2C 0x27    │           │ GPIO27       │
   └─────────────┘           └─────────────┘           └─────────────┘
                                    │
                              ┌─────▼─────┐
                              │ 無源蜂鳴器 │ GPIO25
                              └───────────┘
```

## 2. 實體關係圖／硬體接線

| 元件 | 腳位 | ESP32 接點／備註 |
|---|---|---|
| 搖桿 VRx | X 軸 | GPIO34，ADC |
| 搖桿 VRy | Y 軸 | GPIO35，ADC |
| 搖桿 SW | 按鍵 | GPIO32，`INPUT_PULLUP`，按下為 LOW |
| LCD SDA | I2C | GPIO21 |
| LCD SCL | I2C | GPIO22 |
| LCD 位址 | I2C | `0x27` |
| 單色 LED 正極 | PWM | GPIO27 → 220Ω → LED 正極 |
| 單色 LED 負極 | 電源 | GND |
| 無源蜂鳴器 | PWM | GPIO25 |

## 3. 狀態轉換圖

```text
REST_CLOSED ──開放時間／測試 reset day──> IDLE
IDLE ──方向選擇──> ACCUMULATING
ACCUMULATING ──SW／bags+1──> IDLE
ACCUMULATING ──bags=5──> FULL
ACCUMULATING ──DOWN──> IDLE
IDLE ──bags=5──> FULL
FULL ──清運時間到──> APPROACHING
APPROACHING ──Fur Elise 完成──> ARRIVED
ARRIVED ──順／逆時針 3 圈──> THROW_READY
THROW_READY ──SW──> THROWING
THROWING ──自動──> RESPONDING_CAREFUL / THANKS / RECYCLE
RESPONDING_* ──bags 結算──> COLLECTED
COLLECTED ──dailyCollectCount++──┬── count < 2 ──> IDLE
                                  └── count >= 2 ─> REST_DAILY_LIMIT
REST_DAILY_LIMIT ──新的一天／reset day──> IDLE
```

## 4. 狀態總表

| 狀態 | 進入／離開 | bags | 搖桿／SW | LCD 第二行表情 |
|---|---|---|---|---|
| `REST_CLOSED` | 非開放時間、週三、週日 → 開放時段 | 保留、不顯示 | 無效 | 睡臉 |
| `IDLE` | 開放時間等待清運；滿 5 → `FULL` | 可由剩餘數量繼續增加 | 左/右/上各 3 次；DOWN 取消；SW 確認 | 微笑 |
| `ACCUMULATING` | 選擇方向中；SW 後回 `IDLE`/`FULL` | 確認才 +1 | 有效 | 生氣 |
| `FULL` | `bags == 5`；清運時間 → `APPROACHING` | 不增加 | 無效 | 飽足 |
| `APPROACHING` | 清運時間到；音樂完成 → `ARRIVED` | 不變 | 完全無效 | 驚訝 |
| `ARRIVED` | 等待順／逆時針旋轉 3 圈 | 不變 | `atan2()` 累積 1080° → `THROW_READY` | 開心 |
| `THROW_READY` | 旋轉完成；SW → `THROWING` | 不變 | 只接受 SW | 垃圾袋 |
| `THROWING` | 丟垃圾執行；自動 → `RESPONDING_*` | 不結算 | 無效、不可取消 | 垃圾袋 |
| `RESPONDING_CAREFUL` | 50% | `max(0, bags-5)` | 無效 | 開心 |
| `RESPONDING_THANKS` | 25% | `max(0, bags-5)` | 無效 | 開心 |
| `RESPONDING_RECYCLE` | 25% | 隨機保留 1～原有 bags；0 包維持 0 | 無效 | 第二行顯示 Keep 與保留量 |
| `COLLECTED` | 回應完成；計數 +1 | 不再修改 | 無效 | 放鬆 |
| `REST_DAILY_LIMIT` | `dailyCollectCount >= 2`；隔日才恢復 | 內部保留、不顯示、不使用 | 無效 | 睡臉 |

## 5. bags 與每日清運次數

```text
IDLE → 確認情緒 → bags += 1 → IDLE 或 FULL
FULL → APPROACHING → ARRIVED → THROW_READY → THROWING
THROWING → RESPONDING
  CAREFUL / THANKS: bags = max(0, bags - 5)
  RECYCLE: bags = bags > 0 ? random(1..bags) : 0
  collected = bags_before - bags_after
RESPONDING → COLLECTED（不再清零）
COLLECTED: dailyCollectCount += 1
  count < 2  → IDLE，繼承剩餘 bags
  count >= 2 → REST_DAILY_LIMIT，bags 保留但不顯示、不操作
隔日或 reset day → dailyCollectCount = 0，再進 IDLE 使用保留 bags
```

`reset day` 只清除 `dailyCollectCount`，不清除 bags；`reset` 才同時清除 bags 與每日次數。

## 6. LED 閃爍對照表

| 狀態 | 單色 LED GPIO27 |
|---|---|
| `REST_CLOSED`、`IDLE`、`REST_DAILY_LIMIT` | 熄滅 |
| `ACCUMULATING` | 100/100 ms 短閃 |
| `FULL` | 200/200 ms 快閃 |
| `APPROACHING` | 300/300 ms 慢閃 |
| `ARRIVED`、`COLLECTED` | 長亮 |
| `THROW_READY`、`THROWING`、`RESPONDING_CAREFUL`、`RESPONDING_THANKS` | 雙短閃 |
| `RESPONDING_RECYCLE` | 200/200 ms 連閃 5 次 |

## 7. 搖桿操作對照表

| 狀態 | 情緒選擇 | 旋轉丟垃圾 | SW |
|---|---|---|---|
| `REST_CLOSED`、`FULL`、`APPROACHING`、`THROWING`、`RESPONDING_*`、`COLLECTED`、`REST_DAILY_LIMIT` | 無效 | 無效 | 無效 |
| `IDLE`、`ACCUMULATING` | 左/右/上各 3 次；DOWN 取消 | 無效 | 確認情緒 |
| `ARRIVED` | 無效 | `atan2()` 判定順／逆時針，累積 360°／720°／1080° | 無效 |
| `THROW_READY` | 無效 | 不再計數 | 確認丟垃圾 |

情緒累積時，每次同方向 push 後回到中央再推；切換方向重新計數。三次後按 SW（Web 按 A）才增加 bags。旋轉使用 atan2() 角度累積，不是逐段回中央的計數方式。

## 8. 音效對照表

| 狀態 | 音效 |
|---|---|
| `REST_CLOSED`、`IDLE`、`ARRIVED`、`REST_DAILY_LIMIT` | 無 |
| `ACCUMULATING` | `playAccumulating()` |
| `FULL` | `playFull()` |
| `APPROACHING` | Fur Elise 約 10 秒漸強；完成後停止並進入 `ARRIVED` |
| `THROW_READY` | `playThrow()` 提示音 |
| `THROWING` | 執行中不接受輸入 |
| `RESPONDING_CAREFUL` | `playWorkerCareful()` |
| `RESPONDING_THANKS` | `playWorkerThanks()` |
| `RESPONDING_RECYCLE` | `playWorkerRecycle()` |
| `COLLECTED` | Fur Elise 漸弱介面 |

## 9. LCD 對照表

第一行顯示文字，第二行在第 7 欄顯示自訂 5×8 表情。LCD1602 同時最多 8 個自訂字元，因此 `CAREFUL` 使用開心圖案，不另外佔用第 9 個字元。

| 狀態 | 第一行 | 第二行 |
|---|---|---|
| `REST_CLOSED` | `Truck resting` | 睡臉 |
| `IDLE` | `Bags: X/5` | 微笑 |
| `ACCUMULATING` | `GAN 1/3` 等 | 生氣 |
| `FULL` | `Emotion is full` | 飽足 |
| `APPROACHING` | `Truck coming` | 驚訝 |
| `ARRIVED` | `Truck is here` | 開心 |
| `THROW_READY` | `Press to throw` | 垃圾袋 |
| `THROWING` | `Throwing...` | 垃圾袋 |
| `RESPONDING_CAREFUL` | `Be careful` | 開心 |
| `RESPONDING_THANKS` | `Thank you` | 開心 |
| `RESPONDING_RECYCLE` | `Returned X bags` | 第二行 `Keep: X bags`，顯示保留量 |
| `COLLECTED` | `Emotion cleared` | 放鬆 |
| `REST_DAILY_LIMIT` | `2 trips done` | 睡臉 |

## 10. Serial 旋轉測試

```text
reset
state arrived
status
```

實體操作：

```text
以 `atan2()` 讀取搖桿角度
→ 累積同方向角度變化
→ 360° = 1 圈、720° = 2 圈、1080° = 3 圈
→ LCD 顯示 Rotate 1/3、2/3、3/3
→ LCD 顯示 Press to throw
→ 按 SW
```

預期序列：

```text
ARRIVED → THROW_READY → THROWING → RESPONDING_* → COLLECTED
```

測試指令：`status`、`throw reset`、`reset day`、`state daily limit`。

## 11. Web 模擬器與後端邊界

`truck/simulator.html` 是與 ESP32 共用狀態命名、bags 結算、每日上限與音效規格的獨立展示頁。其主要輸入是虛擬搖桿、A/B 按鍵與展示用測試控制；旋轉流程以三次「旋轉一圈」按鈕代表硬體的 360°、720°、1080°。

模擬器會以 `SIM-001`、`source=SIMULATION` 呼叫 FastAPI。API 可用時寫入模擬事件並讀取統計；請求失敗時使用頁面標示的 demo-data fallback。這不代表 ESP32 已與後端完成雙向即時同步。

Wokwi 專案是外部硬體展示入口：<https://wokwi.com/projects/477775315279623169>。Wokwi 不會自動接收 Web 模擬器狀態。

瀏覽器端 Web Audio 依照 `platformio/src/sounds.h` 與 `sounds/` 的音符、頻率和毫秒時長重現硬體聲音。因瀏覽器自動播放限制，使用者需先啟用音效；即使 AudioContext 失敗，狀態流程仍應可操作。

音量控制依平台區分：ESP32 的 APPROACHING 約 10 秒內將 duty 從 10 調至 80，COLLECTED 從 80 調至 10 後停止，這是 PWM 近似音量而非實測聲壓。Web 接近音樂為固定音量，離開段為每個音符各自淡出，不表示整段音樂漸弱。

方向映射以目前韌體為準：LEFT = GAN、RIGHT = KAO、UP = CAO。三次同方向 push 後按 SW（Web A）才寫入一次已確認情緒；取消不增加 bags。

> SQL 仍保留 `returned_bags` 作為後端相容欄位；在產品語意與前端文件中稱為 collected bags（收走數量），不是「退回數量」。
