# NDH 可抽換時序數據庫架構設計

**作者**: 林志錚 (Chih Cheng Lin, Michael Lin)  
**日期**: 2025 年 10 月 12 日  
**版本**: 1.0

---

## 執行摘要

本文檔設計一個**可抽換時序數據庫的 NDH 架構**,允許根據不同場域的需求靈活切換底層時序數據庫(InfluxDB, TDengine, TimescaleDB, QuestDB 等),而無需修改上層應用代碼。

**核心設計原則**:
1. **抽象層隔離**: 定義統一的時序數據庫接口
2. **適配器模式**: 為每種數據庫實現適配器
3. **配置驅動**: 通過配置文件切換數據庫
4. **零停機切換**: 支援在線遷移和雙寫

**支援的數據庫**:
- ✅ InfluxDB 2.x/3.x
- ✅ TDengine 3.x
- ✅ TimescaleDB 2.x
- ✅ QuestDB
- ✅ 可擴展支援其他數據庫

**預期效益**:
- 🎯 **靈活性**: 根據場域需求選擇最適合的數據庫
- 🎯 **風險管理**: 降低供應鏈風險,避免廠商鎖定
- 🎯 **平滑遷移**: 支援零停機在線遷移
- 🎯 **成本優化**: 在性能、成本、安全之間靈活平衡

---

## 1. 架構總覽

### 1.1 分層架構

```
┌─────────────────────────────────────────────────────────────┐
│              應用層 (Application Layer)                      │
│  Digital Twin Apps, Dashboards, Analytics, AI/ML            │
├─────────────────────────────────────────────────────────────┤
│              API 層 (API Layer)                              │
│  RESTful API, GraphQL, WebSocket                            │
│  - /api/v1/timeseries/query                                 │
│  - /api/v1/timeseries/write                                 │
│  - /api/v1/timeseries/aggregate                             │
├─────────────────────────────────────────────────────────────┤
│         時序數據庫抽象層 (TSDB Abstraction Layer) ⭐         │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  統一接口 (Unified Interface)                          │  │
│  │  - ITSDBWriter: write(), batch_write()                │  │
│  │  - ITSDBReader: query(), aggregate(), stream()        │  │
│  │  - ITSDBAdmin: create_table(), drop_table(), backup() │  │
│  └───────────────────────────────────────────────────────┘  │
├─────────────────────────────────────────────────────────────┤
│         適配器層 (Adapter Layer) ⭐                          │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐      │
│  │ InfluxDB │ │ TDengine │ │Timescale │ │ QuestDB  │      │
│  │ Adapter  │ │ Adapter  │ │   DB     │ │ Adapter  │      │
│  │          │ │          │ │ Adapter  │ │          │      │
│  └────┬─────┘ └────┬─────┘ └────┬─────┘ └────┬─────┘      │
├───────┼────────────┼────────────┼────────────┼─────────────┤
│       │  數據庫層 (Database Layer)  │            │           │
│  ┌────▼─────┐ ┌───▼──────┐ ┌───▼──────┐ ┌───▼──────┐      │
│  │ InfluxDB │ │ TDengine │ │Timescale │ │ QuestDB  │      │
│  │ Cluster  │ │ Cluster  │ │   DB     │ │ Cluster  │      │
│  └──────────┘ └──────────┘ └──────────┘ └──────────┘      │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 核心組件

#### 1.2.1 時序數據庫抽象層 (TSDB Abstraction Layer)

**職責**:
- 定義統一的時序數據庫接口
- 管理適配器生命週期
- 提供配置驅動的數據庫切換
- 支援多數據庫並行運行(雙寫)

**核心接口**:
```python
# tsdb_interface.py

from abc import ABC, abstractmethod
from typing import List, Dict, Any, Iterator
from datetime import datetime

class ITSDBWriter(ABC):
    """時序數據庫寫入接口"""
    
    @abstractmethod
    async def write(self, measurement: str, tags: Dict[str, str], 
                   fields: Dict[str, Any], timestamp: datetime) -> bool:
        """寫入單條數據"""
        pass
    
    @abstractmethod
    async def batch_write(self, points: List[Dict]) -> bool:
        """批量寫入數據"""
        pass
    
    @abstractmethod
    async def write_line_protocol(self, lines: List[str]) -> bool:
        """使用 Line Protocol 寫入"""
        pass


class ITSDBReader(ABC):
    """時序數據庫讀取接口"""
    
    @abstractmethod
    async def query(self, query: str, params: Dict = None) -> List[Dict]:
        """執行查詢"""
        pass
    
    @abstractmethod
    async def query_range(self, measurement: str, tags: Dict, 
                         start: datetime, end: datetime, 
                         fields: List[str] = None) -> List[Dict]:
        """範圍查詢"""
        pass
    
    @abstractmethod
    async def aggregate(self, measurement: str, tags: Dict,
                       start: datetime, end: datetime,
                       aggregation: str, interval: str) -> List[Dict]:
        """聚合查詢"""
        pass
    
    @abstractmethod
    async def stream(self, measurement: str, tags: Dict,
                    callback: callable) -> None:
        """串流查詢"""
        pass


class ITSDBAdmin(ABC):
    """時序數據庫管理接口"""
    
    @abstractmethod
    async def create_measurement(self, name: str, schema: Dict) -> bool:
        """創建 measurement/table"""
        pass
    
    @abstractmethod
    async def drop_measurement(self, name: str) -> bool:
        """刪除 measurement/table"""
        pass
    
    @abstractmethod
    async def list_measurements(self) -> List[str]:
        """列出所有 measurements"""
        pass
    
    @abstractmethod
    async def backup(self, path: str) -> bool:
        """備份數據"""
        pass
    
    @abstractmethod
    async def restore(self, path: str) -> bool:
        """恢復數據"""
        pass
    
    @abstractmethod
    async def health_check(self) -> Dict:
        """健康檢查"""
        pass


class ITSDBAdapter(ITSDBWriter, ITSDBReader, ITSDBAdmin):
    """完整的時序數據庫適配器接口"""
    
    @abstractmethod
    async def connect(self, config: Dict) -> bool:
        """連接數據庫"""
        pass
    
    @abstractmethod
    async def disconnect(self) -> bool:
        """斷開連接"""
        pass
    
    @abstractmethod
    def get_capabilities(self) -> Dict:
        """獲取數據庫能力"""
        pass
```

#### 1.2.2 適配器管理器 (Adapter Manager)

**職責**:
- 載入和管理適配器
- 根據配置選擇適配器
- 支援多適配器並行運行
- 提供適配器切換功能

**實現**:
```python
# adapter_manager.py

from typing import Dict, List, Optional
import importlib
import logging

class TSDBAdapterManager:
    """時序數據庫適配器管理器"""
    
    def __init__(self, config: Dict):
        self.config = config
        self.adapters: Dict[str, ITSDBAdapter] = {}
        self.primary_adapter: Optional[ITSDBAdapter] = None
        self.secondary_adapters: List[ITSDBAdapter] = []
        self.logger = logging.getLogger(__name__)
    
    async def initialize(self):
        """初始化適配器"""
        # 載入主適配器
        primary_config = self.config['primary']
        self.primary_adapter = await self._load_adapter(
            primary_config['type'], 
            primary_config
        )
        
        # 載入次要適配器(用於雙寫)
        if 'secondary' in self.config:
            for secondary_config in self.config['secondary']:
                adapter = await self._load_adapter(
                    secondary_config['type'],
                    secondary_config
                )
                self.secondary_adapters.append(adapter)
        
        self.logger.info(f"Initialized primary adapter: {primary_config['type']}")
        self.logger.info(f"Initialized {len(self.secondary_adapters)} secondary adapters")
    
    async def _load_adapter(self, adapter_type: str, config: Dict) -> ITSDBAdapter:
        """載入適配器"""
        # 動態載入適配器模組
        module_name = f"ndh.adapters.{adapter_type}_adapter"
        class_name = f"{adapter_type.capitalize()}Adapter"
        
        try:
            module = importlib.import_module(module_name)
            adapter_class = getattr(module, class_name)
            adapter = adapter_class()
            await adapter.connect(config)
            
            self.adapters[adapter_type] = adapter
            return adapter
        except Exception as e:
            self.logger.error(f"Failed to load adapter {adapter_type}: {e}")
            raise
    
    async def write(self, measurement: str, tags: Dict, 
                   fields: Dict, timestamp: datetime) -> bool:
        """寫入數據(支援雙寫)"""
        # 寫入主數據庫
        success = await self.primary_adapter.write(
            measurement, tags, fields, timestamp
        )
        
        # 寫入次要數據庫(非阻塞)
        for adapter in self.secondary_adapters:
            try:
                await adapter.write(measurement, tags, fields, timestamp)
            except Exception as e:
                self.logger.warning(f"Secondary write failed: {e}")
        
        return success
    
    async def query(self, query: str, params: Dict = None) -> List[Dict]:
        """查詢數據(僅從主數據庫)"""
        return await self.primary_adapter.query(query, params)
    
    async def switch_primary(self, new_primary_type: str):
        """切換主數據庫"""
        if new_primary_type not in self.adapters:
            raise ValueError(f"Adapter {new_primary_type} not loaded")
        
        old_primary = self.primary_adapter
        self.primary_adapter = self.adapters[new_primary_type]
        
        # 將舊的主數據庫加入次要數據庫列表
        if old_primary not in self.secondary_adapters:
            self.secondary_adapters.append(old_primary)
        
        self.logger.info(f"Switched primary adapter to {new_primary_type}")
    
    async def health_check(self) -> Dict:
        """健康檢查"""
        result = {
            'primary': await self.primary_adapter.health_check(),
            'secondary': []
        }
        
        for adapter in self.secondary_adapters:
            try:
                health = await adapter.health_check()
                result['secondary'].append(health)
            except Exception as e:
                result['secondary'].append({'status': 'error', 'error': str(e)})
        
        return result
```

---

## 2. 適配器實現

### 2.1 InfluxDB 適配器

```python
# influxdb_adapter.py

from influxdb_client import InfluxDBClient, Point
from influxdb_client.client.write_api import SYNCHRONOUS
from typing import Dict, List, Any
from datetime import datetime
import logging

class InfluxdbAdapter(ITSDBAdapter):
    """InfluxDB 適配器"""
    
    def __init__(self):
        self.client: Optional[InfluxDBClient] = None
        self.write_api = None
        self.query_api = None
        self.bucket = None
        self.org = None
        self.logger = logging.getLogger(__name__)
    
    async def connect(self, config: Dict) -> bool:
        """連接 InfluxDB"""
        try:
            self.client = InfluxDBClient(
                url=config['url'],
                token=config['token'],
                org=config['org']
            )
            self.write_api = self.client.write_api(write_options=SYNCHRONOUS)
            self.query_api = self.client.query_api()
            self.bucket = config['bucket']
            self.org = config['org']
            
            # 健康檢查
            health = self.client.health()
            if health.status == "pass":
                self.logger.info("Connected to InfluxDB successfully")
                return True
            else:
                raise Exception(f"InfluxDB health check failed: {health.message}")
        except Exception as e:
            self.logger.error(f"Failed to connect to InfluxDB: {e}")
            raise
    
    async def disconnect(self) -> bool:
        """斷開連接"""
        if self.client:
            self.client.close()
            self.logger.info("Disconnected from InfluxDB")
        return True
    
    async def write(self, measurement: str, tags: Dict[str, str],
                   fields: Dict[str, Any], timestamp: datetime) -> bool:
        """寫入單條數據"""
        try:
            point = Point(measurement)
            for tag_key, tag_value in tags.items():
                point = point.tag(tag_key, tag_value)
            for field_key, field_value in fields.items():
                point = point.field(field_key, field_value)
            point = point.time(timestamp)
            
            self.write_api.write(bucket=self.bucket, org=self.org, record=point)
            return True
        except Exception as e:
            self.logger.error(f"Write failed: {e}")
            return False
    
    async def batch_write(self, points: List[Dict]) -> bool:
        """批量寫入"""
        try:
            influx_points = []
            for point_data in points:
                point = Point(point_data['measurement'])
                for tag_key, tag_value in point_data.get('tags', {}).items():
                    point = point.tag(tag_key, tag_value)
                for field_key, field_value in point_data.get('fields', {}).items():
                    point = point.field(field_key, field_value)
                point = point.time(point_data['timestamp'])
                influx_points.append(point)
            
            self.write_api.write(bucket=self.bucket, org=self.org, record=influx_points)
            return True
        except Exception as e:
            self.logger.error(f"Batch write failed: {e}")
            return False
    
    async def query(self, query: str, params: Dict = None) -> List[Dict]:
        """執行 Flux 查詢"""
        try:
            tables = self.query_api.query(query, org=self.org)
            results = []
            for table in tables:
                for record in table.records:
                    results.append(record.values)
            return results
        except Exception as e:
            self.logger.error(f"Query failed: {e}")
            raise
    
    async def query_range(self, measurement: str, tags: Dict,
                         start: datetime, end: datetime,
                         fields: List[str] = None) -> List[Dict]:
        """範圍查詢"""
        # 構建 Flux 查詢
        flux_query = f'''
        from(bucket: "{self.bucket}")
          |> range(start: {start.isoformat()}, stop: {end.isoformat()})
          |> filter(fn: (r) => r._measurement == "{measurement}")
        '''
        
        # 添加 tag 過濾
        for tag_key, tag_value in tags.items():
            flux_query += f'  |> filter(fn: (r) => r.{tag_key} == "{tag_value}")\n'
        
        # 添加 field 過濾
        if fields:
            field_filter = ' or '.join([f'r._field == "{f}"' for f in fields])
            flux_query += f'  |> filter(fn: (r) => {field_filter})\n'
        
        return await self.query(flux_query)
    
    async def aggregate(self, measurement: str, tags: Dict,
                       start: datetime, end: datetime,
                       aggregation: str, interval: str) -> List[Dict]:
        """聚合查詢"""
        flux_query = f'''
        from(bucket: "{self.bucket}")
          |> range(start: {start.isoformat()}, stop: {end.isoformat()})
          |> filter(fn: (r) => r._measurement == "{measurement}")
        '''
        
        for tag_key, tag_value in tags.items():
            flux_query += f'  |> filter(fn: (r) => r.{tag_key} == "{tag_value}")\n'
        
        flux_query += f'  |> aggregateWindow(every: {interval}, fn: {aggregation})\n'
        
        return await self.query(flux_query)
    
    def get_capabilities(self) -> Dict:
        """獲取能力"""
        return {
            'database': 'InfluxDB',
            'version': '2.x',
            'features': {
                'clustering': False,  # 開源版不支援
                'streaming': True,
                'sql': False,
                'flux': True,
                'compression': True
            },
            'performance': {
                'write_throughput': '200K points/sec',
                'query_latency': 'medium',
                'compression_ratio': '7:1'
            }
        }
    
    async def health_check(self) -> Dict:
        """健康檢查"""
        try:
            health = self.client.health()
            return {
                'status': health.status,
                'message': health.message,
                'version': health.version
            }
        except Exception as e:
            return {
                'status': 'error',
                'message': str(e)
            }
```

### 2.2 TDengine 適配器

```python
# tdengine_adapter.py

import taos
from typing import Dict, List, Any
from datetime import datetime
import logging

class TdengineAdapter(ITSDBAdapter):
    """TDengine 適配器"""
    
    def __init__(self):
        self.conn: Optional[taos.TaosConnection] = None
        self.cursor = None
        self.database = None
        self.logger = logging.getLogger(__name__)
    
    async def connect(self, config: Dict) -> bool:
        """連接 TDengine"""
        try:
            self.conn = taos.connect(
                host=config['host'],
                port=config.get('port', 6030),
                user=config.get('user', 'root'),
                password=config.get('password', 'taosdata'),
                database=config.get('database', 'ndh')
            )
            self.cursor = self.conn.cursor()
            self.database = config.get('database', 'ndh')
            
            # 創建數據庫(如果不存在)
            self.cursor.execute(f"CREATE DATABASE IF NOT EXISTS {self.database}")
            self.cursor.execute(f"USE {self.database}")
            
            self.logger.info("Connected to TDengine successfully")
            return True
        except Exception as e:
            self.logger.error(f"Failed to connect to TDengine: {e}")
            raise
    
    async def disconnect(self) -> bool:
        """斷開連接"""
        if self.cursor:
            self.cursor.close()
        if self.conn:
            self.conn.close()
        self.logger.info("Disconnected from TDengine")
        return True
    
    async def write(self, measurement: str, tags: Dict[str, str],
                   fields: Dict[str, Any], timestamp: datetime) -> bool:
        """寫入單條數據"""
        try:
            # TDengine 使用子表名 = measurement_tagvalue1_tagvalue2...
            subtable_name = f"{measurement}_{'_'.join(tags.values())}"
            
            # 構建 SQL
            field_names = ', '.join(fields.keys())
            field_values = ', '.join([self._format_value(v) for v in fields.values()])
            tag_values = ', '.join([f"'{v}'" for v in tags.values()])
            
            sql = f"""
            INSERT INTO {subtable_name} USING {measurement} TAGS ({tag_values})
            VALUES ('{timestamp.isoformat()}', {field_values})
            """
            
            self.cursor.execute(sql)
            return True
        except Exception as e:
            self.logger.error(f"Write failed: {e}")
            return False
    
    async def batch_write(self, points: List[Dict]) -> bool:
        """批量寫入"""
        try:
            sql_statements = []
            for point in points:
                measurement = point['measurement']
                tags = point['tags']
                fields = point['fields']
                timestamp = point['timestamp']
                
                subtable_name = f"{measurement}_{'_'.join(tags.values())}"
                field_values = ', '.join([self._format_value(v) for v in fields.values()])
                tag_values = ', '.join([f"'{v}'" for v in tags.values()])
                
                sql = f"{subtable_name} USING {measurement} TAGS ({tag_values}) VALUES ('{timestamp.isoformat()}', {field_values})"
                sql_statements.append(sql)
            
            # TDengine 支援批量插入
            batch_sql = "INSERT INTO " + " ".join(sql_statements)
            self.cursor.execute(batch_sql)
            return True
        except Exception as e:
            self.logger.error(f"Batch write failed: {e}")
            return False
    
    async def query(self, query: str, params: Dict = None) -> List[Dict]:
        """執行 SQL 查詢"""
        try:
            self.cursor.execute(query)
            columns = [desc[0] for desc in self.cursor.description]
            results = []
            for row in self.cursor.fetchall():
                results.append(dict(zip(columns, row)))
            return results
        except Exception as e:
            self.logger.error(f"Query failed: {e}")
            raise
    
    async def query_range(self, measurement: str, tags: Dict,
                         start: datetime, end: datetime,
                         fields: List[str] = None) -> List[Dict]:
        """範圍查詢"""
        # 構建 SQL 查詢
        field_list = ', '.join(fields) if fields else '*'
        sql = f"""
        SELECT ts, {field_list}
        FROM {measurement}
        WHERE ts >= '{start.isoformat()}'
          AND ts < '{end.isoformat()}'
        """
        
        # 添加 tag 過濾
        for tag_key, tag_value in tags.items():
            sql += f"  AND {tag_key} = '{tag_value}'\n"
        
        return await self.query(sql)
    
    async def aggregate(self, measurement: str, tags: Dict,
                       start: datetime, end: datetime,
                       aggregation: str, interval: str) -> List[Dict]:
        """聚合查詢"""
        # 映射聚合函數
        agg_func_map = {
            'mean': 'AVG',
            'sum': 'SUM',
            'count': 'COUNT',
            'min': 'MIN',
            'max': 'MAX'
        }
        agg_func = agg_func_map.get(aggregation, 'AVG')
        
        sql = f"""
        SELECT _wstart as window_start, {agg_func}(*) as value
        FROM {measurement}
        WHERE ts >= '{start.isoformat()}'
          AND ts < '{end.isoformat()}'
        """
        
        for tag_key, tag_value in tags.items():
            sql += f"  AND {tag_key} = '{tag_value}'\n"
        
        sql += f"INTERVAL({interval})"
        
        return await self.query(sql)
    
    def _format_value(self, value: Any) -> str:
        """格式化值"""
        if isinstance(value, str):
            return f"'{value}'"
        elif isinstance(value, bool):
            return 'TRUE' if value else 'FALSE'
        else:
            return str(value)
    
    def get_capabilities(self) -> Dict:
        """獲取能力"""
        return {
            'database': 'TDengine',
            'version': '3.x',
            'features': {
                'clustering': True,
                'streaming': True,
                'sql': True,
                'flux': False,
                'compression': True
            },
            'performance': {
                'write_throughput': '1.5M points/sec',
                'query_latency': 'low',
                'compression_ratio': '16:1'
            }
        }
    
    async def health_check(self) -> Dict:
        """健康檢查"""
        try:
            self.cursor.execute("SELECT SERVER_STATUS()")
            result = self.cursor.fetchone()
            return {
                'status': 'healthy' if result else 'unhealthy',
                'database': self.database
            }
        except Exception as e:
            return {
                'status': 'error',
                'message': str(e)
            }
```

### 2.3 TimescaleDB 適配器

```python
# timescaledb_adapter.py

import asyncpg
from typing import Dict, List, Any, Optional
from datetime import datetime
import logging

class TimescaledbAdapter(ITSDBAdapter):
    """TimescaleDB 適配器"""
    
    def __init__(self):
        self.pool: Optional[asyncpg.Pool] = None
        self.database = None
        self.logger = logging.getLogger(__name__)
    
    async def connect(self, config: Dict) -> bool:
        """連接 TimescaleDB"""
        try:
            self.pool = await asyncpg.create_pool(
                host=config['host'],
                port=config.get('port', 5432),
                user=config['user'],
                password=config['password'],
                database=config['database'],
                min_size=config.get('min_connections', 10),
                max_size=config.get('max_connections', 20)
            )
            self.database = config['database']
            
            # 測試連接
            async with self.pool.acquire() as conn:
                version = await conn.fetchval('SELECT version()')
                self.logger.info(f"Connected to TimescaleDB: {version}")
            
            return True
        except Exception as e:
            self.logger.error(f"Failed to connect to TimescaleDB: {e}")
            raise
    
    async def disconnect(self) -> bool:
        """斷開連接"""
        if self.pool:
            await self.pool.close()
        self.logger.info("Disconnected from TimescaleDB")
        return True
    
    async def write(self, measurement: str, tags: Dict[str, str],
                   fields: Dict[str, Any], timestamp: datetime) -> bool:
        """寫入單條數據"""
        try:
            # 構建 INSERT SQL
            all_columns = ['time'] + list(tags.keys()) + list(fields.keys())
            all_values = [timestamp] + list(tags.values()) + list(fields.values())
            
            placeholders = ', '.join([f'${i+1}' for i in range(len(all_values))])
            columns = ', '.join(all_columns)
            
            sql = f"INSERT INTO {measurement} ({columns}) VALUES ({placeholders})"
            
            async with self.pool.acquire() as conn:
                await conn.execute(sql, *all_values)
            
            return True
        except Exception as e:
            self.logger.error(f"Write failed: {e}")
            return False
    
    async def batch_write(self, points: List[Dict]) -> bool:
        """批量寫入"""
        try:
            # 按 measurement 分組
            grouped_points = {}
            for point in points:
                measurement = point['measurement']
                if measurement not in grouped_points:
                    grouped_points[measurement] = []
                grouped_points[measurement].append(point)
            
            async with self.pool.acquire() as conn:
                for measurement, points_list in grouped_points.items():
                    # 構建批量 INSERT
                    first_point = points_list[0]
                    all_columns = ['time'] + list(first_point['tags'].keys()) + list(first_point['fields'].keys())
                    columns = ', '.join(all_columns)
                    
                    values_list = []
                    for point in points_list:
                        values = [point['timestamp']] + list(point['tags'].values()) + list(point['fields'].values())
                        values_list.append(values)
                    
                    # 使用 COPY 進行批量插入(更快)
                    await conn.copy_records_to_table(
                        measurement,
                        columns=all_columns,
                        records=values_list
                    )
            
            return True
        except Exception as e:
            self.logger.error(f"Batch write failed: {e}")
            return False
    
    async def query(self, query: str, params: Dict = None) -> List[Dict]:
        """執行 SQL 查詢"""
        try:
            async with self.pool.acquire() as conn:
                if params:
                    rows = await conn.fetch(query, *params.values())
                else:
                    rows = await conn.fetch(query)
                
                results = [dict(row) for row in rows]
                return results
        except Exception as e:
            self.logger.error(f"Query failed: {e}")
            raise
    
    async def query_range(self, measurement: str, tags: Dict,
                         start: datetime, end: datetime,
                         fields: List[str] = None) -> List[Dict]:
        """範圍查詢"""
        field_list = ', '.join(fields) if fields else '*'
        sql = f"""
        SELECT time, {field_list}
        FROM {measurement}
        WHERE time >= $1 AND time < $2
        """
        
        # 添加 tag 過濾
        param_index = 3
        for tag_key, tag_value in tags.items():
            sql += f"  AND {tag_key} = ${param_index}\n"
            param_index += 1
        
        sql += "ORDER BY time"
        
        params = [start, end] + list(tags.values())
        
        async with self.pool.acquire() as conn:
            rows = await conn.fetch(sql, *params)
            return [dict(row) for row in rows]
    
    async def aggregate(self, measurement: str, tags: Dict,
                       start: datetime, end: datetime,
                       aggregation: str, interval: str) -> List[Dict]:
        """聚合查詢"""
        # 使用 TimescaleDB 的 time_bucket 函數
        sql = f"""
        SELECT time_bucket('{interval}', time) as bucket,
               {aggregation}(*) as value
        FROM {measurement}
        WHERE time >= $1 AND time < $2
        """
        
        param_index = 3
        for tag_key, tag_value in tags.items():
            sql += f"  AND {tag_key} = ${param_index}\n"
            param_index += 1
        
        sql += "GROUP BY bucket ORDER BY bucket"
        
        params = [start, end] + list(tags.values())
        
        async with self.pool.acquire() as conn:
            rows = await conn.fetch(sql, *params)
            return [dict(row) for row in rows]
    
    def get_capabilities(self) -> Dict:
        """獲取能力"""
        return {
            'database': 'TimescaleDB',
            'version': '2.x',
            'features': {
                'clustering': True,  # 企業版
                'streaming': False,
                'sql': True,
                'flux': False,
                'compression': True
            },
            'performance': {
                'write_throughput': '600K points/sec',
                'query_latency': 'medium',
                'compression_ratio': '10:1'
            }
        }
    
    async def health_check(self) -> Dict:
        """健康檢查"""
        try:
            async with self.pool.acquire() as conn:
                result = await conn.fetchval('SELECT 1')
                return {
                    'status': 'healthy' if result == 1 else 'unhealthy',
                    'database': self.database
                }
        except Exception as e:
            return {
                'status': 'error',
                'message': str(e)
            }
```

### 2.4 QuestDB 適配器

```python
# questdb_adapter.py

import asyncpg
from typing import Dict, List, Any, Optional
from datetime import datetime
import logging

class QuestdbAdapter(ITSDBAdapter):
    """QuestDB 適配器"""
    
    def __init__(self):
        self.pool: Optional[asyncpg.Pool] = None
        self.logger = logging.getLogger(__name__)
    
    async def connect(self, config: Dict) -> bool:
        """連接 QuestDB (使用 PostgreSQL 協議)"""
        try:
            self.pool = await asyncpg.create_pool(
                host=config['host'],
                port=config.get('port', 8812),  # QuestDB PostgreSQL port
                user=config.get('user', 'admin'),
                password=config.get('password', 'quest'),
                database=config.get('database', 'qdb'),
                min_size=config.get('min_connections', 10),
                max_size=config.get('max_connections', 20)
            )
            
            # 測試連接
            async with self.pool.acquire() as conn:
                version = await conn.fetchval('SELECT version()')
                self.logger.info(f"Connected to QuestDB: {version}")
            
            return True
        except Exception as e:
            self.logger.error(f"Failed to connect to QuestDB: {e}")
            raise
    
    async def disconnect(self) -> bool:
        """斷開連接"""
        if self.pool:
            await self.pool.close()
        self.logger.info("Disconnected from QuestDB")
        return True
    
    async def write(self, measurement: str, tags: Dict[str, str],
                   fields: Dict[str, Any], timestamp: datetime) -> bool:
        """寫入單條數據"""
        try:
            all_columns = ['timestamp'] + list(tags.keys()) + list(fields.keys())
            all_values = [timestamp] + list(tags.values()) + list(fields.values())
            
            placeholders = ', '.join([f'${i+1}' for i in range(len(all_values))])
            columns = ', '.join(all_columns)
            
            sql = f"INSERT INTO {measurement} ({columns}) VALUES ({placeholders})"
            
            async with self.pool.acquire() as conn:
                await conn.execute(sql, *all_values)
            
            return True
        except Exception as e:
            self.logger.error(f"Write failed: {e}")
            return False
    
    async def batch_write(self, points: List[Dict]) -> bool:
        """批量寫入"""
        # QuestDB 實現類似 TimescaleDB
        try:
            grouped_points = {}
            for point in points:
                measurement = point['measurement']
                if measurement not in grouped_points:
                    grouped_points[measurement] = []
                grouped_points[measurement].append(point)
            
            async with self.pool.acquire() as conn:
                for measurement, points_list in grouped_points.items():
                    first_point = points_list[0]
                    all_columns = ['timestamp'] + list(first_point['tags'].keys()) + list(first_point['fields'].keys())
                    
                    values_list = []
                    for point in points_list:
                        values = [point['timestamp']] + list(point['tags'].values()) + list(point['fields'].values())
                        values_list.append(values)
                    
                    await conn.copy_records_to_table(
                        measurement,
                        columns=all_columns,
                        records=values_list
                    )
            
            return True
        except Exception as e:
            self.logger.error(f"Batch write failed: {e}")
            return False
    
    async def query(self, query: str, params: Dict = None) -> List[Dict]:
        """執行 SQL 查詢"""
        try:
            async with self.pool.acquire() as conn:
                if params:
                    rows = await conn.fetch(query, *params.values())
                else:
                    rows = await conn.fetch(query)
                
                results = [dict(row) for row in rows]
                return results
        except Exception as e:
            self.logger.error(f"Query failed: {e}")
            raise
    
    def get_capabilities(self) -> Dict:
        """獲取能力"""
        return {
            'database': 'QuestDB',
            'version': '7.x',
            'features': {
                'clustering': False,  # 主從複製
                'streaming': False,
                'sql': True,
                'flux': False,
                'compression': True
            },
            'performance': {
                'write_throughput': '1.2M points/sec',
                'query_latency': 'low',
                'compression_ratio': '12:1'
            }
        }
    
    async def health_check(self) -> Dict:
        """健康檢查"""
        try:
            async with self.pool.acquire() as conn:
                result = await conn.fetchval('SELECT 1')
                return {
                    'status': 'healthy' if result == 1 else 'unhealthy'
                }
        except Exception as e:
            return {
                'status': 'error',
                'message': str(e)
            }
```

---

## 3. 配置管理

### 3.1 配置文件格式

```yaml
# ndh_config.yaml

tsdb:
  # 主數據庫配置
  primary:
    type: timescaledb  # influxdb, tdengine, timescaledb, questdb
    enabled: true
    config:
      host: timescaledb-primary.ndh.svc.cluster.local
      port: 5432
      user: ndh_user
      password: ${TIMESCALEDB_PASSWORD}
      database: ndh
      min_connections: 10
      max_connections: 20
  
  # 次要數據庫配置(用於雙寫)
  secondary:
    - type: tdengine
      enabled: false  # 可選的雙寫
      config:
        host: tdengine-0.tdengine.ndh.svc.cluster.local
        port: 6030
        user: root
        password: ${TDENGINE_PASSWORD}
        database: ndh
  
  # 適配器配置
  adapters:
    influxdb:
      module: ndh.adapters.influxdb_adapter
      class: InfluxdbAdapter
    tdengine:
      module: ndh.adapters.tdengine_adapter
      class: TdengineAdapter
    timescaledb:
      module: ndh.adapters.timescaledb_adapter
      class: TimescaledbAdapter
    questdb:
      module: ndh.adapters.questdb_adapter
      class: QuestdbAdapter
  
  # 雙寫配置
  dual_write:
    enabled: false
    async: true  # 非阻塞寫入次要數據庫
    retry: 3
    timeout: 5  # seconds
  
  # 切換配置
  switchover:
    enabled: true
    validation: true  # 切換前驗證數據一致性
    rollback: true  # 支援回滾
```

### 3.2 配置載入器

```python
# config_loader.py

import yaml
import os
from typing import Dict

class TSDBConfigLoader:
    """時序數據庫配置載入器"""
    
    @staticmethod
    def load_config(config_path: str) -> Dict:
        """載入配置文件"""
        with open(config_path, 'r') as f:
            config = yaml.safe_load(f)
        
        # 替換環境變數
        config = TSDBConfigLoader._replace_env_vars(config)
        
        return config['tsdb']
    
    @staticmethod
    def _replace_env_vars(config: Dict) -> Dict:
        """替換配置中的環境變數"""
        if isinstance(config, dict):
            return {k: TSDBConfigLoader._replace_env_vars(v) for k, v in config.items()}
        elif isinstance(config, list):
            return [TSDBConfigLoader._replace_env_vars(item) for item in config]
        elif isinstance(config, str) and config.startswith('${') and config.endswith('}'):
            env_var = config[2:-1]
            return os.getenv(env_var, config)
        else:
            return config
```

---

## 4. 在線遷移方案

### 4.1 遷移流程

```
階段 1: 準備階段
  ├── 部署新數據庫
  ├── 配置雙寫
  └── 驗證連接

階段 2: 雙寫階段
  ├── 啟用雙寫(舊數據庫 + 新數據庫)
  ├── 監控數據一致性
  └── 遷移歷史數據

階段 3: 驗證階段
  ├── 對比數據
  ├── 性能測試
  └── 切換讀取到新數據庫

階段 4: 切換階段
  ├── 切換主數據庫
  ├── 停止寫入舊數據庫
  └── 監控穩定性

階段 5: 清理階段
  ├── 保留舊數據庫 N 天
  ├── 確認無問題
  └── 下線舊數據庫
```

### 4.2 遷移工具

```python
# migration_tool.py

from typing import Dict, List
from datetime import datetime, timedelta
import asyncio
import logging

class TSDBMigrationTool:
    """時序數據庫遷移工具"""
    
    def __init__(self, adapter_manager: TSDBAdapterManager):
        self.adapter_manager = adapter_manager
        self.logger = logging.getLogger(__name__)
    
    async def enable_dual_write(self, target_adapter_type: str):
        """啟用雙寫"""
        self.logger.info(f"Enabling dual write to {target_adapter_type}")
        
        # 載入目標適配器
        if target_adapter_type not in self.adapter_manager.adapters:
            await self.adapter_manager._load_adapter(
                target_adapter_type,
                self.adapter_manager.config['secondary'][0]
            )
        
        # 添加到次要適配器列表
        target_adapter = self.adapter_manager.adapters[target_adapter_type]
        if target_adapter not in self.adapter_manager.secondary_adapters:
            self.adapter_manager.secondary_adapters.append(target_adapter)
        
        self.logger.info("Dual write enabled")
    
    async def migrate_historical_data(self, 
                                     source_adapter: ITSDBAdapter,
                                     target_adapter: ITSDBAdapter,
                                     start_date: datetime,
                                     end_date: datetime,
                                     measurements: List[str],
                                     batch_size: int = 10000):
        """遷移歷史數據"""
        self.logger.info(f"Starting historical data migration from {start_date} to {end_date}")
        
        total_points = 0
        for measurement in measurements:
            self.logger.info(f"Migrating measurement: {measurement}")
            
            current_date = start_date
            while current_date < end_date:
                batch_end = min(current_date + timedelta(hours=1), end_date)
                
                # 從源數據庫讀取
                data = await source_adapter.query_range(
                    measurement=measurement,
                    tags={},
                    start=current_date,
                    end=batch_end
                )
                
                if data:
                    # 轉換為批量寫入格式
                    points = []
                    for row in data:
                        point = {
                            'measurement': measurement,
                            'timestamp': row['time'],
                            'tags': {k: v for k, v in row.items() if k.startswith('tag_')},
                            'fields': {k: v for k, v in row.items() if k.startswith('field_')}
                        }
                        points.append(point)
                    
                    # 寫入目標數據庫
                    await target_adapter.batch_write(points)
                    total_points += len(points)
                    
                    self.logger.info(f"Migrated {len(points)} points, total: {total_points}")
                
                current_date = batch_end
                
                # 避免過載
                await asyncio.sleep(0.1)
        
        self.logger.info(f"Migration completed. Total points migrated: {total_points}")
        return total_points
    
    async def validate_data_consistency(self,
                                       source_adapter: ITSDBAdapter,
                                       target_adapter: ITSDBAdapter,
                                       measurement: str,
                                       start: datetime,
                                       end: datetime) -> Dict:
        """驗證數據一致性"""
        self.logger.info(f"Validating data consistency for {measurement}")
        
        # 從兩個數據庫讀取相同範圍的數據
        source_data = await source_adapter.query_range(
            measurement=measurement,
            tags={},
            start=start,
            end=end
        )
        
        target_data = await target_adapter.query_range(
            measurement=measurement,
            tags={},
            start=start,
            end=end
        )
        
        # 比較數據量
        source_count = len(source_data)
        target_count = len(target_data)
        
        consistency_ratio = target_count / source_count if source_count > 0 else 0
        
        result = {
            'measurement': measurement,
            'source_count': source_count,
            'target_count': target_count,
            'consistency_ratio': consistency_ratio,
            'status': 'pass' if consistency_ratio >= 0.99 else 'fail'
        }
        
        self.logger.info(f"Validation result: {result}")
        return result
    
    async def switchover(self, new_primary_type: str):
        """切換主數據庫"""
        self.logger.info(f"Starting switchover to {new_primary_type}")
        
        # 驗證新數據庫健康狀態
        new_adapter = self.adapter_manager.adapters[new_primary_type]
        health = await new_adapter.health_check()
        
        if health['status'] != 'healthy':
            raise Exception(f"New primary database is not healthy: {health}")
        
        # 執行切換
        await self.adapter_manager.switch_primary(new_primary_type)
        
        self.logger.info("Switchover completed successfully")
    
    async def disable_dual_write(self, adapter_type: str):
        """停止雙寫"""
        self.logger.info(f"Disabling dual write to {adapter_type}")
        
        adapter = self.adapter_manager.adapters[adapter_type]
        if adapter in self.adapter_manager.secondary_adapters:
            self.adapter_manager.secondary_adapters.remove(adapter)
        
        self.logger.info("Dual write disabled")
```

---

## 5. API 層整合

### 5.1 RESTful API

```python
# api.py

from fastapi import FastAPI, HTTPException, Depends
from pydantic import BaseModel
from typing import List, Dict, Optional
from datetime import datetime

app = FastAPI(title="NDH Time Series API")

# 全局適配器管理器
adapter_manager: Optional[TSDBAdapterManager] = None

class WriteRequest(BaseModel):
    measurement: str
    tags: Dict[str, str]
    fields: Dict[str, float]
    timestamp: Optional[datetime] = None

class QueryRequest(BaseModel):
    measurement: str
    tags: Dict[str, str] = {}
    start: datetime
    end: datetime
    fields: Optional[List[str]] = None

class AggregateRequest(BaseModel):
    measurement: str
    tags: Dict[str, str] = {}
    start: datetime
    end: datetime
    aggregation: str = "mean"
    interval: str = "1h"

@app.on_event("startup")
async def startup():
    """啟動時初始化適配器管理器"""
    global adapter_manager
    config = TSDBConfigLoader.load_config("ndh_config.yaml")
    adapter_manager = TSDBAdapterManager(config)
    await adapter_manager.initialize()

@app.post("/api/v1/timeseries/write")
async def write_data(request: WriteRequest):
    """寫入時序數據"""
    timestamp = request.timestamp or datetime.utcnow()
    
    success = await adapter_manager.write(
        measurement=request.measurement,
        tags=request.tags,
        fields=request.fields,
        timestamp=timestamp
    )
    
    if success:
        return {"status": "success"}
    else:
        raise HTTPException(status_code=500, detail="Write failed")

@app.post("/api/v1/timeseries/query")
async def query_data(request: QueryRequest):
    """查詢時序數據"""
    try:
        data = await adapter_manager.primary_adapter.query_range(
            measurement=request.measurement,
            tags=request.tags,
            start=request.start,
            end=request.end,
            fields=request.fields
        )
        return {"data": data}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/api/v1/timeseries/aggregate")
async def aggregate_data(request: AggregateRequest):
    """聚合查詢"""
    try:
        data = await adapter_manager.primary_adapter.aggregate(
            measurement=request.measurement,
            tags=request.tags,
            start=request.start,
            end=request.end,
            aggregation=request.aggregation,
            interval=request.interval
        )
        return {"data": data}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/api/v1/timeseries/health")
async def health_check():
    """健康檢查"""
    health = await adapter_manager.health_check()
    return health

@app.get("/api/v1/timeseries/capabilities")
async def get_capabilities():
    """獲取當前數據庫能力"""
    capabilities = adapter_manager.primary_adapter.get_capabilities()
    return capabilities

@app.post("/api/v1/admin/switchover")
async def switchover(new_primary: str):
    """切換主數據庫"""
    try:
        migration_tool = TSDBMigrationTool(adapter_manager)
        await migration_tool.switchover(new_primary)
        return {"status": "success", "new_primary": new_primary}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))
```

---

## 6. 部署配置

### 6.1 Kubernetes 部署

```yaml
# ndh-api-deployment.yaml

apiVersion: apps/v1
kind: Deployment
metadata:
  name: ndh-api
  namespace: ndh
spec:
  replicas: 3
  selector:
    matchLabels:
      app: ndh-api
  template:
    metadata:
      labels:
        app: ndh-api
    spec:
      containers:
      - name: ndh-api
        image: ndh/api:latest
        ports:
        - containerPort: 8000
        env:
        - name: TIMESCALEDB_PASSWORD
          valueFrom:
            secretKeyRef:
              name: ndh-secrets
              key: timescaledb-password
        - name: TDENGINE_PASSWORD
          valueFrom:
            secretKeyRef:
              name: ndh-secrets
              key: tdengine-password
        volumeMounts:
        - name: config
          mountPath: /app/config
          readOnly: true
        resources:
          requests:
            memory: "512Mi"
            cpu: "500m"
          limits:
            memory: "1Gi"
            cpu: "1000m"
      volumes:
      - name: config
        configMap:
          name: ndh-config
---
apiVersion: v1
kind: ConfigMap
metadata:
  name: ndh-config
  namespace: ndh
data:
  ndh_config.yaml: |
    tsdb:
      primary:
        type: timescaledb
        enabled: true
        config:
          host: timescaledb-primary.ndh.svc.cluster.local
          port: 5432
          user: ndh_user
          password: ${TIMESCALEDB_PASSWORD}
          database: ndh
      secondary:
        - type: tdengine
          enabled: false
          config:
            host: tdengine-0.tdengine.ndh.svc.cluster.local
            port: 6030
            user: root
            password: ${TDENGINE_PASSWORD}
            database: ndh
```

---

## 7. 測試與驗證

### 7.1 單元測試

```python
# test_adapters.py

import pytest
from datetime import datetime
import asyncio

@pytest.mark.asyncio
async def test_influxdb_adapter():
    """測試 InfluxDB 適配器"""
    adapter = InfluxdbAdapter()
    
    config = {
        'url': 'http://localhost:8086',
        'token': 'test-token',
        'org': 'test-org',
        'bucket': 'test-bucket'
    }
    
    # 連接
    await adapter.connect(config)
    
    # 寫入
    success = await adapter.write(
        measurement='test',
        tags={'location': 'zone_a'},
        fields={'temperature': 25.5},
        timestamp=datetime.utcnow()
    )
    assert success
    
    # 查詢
    data = await adapter.query_range(
        measurement='test',
        tags={'location': 'zone_a'},
        start=datetime.utcnow() - timedelta(hours=1),
        end=datetime.utcnow()
    )
    assert len(data) > 0
    
    # 斷開
    await adapter.disconnect()

@pytest.mark.asyncio
async def test_adapter_manager():
    """測試適配器管理器"""
    config = {
        'primary': {
            'type': 'influxdb',
            'config': {...}
        }
    }
    
    manager = TSDBAdapterManager(config)
    await manager.initialize()
    
    # 寫入
    success = await manager.write(
        measurement='test',
        tags={'location': 'zone_a'},
        fields={'temperature': 25.5},
        timestamp=datetime.utcnow()
    )
    assert success
    
    # 健康檢查
    health = await manager.health_check()
    assert health['primary']['status'] == 'healthy'
```

### 7.2 性能測試

```python
# benchmark.py

import asyncio
import time
from datetime import datetime
import random

async def benchmark_write(adapter: ITSDBAdapter, num_points: int):
    """寫入性能測試"""
    start_time = time.time()
    
    points = []
    for i in range(num_points):
        point = {
            'measurement': 'benchmark',
            'timestamp': datetime.utcnow(),
            'tags': {
                'equipment_id': f'EQ_{i % 100}',
                'location': f'Zone_{i % 10}'
            },
            'fields': {
                'temperature': random.uniform(20, 80),
                'pressure': random.uniform(1, 10),
                'throughput': random.uniform(1000, 15000)
            }
        }
        points.append(point)
    
    await adapter.batch_write(points)
    
    end_time = time.time()
    duration = end_time - start_time
    throughput = num_points / duration
    
    print(f"Write Performance:")
    print(f"  Points: {num_points}")
    print(f"  Duration: {duration:.2f}s")
    print(f"  Throughput: {throughput:.0f} points/sec")

async def benchmark_query(adapter: ITSDBAdapter):
    """查詢性能測試"""
    start_time = time.time()
    
    data = await adapter.query_range(
        measurement='benchmark',
        tags={'equipment_id': 'EQ_1'},
        start=datetime.utcnow() - timedelta(hours=24),
        end=datetime.utcnow()
    )
    
    end_time = time.time()
    duration = end_time - start_time
    
    print(f"Query Performance:")
    print(f"  Points returned: {len(data)}")
    print(f"  Duration: {duration:.2f}s")
```

---

## 8. 最佳實踐

### 8.1 選擇適配器的建議

| 場域 | 推薦數據庫 | 理由 |
|------|-----------|------|
| **高敏感** (台積電) | TimescaleDB | 供應鏈風險最低,符合客戶要求 |
| **中敏感** (一般製造業) | QuestDB | 性能優異,成本低,風險可接受 |
| **低敏感** (非關鍵應用) | TDengine | 性能最佳,成本最低 |
| **開發測試** | InfluxDB | 生態成熟,易於使用 |

### 8.2 遷移建議

1. **充分測試**: 在測試環境完整驗證
2. **逐步遷移**: 先雙寫,再切換讀取,最後切換寫入
3. **監控告警**: 實時監控數據一致性和性能
4. **回滾計畫**: 準備快速回滾方案
5. **保留舊數據**: 至少保留 30 天作為備份

### 8.3 性能優化

1. **批量寫入**: 使用 batch_write 而非單條寫入
2. **連接池**: 使用連接池管理數據庫連接
3. **異步 I/O**: 使用 asyncio 提高並發性能
4. **適當索引**: 為常用查詢字段創建索引
5. **數據分區**: 使用時間分區優化查詢

---

## 9. 總結

### 9.1 核心優勢

✅ **靈活性**: 根據場域需求選擇最適合的數據庫  
✅ **風險管理**: 降低供應鏈風險,避免廠商鎖定  
✅ **平滑遷移**: 支援零停機在線遷移  
✅ **成本優化**: 在性能、成本、安全之間靈活平衡  
✅ **可擴展**: 輕鬆添加新的數據庫適配器

### 9.2 實施建議

1. **立即**: 實現抽象層和適配器管理器
2. **短期**: 實現 InfluxDB 和 TimescaleDB 適配器
3. **中期**: 實現 TDengine 和 QuestDB 適配器
4. **長期**: 支援更多數據庫,完善遷移工具

### 9.3 預期效益

- 🎯 **降低風險**: 不依賴單一數據庫供應商
- 🎯 **提高靈活性**: 根據需求快速切換
- 🎯 **優化成本**: 選擇性價比最高的方案
- 🎯 **保證安全**: 在敏感場域使用安全的數據庫

---

**文檔結束**

**作者**: 林志錚 (Chih Cheng Lin, Michael Lin)  
**Email**: chchlin1018@gmail.com  
**版本**: 1.0  
**日期**: 2025 年 10 月 12 日

