from fastapi import APIRouter
from app.db import execute_query
from app.services.stats_service import get_response_stats

router = APIRouter(prefix="/api/stats", tags=["stats"])

@router.get("/summary")
async def get_summary(source: str = "HARDWARE"):
    sql = """
        SELECT 
            COUNT(*) as total_collections,
            SUM(returned_bags) as total_returned,
            AVG(bags_before) as avg_bags
        FROM collection_log
        WHERE source = :source
    """
    return execute_query(sql, {"source": source})

@router.get("/weekly")
async def get_weekly(source: str = "HARDWARE"):
    sql = """
        SELECT 
            DATE(collected_at) as date,
            COUNT(*) as count
        FROM collection_log
        WHERE source = :source
          AND collected_at >= DATE_SUB(CURDATE(), INTERVAL 7 DAY)
        GROUP BY DATE(collected_at)
        ORDER BY date
    """
    return execute_query(sql, {"source": source})

@router.get("/emotion")
async def get_emotion_stats(source: str = "HARDWARE"):
    sql = """
        SELECT 
            emotion_type,
            COUNT(*) as count
        FROM emotion_log
        WHERE source = :source
        GROUP BY emotion_type
        ORDER BY count DESC
    """
    return execute_query(sql, {"source": source})


@router.get("/responses")
async def get_responses(source: str = "HARDWARE"):
    """工作人員回應分布（支援 HARDWARE / SIMULATION / ALL）"""
    return get_response_stats(source)
