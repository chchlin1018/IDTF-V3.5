"""
NDH Queue Manager
Manages message queues and routing
"""

import asyncio
from typing import Dict, Any, Optional, Callable
from collections import defaultdict
from datetime import datetime

from loguru import logger

from ndh.core.config import get_settings


class QueueManager:
    """
    Queue Manager for NDH
    Handles message queuing and routing
    """
    
    def __init__(self):
        self.settings = get_settings()
        self.queues: Dict[str, asyncio.Queue] = {}
        self.subscribers: Dict[str, list] = defaultdict(list)
        self.running = False
        self.tasks = []
    
    async def initialize(self):
        """Initialize the queue manager"""
        logger.info("Initializing Queue Manager...")
        
        # Create default queues
        await self.create_queue("data_ingestion", maxsize=self.settings.queue_max_size)
        await self.create_queue("data_processing", maxsize=self.settings.queue_max_size)
        await self.create_queue("data_output", maxsize=self.settings.queue_max_size)
        
        self.running = True
        logger.info("Queue Manager initialized")
    
    async def create_queue(self, name: str, maxsize: int = 0):
        """
        Create a new queue
        
        Args:
            name: Queue name
            maxsize: Maximum queue size (0 = unlimited)
        """
        if name not in self.queues:
            self.queues[name] = asyncio.Queue(maxsize=maxsize)
            logger.info(f"Created queue: {name} (maxsize={maxsize})")
    
    async def enqueue(self, queue_name: str, data: Dict[str, Any]):
        """
        Add data to a queue
        
        Args:
            queue_name: Name of the queue
            data: Data to enqueue
        """
        if queue_name not in self.queues:
            await self.create_queue(queue_name)
        
        queue = self.queues[queue_name]
        
        # Add metadata
        data["_enqueued_at"] = datetime.now().isoformat()
        data["_queue"] = queue_name
        
        await queue.put(data)
        logger.debug(f"Enqueued data to {queue_name}: {data.get('asset_id', 'unknown')}")
        
        # Notify subscribers
        await self._notify_subscribers(queue_name, data)
    
    async def dequeue(self, queue_name: str, timeout: Optional[float] = None) -> Optional[Dict[str, Any]]:
        """
        Get data from a queue
        
        Args:
            queue_name: Name of the queue
            timeout: Timeout in seconds
            
        Returns:
            Data from queue or None if timeout
        """
        if queue_name not in self.queues:
            logger.warning(f"Queue not found: {queue_name}")
            return None
        
        queue = self.queues[queue_name]
        
        try:
            if timeout:
                data = await asyncio.wait_for(queue.get(), timeout=timeout)
            else:
                data = await queue.get()
            
            logger.debug(f"Dequeued data from {queue_name}")
            return data
        
        except asyncio.TimeoutError:
            logger.debug(f"Dequeue timeout for {queue_name}")
            return None
    
    async def subscribe(self, queue_name: str, callback: Callable):
        """
        Subscribe to queue events
        
        Args:
            queue_name: Name of the queue
            callback: Callback function to call when data is enqueued
        """
        self.subscribers[queue_name].append(callback)
        logger.info(f"Added subscriber to {queue_name}")
    
    async def _notify_subscribers(self, queue_name: str, data: Dict[str, Any]):
        """
        Notify all subscribers of a queue
        
        Args:
            queue_name: Name of the queue
            data: Data that was enqueued
        """
        if queue_name in self.subscribers:
            for callback in self.subscribers[queue_name]:
                try:
                    if asyncio.iscoroutinefunction(callback):
                        await callback(data)
                    else:
                        callback(data)
                except Exception as e:
                    logger.error(f"Subscriber callback error: {e}")
    
    def get_queue_size(self, queue_name: str) -> int:
        """
        Get current size of a queue
        
        Args:
            queue_name: Name of the queue
            
        Returns:
            Queue size
        """
        if queue_name in self.queues:
            return self.queues[queue_name].qsize()
        return 0
    
    def get_all_queue_sizes(self) -> Dict[str, int]:
        """
        Get sizes of all queues
        
        Returns:
            Dictionary of queue names and sizes
        """
        return {name: queue.qsize() for name, queue in self.queues.items()}
    
    async def cleanup(self):
        """Cleanup queue manager resources"""
        logger.info("Cleaning up Queue Manager...")
        
        self.running = False
        
        # Cancel all tasks
        for task in self.tasks:
            task.cancel()
        
        # Clear all queues
        for name, queue in self.queues.items():
            while not queue.empty():
                try:
                    queue.get_nowait()
                except asyncio.QueueEmpty:
                    break
        
        logger.info("Queue Manager cleaned up")

