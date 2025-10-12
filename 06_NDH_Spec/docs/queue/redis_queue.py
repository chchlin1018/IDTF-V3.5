"""
NDH Redis Queue Implementation
Provides Redis-based message queue
"""

import json
from typing import Dict, Any, Optional

import redis.asyncio as redis
from loguru import logger

from ndh.core.config import get_settings


class RedisQueue:
    """
    Redis-based message queue implementation
    """
    
    def __init__(self):
        self.settings = get_settings()
        self.client: Optional[redis.Redis] = None
        self.pubsub: Optional[redis.client.PubSub] = None
    
    async def connect(self):
        """Connect to Redis server"""
        try:
            self.client = redis.Redis(
                host=self.settings.redis_host,
                port=self.settings.redis_port,
                db=self.settings.redis_db,
                password=self.settings.redis_password,
                decode_responses=True
            )
            
            # Test connection
            await self.client.ping()
            
            # Create pub/sub client
            self.pubsub = self.client.pubsub()
            
            logger.info(f"Connected to Redis at {self.settings.redis_host}:{self.settings.redis_port}")
        
        except Exception as e:
            logger.error(f"Failed to connect to Redis: {e}")
            raise
    
    async def disconnect(self):
        """Disconnect from Redis server"""
        if self.pubsub:
            await self.pubsub.close()
        
        if self.client:
            await self.client.close()
        
        logger.info("Disconnected from Redis")
    
    async def push(self, queue_name: str, data: Dict[str, Any]):
        """
        Push data to Redis list (queue)
        
        Args:
            queue_name: Name of the queue
            data: Data to push
        """
        if not self.client:
            raise RuntimeError("Redis client not connected")
        
        try:
            data_json = json.dumps(data)
            await self.client.rpush(queue_name, data_json)
            logger.debug(f"Pushed to Redis queue {queue_name}")
        
        except Exception as e:
            logger.error(f"Failed to push to Redis: {e}")
            raise
    
    async def pop(self, queue_name: str, timeout: int = 0) -> Optional[Dict[str, Any]]:
        """
        Pop data from Redis list (queue)
        
        Args:
            queue_name: Name of the queue
            timeout: Timeout in seconds (0 = non-blocking)
            
        Returns:
            Data from queue or None
        """
        if not self.client:
            raise RuntimeError("Redis client not connected")
        
        try:
            if timeout > 0:
                result = await self.client.blpop(queue_name, timeout=timeout)
                if result:
                    _, data_json = result
                    return json.loads(data_json)
            else:
                data_json = await self.client.lpop(queue_name)
                if data_json:
                    return json.loads(data_json)
            
            return None
        
        except Exception as e:
            logger.error(f"Failed to pop from Redis: {e}")
            raise
    
    async def publish(self, channel: str, message: Dict[str, Any]):
        """
        Publish message to Redis channel
        
        Args:
            channel: Channel name
            message: Message to publish
        """
        if not self.client:
            raise RuntimeError("Redis client not connected")
        
        try:
            message_json = json.dumps(message)
            await self.client.publish(channel, message_json)
            logger.debug(f"Published to Redis channel {channel}")
        
        except Exception as e:
            logger.error(f"Failed to publish to Redis: {e}")
            raise
    
    async def subscribe(self, *channels: str):
        """
        Subscribe to Redis channels
        
        Args:
            channels: Channel names to subscribe to
        """
        if not self.pubsub:
            raise RuntimeError("Redis pub/sub not initialized")
        
        try:
            await self.pubsub.subscribe(*channels)
            logger.info(f"Subscribed to Redis channels: {channels}")
        
        except Exception as e:
            logger.error(f"Failed to subscribe to Redis: {e}")
            raise
    
    async def get_message(self, timeout: Optional[float] = None) -> Optional[Dict[str, Any]]:
        """
        Get message from subscribed channels
        
        Args:
            timeout: Timeout in seconds
            
        Returns:
            Message or None
        """
        if not self.pubsub:
            raise RuntimeError("Redis pub/sub not initialized")
        
        try:
            message = await self.pubsub.get_message(timeout=timeout)
            
            if message and message["type"] == "message":
                data = json.loads(message["data"])
                return {
                    "channel": message["channel"],
                    "data": data
                }
            
            return None
        
        except Exception as e:
            logger.error(f"Failed to get message from Redis: {e}")
            raise
    
    async def get_queue_length(self, queue_name: str) -> int:
        """
        Get length of Redis queue
        
        Args:
            queue_name: Name of the queue
            
        Returns:
            Queue length
        """
        if not self.client:
            raise RuntimeError("Redis client not connected")
        
        try:
            return await self.client.llen(queue_name)
        
        except Exception as e:
            logger.error(f"Failed to get queue length: {e}")
            raise
    
    async def clear_queue(self, queue_name: str):
        """
        Clear all data from a queue
        
        Args:
            queue_name: Name of the queue
        """
        if not self.client:
            raise RuntimeError("Redis client not connected")
        
        try:
            await self.client.delete(queue_name)
            logger.info(f"Cleared Redis queue {queue_name}")
        
        except Exception as e:
            logger.error(f"Failed to clear queue: {e}")
            raise

