"""
NDH IADL Connector
Connector for IADL Editor integration
"""

import json
from typing import Dict, Any, Optional, List
from pathlib import Path

from loguru import logger

from .base import BaseConnector, ConnectorStatus


class IADLConnector(BaseConnector):
    """
    IADL Connector for integrating with IADL Editor
    Supports reading IADL files and converting to NDH format
    """
    
    def __init__(self, connector_id: str, config: Dict[str, Any]):
        super().__init__(connector_id, config)
        self.iadl_file: Optional[Path] = None
        self.assets: Dict[str, Any] = {}
    
    def get_required_config_fields(self) -> List[str]:
        """Get required configuration fields"""
        return ["iadl_file_path"]
    
    async def connect(self) -> bool:
        """
        Connect to IADL file
        
        Returns:
            True if connection successful
        """
        try:
            self._set_status(ConnectorStatus.CONNECTING)
            
            # Validate configuration
            if not await self.validate_config():
                raise ValueError("Invalid configuration")
            
            # Get IADL file path
            file_path = self.config.get("iadl_file_path")
            self.iadl_file = Path(file_path)
            
            if not self.iadl_file.exists():
                raise FileNotFoundError(f"IADL file not found: {file_path}")
            
            # Load IADL file
            await self._load_iadl_file()
            
            self._set_status(ConnectorStatus.CONNECTED)
            logger.info(f"IADL Connector connected: {self.connector_id}")
            
            return True
        
        except Exception as e:
            error_msg = f"Failed to connect IADL connector: {e}"
            logger.error(error_msg)
            self._set_status(ConnectorStatus.ERROR, error_msg)
            return False
    
    async def disconnect(self) -> bool:
        """
        Disconnect from IADL file
        
        Returns:
            True if disconnection successful
        """
        try:
            self.assets = {}
            self.iadl_file = None
            self._set_status(ConnectorStatus.DISCONNECTED)
            logger.info(f"IADL Connector disconnected: {self.connector_id}")
            return True
        
        except Exception as e:
            logger.error(f"Failed to disconnect IADL connector: {e}")
            return False
    
    async def read(self, tags: Optional[List[str]] = None) -> Dict[str, Any]:
        """
        Read data from IADL file
        
        Args:
            tags: Optional list of asset IDs to read
            
        Returns:
            Dictionary of asset data
        """
        if self.status != ConnectorStatus.CONNECTED:
            raise RuntimeError("Connector not connected")
        
        try:
            if tags:
                # Return specific assets
                result = {
                    tag: self.assets.get(tag)
                    for tag in tags
                    if tag in self.assets
                }
            else:
                # Return all assets
                result = self.assets.copy()
            
            self.data_count += len(result)
            return result
        
        except Exception as e:
            logger.error(f"Failed to read from IADL connector: {e}")
            raise
    
    async def write(self, data: Dict[str, Any]) -> bool:
        """
        Write data to IADL file (update assets)
        
        Args:
            data: Dictionary of asset data to write
            
        Returns:
            True if write successful
        """
        if self.status != ConnectorStatus.CONNECTED:
            raise RuntimeError("Connector not connected")
        
        try:
            # Update assets
            self.assets.update(data)
            
            # Write back to file if auto_save is enabled
            if self.config.get("auto_save", False):
                await self._save_iadl_file()
            
            self.data_count += len(data)
            logger.info(f"Updated {len(data)} assets in IADL connector")
            
            return True
        
        except Exception as e:
            logger.error(f"Failed to write to IADL connector: {e}")
            return False
    
    async def _load_iadl_file(self):
        """Load IADL file and parse assets"""
        try:
            # Check file extension
            if self.iadl_file.suffix.lower() == '.json':
                # Load JSON format
                with open(self.iadl_file, 'r', encoding='utf-8') as f:
                    data = json.load(f)
                    self.assets = data.get("assets", {})
            else:
                # Load IADL format (simplified parser)
                with open(self.iadl_file, 'r', encoding='utf-8') as f:
                    content = f.read()
                    self.assets = self._parse_iadl_content(content)
            
            logger.info(f"Loaded {len(self.assets)} assets from IADL file")
        
        except Exception as e:
            logger.error(f"Failed to load IADL file: {e}")
            raise
    
    async def _save_iadl_file(self):
        """Save assets back to IADL file"""
        try:
            if self.iadl_file.suffix.lower() == '.json':
                # Save as JSON
                with open(self.iadl_file, 'w', encoding='utf-8') as f:
                    json.dump({"assets": self.assets}, f, indent=2)
            else:
                # Save as IADL format
                content = self._generate_iadl_content(self.assets)
                with open(self.iadl_file, 'w', encoding='utf-8') as f:
                    f.write(content)
            
            logger.info(f"Saved {len(self.assets)} assets to IADL file")
        
        except Exception as e:
            logger.error(f"Failed to save IADL file: {e}")
            raise
    
    def _parse_iadl_content(self, content: str) -> Dict[str, Any]:
        """
        Parse IADL content (simplified parser)
        
        Args:
            content: IADL file content
            
        Returns:
            Dictionary of assets
        """
        # This is a simplified parser
        # In a real implementation, you would use a proper IADL parser
        assets = {}
        
        # Simple parsing logic (placeholder)
        lines = content.split('\n')
        current_asset = None
        
        for line in lines:
            line = line.strip()
            if not line or line.startswith('//'):
                continue
            
            if line.startswith('Asset'):
                # Extract asset ID
                parts = line.split()
                if len(parts) >= 2:
                    asset_id = parts[1].strip('{')
                    current_asset = asset_id
                    assets[asset_id] = {"id": asset_id, "properties": {}}
            
            elif current_asset and ':' in line:
                # Parse property
                key, value = line.split(':', 1)
                key = key.strip()
                value = value.strip().rstrip(';')
                assets[current_asset]["properties"][key] = value
        
        return assets
    
    def _generate_iadl_content(self, assets: Dict[str, Any]) -> str:
        """
        Generate IADL content from assets
        
        Args:
            assets: Dictionary of assets
            
        Returns:
            IADL content string
        """
        # This is a simplified generator
        lines = []
        
        for asset_id, asset_data in assets.items():
            lines.append(f"Asset {asset_id} {{")
            
            properties = asset_data.get("properties", {})
            for key, value in properties.items():
                lines.append(f"    {key}: {value};")
            
            lines.append("}")
            lines.append("")
        
        return '\n'.join(lines)
    
    def get_asset_count(self) -> int:
        """Get number of loaded assets"""
        return len(self.assets)
    
    def get_asset_ids(self) -> List[str]:
        """Get list of all asset IDs"""
        return list(self.assets.keys())

