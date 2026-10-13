import random
from app.db import execute_query

def pick_worker_prompt() -> str:
    """工作人員回應機率：小心 50% / 謝謝 25% / 回收 25%"""
    r = random.randint(0, 99)
    if r < 50:
        return "CAREFUL"
    elif r < 75:
        return "THANKS"
    else:
        return "RECYCLE"

def settle_bags(bags: int, worker_prompt: str) -> tuple[int, int]:
    """清運結算：CAREFUL/THANKS 減 5，RECYCLE 隨機減 1-4（退回相應袋數）"""
    if worker_prompt in ["CAREFUL", "THANKS"]:
        new_bags = max(0, bags - 5)
        return new_bags, 0
    elif worker_prompt == "RECYCLE":
        if bags <= 0:
            return 0, 0
        removed = random.randint(1, 4)
        removed = min(removed, bags)
        return bags - removed, removed
    return bags, 0

def update_device_state(device_id: str, state: str, bags: int, daily: int, source: str):
    """UPSERT 裝置狀態"""
    sql = """
        INSERT INTO device_state (device_id, source, current_state, bags_count, daily_count, last_heartbeat)
        VALUES (:device_id, :source, :state, :bags, :daily, NOW())
        ON DUPLICATE KEY UPDATE
            source = :source,
            current_state = :state,
            bags_count = :bags,
            daily_count = :daily,
            last_heartbeat = NOW()
    """
    execute_query(sql, {
        "device_id": device_id,
        "source": source,
        "state": state,
        "bags": bags,
        "daily": daily
    })

def get_device_state(device_id: str):
    """查詢單一裝置當前狀態"""
    sql = "SELECT * FROM device_state WHERE device_id = :device_id LIMIT 1"
    result = execute_query(sql, {"device_id": device_id})
    return result[0] if result else None