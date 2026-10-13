from datetime import datetime
from pathlib import Path

from fastapi import FastAPI
from fastapi.responses import FileResponse
from fastapi.staticfiles import StaticFiles

from .config import get_settings
from .schemas import JoystickRequest, StatusResponse
from .services.state_machine import StateMachine

BASE_DIR = Path(__file__).resolve().parents[2]
app = FastAPI(title="Emotion Garbage Truck API", version="0.1.0")
machine = StateMachine()
settings = get_settings()


def now() -> datetime:
    return datetime.now()


@app.get("/", include_in_schema=False)
def home():
    return FileResponse(BASE_DIR / "frontend" / "index.html")


app.mount("/static", StaticFiles(directory=BASE_DIR / "frontend"), name="static")


@app.get("/api/status", response_model=StatusResponse)
def status():
    return machine.status(now())


@app.get("/api/clock")
def clock():
    current = now()
    return {"date": current.strftime("%Y-%m-%d"), "time": current.strftime("%H:%M:%S"), "last_collect": machine.data.last_collect}


@app.post("/api/joystick", response_model=StatusResponse)
def joystick(payload: JoystickRequest):
    machine.joystick(payload.direction, payload.pressed, now())
    return machine.status(now())


@app.post("/api/arrive", response_model=StatusResponse)
def arrive():
    machine.arrive()
    return machine.status(now())


@app.post("/api/reset", response_model=StatusResponse)
def reset():
    if settings.mode != "test":
        return machine.status(now())
    machine.reset()
    return machine.status(now())
