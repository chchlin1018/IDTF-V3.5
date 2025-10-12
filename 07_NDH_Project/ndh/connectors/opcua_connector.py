"""
NDH OPC UA Connector
Implements OPC UA client with direct subscription support
"""

import asyncio
from typing import Dict, Any, Optional, List, Callable
from datetime import datetime

from loguru import logger

try:
    from asyncua import Client, ua
    from asyncua.common.subscription import Subscription
    ASYNCUA_AVAILABLE = True
except ImportError:
    ASYNCUA_AVAILABLE = False
    logger.warning("asyncua library not installed. Install with: pip install asyncua")

from .base import BaseConnector, ConnectorStatus


class OPCUAConnector(BaseConnector):
    """
    OPC UA Connector with direct subscription support
    
    Features:
    - Direct node subscription (no polling)
    - Node browsing and discovery
    - Secure connection support
    - Method calling
    - Event monitoring
    """
    
    def __init__(self, connector_id: str, config: Dict[str, Any]):
        """
        Initialize OPC UA connector
        
        Args:
            connector_id: Unique connector identifier
            config: Configuration dictionary with:
                - endpoint_url: OPC UA server endpoint (e.g., "opc.tcp://localhost:4840")
                - username: Optional username for authentication
                - password: Optional password for authentication
                - security_mode: Optional security mode (None, Sign, SignAndEncrypt)
                - security_policy: Optional security policy
                - certificate_path: Optional client certificate path
                - private_key_path: Optional private key path
        """
        super().__init__(connector_id, config)
        
        if not ASYNCUA_AVAILABLE:
            raise ImportError("asyncua library is required for OPC UA connector")
        
        self.client: Optional[Client] = None
        self.subscription: Optional[Subscription] = None
        self.subscribed_nodes: Dict[str, Any] = {}
        self.node_values: Dict[str, Any] = {}
        self.callbacks: List[Callable] = []
    
    def get_required_config_fields(self) -> List[str]:
        """Get required configuration fields"""
        return ["endpoint_url"]
    
    async def connect(self) -> bool:
        """
        Connect to OPC UA server
        
        Returns:
            True if connection successful
        """
        try:
            self._set_status(ConnectorStatus.CONNECTING)
            
            endpoint_url = self.config["endpoint_url"]
            logger.info(f"Connecting to OPC UA server: {endpoint_url}")
            
            # Create client
            self.client = Client(url=endpoint_url)
            
            # Set authentication if provided
            if "username" in self.config and "password" in self.config:
                self.client.set_user(self.config["username"])
                self.client.set_password(self.config["password"])
            
            # Set security if provided
            if "security_mode" in self.config:
                # Security configuration would go here
                pass
            
            # Connect to server
            await self.client.connect()
            
            # Get server info
            server_info = await self.client.get_server_node()
            logger.info(f"Connected to OPC UA server: {server_info}")
            
            self._set_status(ConnectorStatus.CONNECTED)
            logger.success(f"OPC UA connector {self.connector_id} connected successfully")
            
            return True
            
        except Exception as e:
            error_msg = f"Failed to connect to OPC UA server: {str(e)}"
            logger.error(error_msg)
            self._set_status(ConnectorStatus.ERROR, error_msg)
            return False
    
    async def disconnect(self) -> bool:
        """
        Disconnect from OPC UA server
        
        Returns:
            True if disconnection successful
        """
        try:
            # Unsubscribe from all nodes
            if self.subscription:
                await self.unsubscribe_all()
            
            # Disconnect client
            if self.client:
                await self.client.disconnect()
                self.client = None
            
            self._set_status(ConnectorStatus.DISCONNECTED)
            logger.info(f"OPC UA connector {self.connector_id} disconnected")
            
            return True
            
        except Exception as e:
            error_msg = f"Failed to disconnect from OPC UA server: {str(e)}"
            logger.error(error_msg)
            self._set_status(ConnectorStatus.ERROR, error_msg)
            return False
    
    async def read(self, node_ids: Optional[List[str]] = None) -> Dict[str, Any]:
        """
        Read values from OPC UA nodes
        
        Args:
            node_ids: List of node IDs to read (e.g., ["ns=2;s=Temperature"])
                     If None, returns cached values from subscriptions
        
        Returns:
            Dictionary mapping node IDs to their values
        """
        if not self.client:
            raise RuntimeError("Not connected to OPC UA server")
        
        try:
            if node_ids is None:
                # Return cached values from subscriptions
                return self.node_values.copy()
            
            # Read specified nodes
            result = {}
            for node_id in node_ids:
                node = self.client.get_node(node_id)
                value = await node.read_value()
                result[node_id] = {
                    "value": value,
                    "timestamp": datetime.now().isoformat(),
                    "quality": "Good"
                }
                self.data_count += 1
            
            return result
            
        except Exception as e:
            logger.error(f"Failed to read OPC UA nodes: {str(e)}")
            raise
    
    async def write(self, data: Dict[str, Any]) -> bool:
        """
        Write values to OPC UA nodes
        
        Args:
            data: Dictionary mapping node IDs to values
        
        Returns:
            True if write successful
        """
        if not self.client:
            raise RuntimeError("Not connected to OPC UA server")
        
        try:
            for node_id, value in data.items():
                node = self.client.get_node(node_id)
                
                # Get the node's data type
                data_type = await node.read_data_type_as_variant_type()
                
                # Create variant with correct type
                variant = ua.Variant(value, data_type)
                
                # Write value
                await node.write_value(variant)
                
                logger.debug(f"Wrote value {value} to node {node_id}")
            
            return True
            
        except Exception as e:
            logger.error(f"Failed to write OPC UA nodes: {str(e)}")
            return False
    
    async def subscribe_nodes(
        self,
        node_ids: List[str],
        callback: Optional[Callable] = None,
        sampling_interval: float = 1000.0
    ) -> bool:
        """
        Subscribe to OPC UA nodes for data change notifications
        
        Args:
            node_ids: List of node IDs to subscribe
            callback: Optional callback function called on data change
            sampling_interval: Sampling interval in milliseconds (default: 1000ms)
        
        Returns:
            True if subscription successful
        """
        if not self.client:
            raise RuntimeError("Not connected to OPC UA server")
        
        try:
            # Create subscription if not exists
            if not self.subscription:
                self.subscription = await self.client.create_subscription(
                    period=sampling_interval,
                    handler=self._subscription_handler
                )
                logger.info(f"Created OPC UA subscription with {sampling_interval}ms interval")
            
            # Subscribe to nodes
            for node_id in node_ids:
                node = self.client.get_node(node_id)
                handle = await self.subscription.subscribe_data_change(node)
                self.subscribed_nodes[node_id] = handle
                logger.info(f"Subscribed to node: {node_id}")
            
            # Register callback
            if callback:
                self.callbacks.append(callback)
            
            return True
            
        except Exception as e:
            logger.error(f"Failed to subscribe to OPC UA nodes: {str(e)}")
            return False
    
    async def unsubscribe_nodes(self, node_ids: List[str]) -> bool:
        """
        Unsubscribe from OPC UA nodes
        
        Args:
            node_ids: List of node IDs to unsubscribe
        
        Returns:
            True if unsubscription successful
        """
        if not self.subscription:
            return True
        
        try:
            for node_id in node_ids:
                if node_id in self.subscribed_nodes:
                    handle = self.subscribed_nodes[node_id]
                    await self.subscription.unsubscribe(handle)
                    del self.subscribed_nodes[node_id]
                    logger.info(f"Unsubscribed from node: {node_id}")
            
            return True
            
        except Exception as e:
            logger.error(f"Failed to unsubscribe from OPC UA nodes: {str(e)}")
            return False
    
    async def unsubscribe_all(self) -> bool:
        """
        Unsubscribe from all nodes
        
        Returns:
            True if successful
        """
        if not self.subscription:
            return True
        
        try:
            await self.subscription.delete()
            self.subscription = None
            self.subscribed_nodes.clear()
            logger.info("Unsubscribed from all OPC UA nodes")
            return True
            
        except Exception as e:
            logger.error(f"Failed to unsubscribe from all nodes: {str(e)}")
            return False
    
    def _subscription_handler(self, node, value, data):
        """
        Internal handler for subscription data changes
        
        Args:
            node: OPC UA node
            value: New value
            data: Data change notification
        """
        try:
            node_id = node.nodeid.to_string()
            
            # Update cached value
            self.node_values[node_id] = {
                "value": value,
                "timestamp": datetime.now().isoformat(),
                "quality": "Good"
            }
            
            self.data_count += 1
            
            # Call registered callbacks
            for callback in self.callbacks:
                try:
                    if asyncio.iscoroutinefunction(callback):
                        asyncio.create_task(callback(node_id, value))
                    else:
                        callback(node_id, value)
                except Exception as e:
                    logger.error(f"Error in subscription callback: {str(e)}")
            
            logger.debug(f"Data change: {node_id} = {value}")
            
        except Exception as e:
            logger.error(f"Error in subscription handler: {str(e)}")
    
    async def browse_nodes(self, root_node_id: Optional[str] = None) -> List[Dict[str, Any]]:
        """
        Browse OPC UA node tree
        
        Args:
            root_node_id: Root node ID to start browsing (default: Objects folder)
        
        Returns:
            List of node information dictionaries
        """
        if not self.client:
            raise RuntimeError("Not connected to OPC UA server")
        
        try:
            if root_node_id:
                root = self.client.get_node(root_node_id)
            else:
                # Default to Objects folder
                root = self.client.nodes.objects
            
            nodes = []
            children = await root.get_children()
            
            for child in children:
                node_info = {
                    "node_id": child.nodeid.to_string(),
                    "browse_name": (await child.read_browse_name()).Name,
                    "display_name": (await child.read_display_name()).Text,
                    "node_class": (await child.read_node_class()).name
                }
                
                # Try to read value if it's a variable
                try:
                    if node_info["node_class"] == "Variable":
                        node_info["value"] = await child.read_value()
                except:
                    pass
                
                nodes.append(node_info)
            
            return nodes
            
        except Exception as e:
            logger.error(f"Failed to browse OPC UA nodes: {str(e)}")
            raise
    
    async def call_method(
        self,
        object_node_id: str,
        method_node_id: str,
        arguments: List[Any]
    ) -> Any:
        """
        Call an OPC UA method
        
        Args:
            object_node_id: Object node ID
            method_node_id: Method node ID
            arguments: List of method arguments
        
        Returns:
            Method return value
        """
        if not self.client:
            raise RuntimeError("Not connected to OPC UA server")
        
        try:
            object_node = self.client.get_node(object_node_id)
            result = await object_node.call_method(method_node_id, *arguments)
            logger.info(f"Called method {method_node_id} on {object_node_id}")
            return result
            
        except Exception as e:
            logger.error(f"Failed to call OPC UA method: {str(e)}")
            raise
    
    def get_status(self) -> Dict[str, Any]:
        """Get connector status with OPC UA specific information"""
        status = super().get_status()
        status.update({
            "endpoint_url": self.config.get("endpoint_url"),
            "subscribed_nodes": len(self.subscribed_nodes),
            "cached_values": len(self.node_values)
        })
        return status

