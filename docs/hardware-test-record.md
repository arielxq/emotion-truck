# 情緒垃圾車 ESP32 實機測試紀錄

> 用途：記錄燒錄後的 LCD、LED、搖桿、蜂鳴器與狀態機實機結果。
> 請填寫「實際結果」與「通過」欄位；程式 Build 成功不代表實機驗證通過。

## 一、測試基本資料

| 項目 | 填寫內容 |
|---|---|
| 測試日期 | 2026/10/13 |
| 測試時間 |  |
| 測試人員 | Ariel |
| ESP32 開發板 |  |
| 韌體版本／Commit |  |
| PlatformIO Build | ✅  通過 ☐ 失敗 |
| 韌體燒錄 | ✅  通過 ☐ 失敗 |
| Serial Baud Rate | 115200 |
| LCD I2C 位址 | `0x27` |

## 二、硬體接線確認

| 元件 | 預期接線 | 實際結果 | 通過 |
|---|---|---|---|
| 搖桿 VRx | GPIO34，ADC |  | ✅  |
| 搖桿 VRy | GPIO35，ADC |  | ✅  |
| 搖桿 SW | GPIO32，INPUT_PULLUP |  | ✅  |
| LCD SDA | GPIO21 |  | ✅  |
| LCD SCL | GPIO22 |  | ✅  |
| LCD 位址 | `0x27` |  | ✅  |
| 單色 LED | GPIO27 → 220Ω → LED → GND |  | ✅  |
| 無源蜂鳴器 | GPIO25 |  | ✅  |
| 電路安全 | 無短路、異常發熱 |  | ✅  |

## 三、輸出元件測試

| 測試項目 | 預期結果 | 實際結果 | 通過 |
|---|---|---|---|
| LCD 背光 | 正常亮起 |  | ✅  |
| LCD 第一行文字 | 可清楚顯示 |  | ✅  |
| LCD 第二行表情 | 可顯示自訂表情 |  | 🏷️ 待優化 |
| LCD 表情置中 | 表情約位於第 7 欄 |  | ✅  |
| LED 熄滅 | GPIO27 關閉時熄滅 |  | ✅  |
| LED 長亮 | 固定亮燈正常 |  | ✅ |
| LED 短閃 | 100/100 ms 閃爍 |  | ✅  |
| LED 快閃 | 200/200 ms 閃爍 |  | ✅  |
| 蜂鳴器短音 | 可聽見提示音 |  | ✅  |
| Fur Elise | 可播放音樂 |  | ✅  |

## 四、狀態測試

### 1. REST_CLOSED

指令：

```text
state closed
status
```

預期：`REST_CLOSED`、LCD `Truck resting`／睡臉、LED 熄滅、蜂鳴器無聲、搖桿與 SW 無效、bags 不顯示且不變。

| 實際狀態 | LCD | bags | LED | 蜂鳴器 | 搖桿／SW | 通過 |

=== State: REST_CLOSED | bags=0/5 ===
State=REST_CLOSED, bags=0/5, daily=0/2, selected=NONE, direction=CENTER, count=0/3, rotation=0/12

|  |  |  |  |  |  | ✅ |

### 2. IDLE

指令：

```text
state idle
status
```

預期：`IDLE`、LCD `Bags: X/5`／微笑、LED 熄滅，可開始情緒選擇。

| 實際狀態 | LCD | bags | LED | 搖桿是否有效 | 通過 |

=== State: IDLE | bags=0/5 ===
State=IDLE, bags=0/5, daily=0/2, selected=NONE, direction=CENTER, count=0/3, rotation=0/12

|  |  |  |  |  | ✅ |

### 3. ACCUMULATING

操作同一方向：

```text
左 → 中央 → 左 → 中央 → 左
```

預期：顯示 `1/3`、`2/3`、情緒選項；LED 短閃；播放累積音效；按 SW 前 bags 不增加。

Emotion option ready: GAN
Confirmed GAN, bags=1/5
=== State: IDLE | bags=1/5 ===

=== State: IDLE | bags=1/5 ===
UP 1/3
UP 2/3
=== State: ACCUMULATING | bags=1/5 ===
Emotion option ready: CAO
Confirmed CAO, bags=2/5
=== State: IDLE | bags=2/5 ===

| 項目 | 實際結果 | 通過 |
|---|---|---|
| 1/3 顯示 |  | ✅ |
| 2/3 顯示 |  | ✅ |
| 3/3 情緒選項 |  | ✅ |
| LED 短閃 |  | ✅ |
| 累積音效 |  | ✅ |
| SW 確認後 bags +1 |  | ✅ |
| DOWN 取消並回 IDLE |  | ✅ |

### 4. FULL

重複完成情緒確認，直到 `bags = 5/5`。

預期：`FULL`、LCD `Emotion is full`／飽足、LED 快閃、FULL 音效、搖桿與 SW 無效。

| 項目 | 實際結果 | 通過 |
|---|---|---|
| bags 是否為 5/5 |  | ✅ |
| LCD 與飽足表情 |  | ✅  |
| LED 快閃 |  | ✅  |
| FULL 音效 |  | ✅  |
| 不會超過 5 包 |  | ✅  |

### 5. APPROACHING

指令：

```text
state approaching
```

預期：LCD `Truck coming`／驚訝、LED 慢閃、Fur Elise 約 10 秒；搖桿與 SW 無效；音樂完成後進入 `ARRIVED`。

| 項目 | 實際結果 | 通過 |
|---|---|---|
| 進入 APPROACHING |  | ✅ |
| Fur Elise 播放 |  | ✅ |
| 播放時間 |  | ✅ |
| LED 慢閃 |  | ✅ |
| bags 不變 |  | ✅ |
| 自動進入 ARRIVED |  | ✅ |

### 6. ARRIVED

指令：

```text
state arrived
throw reset
status
```

預期：LCD `Truck is here`／開心、LED 長亮、Fur Elise 停止、bags 不變，進入旋轉偵測。

| 項目 | 實際結果 | 通過 |
|---|---|---|
| LCD 與開心表情 |  | ✅ |
| LED 長亮 |  | ✅ |
| Fur Elise 停止 |  | ✅ |
| bags 不變 |  | ✅ |
| 情緒選擇無效 |  | ✅ |

### 7. THROW_READY

在 `ARRIVED` 以 `atan2()` 累積搖桿角度，順時針或逆時針達 1080°，即完成 3 圈。


預期：LCD 顯示 `Rotate 1/3`、`Rotate 2/3`、`Rotate 3/3`；達 1080° 後進入 `THROW_READY`，並播放 THROW 提示音。

| 項目 | 實際結果 | 通過 |
|---|---|---|
| 第一個方向 |  | ✅ |
| 判定 CW／CCW |  | ✅ |
| 累積 360°／1 圈 |  | ✅ |
| 累積 720°／2 圈 |  | ✅ |
| 累積 1080°／3 圈 |  | ✅ |
| 中央死區處理 |  | ✅ |
| 中途反向行為 |  | ✅reset |
| LCD Press to throw |  | ✅ |
| THROW 提示音 |  | ✅ |
| LED 雙短閃 |  | ✅ |

### 8. THROWING

在 `THROW_READY` 按下 SW。

預期：LCD `Throwing...`／垃圾袋、LED 雙短閃、搖桿無效、不可取消、bags 尚未結算，約 1 秒後進入 `RESPONDING_*`。

State=THROW_READY, bags=2/5, daily=0/2, selected=NONE, direction=CENTER, count=0/3, rotation=1080/1080 degrees

| 項目 | 實際結果 | 通過 |
|---|---|---|
| 進入 THROWING |  | ✅ |
| THROW 音效／LED |  | ✅ LED off |
| 搖桿無效 |  | ✅ |
| DOWN 不取消 |  | ✅ |
| bags 尚未結算 |  | ✅ |
| 自動進入 RESPONDING |  | ✅ |

### 9. RESPONDING_CAREFUL

指令：

```text
state careful
status
```

預期：`bags = max(0, bags - 5)`，LCD `Be careful`／開心，播放 CAREFUL 音效，之後進入 `COLLECTED`。

=== State: RESPONDING_CAREFUL | bags=0/5 ===
=== State: COLLECTED | bags=0/5 ===
State=COLLECTED, bags=0/5, daily=1/2, selected=NONE, direction=CENTER, count=0/3, rotation=12/12
Daily collection count: 2/2
=== State: REST_DAILY_LIMIT | bags=0/5 ===
State=REST_DAILY_LIMIT, bags=0/5, daily=2/2, selected=NONE, direction=CENTER, count=0/3, rotation=0/12

=== State: RESPONDING_CAREFUL | bags=0/5 ===
=== State: COLLECTED | bags=0/5 ===
Daily collection count: 3/2
=== State: REST_DAILY_LIMIT | bags=0/5 ===
! 這裡顯示collection>2的情況，要糾正

| 原始 bags | 結算後 bags | LCD／音效 | 是否進入 COLLECTED | 通過 |
|---:|---:|---|---|---|
|  |  |  |  | ✅ |

### 10. RESPONDING_THANKS

指令：

```text
state thanks
status
```

預期：`bags = max(0, bags - 5)`，LCD `Thank you`／開心，播放 THANKS 音效，之後進入 `COLLECTED`。

=== State: RESPONDING_THANKS | bags=0/5 ===
=== State: COLLECTED | bags=0/5 ===
State=COLLECTED, bags=0/5, daily=3/2, selected=NONE, direction=CENTER, count=0/3, rotation=0/12
Daily collection count: 4/2
=== State: REST_DAILY_LIMIT | bags=0/5 ===
! 這裡顯示collection>2的情況，要糾正

| 原始 bags | 結算後 bags | LCD／音效 | 是否進入 COLLECTED | 通過 |
|---:|---:|---|---|---|
|  |  |  |  | ✅ |

### 11. RESPONDING_RECYCLE

指令：

```text
state recycle
status
```

預期：隨機減少 1～4 包但不超過原始 bags，剩餘 bags 保留；LCD 顯示 `Remain: X bags`／生氣；之後進入 `COLLECTED`。


| 原始 bags | 減少數量 | 結算後 bags | LCD／音效／LED | 進入 COLLECTED | 通過 |
|---:|---:|---:|---|---|---|
|  |  |  |  |  | 🏷️ |

### 12. COLLECTED

指令：

```text
state collected
status
```

預期：LCD `Emotion cleared`／放鬆、LED 長亮、bags 不再修改、`dailyCollectCount +1`。

| 項目 | 實際結果 | 通過 |
|---|---|---|
| LCD 與放鬆表情 |  | ✅ |
| bags 不再清零 |  | ✅ |
| Fur Elise 漸弱 |  | 已刪除 |
| dailyCollectCount +1 |  | ✅ |
| count < 2 時進入 IDLE |  | ✅ |
| count >= 2 時進入 REST_DAILY_LIMIT |  | ✅ |

### 13. REST_DAILY_LIMIT

指令：

```text
state daily limit
status
```

預期：LCD `2 trips done`／睡臉、LED 熄滅、蜂鳴器無聲、bags 內部保留但不顯示、不操作。

| 項目 | 實際結果 | 通過 |
|---|---|---|
| 狀態正確 |  | ✅ |
| LCD 不顯示 bags |  | ✅ |
| bags 內部保留 |  | ✅ |
| LED 熄滅 |  | ✅ |
| 蜂鳴器無聲 |  | ✅ |
| 搖桿與 SW 無效 |  | ✅ |

## 五、每日清運次數與跨日測試

### 第一次清運

```text
reset
state arrived
```

完成旋轉 3 圈、按 SW，等待：

```text
THROWING → RESPONDING_* → COLLECTED → IDLE
```

| 項目 | 實際結果 | 通過 |
|---|---|---|
| dailyCollectCount |  | ✅ |
| 下一狀態為 IDLE |  | ✅ |
| 剩餘 bags 是否繼承 |  | ✅ |

### 第二次清運

再次完成旋轉 3 圈、按 SW並等待流程結束。

| 項目 | 實際結果 | 通過 |
|---|---|---|
| dailyCollectCount = 2 |  | ✅ |
| 進入 REST_DAILY_LIMIT |  | ✅ |
| REST 不顯示 bags |  | ✅ |
| bags 仍保留於內部 |  | ✅ |

### `reset day`

在 `REST_DAILY_LIMIT` 執行：

```text
reset day
status
```

預期：`dailyCollectCount = 0`、進入 `IDLE`、bags 保留並可繼續累積。

| 項目 | 實際結果 | 通過 |
|---|---|---|
| dailyCollectCount 清為 0 |  | ✅ |
| bags 沒有被清除 |  | ✅ |
| 進入 IDLE |  | ✅ |
| IDLE 顯示保留 bags |  | ✅ |

## 六、測試結果總結

| 類別 | 通過數 | 未通過數 | 備註 |
|---|---:|---:|---|
| 硬體接線 |  |  |  |
| LCD 與自訂表情 |  |  |  |
| LED |  |  |  |
| 搖桿情緒選擇 |  |  |  |
| 搖桿旋轉 3 圈 |  |  |  |
| SW 確認 |  |  |  |
| 蜂鳴器音效 |  |  |  |
| bags 結算 |  |  |  |
| dailyCollectCount |  |  |  |
| REST 狀態 |  |  |  |

## 七、Serial 紀錄貼上區

請貼上重要測試輸出：

```text
1. state closed / status
2. state idle / 情緒累積
3. state approaching
4. state arrived / 旋轉 3 圈
5. THROW_READY / 按 SW
6. RESPONDING / COLLECTED
7. 第二次清運
8. reset day / status
```

## 八、問題與修正紀錄

| 編號 | 發生狀態 | 問題描述 | 重現方式 | 修正方式 | 日期 |
|---:|---|---|---|---|---|
| 1 |  |  |  |  |  |
| 2 |  |  |  |  |  |
| 3 |  |  |  |  |  |

## 九、最終結論

```text
☐ 全部通過
☐ 部分通過，需要修正
☐ 無法完成測試
```

備註：

```text

```

---

## 十一、Web、Wokwi 與後端展示測試

> 本區與 ESP32 實機紀錄分開。Web 通過不代表硬體通過。

| 測試項目 | 預期結果 | 實際結果 | 通過 |
|---|---|---|---|
| `truck/showcase.html` Section 10 載入 | 頁面可開啟、LCD 與控制區尺寸固定 |  | ⏳ |
| 操作台搖桿／SW／重設 | 上／左／右累積情緒，下取消；SW 確認；重設歸零 |  | ⏳ |
| 13-state 流程 | 可由 IDLE 完成至 COLLECTED／REST_DAILY_LIMIT |  | ⏳ |
| 三次旋轉展示 | ARRIVED 按三次後進入 THROW_READY |  | ⏳ |
| 硬體音效啟用 | 使用者按鈕後 Web Audio 可播放 |  | ⏳ |
| 音效失敗隔離 | 無 AudioContext 時仍可操作狀態機 |  | ⏳ |
| APPROACHING | showcase 3 秒展示；實機 Fur Elise 約 10 秒 |  | ⏳ |
| Section 10b 圖表 | 三張示範圖表與今日狀態可顯示；Chart.js 失敗時有 fallback |  | ⏳ |
| FastAPI 寫入 | 後端 `/api/sim/*` 可獨立測試；showcase 不即時呼叫 |  | ⏳ |
| 工作人員回應統計 | `GET /api/stats/responses` 回傳 CAREFUL／THANKS／RECYCLE 分布 |  | ⏳ |
| Wokwi 外部連結 | 開啟指定 Wokwi 專案 |  | ⏳ |
| 手機瀏覽器 | 版面與觸控按鈕可用 |  | ⏳ |
