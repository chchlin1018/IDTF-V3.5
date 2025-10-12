"""
NDH (Neutral Data Hub) Main Application
Main entry point for the NDH service
"""

import asyncio
import sys
from pathlib import Path

import uvicorn
from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from loguru import logger

# Add project root to path
sys.path.insert(0, str(Path(__file__).parent.parent))

from ndh.api import health, data, config as config_api
from ndh.core.config import get_settings
from ndh.core.lifecycle import lifespan

# Configure logger
logger.remove()
logger.add(
    sys.stdout,
    format="<green>{time:YYYY-MM-DD HH:mm:ss}</green> | <level>{level: <8}</level> | <cyan>{name}</cyan>:<cyan>{function}</cyan>:<cyan>{line}</cyan> - <level>{message}</level>",
    level="INFO"
)
logger.add(
    "logs/ndh_{time:YYYY-MM-DD}.log",
    rotation="00:00",
    retention="30 days",
    level="DEBUG"
)

# Create FastAPI app
app = FastAPI(
    title="NDH - Neutral Data Hub",
    description="Industrial Data Integration Middleware Platform",
    version="1.0.0",
    lifespan=lifespan
)

# Configure CORS
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # In production, specify allowed origins
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Include routers
app.include_router(health.router, tags=["Health"])
app.include_router(data.router, prefix="/api/v1/data", tags=["Data"])
app.include_router(config_api.router, prefix="/api/v1/config", tags=["Configuration"])


@app.get("/")
async def root():
    """Root endpoint"""
    return {
        "service": "NDH - Neutral Data Hub",
        "version": "1.0.0",
        "status": "running",
        "docs": "/docs",
        "health": "/health"
    }


def main():
    """Main function to start the NDH service"""
    settings = get_settings()
    
    logger.info("=" * 60)
    logger.info("Starting NDH (Neutral Data Hub)")
    logger.info("=" * 60)
    logger.info(f"Host: {settings.host}")
    logger.info(f"Port: {settings.port}")
    logger.info(f"Environment: {settings.environment}")
    logger.info(f"Redis: {'Enabled' if settings.redis_enabled else 'Disabled'}")
    logger.info("=" * 60)
    
    try:
        uvicorn.run(
            "ndh.main:app",
            host=settings.host,
            port=settings.port,
            reload=settings.environment == "development",
            log_level="info"
        )
    except KeyboardInterrupt:
        logger.info("Shutting down NDH service...")
    except Exception as e:
        logger.error(f"Failed to start NDH service: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()

