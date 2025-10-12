"""
NDH Configuration API
Handles configuration management
"""

from typing import Dict, Any

from fastapi import APIRouter
from pydantic import BaseModel

from ndh.core.config import get_settings

router = APIRouter()


class ConfigResponse(BaseModel):
    """Configuration response model"""
    status: str
    config: Dict[str, Any]


@router.get("/", response_model=ConfigResponse)
async def get_config():
    """
    Get current NDH configuration
    
    Returns:
        Current configuration (excluding sensitive data)
    """
    settings = get_settings()
    
    # Return non-sensitive configuration
    config = {
        "server": {
            "host": settings.host,
            "port": settings.port,
            "environment": settings.environment
        },
        "redis": {
            "enabled": settings.redis_enabled,
            "host": settings.redis_host if settings.redis_enabled else None,
            "port": settings.redis_port if settings.redis_enabled else None
        },
        "queue": {
            "max_size": settings.queue_max_size,
            "timeout": settings.queue_timeout
        },
        "api": {
            "rate_limit": settings.api_rate_limit,
            "timeout": settings.api_timeout
        },
        "websocket": {
            "heartbeat_interval": settings.ws_heartbeat_interval,
            "max_connections": settings.ws_max_connections
        }
    }
    
    return ConfigResponse(
        status="success",
        config=config
    )

