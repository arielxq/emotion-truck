# 情緒垃圾車 ESP32 實機測試紀錄

> 本紀錄只保留已完成並確認的 ESP32 實機結果。所有列出的硬體、輸入、輸出與 13-state 流程均已完成驗證。

## 一、測試基本資料

| 項目 | 結果 |
|---|---|
| 測試日期 | 2026/10/13 |
| 測試人員 | Ariel |
| PlatformIO Build | ✅ 通過 |
| 韌體燒錄 | ✅ 通過 |
| Serial Baud Rate | 115200 |
| LCD I2C 位址 | `0x27`，✅ 已確認 |

## 二、硬體接線與元件測試

| 項目 | 接線／預期結果 | 實際結果 | 通過 |
|---|---|---|---|
| 搖桿 VRx | GPIO34，ADC | 可正常讀取 X 軸輸入 | ✅ |
| 搖桿 VRy | GPIO35，ADC | 可正常讀取 Y 軸輸入 | ✅ |
| 搖桿 SW | GPIO32，INPUT_PULLUP | 按鍵可正常觸發確認 | ✅ |
| LCD SDA／SCL | GPIO21／GPIO22 | I2C 通訊正常 | ✅ |
| LCD 位址 | `0x27` | 可正常初始化與顯示 | ✅ |
| 單色 LED | GPIO27 → 220Ω → LED → GND | 熄滅、長亮與各狀態閃爍正常 | ✅ |
| 無源蜂鳴器 | GPIO25 | 短音、提示音與音樂正常 | ✅ |
| 電路安全 | 無短路、異常發熱 | 長時間操作正常 | ✅ |
| LCD 背光與文字 | 可清楚顯示 | 背光、第一行文字正常 | ✅ |
| LCD 自訂表情 | 第二行置中顯示 | 表情可正常顯示並置中 | ✅ |
| Fur Elise | 接近與離開音樂 | 可正常播放，時序與狀態切換正常 | ✅ |

## 三、13-state 實機流程

| 狀態／功能 | 實際結果 | 通過 |
|---|---|---|
| `REST_CLOSED` | LCD 顯示休息狀態，LED 與蜂鳴器關閉，搖桿及 SW 無效 | ✅ |
| `IDLE` | LCD 顯示 bags 與待機表情，可開始情緒選擇 | ✅ |
| `ACCUMULATING` | 同方向完成 1/3、2/3、3/3，LED 短閃並播放累積音效 | ✅ |
| 情緒確認 | 按下 SW 後 bags 增加 1，回到 `IDLE` 或進入 `FULL` | ✅ |
| DOWN 取消 | 清除目前選擇並回到 `IDLE` | ✅ |
| `FULL` | bags 到達 5/5，LCD、LED 與 FULL 音效正確，數量不超過 5 | ✅ |
| `APPROACHING` | 垃圾車音樂播放約 10 秒，LED 持續慢閃，完成後自動進入 `ARRIVED` | ✅ |
| `ARRIVED` | LCD 顯示抵達，LED 長亮，停止接近音樂 | ✅ |
| 旋轉偵測 | 以 `atan2()` 累積順時針或逆時針角度，360°、720°、1080° 正確對應 1、2、3 圈 | ✅ |
| `THROW_READY` | 完成三圈後顯示丟出提示並播放提示音，只接受 SW | ✅ |
| `THROWING` | 按下 SW 後進入不可取消清運，輸入全部暫停 | ✅ |
| `RESPONDING_CAREFUL` | 播放對應音效，bags 依規則結算並進入 `COLLECTED` | ✅ |
| `RESPONDING_THANKS` | 播放對應音效，bags 依規則結算並進入 `COLLECTED` | ✅ |
| `RESPONDING_RECYCLE` | 播放對應音效，收走 1–4 包並保留剩餘 bags | ✅ |
| `COLLECTED` | 顯示清運完成，播放離開段落並增加每日清運次數 | ✅ |
| `REST_DAILY_LIMIT` | 第二次清運後進入休息，bags 保留但不顯示、不接受輸入 | ✅ |
| `reset day` | 每日清運次數歸零，保留 bags 並回到 `IDLE` | ✅ |

## 四、結算與限制驗證

| 項目 | 實際結果 | 通過 |
|---|---|---|
| bags 上限 | 最大 5 包，第 6 次不再增加 | ✅ |
| CAREFUL／THANKS 結算 | `bags = max(0, bags - 5)` | ✅ |
| RECYCLE 結算 | 收走 1–4 包，不超過目前 bags | ✅ |
| bags 繼承 | 清運後保留剩餘 bags，不會無條件清零 | ✅ |
| 每日清運上限 | `dailyCollectCount` 最大為 2 | ✅ |
| 第二次清運後狀態 | 正確進入 `REST_DAILY_LIMIT` | ✅ |
| `reset day` | 只清除每日次數，不清除 bags | ✅ |

## 五、最終結論

```text
ESP32 硬體接線：✅ 完成
LCD 顯示：✅ 完成
LED 狀態提示：✅ 完成
搖桿情緒選擇：✅ 完成
搖桿三圈旋轉：✅ 完成
SW 確認與不可取消清運：✅ 完成
蜂鳴器音效與音樂：✅ 完成
13-state 狀態機：✅ 完成
bags 結算：✅ 完成
每日清運上限：✅ 完成
跨日 reset day：✅ 完成
```
