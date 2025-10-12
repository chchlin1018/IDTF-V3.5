"""
Basic Sparkplug Subscriber Example

This example demonstrates how to use the NDH Sparkplug connector to subscribe
to Sparkplug MQTT topics and receive device data.

Prerequisites:
- MQTT Broker (Mosquitto or HiveMQ) running on localhost:1883
- Sparkplug devices publishing data

Author: Chih-Cheng Lin (Michael Lin)
Date: 2025-10-10
"""

import asyncio
import sys
from pathlib import Path

# Add parent directory to path
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

from ndh.connectors.sparkplug_connector import SparkplugConnector
from loguru import logger


async def on_event(event):
    """
    Event callback function
    
    This function is called whenever a Sparkplug event is received.
    """
    event_type = event['event_type']
    asset_id = event['data'].get('asset_id', 'unknown')
    
    logger.info(f"📨 Event: {event_type}")
    logger.info(f"   Asset: {asset_id}")
    logger.info(f"   Time: {event['timestamp']}")
    
    if event_type == "asset.created":
        logger.info(f"   ✅ New device online!")
        logger.info(f"   Metrics: {list(event['data'].get('metrics', {}).keys())}")
    elif event_type == "asset.data_changed":
        logger.info(f"   📊 Data updated")
        logger.info(f"   Metrics: {event['data'].get('metrics', {})}")
    elif event_type == "asset.status_changed":
        status = event['data'].get('status', 'unknown')
        logger.info(f"   🔴 Status: {status}")
    
    print()  # Empty line for readability


async def main():
    """Main function"""
    
    logger.info("=" * 60)
    logger.info("NDH Sparkplug Subscriber Example")
    logger.info("=" * 60)
    print()
    
    # Configuration
    config = {
        "broker_host": "localhost",
        "broker_port": 1883,
        "group_id": "#",  # Subscribe to all groups
        "qos": 1,
    }
    
    logger.info("Configuration:")
    logger.info(f"  Broker: {config['broker_host']}:{config['broker_port']}")
    logger.info(f"  Group ID: {config['group_id']}")
    logger.info(f"  QoS: {config['qos']}")
    print()
    
    # Create connector
    connector = SparkplugConnector(
        connector_id="sparkplug_subscriber_001",
        config=config
    )
    
    # Add event callback
    connector.add_callback(on_event)
    
    try:
        # Connect
        logger.info("Connecting to MQTT Broker...")
        success = await connector.connect()
        
        if not success:
            logger.error("Failed to connect to MQTT Broker")
            return
        
        logger.success("Connected successfully!")
        print()
        
        logger.info("Waiting for Sparkplug messages...")
        logger.info("Press Ctrl+C to stop")
        logger.info("-" * 60)
        print()
        
        # Keep running and print statistics every 10 seconds
        while True:
            await asyncio.sleep(10)
            
            # Print statistics
            stats = connector.get_statistics()
            logger.info("📊 Statistics:")
            logger.info(f"   Messages: {stats['message_count']}")
            logger.info(f"   Births: {stats['birth_count']}")
            logger.info(f"   Deaths: {stats['death_count']}")
            logger.info(f"   Data: {stats['data_count']}")
            logger.info(f"   Edge Nodes: {stats['edge_node_count']} ({stats['online_edge_nodes']} online)")
            logger.info(f"   Devices: {stats['device_count']} ({stats['online_devices']} online)")
            print()
    
    except KeyboardInterrupt:
        logger.info("\nStopping...")
    
    except Exception as e:
        logger.error(f"Error: {str(e)}")
    
    finally:
        # Disconnect
        logger.info("Disconnecting...")
        await connector.disconnect()
        logger.success("Disconnected")


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

