from functools import lru_cache

from pydantic_settings import BaseSettings, SettingsConfigDict


class Settings(BaseSettings):
    mode: str = "test"
    app_timezone: str = "Asia/Taipei"
    open_time_start: str = "09:00"
    open_time_end: str = "20:00"
    collection_interval_minutes: int = 10

    model_config = SettingsConfigDict(env_file=".env", case_sensitive=False)


@lru_cache
def get_settings() -> Settings:
    return Settings()
