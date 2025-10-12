"""
Modbus TCP Connector Plugin for NDH
"""

import asyncio
import logging
from typing import Any, Callable, Dict, List, Optional

from pymodbus.client import AsyncModbusTcpClient
from pymodbus.exceptions import ModbusException

# 建立日誌記錄器
logger = logging.getLogger(__name__)


class ModbusTCPPlugin:
    """Modbus TCP 連接器插件"""

    def __init__(self, config: Dict):
        """
        初始化 Modbus TCP 插件

        Args:
            config: 插件配置字典
        """
        self.config = config
        self.client: Optional[AsyncModbusTcpClient] = None
        self.is_connected = False
        self.subscription_tasks = []

        # 從配置中取得參數
        self.host = config.get("host")
        self.port = config.get("port", 502)
        self.unit_id = config.get("unit_id", 1)
        self.timeout = config.get("timeout", 10)
        self.retry_on_empty = config.get("retry_on_empty", False)
        self.retries = config.get("retries", 3)

        # 統計資訊
        self.stats = {
            "reads": 0,
            "writes": 0,
            "errors": 0,
            "reconnections": 0
        }

    async def on_load(self):
        """插件載入時呼叫"""
        logger.info("Modbus TCP Plugin loaded for %s:%d", self.host, self.port)

    async def on_start(self):
        """插件啟動時呼叫"""
        logger.info("Modbus TCP Plugin starting...")
        await self.connect()

    async def on_stop(self):
        """插件停止時呼叫"""
        logger.info("Modbus TCP Plugin stopping...")
        
        # 停止所有訂閱任務
        for task in self.subscription_tasks:
            task.cancel()
        
        await self.disconnect()

    async def on_unload(self):
        """插件卸載時呼叫"""
        logger.info("Modbus TCP Plugin unloaded")

    async def connect(self) -> bool:
        """
        建立與 Modbus TCP 伺服器的連接

        Returns:
            bool: 連接成功返回 True,否則返回 False
        """
        try:
            logger.info("Connecting to Modbus TCP server at %s:%d", self.host, self.port)
            
            # 建立 Modbus TCP 客戶端
            self.client = AsyncModbusTcpClient(
                host=self.host,
                port=self.port,
                timeout=self.timeout,
                retry_on_empty=self.retry_on_empty,
                retries=self.retries
            )
            
            # 連接到伺服器
            await self.client.connect()
            
            self.is_connected = True
            logger.info("Connected to Modbus TCP server successfully")
            return True

        except Exception as e:
            logger.error("Failed to connect to Modbus TCP server: %s", e)
            self.stats["errors"] += 1
            return False

    async def disconnect(self):
        """斷開與 Modbus TCP 伺服器的連接"""
        if self.client:
            logger.info("Disconnecting from Modbus TCP server...")
            self.client.close()
            self.is_connected = False
            logger.info("Disconnected from Modbus TCP server")

    async def read(self, address: str) -> Any:
        """
        從 Modbus 設備讀取資料

        Args:
            address: 資料位址,格式: "type:address"
                     例如: "holding:100", "coil:0", "input:200"

        Returns:
            Any: 讀取的資料值
        """
        if not self.is_connected:
            logger.warning("Not connected to Modbus server")
            return None

        try:
            # 解析位址
            register_type, register_address = self._parse_address(address)
            register_address = int(register_address)

            # 根據類型讀取
            if register_type == "holding":
                result = await self.client.read_holding_registers(
                    address=register_address,
                    count=1,
                    slave=self.unit_id
                )
                if not result.isError():
                    value = result.registers[0]
                    self.stats["reads"] += 1
                    logger.debug("Read holding register %d: %d", register_address, value)
                    return value
                else:
                    logger.error("Error reading holding register %d: %s", register_address, result)
                    self.stats["errors"] += 1
                    return None

            elif register_type == "coil":
                result = await self.client.read_coils(
                    address=register_address,
                    count=1,
                    slave=self.unit_id
                )
                if not result.isError():
                    value = result.bits[0]
                    self.stats["reads"] += 1
                    logger.debug("Read coil %d: %s", register_address, value)
                    return value
                else:
                    logger.error("Error reading coil %d: %s", register_address, result)
                    self.stats["errors"] += 1
                    return None

            elif register_type == "input":
                result = await self.client.read_input_registers(
                    address=register_address,
                    count=1,
                    slave=self.unit_id
                )
                if not result.isError():
                    value = result.registers[0]
                    self.stats["reads"] += 1
                    logger.debug("Read input register %d: %d", register_address, value)
                    return value
                else:
                    logger.error("Error reading input register %d: %s", register_address, result)
                    self.stats["errors"] += 1
                    return None

            elif register_type == "discrete":
                result = await self.client.read_discrete_inputs(
                    address=register_address,
                    count=1,
                    slave=self.unit_id
                )
                if not result.isError():
                    value = result.bits[0]
                    self.stats["reads"] += 1
                    logger.debug("Read discrete input %d: %s", register_address, value)
                    return value
                else:
                    logger.error("Error reading discrete input %d: %s", register_address, result)
                    self.stats["errors"] += 1
                    return None

            else:
                logger.error("Unknown register type: %s", register_type)
                return None

        except Exception as e:
            logger.error("Error reading from Modbus: %s", e)
            self.stats["errors"] += 1
            return None

    async def write(self, address: str, value: Any) -> bool:
        """
        將資料寫入 Modbus 設備

        Args:
            address: 資料位址,格式: "type:address"
            value: 要寫入的值

        Returns:
            bool: 寫入成功返回 True,否則返回 False
        """
        if not self.is_connected:
            logger.warning("Not connected to Modbus server")
            return False

        try:
            # 解析位址
            register_type, register_address = self._parse_address(address)
            register_address = int(register_address)

            # 根據類型寫入
            if register_type == "holding":
                result = await self.client.write_register(
                    address=register_address,
                    value=int(value),
                    slave=self.unit_id
                )
                if not result.isError():
                    self.stats["writes"] += 1
                    logger.debug("Wrote holding register %d: %d", register_address, value)
                    return True
                else:
                    logger.error("Error writing holding register %d: %s", register_address, result)
                    self.stats["errors"] += 1
                    return False

            elif register_type == "coil":
                result = await self.client.write_coil(
                    address=register_address,
                    value=bool(value),
                    slave=self.unit_id
                )
                if not result.isError():
                    self.stats["writes"] += 1
                    logger.debug("Wrote coil %d: %s", register_address, value)
                    return True
                else:
                    logger.error("Error writing coil %d: %s", register_address, result)
                    self.stats["errors"] += 1
                    return False

            else:
                logger.error("Cannot write to register type: %s", register_type)
                return False

        except Exception as e:
            logger.error("Error writing to Modbus: %s", e)
            self.stats["errors"] += 1
            return False

    async def subscribe(self, addresses: List[str], callback: Callable):
        """
        訂閱 Modbus 資料變更 (透過輪詢實作)

        Args:
            addresses: 要訂閱的位址列表
            callback: 資料變更時的回調函數
        """
        logger.info("Subscribing to %d addresses", len(addresses))

        async def _poll_task():
            """輪詢任務"""
            previous_values = {}
            
            while True:
                try:
                    for address in addresses:
                        # 讀取當前值
                        current_value = await self.read(address)
                        
                        # 檢查是否有變更
                        if address not in previous_values or previous_values[address] != current_value:
                            previous_values[address] = current_value
                            
                            # 呼叫回調函數
                            await callback({
                                "address": address,
                                "value": current_value,
                                "timestamp": asyncio.get_event_loop().time()
                            })
                    
                    # 等待一秒後再次輪詢
                    await asyncio.sleep(1)

                except asyncio.CancelledError:
                    logger.info("Subscription task cancelled")
                    break
                except Exception as e:
                    logger.error("Error in subscription task: %s", e)
                    await asyncio.sleep(5)  # 發生錯誤時等待較長時間

        # 建立並啟動輪詢任務
        task = asyncio.create_task(_poll_task())
        self.subscription_tasks.append(task)

    async def batch_read(self, addresses: List[str]) -> Dict[str, Any]:
        """
        批次讀取多個位址

        Args:
            addresses: 位址列表

        Returns:
            Dict[str, Any]: 位址到值的映射
        """
        results = {}
        for address in addresses:
            value = await self.read(address)
            results[address] = value
        return results

    async def batch_write(self, data: Dict[str, Any]) -> Dict[str, bool]:
        """
        批次寫入多個位址

        Args:
            data: 位址到值的映射

        Returns:
            Dict[str, bool]: 位址到寫入結果的映射
        """
        results = {}
        for address, value in data.items():
            success = await self.write(address, value)
            results[address] = success
        return results

    def get_statistics(self) -> Dict:
        """
        取得統計資訊

        Returns:
            Dict: 統計資訊字典
        """
        return {
            "is_connected": self.is_connected,
            "reads": self.stats["reads"],
            "writes": self.stats["writes"],
            "errors": self.stats["errors"],
            "reconnections": self.stats["reconnections"]
        }

    def _parse_address(self, address: str) -> tuple:
        """
        解析位址字串

        Args:
            address: 位址字串,格式: "type:address"

        Returns:
            tuple: (register_type, register_address)
        """
        if ":" in address:
            register_type, register_address = address.split(":", 1)
            return register_type.lower(), register_address
        else:
            # 預設為 holding register
            return "holding", address

