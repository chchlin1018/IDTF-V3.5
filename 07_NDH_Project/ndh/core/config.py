"""
NDH Configuration Module
Handles application configuration using Pydantic Settings
"""

from functools import lru_cache
from typing import Optional

from pydantic import Field
from pydantic_settings import BaseSettings, SettingsConfigDict


class Settings(BaseSettings):
    """NDH Application Settings"""
    
    # Server Configuration
    host: str = Field(default="0.0.0.0", description="Server host")
    port: int = Field(default=8000, description="Server port")
    environment: str = Field(default="development", description="Environment (development/production)")
    
    # Redis Configuration
    redis_enabled: bool = Field(default=False, description="Enable Redis support")
    redis_host: str = Field(default="localhost", description="Redis host")
    redis_port: int = Field(default=6379, description="Redis port")
    redis_db: int = Field(default=0, description="Redis database number")
    redis_password: Optional[str] = Field(default=None, description="Redis password")
    
    # Database Configuration
    database_url: str = Field(default="sqlite+aiosqlite:///./ndh.db", description="Database URL")
    
    # Queue Configuration
    queue_max_size: int = Field(default=10000, description="Maximum queue size")
    queue_timeout: int = Field(default=30, description="Queue timeout in seconds")
    
    # Data Retention
    data_retention_days: int = Field(default=30, description="Data retention period in days")
    
    # API Configuration
    api_rate_limit: int = Field(default=100, description="API rate limit per minute")
    api_timeout: int = Field(default=30, description="API timeout in seconds")
    
    # WebSocket Configuration
    ws_heartbeat_interval: int = Field(default=30, description="WebSocket heartbeat interval in seconds")
    ws_max_connections: int = Field(default=1000, description="Maximum WebSocket connections")
    
    # Logging
    log_level: str = Field(default="INFO", description="Logging level")
    log_file: str = Field(default="logs/ndh.log", description="Log file path")
    
    model_config = SettingsConfigDict(
        env_file=".env",
        env_file_encoding="utf-8",
        case_sensitive=False,
        env_prefix="NDH_"
    )


@lru_cache()
def get_settings() -> Settings:
    """Get cached settings instance"""
    return Settings()

