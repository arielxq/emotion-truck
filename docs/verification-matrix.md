# 硬體、Web 與後端驗證矩陣

> 本版本只保留已完成的驗證結果。ESP32 實機欄位全部已完成確認；Web 與後端項目依其平台分開記錄。

## 標記

| 標記 | 意義 |
|---|---|
| ✅ | 已完成並確認 |
| ⚠️ | 平台限制或使用條件，不代表功能未完成 |

## ESP32 硬體與韌體

| 項目 | Build | 已燒錄 | 實機 | 備註 |
|---|---:|---:|---:|---|
| LCD I2C 掃描與 `0x27` | ✅ | ✅ | ✅ | 通訊正常 |
| LCD 背光與文字 | ✅ | ✅ | ✅ | 顯示正常 |
| LCD 自訂表情與置中 | ✅ | ✅ | ✅ | 第二行表情正常 |
| HW504 VRx / VRy / SW | ✅ | ✅ | ✅ | GPIO34 / GPIO35 / GPIO32 |
| 單色 LED | ✅ | ✅ | ✅ | GPIO27，220Ω |
| 無源蜂鳴器 | ✅ | ✅ | ✅ | GPIO25 |
| LCD、LED、搖桿、蜂鳴器整合 | ✅ | ✅ | ✅ | 整合操作正常 |
| 13-state 本地狀態機 | ✅ | ✅ | ✅ | 全部狀態已確認 |
| 情緒累積 1/3、2/3、3/3 | ✅ | ✅ | ✅ | 完成後可確認情緒 |
| bags 上限 5 | ✅ | ✅ | ✅ | 不超過 5 包 |
| `APPROACHING` 音樂與 LED | ✅ | ✅ | ✅ | 約 10 秒後抵達 |
| `atan2()` 360°／720°／1080° | ✅ | ✅ | ✅ | 三圈後進入 `THROW_READY` |
| `THROW_READY` 確認 | ✅ | ✅ | ✅ | SW 可正常觸發 |
| `THROWING` 不可取消 | ✅ | ✅ | ✅ | 清運期間輸入無效 |
| 三種工作人員回應 | ✅ | ✅ | ✅ | CAREFUL／THANKS／RECYCLE |
| 回應音效 | ✅ | ✅ | ✅ | 對應狀態播放正常 |
| bags 結算與繼承 | ✅ | ✅ | ✅ | 收走後保留剩餘數量 |
| `COLLECTED` | ✅ | ✅ | ✅ | 清運完成與計數正常 |
| 每日清運上限 2 | ✅ | ✅ | ✅ | 第二次後進入休息 |
| `REST_DAILY_LIMIT` | ✅ | ✅ | ✅ | bags 保留且輸入無效 |
| `reset day` | ✅ | ✅ | ✅ | 清除每日次數、保留 bags |

## Web simulator

| 項目 | 狀態 | 備註 |
|---|---:|---|
| `truck/simulator.html` 載入 | ✅ | 獨立靜態頁 |
| Game Boy 虛擬搖桿與 A/B | ✅ | 方向、確認、重設正常 |
| 13-state 流程 | ✅ | 含三種 `RESPONDING_*` |
| 固定 LCD／控制區尺寸 | ✅ | 狀態文字變化不造成跳動 |
| 三次旋轉展示 | ✅ | 三次「旋轉一圈」後進入 `THROW_READY` |
| Web Audio 硬體音符 | ✅ | 依硬體音符重現 |
| Fur Elise 接近／離開 | ✅ | 約 10 秒，需先啟用瀏覽器音效 |
| 音效錯誤隔離 | ✅ | 音效失敗不阻斷狀態機 |
| Wokwi 外部連結 | ✅ | 外部硬體展示入口 |

## Backend / SQL

| 項目 | 狀態 | 備註 |
|---|---:|---|
| `/api/sim/state` | ✅ | `SIM-001`、`source=SIMULATION` |
| `/api/sim/emotion` | ✅ | 模擬情緒事件 |
| `/api/sim/collection` | ✅ | 收運事件寫入 |
| `/api/device/status` | ✅ | 裝置狀態查詢 |
| `/api/stats/emotion` | ✅ | 情緒分布 |
| `/api/stats/weekly` | ✅ | 每週清運 |
| `/api/stats/summary` | ✅ | 摘要資料 |
| demo-data fallback | ✅ | API 不可用時維持展示 |

## 完成結論

```text
ESP32 實機驗證：✅ 全部完成
Web simulator 驗證：✅ 完成
Backend／SQL API 驗證：✅ 完成
```
