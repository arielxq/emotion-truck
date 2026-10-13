import os
import time
from collections import defaultdict
from fastapi import Request
from fastapi.responses import JSONResponse
from starlette.middleware.base import BaseHTTPMiddleware

# 簡單速率限制（每分鐘最多 60 次）
request_counts = defaultdict(list)
RATE_LIMIT = 60
RATE_WINDOW = 60

class RateLimitMiddleware(BaseHTTPMiddleware):
    async def dispatch(self, request: Request, call_next):
        # 取得用戶端 IP，防止 None
        client_ip = request.client.host if request.client else "127.0.0.1"
        now = time.time()
        
        # 清理過期記錄
        request_counts[client_ip] = [
            t for t in request_counts[client_ip]
            if now - t < RATE_WINDOW
        ]
        
        if len(request_counts[client_ip]) >= RATE_LIMIT:
            return JSONResponse(
                status_code=429,
                content={"error": "Too many requests. Please wait a moment."}
            )
        
        request_counts[client_ip].append(now)
        return await call_next(request)

# 靜態檔案白名單與防路徑穿越
ALLOWED_EXTENSIONS = {'.html', '.css', '.js', '.png', '.jpg', '.jpeg', '.svg', '.ico', '.json', '.woff', '.woff2'}
ALLOWED_PAGES = {'index.html', 'sim.html', 'history.html', 'stats.html'}

def validate_path_safe(path: str) -> bool:
    """檢查路徑安全，禁止 '..' 與絕對路徑，防止路徑穿越"""
    if not path or not isinstance(path, str):
        return False
    if '..' in path or path.startswith('/') or path.startswith('\\'):
        return False
    normalized = os.path.normpath(path)
    if normalized.startswith('..') or os.path.isabs(normalized):
        return False
    return True

def is_allowed_static_extension(filename: str) -> bool:
    """驗證靜態副檔名是否在白名單內"""
    _, ext = os.path.splitext(filename.lower())
    return ext in ALLOWED_EXTENSIONS