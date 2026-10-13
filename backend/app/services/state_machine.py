from dataclasses import dataclass
from datetime import datetime, timedelta

from .direction_mapper import emotion_for
from .worker_prompt import pick_worker_prompt


MESSAGES = {
    "REST": "垃圾車休息中",
    "IDLE": "垃圾車還沒來",
    "ARRIVED": "垃圾車來了！",
    "COLLECTED": "垃圾已清運，心情清爽",
    "RETURNED": "不好意思，垃圾車不接回收垃圾",
}


@dataclass
class TruckState:
    state: str = "IDLE"
    selected_emotion: dict | None = None
    last_collect: datetime | None = None
    worker_prompt: str | None = None


class StateMachine:
    def __init__(self):
        self.data = TruckState()

    def status(self, now: datetime, is_open: bool = True) -> dict:
        state = self.data.state if is_open else "REST"
        colors = {"REST": "GRAY", "IDLE": "BLUE", "ARRIVED": "GREEN", "COLLECTED": "GREEN", "RETURNED": "RED"}
        patterns = {"RETURNED": "BLINK_FAST", "COLLECTED": "DOUBLE_BLINK"}
        return {
            "state": state,
            "message": MESSAGES[state],
            "can_throw": state == "ARRIVED" and is_open,
            "current_time": now.isoformat(timespec="seconds"),
            "last_collect": self.data.last_collect.isoformat(timespec="minutes") if self.data.last_collect else None,
            "selected_emotion": self.data.selected_emotion["text"] if self.data.selected_emotion else None,
            "led": {"color": colors[state], "pattern": patterns.get(state, "SOLID")},
            "worker_prompt": self.data.worker_prompt,
        }

    def arrive(self):
        self.data.state = "ARRIVED"
        self.data.worker_prompt = None

    def joystick(self, direction: str | None, pressed: bool, now: datetime):
        if self.data.state == "RETURNED" and pressed:
            self.data.state = "IDLE"
            self.data.selected_emotion = None
            self.data.worker_prompt = None
            return
        if direction and self.data.state == "ARRIVED":
            self.data.selected_emotion = emotion_for(direction)
        if pressed and self.data.state == "ARRIVED" and self.data.selected_emotion:
            if self.data.last_collect and now - self.data.last_collect < timedelta(minutes=10):
                return
            prompt = pick_worker_prompt()
            self.data.worker_prompt = prompt["text"]
            self.data.state = prompt["result"]
            if prompt["result"] == "COLLECTED":
                self.data.last_collect = now

    def reset(self):
        self.data = TruckState()
