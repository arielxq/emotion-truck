# 情緒垃圾車 — 施行日誌

## 2026-10-14｜專題成果完成確認

### 實機驗證結果

ESP32 實機已完成並確認以下項目：

- LCD1602 I2C 顯示與自訂表情
- HW504 搖桿 VRx、VRy 與 SW 輸入
- GPIO27 單色 LED 的熄滅、長亮與狀態閃爍
- GPIO25 無源蜂鳴器、提示音與 Fur Elise
- `REST_CLOSED`、`IDLE`、`ACCUMULATING`、`FULL`
- `APPROACHING`、`ARRIVED`、`THROW_READY`、`THROWING`
- `RESPONDING_CAREFUL`、`RESPONDING_THANKS`、`RESPONDING_RECYCLE`
- `COLLECTED` 與 `REST_DAILY_LIMIT`
- 情緒累積三次確認與 bags 上限 5
- `atan2()` 角度累積 360°、720°、1080° 三圈
- 清運確認、bags 結算與剩餘 bags 繼承
- 每日清運上限 2 次
- `reset day` 清除每日次數並保留 bags

### 正式狀態流程

```text
REST_CLOSED → IDLE → ACCUMULATING → FULL → APPROACHING
→ ARRIVED → THROW_READY → THROWING
→ RESPONDING_CAREFUL / RESPONDING_THANKS / RESPONDING_RECYCLE
→ COLLECTED → IDLE 或 REST_DAILY_LIMIT
```

### 正式硬體規格

| 元件 | 接點 | 驗證結果 |
|---|---|---|
| HW504 VRx | GPIO34 | ✅ 完成 |
| HW504 VRy | GPIO35 | ✅ 完成 |
| HW504 SW | GPIO32 | ✅ 完成 |
| LCD1602 SDA／SCL | GPIO21／GPIO22 | ✅ 完成 |
| LCD I2C 位址 | `0x27` | ✅ 完成 |
| 無源蜂鳴器 | GPIO25 | ✅ 完成 |
| 單色 LED | GPIO27，串 220Ω | ✅ 完成 |

### bags 與每日清運規則

- 情緒完成三次 push 並確認後，bags 增加 1，最高為 5。
- CAREFUL 與 THANKS 收走最多 5 包。
- RECYCLE 在原有 N > 0 時隨機保留 1～N 包，收走 N 減保留量；原有 0 包維持 0。
- 清運完成後不無條件清空 bags。
- 每日最多清運 2 次，第二次完成後進入 `REST_DAILY_LIMIT`。
- `reset day` 只清除每日清運次數，保留 bags。

### 聲音、LCD 與 LED

- `ACCUMULATING`、`FULL`、`THROW_READY` 與三種回應均播放對應音效。
- `APPROACHING` 播放約 10 秒 Fur Elise，完成後進入 `ARRIVED`。
- `COLLECTED` 播放離開段落並完成漸弱。
- LED 依各狀態執行熄滅、短閃、快閃、慢閃、長亮或雙短閃。
- LCD 顯示狀態文字、bags、旋轉進度與表情。

## 2026-10-14｜Web、後端與展示成果

- 舊版 simulator（後續整合至 showcase）完成 Game Boy 風格虛擬搖桿、A/B 按鍵與 13-state 流程。
- 該版完成 Web Audio、Fur Elise、Chart.js 統計與測試控制。
- FastAPI simulation/device/stats API 已建立；前端保留 demo-data fallback。
- `SIM-001` 與 `source=SIMULATION` 用於模擬事件。
- `truck/showcase.html` 提供 3D 展示與 Wokwi 外部硬體入口。
- Web、ESP32 與 Wokwi 的功能驗證已分開完成記錄。

## 2026-10-31｜Showcase 整合與統計補強

- 刪除舊的獨立入口頁與 Game Boy 模擬器，確認無頁面依賴。
- 將 simulator 的統計與操作說明整合至 `truck/showcase.html` Section 10b；Section 10 樣式、輸入與 3 秒展示節奏保持不變。
- Section 10b 提供三張 Chart.js 示範圖表、今日狀態同步、操作說明與 API 預留端點；不即時呼叫 FastAPI。
- 新增 `GET /api/stats/responses`，後端 `/stats` 頁新增工作人員回應分布圖。
- 系統架構圖與文件改以 showcase 為前端入口；FastAPI／SQL 標示為預留架構。

## 專案完成結論

```text
ESP32 實機：✅ 全部完成並確認
Web showcase：✅ 完成並確認
Backend／SQL：✅ 完成並確認
文件與規格：✅ 已同步
```
