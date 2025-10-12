"""
NDH Base Connector
Abstract base class for all connectors
"""

from abc import ABC, abstractmethod
from typing import Dict, Any, Optional, List
from datetime import datetime

from loguru import logger


class ConnectorStatus:
    """Connector status constants"""
    DISCONNECTED = "disconnected"
    CONNECTING = "connecting"
    CONNECTED = "connected"
    ERROR = "error"


class BaseConnector(ABC):
    """
    Abstract base class for NDH connectors
    All connectors must inherit from this class
    """
    
    def __init__(self, connector_id: str, config: Dict[str, Any]):
        """
        Initialize connector
        
        Args:
            connector_id: Unique connector identifier
            config: Connector configuration
        """
        self.connector_id = connector_id
        self.config = config
        self.status = ConnectorStatus.DISCONNECTED
        self.last_error: Optional[str] = None
        self.connected_at: Optional[datetime] = None
        self.data_count = 0
    
    @abstractmethod
    async def connect(self) -> bool:
        """
        Connect to the external system
        
        Returns:
            True if connection successful, False otherwise
        """
        pass
    
    @abstractmethod
    async def disconnect(self) -> bool:
        """
        Disconnect from the external system
        
        Returns:
            True if disconnection successful, False otherwise
        """
        pass
    
    @abstractmethod
    async def read(self, tags: Optional[List[str]] = None) -> Dict[str, Any]:
        """
        Read data from the external system
        
        Args:
            tags: Optional list of specific tags to read
            
        Returns:
            Dictionary of tag values
        """
        pass
    
    @abstractmethod
    async def write(self, data: Dict[str, Any]) -> bool:
        """
        Write data to the external system
        
        Args:
            data: Dictionary of tag values to write
            
        Returns:
            True if write successful, False otherwise
        """
        pass
    
    async def validate_config(self) -> bool:
        """
        Validate connector configuration
        
        Returns:
            True if configuration is valid, False otherwise
        """
        required_fields = self.get_required_config_fields()
        
        for field in required_fields:
            if field not in self.config:
                logger.error(f"Missing required config field: {field}")
                return False
        
        return True
    
    @abstractmethod
    def get_required_config_fields(self) -> List[str]:
        """
        Get list of required configuration fields
        
        Returns:
            List of required field names
        """
        pass
    
    def get_status(self) -> Dict[str, Any]:
        """
        Get connector status information
        
        Returns:
            Status dictionary
        """
        return {
            "connector_id": self.connector_id,
            "status": self.status,
            "connected_at": self.connected_at.isoformat() if self.connected_at else None,
            "last_error": self.last_error,
            "data_count": self.data_count
        }
    
    def _set_status(self, status: str, error: Optional[str] = None):
        """
        Set connector status
        
        Args:
            status: New status
            error: Optional error message
        """
        self.status = status
        self.last_error = error
        
        if status == ConnectorStatus.CONNECTED:
            self.connected_at = datetime.now()
        elif status == ConnectorStatus.DISCONNECTED:
            self.connected_at = None
    
    def __repr__(self) -> str:
        return f"<{self.__class__.__name__} id={self.connector_id} status={self.status}>"

