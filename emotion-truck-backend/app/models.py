import re
from typing import Optional
from pydantic import BaseModel, Field, field_validator

DEVICE_ID_REGEX = re.compile(r'^[A-Za-z0-9_-]+$')

def validate_device_id_str(v: str) -> str:
    if not v or not DEVICE_ID_REGEX.match(v):
        raise ValueError('device_id must be alphanumeric and may only contain - or _')
    return v

class DeviceStateReport(BaseModel):
    device_id: str = Field(..., min_length=1, max_length=32)
    state: str = Field(..., min_length=1, max_length=32)
    bags: int = Field(0, ge=0, le=100)
    daily_count: int = Field(0, ge=0, le=100)
    
    @field_validator('device_id')
    @classmethod
    def check_device_id(cls, v: str) -> str:
        return validate_device_id_str(v)

class DeviceHeartbeatReport(BaseModel):
    device_id: str = Field("ESP32-001", min_length=1, max_length=32)
    state: Optional[str] = Field("REST_CLOSED", max_length=32)
    bags: Optional[int] = Field(0, ge=0, le=100)
    daily_count: Optional[int] = Field(0, ge=0, le=100)
    
    @field_validator('device_id')
    @classmethod
    def check_device_id(cls, v: str) -> str:
        return validate_device_id_str(v)

class SimStateReport(BaseModel):
    device_id: str = Field("SIM-001", min_length=1, max_length=32)
    state: str = Field(..., min_length=1, max_length=32)
    bags: int = Field(0, ge=0, le=100)
    daily_count: int = Field(0, ge=0, le=100)

    @field_validator('device_id')
    @classmethod
    def check_device_id(cls, v: str) -> str:
        return validate_device_id_str(v)

class SimEmotionReport(BaseModel):
    device_id: str = Field("SIM-001", min_length=1, max_length=32)
    direction: str = Field(..., max_length=16, pattern=r'^(UP|DOWN|LEFT|RIGHT)$')
    emotion_type: str = Field(..., max_length=32, pattern=r'^(GAN|KAO|CAO)$')
    emotion_text: str = Field(..., max_length=16)

    @field_validator('device_id')
    @classmethod
    def check_device_id(cls, v: str) -> str:
        return validate_device_id_str(v)

class SimCollectionReport(BaseModel):
    device_id: str = Field("SIM-001", min_length=1, max_length=32)
    worker_prompt: str = Field(..., max_length=32, pattern=r'^(CAREFUL|THANKS|RECYCLE)$')
    bags_before: int = Field(..., ge=0, le=100)
    bags_after: int = Field(..., ge=0, le=100)
    returned_bags: int = Field(0, ge=0, le=100)

    @field_validator('device_id')
    @classmethod
    def check_device_id(cls, v: str) -> str:
        return validate_device_id_str(v)