from fastapi import APIRouter, Query
from app.db import execute_query

router = APIRouter(prefix="/api/log", tags=["log"])

@router.get("/emotion")
async def get_emotion_logs(
    source: str = Query("HARDWARE", pattern=r"^(HARDWARE|SIMULATION|ALL)$"),
    limit: int = Query(100, ge=1, le=200)
):
    sql = """
        SELECT * FROM emotion_log
        WHERE (:source = 'ALL' OR source = :source)
        ORDER BY created_at DESC
        LIMIT :limit
    """
    return execute_query(sql, {"source": source, "limit": limit})

@router.get("/collection")
async def get_collection_logs(
    source: str = Query("HARDWARE", pattern=r"^(HARDWARE|SIMULATION|ALL)$"),
    limit: int = Query(100, ge=1, le=200)
):
    sql = """
        SELECT * FROM collection_log
        WHERE (:source = 'ALL' OR source = :source)
        ORDER BY collected_at DESC
        LIMIT :limit
    """
    return execute_query(sql, {"source": source, "limit": limit})

@router.get("/today")
async def get_today_logs(
    source: str = Query("HARDWARE", pattern=r"^(HARDWARE|SIMULATION|ALL)$")
):
    sql = """
        SELECT * FROM collection_log
        WHERE (:source = 'ALL' OR source = :source)
          AND DATE(collected_at) = CURDATE()
        ORDER BY collected_at DESC
    """
    return execute_query(sql, {"source": source})