"""
NDH Health Check API
Provides health check and status endpoints
"""

import platform
import psutil
from datetime import datetime
from typing import Dict, Any

from fastapi import APIRouter
from pydantic import BaseModel

from ndh.core.config import get_settings

router = APIRouter()


class HealthResponse(BaseModel):
    """Health check response model"""
    status: str
    timestamp: datetime
    version: str
    environment: str


class SystemInfoResponse(BaseModel):
    """System information response model"""
    status: str
    timestamp: datetime
    system: Dict[str, Any]
    resources: Dict[str, Any]
    services: Dict[str, Any]


@router.get("/health", response_model=HealthResponse)
async def health_check():
    """
    Basic health check endpoint
    Returns the service status
    """
    settings = get_settings()
    
    return HealthResponse(
        status="healthy",
        timestamp=datetime.now(),
        version="1.0.0",
        environment=settings.environment
    )


@router.get("/health/detailed", response_model=SystemInfoResponse)
async def detailed_health_check():
    """
    Detailed health check endpoint
    Returns comprehensive system information
    """
    settings = get_settings()
    
    # System information
    system_info = {
        "platform": platform.system(),
        "platform_release": platform.release(),
        "platform_version": platform.version(),
        "architecture": platform.machine(),
        "processor": platform.processor(),
        "python_version": platform.python_version()
    }
    
    # Resource usage
    memory = psutil.virtual_memory()
    disk = psutil.disk_usage('/')
    
    resources = {
        "cpu_percent": psutil.cpu_percent(interval=1),
        "cpu_count": psutil.cpu_count(),
        "memory_total_gb": round(memory.total / (1024**3), 2),
        "memory_available_gb": round(memory.available / (1024**3), 2),
        "memory_percent": memory.percent,
        "disk_total_gb": round(disk.total / (1024**3), 2),
        "disk_used_gb": round(disk.used / (1024**3), 2),
        "disk_percent": disk.percent
    }
    
    # Service status
    services = {
        "redis": "enabled" if settings.redis_enabled else "disabled",
        "queue": "running",
        "api": "running"
    }
    
    return SystemInfoResponse(
        status="healthy",
        timestamp=datetime.now(),
        system=system_info,
        resources=resources,
        services=services
    )


@router.get("/ping")
async def ping():
    """Simple ping endpoint"""
    return {"ping": "pong", "timestamp": datetime.now()}

