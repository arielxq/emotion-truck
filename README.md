# Emotion Garbage Truck

情緒垃圾車：用搖桿選擇情緒，交給一台有脾氣的垃圾車。

## MVP

目前版本先以 Web 模擬器驗證核心流程：

- FastAPI 狀態 API
- REST / IDLE / ARRIVED / COLLECTED / RETURNED 狀態機
- 測試模式可快速觸發垃圾車
- 虛擬搖桿與垃圾袋動畫
- 工作人員提示隨機結果

## 啟動

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
uvicorn backend.app.main:app --reload
```

開啟 <http://127.0.0.1:8000>。

測試：

```bash
pytest
```

預設為 `MODE=test`。正式模式與 MySQL、ESP32 接線將在後續階段加入。
