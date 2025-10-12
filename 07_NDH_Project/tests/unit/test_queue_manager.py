"""
Unit Tests for Queue Manager
"""

import pytest
import asyncio
from datetime import datetime

from ndh.queue.manager import QueueManager


@pytest.mark.asyncio
async def test_queue_manager_initialization():
    """Test queue manager initialization"""
    manager = QueueManager()
    await manager.initialize()
    
    assert manager.running is True
    assert "data_ingestion" in manager.queues
    assert "data_processing" in manager.queues
    assert "data_output" in manager.queues
    
    await manager.cleanup()


@pytest.mark.asyncio
async def test_enqueue_dequeue():
    """Test enqueue and dequeue operations"""
    manager = QueueManager()
    await manager.initialize()
    
    # Enqueue data
    test_data = {
        "asset_id": "TEST_001",
        "timestamp": datetime.now().isoformat(),
        "values": {"temperature": 25.0}
    }
    
    await manager.enqueue("test_queue", test_data)
    
    # Dequeue data
    result = await manager.dequeue("test_queue", timeout=1.0)
    
    assert result is not None
    assert result["asset_id"] == "TEST_001"
    assert "_enqueued_at" in result
    assert "_queue" in result
    
    await manager.cleanup()


@pytest.mark.asyncio
async def test_queue_size():
    """Test queue size tracking"""
    manager = QueueManager()
    await manager.initialize()
    
    # Enqueue multiple items
    for i in range(5):
        await manager.enqueue("test_queue", {"id": i})
    
    # Check queue size
    size = manager.get_queue_size("test_queue")
    assert size == 5
    
    # Dequeue one item
    await manager.dequeue("test_queue")
    
    # Check size again
    size = manager.get_queue_size("test_queue")
    assert size == 4
    
    await manager.cleanup()


@pytest.mark.asyncio
async def test_subscriber():
    """Test queue subscriber functionality"""
    manager = QueueManager()
    await manager.initialize()
    
    received_data = []
    
    async def callback(data):
        received_data.append(data)
    
    # Subscribe to queue
    await manager.subscribe("test_queue", callback)
    
    # Enqueue data
    test_data = {"asset_id": "TEST_002", "value": 42}
    await manager.enqueue("test_queue", test_data)
    
    # Wait a bit for callback
    await asyncio.sleep(0.1)
    
    # Check if callback was called
    assert len(received_data) == 1
    assert received_data[0]["asset_id"] == "TEST_002"
    
    await manager.cleanup()


@pytest.mark.asyncio
async def test_dequeue_timeout():
    """Test dequeue timeout"""
    manager = QueueManager()
    await manager.initialize()
    
    # Try to dequeue from empty queue with timeout
    result = await manager.dequeue("empty_queue", timeout=0.5)
    
    assert result is None
    
    await manager.cleanup()


@pytest.mark.asyncio
async def test_get_all_queue_sizes():
    """Test getting all queue sizes"""
    manager = QueueManager()
    await manager.initialize()
    
    # Enqueue to different queues
    await manager.enqueue("data_ingestion", {"id": 1})
    await manager.enqueue("data_ingestion", {"id": 2})
    await manager.enqueue("data_processing", {"id": 3})
    
    # Get all sizes
    sizes = manager.get_all_queue_sizes()
    
    assert sizes["data_ingestion"] == 2
    assert sizes["data_processing"] == 1
    assert sizes["data_output"] == 0
    
    await manager.cleanup()

