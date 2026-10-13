from app.db import execute_query

def get_summary_stats(source: str = "HARDWARE") -> list[dict]:
    """取得清運總統計（支援 HARDWARE / SIMULATION / ALL）"""
    sql = """
        SELECT 
            COUNT(*) as total_collections,
            COALESCE(SUM(returned_bags), 0) as total_returned,
            COALESCE(ROUND(AVG(bags_before), 2), 0.0) as avg_bags
        FROM collection_log
        WHERE (:source = 'ALL' OR source = :source)
    """
    return execute_query(sql, {"source": source})

def get_weekly_stats(source: str = "HARDWARE") -> list[dict]:
    """取得最近 7 天的每日清運次數"""
    sql = """
        SELECT 
            DATE(collected_at) as date,
            COUNT(*) as count
        FROM collection_log
        WHERE (:source = 'ALL' OR source = :source)
          AND collected_at >= DATE_SUB(CURDATE(), INTERVAL 7 DAY)
        GROUP BY DATE(collected_at)
        ORDER BY date ASC
    """
    return execute_query(sql, {"source": source})

def get_emotion_stats(source: str = "HARDWARE") -> list[dict]:
    """取得各類情緒被丟棄的次數分布"""
    sql = """
        SELECT 
            emotion_type,
            COUNT(*) as count
        FROM emotion_log
        WHERE (:source = 'ALL' OR source = :source)
        GROUP BY emotion_type
        ORDER BY count DESC
    """
    return execute_query(sql, {"source": source})

