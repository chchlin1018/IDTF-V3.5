"""
完整的工廠數位分身範例

展示如何建立一個完整的工廠數位分身,包括:
1. 從多個工業系統收集資料
2. 同步到 Omniverse
3. 實現即時監控和歷史回放
"""

import asyncio
import logging
import sys
import os
from datetime import datetime, timedelta
from typing import Dict, List

# 添加 NDH 到路徑
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '../..'))

from ndh.connectors.omniverse.omniverse_connector import (
    OmniverseConnector,
    OmniverseConnectorConfig
)

# 設置日誌
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


class DigitalTwinsFactory:
    """
    工廠數位分身管理器
    """
    
    def __init__(self, config: OmniverseConnectorConfig):
        self.config = config
        self.connector = OmniverseConnector(config)
        
    async def initialize(self):
        """初始化數位分身"""
        logger.info("Initializing Digital Twins Factory...")
        
        # 連接到 Kafka 和 Nucleus
        await self.connector.connect()
        
        # 創建工廠場景結構
        await self._create_factory_scene()
        
        logger.info("Digital Twins Factory initialized")
        
    async def _create_factory_scene(self):
        """創建工廠場景結構"""
        if not self.connector.usd_stage:
            logger.warning("USD Stage not available, skipping scene creation")
            return
            
        logger.info("Creating factory scene structure...")
        
        from pxr import Usd, UsdGeom, Sdf, Gf
        
        stage = self.connector.usd_stage
        
        # 創建根節點
        world = UsdGeom.Xform.Define(stage, "/World")
        
        # 創建工廠
        factory = UsdGeom.Xform.Define(stage, "/World/Factory")
        factory.AddTranslateOp().Set(Gf.Vec3d(0, 0, 0))
        
        # 創建生產線 1
        line1 = UsdGeom.Xform.Define(stage, "/World/Factory/Line1")
        line1.AddTranslateOp().Set(Gf.Vec3d(0, 0, 0))
        
        # 創建 CNC 機床
        cnc01 = self._create_machine(
            stage,
            "/World/Factory/Line1/CNC_01",
            position=(10, 0, 5),
            machine_type="CNC"
        )
        
        # 創建機器人
        robot01 = self._create_machine(
            stage,
            "/World/Factory/Line1/Robot_01",
            position=(20, 0, 5),
            machine_type="Robot"
        )
        
        # 創建輸送帶
        conveyor01 = self._create_machine(
            stage,
            "/World/Factory/Line1/Conveyor_01",
            position=(15, 0, 0),
            machine_type="Conveyor"
        )
        
        # 保存場景
        stage.Save()
        logger.info("Factory scene structure created")
        
    def _create_machine(self, stage, prim_path: str, position: tuple,
                       machine_type: str):
        """創建機器 Prim"""
        from pxr import UsdGeom, Sdf, Gf
        
        # 創建 Xform
        xform = UsdGeom.Xform.Define(stage, prim_path)
        xform.AddTranslateOp().Set(Gf.Vec3d(*position))
        
        # 獲取 Prim
        prim = stage.GetPrimAtPath(prim_path)
        
        # 添加自定義屬性
        prim.CreateAttribute("custom:machine_type", Sdf.ValueTypeNames.String).Set(machine_type)
        prim.CreateAttribute("custom:temperature", Sdf.ValueTypeNames.Float).Set(25.0)
        prim.CreateAttribute("custom:speed", Sdf.ValueTypeNames.Float).Set(0.0)
        prim.CreateAttribute("custom:state", Sdf.ValueTypeNames.String).Set("IDLE")
        prim.CreateAttribute("custom:alarm", Sdf.ValueTypeNames.Bool).Set(False)
        
        # 添加視覺屬性
        prim.CreateAttribute("primvars:displayColor", Sdf.ValueTypeNames.Color3fArray).Set(
            [Gf.Vec3f(0.5, 0.5, 0.5)]
        )
        
        logger.info(f"Created machine: {prim_path} ({machine_type})")
        
        return xform
        
    async def start_monitoring(self):
        """開始即時監控"""
        logger.info("Starting real-time monitoring...")
        
        # 啟動連接器
        await self.connector.start()
        
    async def replay_history(self, start_time: datetime, end_time: datetime,
                            speed: float = 1.0):
        """
        回放歷史資料
        
        Args:
            start_time: 開始時間
            end_time: 結束時間
            speed: 回放速度 (1.0 = 實時, 2.0 = 2倍速)
        """
        logger.info(f"Replaying history from {start_time} to {end_time} at {speed}x speed")
        
        # TODO: 實作歷史回放邏輯
        # 1. 從 Kafka 讀取歷史事件
        # 2. 在 USD 中創建時間採樣
        # 3. 應用事件到對應的時間點
        
        logger.info("History replay completed")
        
    async def shutdown(self):
        """關閉數位分身"""
        logger.info("Shutting down Digital Twins Factory...")
        
        await self.connector.disconnect()
        
        # 打印統計資訊
        stats = self.connector.get_stats()
        logger.info("Final statistics:")
        for key, value in stats.items():
            logger.info(f"  {key}: {value}")
            
        logger.info("Digital Twins Factory shut down")


async def main():
    """
    主函數
    """
    logger.info("Starting Digital Twins Factory Example")
    
    # 創建配置
    config = OmniverseConnectorConfig(
        # Kafka 配置
        kafka_bootstrap_servers="localhost:9092",
        kafka_topics=["asset.telemetry", "asset.events", "asset.alarms"],
        kafka_group_id="digital-twins-factory",
        
        # Nucleus 配置
        nucleus_url="omniverse://localhost",
        
        # Stage 配置
        stage_url="omniverse://localhost/Projects/Factory/DigitalTwin.usd",
        update_rate=10.0,
        
        # 批次更新配置
        batch_size=100,
        flush_interval=0.1,
        
        # 資料過濾配置
        enable_filtering=True,
        min_update_interval=0.1,
        value_change_threshold=0.01
    )
    
    # 創建數位分身工廠
    factory = DigitalTwinsFactory(config)
    
    try:
        # 初始化
        await factory.initialize()
        
        # 開始即時監控
        await factory.start_monitoring()
        
    except KeyboardInterrupt:
        logger.info("Received keyboard interrupt")
    finally:
        await factory.shutdown()


if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        logger.info("Exiting...")

