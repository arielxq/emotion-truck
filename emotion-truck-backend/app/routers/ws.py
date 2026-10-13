from fastapi import APIRouter, WebSocket, WebSocketDisconnect
import asyncio
from app.services.state_service import get_device_state

router = APIRouter()

@router.websocket("/ws/status")
async def websocket_status(websocket: WebSocket):
    await websocket.accept()
    try:
        while True:
            hardware = get_device_state("ESP32-001")
            simulation = get_device_state("SIM-001")
            await websocket.send_json({
                "hardware": hardware,
                "simulation": simulation
            })
            await asyncio.sleep(1)
    except WebSocketDisconnect:
        pass