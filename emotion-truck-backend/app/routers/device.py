from fastapi import APIRouter, HTTPException, Query
from app.models import DeviceStateReport, DeviceHeartbeatReport
from app.services.state_service import update_device_state, get_device_state

router = APIRouter(prefix="/api/device", tags=["device"])

@router.post("/state")
async def report_state(report: DeviceStateReport):
    update_device_state(
        report.device_id, report.state,
        report.bags, report.daily_count, "HARDWARE"
    )
    return {"status": "ok"}

@router.post("/heartbeat")
async def heartbeat(report: DeviceHeartbeatReport):
    update_device_state(
        report.device_id, report.state or "REST_CLOSED",
        report.bags or 0, report.daily_count or 0, "HARDWARE"
    )
    return {"status": "ok"}

@router.get("/status")
async def get_status(device_id: str = Query("ESP32-001", pattern=r'^[A-Za-z0-9_-]+$')):
    state = get_device_state(device_id)
    if not state:
        raise HTTPException(status_code=404, detail="Device not found")
    return state