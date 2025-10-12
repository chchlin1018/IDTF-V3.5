"""
OPC UA Connector - Basic Subscription Example

This example demonstrates:
1. Connecting to an OPC UA server
2. Subscribing to nodes for data change notifications
3. Reading and writing node values
4. Browsing the node tree
"""

import asyncio
from ndh.connectors.opcua_connector import OPCUAConnector


async def on_data_change(node_id: str, value):
    """Callback function called when subscribed data changes"""
    print(f"Data change notification: {node_id} = {value}")


async def main():
    # Configuration
    config = {
        "endpoint_url": "opc.tcp://localhost:4840",  # Replace with your OPC UA server
        # Optional authentication
        # "username": "user",
        # "password": "password"
    }
    
    # Create connector
    connector = OPCUAConnector(
        connector_id="opcua_001",
        config=config
    )
    
    try:
        # 1. Connect to server
        print("Connecting to OPC UA server...")
        success = await connector.connect()
        if not success:
            print("Failed to connect")
            return
        
        print("Connected successfully!")
        print(f"Status: {connector.get_status()}")
        
        # 2. Browse nodes
        print("\nBrowsing nodes...")
        nodes = await connector.browse_nodes()
        print(f"Found {len(nodes)} nodes:")
        for node in nodes[:5]:  # Show first 5 nodes
            print(f"  - {node['display_name']} ({node['node_id']})")
        
        # 3. Subscribe to nodes
        print("\nSubscribing to nodes...")
        node_ids = [
            "ns=2;s=Temperature",
            "ns=2;s=Pressure",
            "ns=2;s=FlowRate"
        ]
        
        await connector.subscribe_nodes(
            node_ids=node_ids,
            callback=on_data_change,
            sampling_interval=1000.0  # 1 second
        )
        
        print(f"Subscribed to {len(node_ids)} nodes")
        
        # 4. Wait for data changes
        print("\nWaiting for data changes (30 seconds)...")
        await asyncio.sleep(30)
        
        # 5. Read current values
        print("\nReading current values...")
        values = await connector.read()
        for node_id, data in values.items():
            print(f"  {node_id}: {data['value']} at {data['timestamp']}")
        
        # 6. Write a value (optional)
        print("\nWriting value...")
        await connector.write({
            "ns=2;s=Setpoint": 25.5
        })
        print("Value written successfully")
        
        # 7. Unsubscribe
        print("\nUnsubscribing...")
        await connector.unsubscribe_all()
        
    except Exception as e:
        print(f"Error: {str(e)}")
    
    finally:
        # Disconnect
        print("\nDisconnecting...")
        await connector.disconnect()
        print("Disconnected")


if __name__ == "__main__":
    asyncio.run(main())

