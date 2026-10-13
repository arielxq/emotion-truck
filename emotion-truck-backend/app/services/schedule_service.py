import random
from datetime import date
from app.db import execute_query

def generate_daily_schedule(target_date: date = None) -> dict:
    """隨機產生兩次清運時間（09:00-20:00），間隔至少 3 小時"""
    target_date = target_date or date.today()
    
    # 判斷是否公休日（週三=2, 週日=6）
    is_holiday = 1 if target_date.weekday() in (2, 6) else 0
    
    # 第一次清運：09:00 - 13:59
    hour1 = random.randint(9, 13)
    minute1 = random.randint(0, 59)
    
    # 第二次清運：hour1 + 3 到 20:00
    min_hour2 = max(hour1 + 3, 14)
    hour2 = random.randint(min_hour2, 19)
    minute2 = random.randint(0, 59)
    
    play_time_1 = f"{hour1:02d}:{minute1:02d}:00"
    play_time_2 = f"{hour2:02d}:{minute2:02d}:00"
    
    sql = """
        INSERT INTO truck_schedule (schedule_date, play_time_1, play_time_2, is_holiday, collection_count)
        VALUES (:date, :time1, :time2, :is_holiday, 0)
        ON DUPLICATE KEY UPDATE
            play_time_1 = :time1,
            play_time_2 = :time2,
            is_holiday = :is_holiday
    """
    execute_query(sql, {
        "date": target_date,
        "time1": play_time_1,
        "time2": play_time_2,
        "is_holiday": is_holiday
    })
    
    return {
        "schedule_date": str(target_date),
        "play_time_1": play_time_1,
        "play_time_2": play_time_2,
        "is_holiday": is_holiday,
        "collection_count": 0
    }

def get_today_schedule() -> dict:
    """獲取今日排程，若無則自動產生"""
    sql = "SELECT * FROM truck_schedule WHERE schedule_date = CURDATE() LIMIT 1"
    result = execute_query(sql)
    if not result:
        return generate_daily_schedule()
    return result[0]

def reset_schedule() -> dict:
    """重置今日排程"""
    return generate_daily_schedule(date.today())