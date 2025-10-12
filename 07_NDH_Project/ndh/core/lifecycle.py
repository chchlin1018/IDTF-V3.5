"""
NDH Lifecycle Management
Handles application startup and shutdown events
"""

from contextlib import asynccontextmanager

from fastapi import FastAPI
from loguru import logger

from ndh.core.config import get_settings
from ndh.queue.manager import QueueManager


# Global instances
queue_manager: QueueManager = None


@asynccontextmanager
async def lifespan(app: FastAPI):
    """
    Application lifespan context manager
    Handles startup and shutdown events
    """
    # Startup
    logger.info("NDH Service Starting...")
    
    settings = get_settings()
    
    # Initialize Queue Manager
    global queue_manager
    queue_manager = QueueManager()
    await queue_manager.initialize()
    logger.info("Queue Manager initialized")
    
    # Initialize Redis if enabled
    if settings.redis_enabled:
        try:
            from ndh.queue.redis_queue import RedisQueue
            redis_queue = RedisQueue()
            await redis_queue.connect()
            logger.info(f"Connected to Redis at {settings.redis_host}:{settings.redis_port}")
        except Exception as e:
            logger.warning(f"Failed to connect to Redis: {e}")
            logger.warning("Continuing without Redis support")
    
    logger.info("NDH Service Started Successfully")
    
    yield
    
    # Shutdown
    logger.info("NDH Service Shutting Down...")
    
    # Cleanup Queue Manager
    if queue_manager:
        await queue_manager.cleanup()
        logger.info("Queue Manager cleaned up")
    
    logger.info("NDH Service Stopped")


def get_queue_manager() -> QueueManager:
    """Get the global queue manager instance"""
    return queue_manager

