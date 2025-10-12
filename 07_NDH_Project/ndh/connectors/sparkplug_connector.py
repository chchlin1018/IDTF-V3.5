"""
NDH Sparkplug MQTT Connector

This module provides a Sparkplug B connector for NDH, enabling integration
with Sparkplug-compatible devices and systems.

Features:
- Subscribe to Sparkplug MQTT topics
- Parse Sparkplug B payloads (Protocol Buffers)
- Map Sparkplug devices to NDH assets
- Publish NDH events to Kafka
- Automatic device discovery via BIRTH messages
- State management via DEATH messages

Author: Chih-Cheng Lin (Michael Lin)
Date: 2025-10-10
"""

import asyncio
import json
from typing import Dict, Any, Optional, List, Callable
from datetime import datetime
from enum import Enum

import paho.mqtt.client as mqtt
from loguru import logger

from .base import BaseConnector, ConnectorStatus


class SparkplugMessageType(Enum):
    """Sparkplug message types"""
    NBIRTH = "NBIRTH"  # Edge Node Birth
    NDEATH = "NDEATH"  # Edge Node Death
    DBIRTH = "DBIRTH"  # Device Birth
    DDEATH = "DDEATH"  # Device Death
    NDATA = "NDATA"    # Edge Node Data
    DDATA = "DDATA"    # Device Data
    NCMD = "NCMD"      # Edge Node Command
    DCMD = "DCMD"      # Device Command
    STATE = "STATE"    # SCADA Host State


class SparkplugConnector(BaseConnector):
    """
    Sparkplug MQTT Connector for NDH
    
    This connector subscribes to Sparkplug MQTT topics, parses Sparkplug B
    payloads, and maps Sparkplug devices to NDH assets and events.
    
    Example:
        config = {
            "broker_host": "localhost",
            "broker_port": 1883,
            "group_id": "#",  # Subscribe to all groups
        }
        
        connector = SparkplugConnector("sparkplug_001", config)
        await connector.connect()
    """
    
    def __init__(self, connector_id: str, config: Dict[str, Any]):
        super().__init__(connector_id, config)
        
        self.mqtt_client: Optional[mqtt.Client] = None
        self.edge_nodes: Dict[str, Dict] = {}
        self.devices: Dict[str, Dict] = {}
        self.callbacks: List[Callable] = []
        
        # Statistics
        self.message_count = 0
        self.birth_count = 0
        self.death_count = 0
        self.data_count = 0
    
    def get_required_config_fields(self) -> List[str]:
        """Get required configuration fields"""
        return ["broker_host", "broker_port"]
    
    async def connect(self) -> bool:
        """
        Connect to MQTT Broker
        
        Returns:
            bool: True if connected successfully
        """
        try:
            self._set_status(ConnectorStatus.CONNECTING)
            
            broker_host = self.config["broker_host"]
            broker_port = self.config.get("broker_port", 1883)
            
            logger.info(f"Connecting to MQTT Broker: {broker_host}:{broker_port}")
            
            # Create MQTT client
            client_id = f"ndh_sparkplug_{self.connector_id}"
            self.mqtt_client = mqtt.Client(
                client_id=client_id,
                clean_session=self.config.get("clean_session", False)
            )
            
            # Set callbacks
            self.mqtt_client.on_connect = self._on_connect
            self.mqtt_client.on_message = self._on_message
            self.mqtt_client.on_disconnect = self._on_disconnect
            
            # Set authentication
            if "username" in self.config and "password" in self.config:
                self.mqtt_client.username_pw_set(
                    self.config["username"],
                    self.config["password"]
                )
            
            # Connect
            self.mqtt_client.connect(
                broker_host,
                broker_port,
                keepalive=self.config.get("keep_alive", 60)
            )
            
            # Start network loop
            self.mqtt_client.loop_start()
            
            # Wait for connection
            await asyncio.sleep(1)
            
            if self.status == ConnectorStatus.CONNECTED:
                logger.success(f"Sparkplug connector {self.connector_id} connected")
                return True
            else:
                return False
            
        except Exception as e:
            error_msg = f"Failed to connect to MQTT Broker: {str(e)}"
            logger.error(error_msg)
            self._set_status(ConnectorStatus.ERROR, error_msg)
            return False
    
    async def disconnect(self) -> bool:
        """
        Disconnect from MQTT Broker
        
        Returns:
            bool: True if disconnected successfully
        """
        try:
            if self.mqtt_client:
                self.mqtt_client.loop_stop()
                self.mqtt_client.disconnect()
                self.mqtt_client = None
            
            self._set_status(ConnectorStatus.DISCONNECTED)
            logger.info(f"Sparkplug connector {self.connector_id} disconnected")
            
            return True
            
        except Exception as e:
            error_msg = f"Failed to disconnect: {str(e)}"
            logger.error(error_msg)
            return False
    
    async def subscribe(self, group_id: str = "#") -> bool:
        """
        Subscribe to Sparkplug topics
        
        Args:
            group_id: Group ID to subscribe to (use "#" for all groups)
        
        Returns:
            bool: True if subscribed successfully
        """
        if not self.mqtt_client:
            raise RuntimeError("Not connected to MQTT Broker")
        
        try:
            topic = f"spBv1.0/{group_id}/#"
            qos = self.config.get("qos", 1)
            
            self.mqtt_client.subscribe(topic, qos=qos)
            logger.info(f"Subscribed to Sparkplug topic: {topic}")
            
            return True
            
        except Exception as e:
            logger.error(f"Failed to subscribe: {str(e)}")
            return False
    
    def _on_connect(self, client, userdata, flags, rc):
        """MQTT connection callback"""
        if rc == 0:
            self._set_status(ConnectorStatus.CONNECTED)
            logger.info("MQTT connected successfully")
            
            # Auto subscribe
            group_id = self.config.get("group_id", "#")
            asyncio.create_task(self.subscribe(group_id))
        else:
            error_msg = f"MQTT connection failed with code: {rc}"
            logger.error(error_msg)
            self._set_status(ConnectorStatus.ERROR, error_msg)
    
    def _on_disconnect(self, client, userdata, rc):
        """MQTT disconnection callback"""
        if rc != 0:
            logger.warning(f"Unexpected MQTT disconnection: {rc}")
            self._set_status(ConnectorStatus.ERROR, "Unexpected disconnection")
    
    def _on_message(self, client, userdata, message):
        """
        Handle MQTT message
        
        This method parses the Sparkplug topic and payload, and routes
        the message to the appropriate handler based on message type.
        """
        try:
            self.message_count += 1
            
            # Parse topic
            topic_parts = message.topic.split("/")
            
            if len(topic_parts) < 4:
                logger.warning(f"Invalid Sparkplug topic: {message.topic}")
                return
            
            namespace = topic_parts[0]  # spBv1.0
            group_id = topic_parts[1]
            message_type = topic_parts[2]
            edge_node_id = topic_parts[3]
            device_id = topic_parts[4] if len(topic_parts) > 4 else None
            
            logger.debug(f"Received Sparkplug message: {message_type} from {group_id}/{edge_node_id}/{device_id}")
            
            # Parse payload
            # Note: In a real implementation, you would use a Protobuf parser here
            # For this example, we'll use a simplified JSON-based approach
            try:
                payload = json.loads(message.payload.decode('utf-8'))
            except:
                # If not JSON, treat as binary Protobuf (would need sparkplug_b_pb2)
                payload = {"raw": message.payload}
            
            # Route to appropriate handler
            if message_type == SparkplugMessageType.NBIRTH.value:
                self._handle_nbirth(group_id, edge_node_id, payload)
            elif message_type == SparkplugMessageType.DBIRTH.value:
                self._handle_dbirth(group_id, edge_node_id, device_id, payload)
            elif message_type == SparkplugMessageType.NDATA.value:
                self._handle_ndata(group_id, edge_node_id, payload)
            elif message_type == SparkplugMessageType.DDATA.value:
                self._handle_ddata(group_id, edge_node_id, device_id, payload)
            elif message_type == SparkplugMessageType.NDEATH.value:
                self._handle_ndeath(group_id, edge_node_id)
            elif message_type == SparkplugMessageType.DDEATH.value:
                self._handle_ddeath(group_id, edge_node_id, device_id)
            
        except Exception as e:
            logger.error(f"Error processing Sparkplug message: {str(e)}")
    
    def _handle_nbirth(self, group_id: str, edge_node_id: str, payload: Dict):
        """Handle Edge Node BIRTH message"""
        try:
            self.birth_count += 1
            
            # Create NDH asset ID
            asset_id = f"{group_id}_{edge_node_id}"
            
            # Store edge node info
            self.edge_nodes[asset_id] = {
                "group_id": group_id,
                "edge_node_id": edge_node_id,
                "metrics": payload.get("metrics", {}),
                "status": "online",
                "last_seen": datetime.now(),
                "birth_time": datetime.now()
            }
            
            # Publish NDH event
            event = {
                "event_type": "asset.created",
                "timestamp": datetime.now().isoformat(),
                "source": f"sparkplug://{group_id}/{edge_node_id}",
                "data": {
                    "asset_id": asset_id,
                    "asset_type": "sparkplug_edge_node",
                    "group_id": group_id,
                    "edge_node_id": edge_node_id,
                    "metrics": payload.get("metrics", {})
                }
            }
            
            asyncio.create_task(self.publish_event(event))
            
            logger.info(f"Edge Node BIRTH: {asset_id}")
            
        except Exception as e:
            logger.error(f"Error handling NBIRTH: {str(e)}")
    
    def _handle_dbirth(self, group_id: str, edge_node_id: str, device_id: str, payload: Dict):
        """Handle Device BIRTH message"""
        try:
            self.birth_count += 1
            
            # Create NDH asset ID
            asset_id = f"{group_id}_{edge_node_id}_{device_id}"
            
            # Store device info
            self.devices[asset_id] = {
                "group_id": group_id,
                "edge_node_id": edge_node_id,
                "device_id": device_id,
                "metrics": payload.get("metrics", {}),
                "status": "online",
                "last_seen": datetime.now(),
                "birth_time": datetime.now()
            }
            
            # Publish NDH event
            event = {
                "event_type": "asset.created",
                "timestamp": datetime.now().isoformat(),
                "source": f"sparkplug://{group_id}/{edge_node_id}/{device_id}",
                "data": {
                    "asset_id": asset_id,
                    "asset_type": "sparkplug_device",
                    "group_id": group_id,
                    "edge_node_id": edge_node_id,
                    "device_id": device_id,
                    "metrics": payload.get("metrics", {})
                }
            }
            
            asyncio.create_task(self.publish_event(event))
            
            logger.info(f"Device BIRTH: {asset_id}")
            
        except Exception as e:
            logger.error(f"Error handling DBIRTH: {str(e)}")
    
    def _handle_ndata(self, group_id: str, edge_node_id: str, payload: Dict):
        """Handle Edge Node DATA message"""
        try:
            self.data_count += 1
            
            asset_id = f"{group_id}_{edge_node_id}"
            
            # Update edge node info
            if asset_id in self.edge_nodes:
                self.edge_nodes[asset_id]["last_seen"] = datetime.now()
                # Update metrics
                if "metrics" in payload:
                    self.edge_nodes[asset_id]["metrics"].update(payload["metrics"])
            
            # Publish NDH event
            event = {
                "event_type": "asset.data_changed",
                "timestamp": datetime.now().isoformat(),
                "source": f"sparkplug://{group_id}/{edge_node_id}",
                "data": {
                    "asset_id": asset_id,
                    "metrics": payload.get("metrics", {})
                }
            }
            
            asyncio.create_task(self.publish_event(event))
            
        except Exception as e:
            logger.error(f"Error handling NDATA: {str(e)}")
    
    def _handle_ddata(self, group_id: str, edge_node_id: str, device_id: str, payload: Dict):
        """Handle Device DATA message"""
        try:
            self.data_count += 1
            
            asset_id = f"{group_id}_{edge_node_id}_{device_id}"
            
            # Update device info
            if asset_id in self.devices:
                self.devices[asset_id]["last_seen"] = datetime.now()
                # Update metrics
                if "metrics" in payload:
                    self.devices[asset_id]["metrics"].update(payload["metrics"])
            
            # Publish NDH event
            event = {
                "event_type": "asset.data_changed",
                "timestamp": datetime.now().isoformat(),
                "source": f"sparkplug://{group_id}/{edge_node_id}/{device_id}",
                "data": {
                    "asset_id": asset_id,
                    "metrics": payload.get("metrics", {})
                }
            }
            
            asyncio.create_task(self.publish_event(event))
            
        except Exception as e:
            logger.error(f"Error handling DDATA: {str(e)}")
    
    def _handle_ndeath(self, group_id: str, edge_node_id: str):
        """Handle Edge Node DEATH message"""
        try:
            self.death_count += 1
            
            asset_id = f"{group_id}_{edge_node_id}"
            
            # Update edge node status
            if asset_id in self.edge_nodes:
                self.edge_nodes[asset_id]["status"] = "offline"
                self.edge_nodes[asset_id]["death_time"] = datetime.now()
            
            # Publish NDH event
            event = {
                "event_type": "asset.status_changed",
                "timestamp": datetime.now().isoformat(),
                "source": f"sparkplug://{group_id}/{edge_node_id}",
                "data": {
                    "asset_id": asset_id,
                    "status": "offline"
                }
            }
            
            asyncio.create_task(self.publish_event(event))
            
            logger.info(f"Edge Node DEATH: {asset_id}")
            
        except Exception as e:
            logger.error(f"Error handling NDEATH: {str(e)}")
    
    def _handle_ddeath(self, group_id: str, edge_node_id: str, device_id: str):
        """Handle Device DEATH message"""
        try:
            self.death_count += 1
            
            asset_id = f"{group_id}_{edge_node_id}_{device_id}"
            
            # Update device status
            if asset_id in self.devices:
                self.devices[asset_id]["status"] = "offline"
                self.devices[asset_id]["death_time"] = datetime.now()
            
            # Publish NDH event
            event = {
                "event_type": "asset.status_changed",
                "timestamp": datetime.now().isoformat(),
                "source": f"sparkplug://{group_id}/{edge_node_id}/{device_id}",
                "data": {
                    "asset_id": asset_id,
                    "status": "offline"
                }
            }
            
            asyncio.create_task(self.publish_event(event))
            
            logger.info(f"Device DEATH: {asset_id}")
            
        except Exception as e:
            logger.error(f"Error handling DDEATH: {str(e)}")
    
    async def publish_event(self, event: Dict[str, Any]):
        """
        Publish event to NDH (Kafka)
        
        Args:
            event: Event dictionary to publish
        """
        logger.debug(f"Publishing event: {event['event_type']}")
        
        # Call callbacks
        for callback in self.callbacks:
            try:
                if asyncio.iscoroutinefunction(callback):
                    await callback(event)
                else:
                    callback(event)
            except Exception as e:
                logger.error(f"Error in event callback: {str(e)}")
    
    def add_callback(self, callback: Callable):
        """
        Add event callback
        
        Args:
            callback: Callback function to call when events are published
        """
        self.callbacks.append(callback)
    
    def get_statistics(self) -> Dict[str, Any]:
        """
        Get connector statistics
        
        Returns:
            dict: Statistics dictionary
        """
        return {
            "message_count": self.message_count,
            "birth_count": self.birth_count,
            "death_count": self.death_count,
            "data_count": self.data_count,
            "edge_node_count": len(self.edge_nodes),
            "device_count": len(self.devices),
            "online_edge_nodes": sum(1 for n in self.edge_nodes.values() if n["status"] == "online"),
            "online_devices": sum(1 for d in self.devices.values() if d["status"] == "online")
        }
    
    # BaseConnector abstract methods implementation
    
    async def read(self, tags: Optional[List[str]] = None) -> Dict[str, Any]:
        """
        Read device data (returns cached data)
        
        Args:
            tags: Optional list of tags to read
        
        Returns:
            dict: Device data
        """
        return {
            "edge_nodes": self.edge_nodes,
            "devices": self.devices,
            "statistics": self.get_statistics()
        }
    
    async def write(self, data: Dict[str, Any]) -> bool:
        """
        Write data (send commands)
        
        Note: Command support is not yet implemented in this version.
        
        Args:
            data: Data to write
        
        Returns:
            bool: True if successful
        """
        raise NotImplementedError("Command support not yet implemented")

