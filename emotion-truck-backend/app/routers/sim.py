from fastapi import APIRouter
from app.models import SimStateReport, SimEmotionReport, SimCollectionReport
from app.services.state_service import update_device_state
from app.db import execute_query

router = APIRouter(prefix="/api/sim", tags=["simulation"])

@router.post("/state")
async def sim_state(report: SimStateReport):
    update_device_state(
        report.device_id, report.state,
        report.bags, report.daily_count, "SIMULATION"
    )
    return {"status": "ok"}

@router.post("/emotion")
async def sim_emotion(report: SimEmotionReport):
    sql = """
        INSERT INTO emotion_log (device_id, source, direction, emotion_type, emotion_text)
        VALUES (:device_id, 'SIMULATION', :direction, :emotion_type, :emotion_text)
    """
    execute_query(sql, {
        "device_id": report.device_id,
        "direction": report.direction,
        "emotion_type": report.emotion_type,
        "emotion_text": report.emotion_text
    })
    return {"status": "ok"}

@router.post("/collection")
async def sim_collection(report: SimCollectionReport):
    sql = """
        INSERT INTO collection_log 
        (device_id, source, worker_prompt, bags_before, bags_after, returned_bags)
        VALUES (:device_id, 'SIMULATION', :prompt, :before, :after, :returned)
    """
    execute_query(sql, {
        "device_id": report.device_id,
        "prompt": report.worker_prompt,
        "before": report.bags_before,
        "after": report.bags_after,
        "returned": report.returned_bags
    })
    return {"status": "ok"}

@router.post("/reset")
async def sim_reset():
    execute_query("DELETE FROM emotion_log WHERE source = 'SIMULATION'")
    execute_query("DELETE FROM collection_log WHERE source = 'SIMULATION'")
    execute_query(
        "UPDATE device_state SET current_state = 'REST_CLOSED', bags_count = 0, daily_count = 0 WHERE source = 'SIMULATION'"
    )
    return {"status": "ok"}