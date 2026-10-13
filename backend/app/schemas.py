from typing import Literal

from pydantic import BaseModel


State = Literal["REST", "IDLE", "ARRIVED", "COLLECTED", "RETURNED"]
Direction = Literal["UP", "LEFT", "RIGHT"]


class JoystickRequest(BaseModel):
    direction: Direction | None = None
    pressed: bool = False


class StatusResponse(BaseModel):
    state: State
    message: str
    can_throw: bool
    current_time: str
    last_collect: str | None
    selected_emotion: str | None
    led: dict[str, str]
    worker_prompt: str | None = None
