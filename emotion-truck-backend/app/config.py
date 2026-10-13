import os
from dotenv import load_dotenv

load_dotenv()

class Settings:
    DB_HOST: str = os.getenv("DB_HOST", "localhost")
    DB_PORT: int = int(os.getenv("DB_PORT", 3306))
    DB_USER: str = os.getenv("DB_USER", "emotion")
    DB_PASSWORD: str = os.getenv("DB_PASSWORD", "")
    DB_NAME: str = os.getenv("DB_NAME", "emotion_truck")
    
    APP_HOST: str = os.getenv("APP_HOST", "0.0.0.0")
    APP_PORT: int = int(os.getenv("APP_PORT", 8000))
    
    # CORS: 僅允許指定來源，不允許萬用字元 *
    ALLOWED_ORIGINS: list = [
        "http://localhost:8000",
        "http://127.0.0.1:8000",
        f"http://{os.getenv('APP_HOST', 'localhost')}:{os.getenv('APP_PORT', 8000)}"
    ]

settings = Settings()