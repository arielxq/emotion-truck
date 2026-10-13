import os
from sqlalchemy import create_engine, text
from sqlalchemy.orm import sessionmaker
from app.config import settings

# 優先讀取環境變數 DATABASE_URL，預設採用 MySQL 8.0 + PyMySQL
DATABASE_URL = os.getenv(
    "DATABASE_URL",
    f"mysql+pymysql://{settings.DB_USER}:{settings.DB_PASSWORD}"
    f"@{settings.DB_HOST}:{settings.DB_PORT}/{settings.DB_NAME}"
    "?charset=utf8mb4"
)

# 連線池設定：啟用 pool_pre_ping 確保連線有效，回收時間設為 3600 秒
engine = create_engine(
    DATABASE_URL,
    pool_pre_ping=True,
    pool_recycle=3600,
    echo=False
)

SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)

def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()

def execute_query(sql: str, params: dict = None):
    """參數化查詢，防止 SQL 注入（使用 SQLAlchemy text() 與綁定參數）"""
    with engine.connect() as conn:
        result = conn.execute(text(sql), params or {})
        if result.returns_rows:
            return [dict(row._mapping) for row in result]
        conn.commit()
        return None