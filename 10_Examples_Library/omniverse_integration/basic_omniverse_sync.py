"""
基本的 Omniverse 同步範例

展示如何使用 NDH Omniverse Connector 將工業資料同步到 Omniverse。
"""

import asyncio
import logging
import sys
import os

# 添加 NDH 到路徑
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '../..'))

from ndh.connectors.omniverse.omniverse_connector import (
    OmniverseConnector,
    OmniverseConnectorConfig,
    create_and_run_connector
)

# 設置日誌
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


async def main():
    """
    主函數
    """
    logger.info("Starting NDH Omniverse Connector Example")
    
    # 創建配置
    config = OmniverseConnectorConfig(
        # Kafka 配置
        kafka_bootstrap_servers="localhost:9092",
        kafka_topics=["asset.telemetry", "asset.events", "asset.alarms"],
        kafka_group_id="omniverse-connector-example",
        
        # Nucleus 配置
        nucleus_url="omniverse://localhost",
        # nucleus_username="your_username",  # 如果需要認證
        # nucleus_api_token="your_api_token",
        
        # Stage 配置
        stage_url="omniverse://localhost/Projects/Factory/DigitalTwin.usd",
        update_rate=10.0,  # 10 Hz
        
        # 批次更新配置
        batch_size=50,
        flush_interval=0.1,  # 100ms
        
        # 資料過濾配置
        enable_filtering=True,
        min_update_interval=0.1,  # 100ms
        value_change_threshold=0.01  # 1%
    )
    
    # 創建並運行連接器
    await create_and_run_connector(config)


if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        logger.info("Received keyboard interrupt, exiting...")

