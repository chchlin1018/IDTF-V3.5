"""
OPC UA + NDH Integration Example

This example demonstrates integrating OPC UA with NDH Broker:
1. Connect OPC UA to NDH
2. Subscribe to OPC UA nodes
3. Publish data changes as NDH events
4. Map OPC UA nodes to NDH assets
"""

import asyncio
from typing import Dict, Any
from datetime import datetime

from ndh.connectors.opcua_connector import OPCUAConnector


class OPCUAToNDHBridge:
    """Bridge between OPC UA and NDH"""
    
    def __init__(self, opcua_connector: OPCUAConnector, ndh_broker=None):
        self.opcua = opcua_connector
        self.ndh_broker = ndh_broker
        self.node_to_asset_map: Dict[str, str] = {}
    
    def map_node_to_asset(self, node_id: str, asset_id: str):
        """Map OPC UA node to NDH asset"""
        self.node_to_asset_map[node_id] = asset_id
        print(f"Mapped {node_id} -> {asset_id}")
    
    async def on_opcua_data_change(self, node_id: str, value: Any):
        """Handle OPC UA data change and publish to NDH"""
        # Get mapped asset ID
        asset_id = self.node_to_asset_map.get(node_id)
        if not asset_id:
            print(f"Warning: No asset mapping for node {node_id}")
            return
        
        # Create NDH event
        event = {
            "event_type": "asset.data_changed",
            "timestamp": datetime.now().isoformat(),
            "source": f"opcua://{node_id}",
            "data": {
                "asset_id": asset_id,
                "node_id": node_id,
                "value": value,
                "quality": "Good"
            }
        }
        
        # Publish to NDH (if broker available)
        if self.ndh_broker:
            await self.ndh_broker.publish_event(event)
        
        print(f"Published event: {asset_id} = {value}")
    
    async def start(self, node_mappings: Dict[str, str]):
        """
        Start the bridge
        
        Args:
            node_mappings: Dictionary mapping node IDs to asset IDs
        """
        # Set up mappings
        for node_id, asset_id in node_mappings.items():
            self.map_node_to_asset(node_id, asset_id)
        
        # Subscribe to all mapped nodes
        node_ids = list(node_mappings.keys())
        await self.opcua.subscribe_nodes(
            node_ids=node_ids,
            callback=self.on_opcua_data_change,
            sampling_interval=1000.0
        )
        
        print(f"Bridge started with {len(node_ids)} node mappings")


async def main():
    # OPC UA configuration
    opcua_config = {
        "endpoint_url": "opc.tcp://localhost:4840"
    }
    
    # Create OPC UA connector
    opcua_connector = OPCUAConnector(
        connector_id="opcua_main",
        config=opcua_config
    )
    
    # Create bridge (without actual NDH broker for this example)
    bridge = OPCUAToNDHBridge(
        opcua_connector=opcua_connector,
        ndh_broker=None  # Replace with actual NDH broker
    )
    
    try:
        # Connect to OPC UA server
        print("Connecting to OPC UA server...")
        await opcua_connector.connect()
        print("Connected!")
        
        # Define node-to-asset mappings
        node_mappings = {
            "ns=2;s=Temperature": "asset_temp_sensor_001",
            "ns=2;s=Pressure": "asset_pressure_sensor_001",
            "ns=2;s=FlowRate": "asset_flow_meter_001",
            "ns=2;s=Level": "asset_level_sensor_001"
        }
        
        # Start bridge
        print("\nStarting OPC UA to NDH bridge...")
        await bridge.start(node_mappings)
        
        # Run for 60 seconds
        print("\nBridge running (60 seconds)...")
        print("Data changes will be published as NDH events")
        await asyncio.sleep(60)
        
    except Exception as e:
        print(f"Error: {str(e)}")
    
    finally:
        # Clean up
        print("\nShutting down...")
        await opcua_connector.disconnect()
        print("Bridge stopped")


if __name__ == "__main__":
    asyncio.run(main())

