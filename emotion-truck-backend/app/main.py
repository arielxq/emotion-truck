from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from fastapi.responses import FileResponse
from fastapi.middleware.cors import CORSMiddleware
from app.config import settings
from app.security import RateLimitMiddleware
from app.routers import device, sim, log, stats, ws
import os

app = FastAPI(title="Emotion Truck Backend", version="1.0")

# CORS
app.add_middleware(
    CORSMiddleware,
    allow_origins=settings.ALLOWED_ORIGINS,
    allow_methods=["GET", "POST"],
    allow_headers=["Content-Type"],
)

# 速率限制
app.add_middleware(RateLimitMiddleware)

# 路由
app.include_router(device.router)
app.include_router(sim.router)
app.include_router(log.router)
app.include_router(stats.router)
app.include_router(ws.router)

# 靜態檔案
app.mount("/static", StaticFiles(directory="static"), name="static")

@app.get("/")
async def index():
    return FileResponse("static/index.html")

@app.get("/sim")
async def sim_page():
    return FileResponse("static/sim.html")

@app.get("/history")
async def history_page():
    return FileResponse("static/history.html")

@app.get("/stats")
async def stats_page():
    return FileResponse("static/stats.html")