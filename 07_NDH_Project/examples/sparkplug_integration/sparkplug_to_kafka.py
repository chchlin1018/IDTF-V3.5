"""
Sparkplug to Kafka Integration Example

This example demonstrates how to integrate Sparkplug MQTT with NDH's Kafka
event service, creating a complete data flow from edge devices to enterprise systems.

Architecture:
    Sparkplug Devices → MQTT Broker → Sparkplug Connector → Kafka → Enterprise Systems

Prerequisites:
- MQTT Broker running on localhost:1883
- Kafka running on localhost:9092
- Sparkplug devices publishing data

Author: Chih-Cheng Lin (Michael Lin)
Date: 2025-10-10
"""

import asyncio
import json
import sys
from pathlib import Path
from typing import Dict, Any

# Add parent directory to path
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

from ndh.connectors.sparkplug_connector import SparkplugConnector
from loguru import logger

try:
    from aiokafka import AIOKafkaProducer
    KAFKA_AVAILABLE = True
except ImportError:
    KAFKA_AVAILABLE = False
    logger.warning("aiokafka not installed, using mock Kafka producer")


class MockKafkaProducer:
    """Mock Kafka producer for testing without Kafka"""
    
    async def start(self):
        logger.info("Mock Kafka producer started")
    
    async def stop(self):
        logger.info("Mock Kafka producer stopped")
    
    async def send_and_wait(self, topic: str, value: bytes):
        logger.debug(f"Mock send to {topic}: {value[:100]}...")


class SparkplugToKafkaBridge:
    """
    Bridge between Sparkplug MQTT and Kafka
    
    This class receives Sparkplug events and publishes them to Kafka topics.
    """
    
    def __init__(
        self,
        sparkplug_config: Dict[str, Any],
        kafka_bootstrap_servers: str = "localhost:9092"
    ):
        self.sparkplug_config = sparkplug_config
        self.kafka_bootstrap_servers = kafka_bootstrap_servers
        
        self.sparkplug_connector = None
        self.kafka_producer = None
        
        # Statistics
        self.events_published = 0
        self.events_failed = 0
    
    async def start(self):
        """Start the bridge"""
        logger.info("Starting Sparkplug to Kafka bridge...")
        
        # Create Sparkplug connector
        self.sparkplug_connector = SparkplugConnector(
            connector_id="sparkplug_kafka_bridge",
            config=self.sparkplug_config
        )
        
        # Add event callback
        self.sparkplug_connector.add_callback(self.on_sparkplug_event)
        
        # Connect to MQTT
        logger.info("Connecting to MQTT Broker...")
        success = await self.sparkplug_connector.connect()
        if not success:
            raise RuntimeError("Failed to connect to MQTT Broker")
        
        # Create Kafka producer
        logger.info("Creating Kafka producer...")
        if KAFKA_AVAILABLE:
            self.kafka_producer = AIOKafkaProducer(
                bootstrap_servers=self.kafka_bootstrap_servers,
                value_serializer=lambda v: json.dumps(v).encode('utf-8')
            )
            await self.kafka_producer.start()
        else:
            self.kafka_producer = MockKafkaProducer()
            await self.kafka_producer.start()
        
        logger.success("Bridge started successfully!")
    
    async def stop(self):
        """Stop the bridge"""
        logger.info("Stopping bridge...")
        
        if self.sparkplug_connector:
            await self.sparkplug_connector.disconnect()
        
        if self.kafka_producer:
            await self.kafka_producer.stop()
        
        logger.success("Bridge stopped")
    
    async def on_sparkplug_event(self, event: Dict[str, Any]):
        """
        Handle Sparkplug event
        
        This method is called when a Sparkplug event is received.
        It publishes the event to the appropriate Kafka topic.
        """
        try:
            event_type = event['event_type']
            
            # Determine Kafka topic based on event type
            topic_mapping = {
                "asset.created": "ndh.assets.created",
                "asset.data_changed": "ndh.assets.data_changed",
                "asset.status_changed": "ndh.assets.status_changed",
            }
            
            kafka_topic = topic_mapping.get(event_type, "ndh.assets.unknown")
            
            # Publish to Kafka
            await self.kafka_producer.send_and_wait(
                kafka_topic,
                value=event
            )
            
            self.events_published += 1
            
            logger.debug(f"Published event to Kafka: {event_type} → {kafka_topic}")
            
        except Exception as e:
            self.events_failed += 1
            logger.error(f"Failed to publish event to Kafka: {str(e)}")
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get bridge statistics"""
        sparkplug_stats = self.sparkplug_connector.get_statistics() if self.sparkplug_connector else {}
        
        return {
            **sparkplug_stats,
            "events_published": self.events_published,
            "events_failed": self.events_failed,
        }


async def main():
    """Main function"""
    
    logger.info("=" * 60)
    logger.info("Sparkplug to Kafka Bridge Example")
    logger.info("=" * 60)
    print()
    
    # Configuration
    sparkplug_config = {
        "broker_host": "localhost",
        "broker_port": 1883,
        "group_id": "#",  # Subscribe to all groups
        "qos": 1,
    }
    
    kafka_bootstrap_servers = "localhost:9092"
    
    logger.info("Configuration:")
    logger.info(f"  MQTT Broker: {sparkplug_config['broker_host']}:{sparkplug_config['broker_port']}")
    logger.info(f"  Kafka: {kafka_bootstrap_servers}")
    logger.info(f"  Group ID: {sparkplug_config['group_id']}")
    print()
    
    # Create bridge
    bridge = SparkplugToKafkaBridge(
        sparkplug_config=sparkplug_config,
        kafka_bootstrap_servers=kafka_bootstrap_servers
    )
    
    try:
        # Start bridge
        await bridge.start()
        print()
        
        logger.info("Bridge is running...")
        logger.info("Sparkplug events will be published to Kafka topics:")
        logger.info("  - asset.created → ndh.assets.created")
        logger.info("  - asset.data_changed → ndh.assets.data_changed")
        logger.info("  - asset.status_changed → ndh.assets.status_changed")
        logger.info("")
        logger.info("Press Ctrl+C to stop")
        logger.info("-" * 60)
        print()
        
        # Keep running and print statistics every 30 seconds
        while True:
            await asyncio.sleep(30)
            
            # Print statistics
            stats = bridge.get_statistics()
            logger.info("📊 Statistics:")
            logger.info(f"   Sparkplug Messages: {stats['message_count']}")
            logger.info(f"   Births: {stats['birth_count']}")
            logger.info(f"   Deaths: {stats['death_count']}")
            logger.info(f"   Data: {stats['data_count']}")
            logger.info(f"   Edge Nodes: {stats['edge_node_count']} ({stats['online_edge_nodes']} online)")
            logger.info(f"   Devices: {stats['device_count']} ({stats['online_devices']} online)")
            logger.info(f"   Kafka Events Published: {stats['events_published']}")
            logger.info(f"   Kafka Events Failed: {stats['events_failed']}")
            print()
    
    except KeyboardInterrupt:
        logger.info("\nStopping...")
    
    except Exception as e:
        logger.error(f"Error: {str(e)}")
        import traceback
        traceback.print_exc()
    
    finally:
        # Stop bridge
        await bridge.stop()


if __name__ == "__main__":
    # Configure logger
    logger.remove()
    logger.add(
        sys.stdout,
        format="<green>{time:HH:mm:ss}</green> | <level>{level: <8}</level> | <level>{message}</level>",
        level="INFO"
    )
    
    # Run
    asyncio.run(main())

