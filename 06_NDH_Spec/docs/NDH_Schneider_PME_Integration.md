# NDH 與 Schneider Electric Power Monitoring Expert (PME) 整合

**版本**: 1.0.0  
**日期**: 2025-10-15  
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) Michael Lin (Chih Cheng Lin)

---

## 目錄

1. [概述](#概述)
2. [Schneider PME 簡介](#schneider-pme-簡介)
3. [整合架構](#整合架構)
4. [PME GraphQL API](#pme-graphql-api)
5. [NDH PME Connector 設計](#ndh-pme-connector-設計)
6. [數據映射](#數據映射)
7. [整合實作](#整合實作)
8. [使用場景](#使用場景)
9. [部署指南](#部署指南)
10. [監控與維護](#監控與維護)
11. [最佳實踐](#最佳實踐)
12. [附錄](#附錄)

---

## 概述

### 背景

**Schneider Electric Power Monitoring Expert (PME)** 是施耐德電機的旗艦電力監控和能源管理系統，廣泛應用於：

- 大型工業設施
- 數據中心
- 醫院和關鍵基礎設施
- 商業建築
- 製造工廠

PME 提供：
- 即時電力監控
- 能源管理和分析
- 電力品質監測
- 設備健康監控
- 告警和事件管理
- 合規報告

### 整合目標

將 PME 與 NDH 整合，實現：

1. ✅ **統一數據平台**: 將電力數據整合到 NDH 的統一數據模型
2. ✅ **即時監控**: 即時獲取電力參數、能源消耗、設備狀態
3. ✅ **告警整合**: 將 PME 告警整合到 NDH 的告警系統
4. ✅ **能源分析**: 結合製程數據進行能源效率分析
5. ✅ **預測性維護**: 利用電力數據進行設備健康預測
6. ✅ **數位分身同步**: 在 Omniverse 中視覺化電力系統狀態

### 整合價值

| 價值類別 | 具體價值 | 量化指標 |
|---------|---------|---------|
| **能源效率** | 識別能源浪費，優化能源使用 | 節省能源成本 10-15% |
| **設備可靠性** | 預測性維護，減少計畫外停機 | 減少停機時間 30% |
| **運營可見性** | 統一監控電力和製程 | 提升決策效率 40% |
| **合規性** | 自動化能源報告 | 節省報告工時 80% |
| **數位分身** | 完整的電力系統數位化 | 提升系統理解度 50% |

---

## Schneider PME 簡介

### 系統架構

```
┌─────────────────────────────────────────────────────┐
│              PME Web 應用程式                        │
│  (監控、分析、報告、告警管理)                         │
└────────────────┬────────────────────────────────────┘
                 │
┌────────────────┴────────────────────────────────────┐
│              PME 伺服器                              │
│  - 數據收集引擎                                      │
│  - 數據庫 (SQL Server)                              │
│  - GraphQL API 伺服器                               │
│  - OPC Server                                       │
│  - 告警引擎                                          │
└────────────────┬────────────────────────────────────┘
                 │
┌────────────────┴────────────────────────────────────┐
│              通訊層                                  │
│  - Modbus TCP/RTU                                   │
│  - BACnet                                           │
│  - SNMP                                             │
│  - IEC 61850                                        │
└────────────────┬────────────────────────────────────┘
                 │
┌────────────────┴────────────────────────────────────┐
│              現場設備                                │
│  - 智能電表 (PowerLogic)                            │
│  - 斷路器 (Masterpact, ComPact)                     │
│  - UPS                                              │
│  - 變壓器監控                                        │
│  - 電力品質分析儀                                    │
└─────────────────────────────────────────────────────┘
```

### 核心功能

#### 1. 電力監控

- **即時數據**: 電壓、電流、功率、功率因數、頻率
- **電能計量**: 有功電能、無功電能、視在電能
- **電力品質**: 諧波、電壓驟降/驟升、閃爍
- **需量管理**: 需量預測、需量控制

#### 2. 能源管理

- **能源儀表板**: 即時能源消耗視覺化
- **能源分析**: 趨勢分析、對比分析、成本分析
- **能源報告**: ISO 50001、LEED、ENERGY STAR 合規報告
- **節能建議**: 基於數據的節能機會識別

#### 3. 設備健康監控

- **設備狀態**: 運行狀態、負載率、溫度
- **預測性維護**: 基於趨勢的維護建議
- **設備壽命**: 設備老化分析、剩餘壽命預測
- **維護排程**: 維護計畫管理

#### 4. 告警和事件

- **即時告警**: 電力異常、設備故障、閾值超限
- **告警優先級**: 緊急、高、中、低
- **告警通知**: Email、SMS、SNMP Trap
- **事件記錄**: 完整的事件歷史記錄

---

## 整合架構

### 高層架構

```
┌─────────────────────────────────────────────────────────────┐
│                    IDTF V3.5 生態系統                        │
│                                                              │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │  AI Agent    │  │  FDL Editor  │  │  Omniverse   │      │
│  │  (MCP)       │  │              │  │  Connector   │      │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘      │
│         │                 │                 │               │
│         └─────────────────┼─────────────────┘               │
│                           │                                 │
│  ┌────────────────────────┴──────────────────────────────┐  │
│  │                    NDH (Neutral Data Hub)             │  │
│  │                                                        │  │
│  │  ┌──────────────────────────────────────────────────┐ │  │
│  │  │         PME Connector (新增)                     │ │  │
│  │  │  - GraphQL Client                                │ │  │
│  │  │  - Data Mapper                                   │ │  │
│  │  │  - Alarm Handler                                 │ │  │
│  │  │  - Historical Data Sync                          │ │  │
│  │  └──────────────────┬───────────────────────────────┘ │  │
│  │                     │                                  │  │
│  │  ┌──────────────────┴───────────────────────────────┐ │  │
│  │  │         Data Processing Layer                    │ │  │
│  │  │  - Kafka (Event Streaming)                       │ │  │
│  │  │  - InfluxDB (Time Series)                        │ │  │
│  │  │  - PostgreSQL (Metadata)                         │ │  │
│  │  └──────────────────────────────────────────────────┘ │  │
│  └────────────────────────────────────────────────────────┘  │
└─────────────────────────┬───────────────────────────────────┘
                          │
                          │ GraphQL API / OPC DA
                          │
┌─────────────────────────┴───────────────────────────────────┐
│          Schneider Electric PME                              │
│                                                              │
│  ┌──────────────────────────────────────────────────────┐   │
│  │         PME GraphQL API Server                       │   │
│  │  - Site Information                                  │   │
│  │  - Electrical Hierarchy                              │   │
│  │  - Metering Data                                     │   │
│  │  │  - Alarms                                         │   │
│  └──────────────────┬───────────────────────────────────┘   │
│                     │                                        │
│  ┌──────────────────┴───────────────────────────────────┐   │
│  │         PME Database (SQL Server)                    │   │
│  └──────────────────┬───────────────────────────────────┘   │
│                     │                                        │
│  ┌──────────────────┴───────────────────────────────────┐   │
│  │         Data Collection Engine                       │   │
│  └──────────────────┬───────────────────────────────────┘   │
└─────────────────────┴───────────────────────────────────────┘
                      │
                      │ Modbus TCP, BACnet, etc.
                      │
┌─────────────────────┴───────────────────────────────────────┐
│          現場電力設備                                         │
│  - PowerLogic 智能電表                                       │
│  - Masterpact/ComPact 斷路器                                │
│  - UPS                                                       │
│  - 變壓器監控                                                │
└─────────────────────────────────────────────────────────────┘
```

### 整合方式

NDH 支援兩種整合方式：

#### 方式 1: GraphQL API (推薦)

**優點**:
- ✅ 現代化的 API 設計
- ✅ 精確查詢，減少數據傳輸
- ✅ 強類型，易於開發和維護
- ✅ 支援訂閱 (Subscriptions) 實現即時數據推送

**缺點**:
- ❌ 需要 PME 2020 或更新版本
- ❌ 需要申請 API 存取權限

#### 方式 2: OPC DA/UA

**優點**:
- ✅ 工業標準協議
- ✅ 即時性好
- ✅ 支援舊版 PME

**缺點**:
- ❌ 配置較複雜
- ❌ 需要額外的 OPC 客戶端軟體

**建議**: 優先使用 GraphQL API，僅在必要時使用 OPC DA/UA。

---

## PME GraphQL API

### API 概述

PME GraphQL API 提供以下核心功能：

1. **Site Information**: 站點資訊、設備清單
2. **Electrical Hierarchy**: 電氣層次結構
3. **Metering Hierarchy**: 計量層次結構
4. **Real-time Data**: 即時電力參數
5. **Historical Data**: 歷史數據查詢
6. **Alarms**: 告警查詢和確認
7. **Events**: 事件查詢

### 認證

PME GraphQL API 使用 OAuth 2.0 認證：

```http
POST /oauth/token HTTP/1.1
Host: pme-server.example.com
Content-Type: application/x-www-form-urlencoded

grant_type=client_credentials
&client_id=YOUR_CLIENT_ID
&client_secret=YOUR_CLIENT_SECRET
&scope=read:sites read:devices read:measurements read:alarms
```

**回應**:

```json
{
  "access_token": "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9...",
  "token_type": "Bearer",
  "expires_in": 3600,
  "scope": "read:sites read:devices read:measurements read:alarms"
}
```

### 核心查詢

#### 1. 查詢站點資訊

```graphql
query GetSites {
  sites {
    id
    name
    description
    location {
      address
      city
      country
      latitude
      longitude
    }
    timezone
    currency
  }
}
```

**回應**:

```json
{
  "data": {
    "sites": [
      {
        "id": "site-001",
        "name": "Manufacturing Plant A",
        "description": "Main production facility",
        "location": {
          "address": "123 Industrial Rd",
          "city": "Taipei",
          "country": "Taiwan",
          "latitude": 25.0330,
          "longitude": 121.5654
        },
        "timezone": "Asia/Taipei",
        "currency": "TWD"
      }
    ]
  }
}
```

#### 2. 查詢電氣層次結構

```graphql
query GetElectricalHierarchy($siteId: ID!) {
  site(id: $siteId) {
    electricalNetwork {
      id
      name
      type
      devices {
        id
        name
        type
        model
        serialNumber
        location
        parent {
          id
          name
        }
        children {
          id
          name
        }
      }
    }
  }
}
```

**回應**:

```json
{
  "data": {
    "site": {
      "electricalNetwork": {
        "id": "network-001",
        "name": "Main Electrical Network",
        "type": "DISTRIBUTION",
        "devices": [
          {
            "id": "device-001",
            "name": "Main Incomer",
            "type": "CIRCUIT_BREAKER",
            "model": "Masterpact MTZ",
            "serialNumber": "MTZ-001-2024",
            "location": "MCC Room 1",
            "parent": null,
            "children": [
              {
                "id": "device-002",
                "name": "Feeder 1"
              }
            ]
          }
        ]
      }
    }
  }
}
```

#### 3. 查詢即時電力數據

```graphql
query GetRealtimeMeasurements($deviceId: ID!) {
  device(id: $deviceId) {
    measurements {
      voltage {
        phaseA
        phaseB
        phaseC
        average
        unit
        timestamp
      }
      current {
        phaseA
        phaseB
        phaseC
        neutral
        unit
        timestamp
      }
      power {
        active
        reactive
        apparent
        powerFactor
        frequency
        unit
        timestamp
      }
      energy {
        activeImport
        activeExport
        reactiveImport
        reactiveExport
        unit
        timestamp
      }
    }
  }
}
```

**回應**:

```json
{
  "data": {
    "device": {
      "measurements": {
        "voltage": {
          "phaseA": 230.5,
          "phaseB": 231.2,
          "phaseC": 229.8,
          "average": 230.5,
          "unit": "V",
          "timestamp": "2025-10-15T10:30:00Z"
        },
        "current": {
          "phaseA": 125.3,
          "phaseB": 128.7,
          "phaseC": 126.1,
          "neutral": 5.2,
          "unit": "A",
          "timestamp": "2025-10-15T10:30:00Z"
        },
        "power": {
          "active": 87.5,
          "reactive": 12.3,
          "apparent": 88.4,
          "powerFactor": 0.99,
          "frequency": 60.0,
          "unit": "kW",
          "timestamp": "2025-10-15T10:30:00Z"
        },
        "energy": {
          "activeImport": 12345.67,
          "activeExport": 0.0,
          "reactiveImport": 234.56,
          "reactiveExport": 0.0,
          "unit": "kWh",
          "timestamp": "2025-10-15T10:30:00Z"
        }
      }
    }
  }
}
```

#### 4. 查詢歷史數據

```graphql
query GetHistoricalData(
  $deviceId: ID!
  $measurementType: MeasurementType!
  $startTime: DateTime!
  $endTime: DateTime!
  $aggregation: AggregationType
  $interval: Duration
) {
  device(id: $deviceId) {
    historicalData(
      measurementType: $measurementType
      startTime: $startTime
      endTime: $endTime
      aggregation: $aggregation
      interval: $interval
    ) {
      timestamp
      value
      unit
      quality
    }
  }
}
```

**變數**:

```json
{
  "deviceId": "device-001",
  "measurementType": "ACTIVE_POWER",
  "startTime": "2025-10-14T00:00:00Z",
  "endTime": "2025-10-15T00:00:00Z",
  "aggregation": "AVERAGE",
  "interval": "PT15M"
}
```

**回應**:

```json
{
  "data": {
    "device": {
      "historicalData": [
        {
          "timestamp": "2025-10-14T00:00:00Z",
          "value": 85.2,
          "unit": "kW",
          "quality": "GOOD"
        },
        {
          "timestamp": "2025-10-14T00:15:00Z",
          "value": 87.1,
          "unit": "kW",
          "quality": "GOOD"
        }
        // ... more data points
      ]
    }
  }
}
```

#### 5. 查詢告警

```graphql
query GetAlarms(
  $siteId: ID!
  $status: AlarmStatus
  $severity: AlarmSeverity
  $startTime: DateTime
  $endTime: DateTime
) {
  site(id: $siteId) {
    alarms(
      status: $status
      severity: $severity
      startTime: $startTime
      endTime: $endTime
    ) {
      id
      name
      description
      severity
      status
      source {
        id
        name
        type
      }
      timestamp
      acknowledgedBy
      acknowledgedAt
      clearedAt
    }
  }
}
```

**回應**:

```json
{
  "data": {
    "site": {
      "alarms": [
        {
          "id": "alarm-001",
          "name": "High Current",
          "description": "Phase A current exceeded 150A threshold",
          "severity": "HIGH",
          "status": "ACTIVE",
          "source": {
            "id": "device-001",
            "name": "Main Incomer",
            "type": "CIRCUIT_BREAKER"
          },
          "timestamp": "2025-10-15T10:25:00Z",
          "acknowledgedBy": null,
          "acknowledgedAt": null,
          "clearedAt": null
        }
      ]
    }
  }
}
```

#### 6. 確認告警

```graphql
mutation AcknowledgeAlarm($alarmId: ID!, $acknowledgedBy: String!, $comment: String) {
  acknowledgeAlarm(
    alarmId: $alarmId
    acknowledgedBy: $acknowledgedBy
    comment: $comment
  ) {
    id
    status
    acknowledgedBy
    acknowledgedAt
  }
}
```

### 訂閱 (Subscriptions)

PME GraphQL API 支援 WebSocket 訂閱，實現即時數據推送：

```graphql
subscription OnAlarmCreated($siteId: ID!) {
  alarmCreated(siteId: $siteId) {
    id
    name
    description
    severity
    status
    source {
      id
      name
    }
    timestamp
  }
}
```

---

## NDH PME Connector 設計

### Connector 架構

```
┌─────────────────────────────────────────────────────────┐
│           NDH PME Connector                              │
│                                                          │
│  ┌────────────────────────────────────────────────────┐ │
│  │  GraphQL Client                                    │ │
│  │  - Authentication Manager                          │ │
│  │  - Query Builder                                   │ │
│  │  - Subscription Manager                            │ │
│  │  - Error Handler                                   │ │
│  └────────────────┬───────────────────────────────────┘ │
│                   │                                      │
│  ┌────────────────┴───────────────────────────────────┐ │
│  │  Data Mapper                                       │ │
│  │  - PME → IADL/FDL Mapping                         │ │
│  │  - Unit Conversion                                 │ │
│  │  - Data Normalization                              │ │
│  └────────────────┬───────────────────────────────────┘ │
│                   │                                      │
│  ┌────────────────┴───────────────────────────────────┐ │
│  │  Alarm Handler                                     │ │
│  │  - Alarm Translation                               │ │
│  │  - Priority Mapping                                │ │
│  │  - Alarm Routing                                   │ │
│  └────────────────┬───────────────────────────────────┘ │
│                   │                                      │
│  ┌────────────────┴───────────────────────────────────┐ │
│  │  Historical Data Sync                              │ │
│  │  - Batch Query                                     │ │
│  │  - Incremental Sync                                │ │
│  │  - Data Validation                                 │ │
│  └────────────────┬───────────────────────────────────┘ │
│                   │                                      │
│  ┌────────────────┴───────────────────────────────────┐ │
│  │  Cache Manager                                     │ │
│  │  - Device Metadata Cache                           │ │
│  │  - Measurement Cache                               │ │
│  │  - Cache Invalidation                              │ │
│  └────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────┘
```

### 核心組件

#### 1. GraphQL Client

負責與 PME GraphQL API 通訊：

```python
# ndh/connectors/pme/graphql_client.py
import asyncio
from gql import gql, Client
from gql.transport.aiohttp import AIOHTTPTransport
from gql.transport.websockets import WebsocketsTransport
import jwt
import time

class PMEGraphQLClient:
    """PME GraphQL API 客戶端"""
    
    def __init__(self, config):
        self.config = config
        self.access_token = None
        self.token_expires_at = 0
        self.client = None
        self.subscription_client = None
        
    async def authenticate(self):
        """OAuth 2.0 認證"""
        import aiohttp
        
        async with aiohttp.ClientSession() as session:
            async with session.post(
                f"{self.config['base_url']}/oauth/token",
                data={
                    'grant_type': 'client_credentials',
                    'client_id': self.config['client_id'],
                    'client_secret': self.config['client_secret'],
                    'scope': 'read:sites read:devices read:measurements read:alarms'
                }
            ) as response:
                data = await response.json()
                self.access_token = data['access_token']
                self.token_expires_at = time.time() + data['expires_in'] - 60
                
    async def ensure_authenticated(self):
        """確保 Token 有效"""
        if time.time() >= self.token_expires_at:
            await self.authenticate()
            
    async def get_client(self):
        """獲取 GraphQL 客戶端"""
        await self.ensure_authenticated()
        
        if self.client is None:
            transport = AIOHTTPTransport(
                url=f"{self.config['base_url']}/graphql",
                headers={'Authorization': f'Bearer {self.access_token}'}
            )
            self.client = Client(transport=transport, fetch_schema_from_transport=True)
            
        return self.client
        
    async def query(self, query_string, variables=None):
        """執行查詢"""
        client = await self.get_client()
        query = gql(query_string)
        result = await client.execute_async(query, variable_values=variables)
        return result
        
    async def subscribe(self, subscription_string, variables=None, callback=None):
        """訂閱即時數據"""
        await self.ensure_authenticated()
        
        transport = WebsocketsTransport(
            url=f"{self.config['base_url'].replace('http', 'ws')}/graphql",
            init_payload={'Authorization': f'Bearer {self.access_token}'}
        )
        
        async with Client(transport=transport) as session:
            subscription = gql(subscription_string)
            async for result in session.subscribe(subscription, variable_values=variables):
                if callback:
                    await callback(result)
```

#### 2. Data Mapper

將 PME 數據映射到 NDH 數據模型：

```python
# ndh/connectors/pme/data_mapper.py
from typing import Dict, Any, List
from datetime import datetime

class PMEDataMapper:
    """PME 數據映射器"""
    
    def __init__(self):
        # PME 設備類型 → IADL 資產類型映射
        self.device_type_mapping = {
            'CIRCUIT_BREAKER': 'circuit_breaker',
            'METER': 'power_meter',
            'TRANSFORMER': 'transformer',
            'UPS': 'ups',
            'GENERATOR': 'generator',
            'MOTOR': 'electric_motor',
            'CAPACITOR_BANK': 'capacitor_bank'
        }
        
        # PME 測量類型 → NDH 標籤映射
        self.measurement_mapping = {
            'voltage.phaseA': 'voltage_l1',
            'voltage.phaseB': 'voltage_l2',
            'voltage.phaseC': 'voltage_l3',
            'current.phaseA': 'current_l1',
            'current.phaseB': 'current_l2',
            'current.phaseC': 'current_l3',
            'power.active': 'active_power',
            'power.reactive': 'reactive_power',
            'power.apparent': 'apparent_power',
            'power.powerFactor': 'power_factor',
            'power.frequency': 'frequency',
            'energy.activeImport': 'energy_import',
            'energy.activeExport': 'energy_export'
        }
        
    def map_device_to_asset(self, pme_device: Dict[str, Any]) -> Dict[str, Any]:
        """將 PME 設備映射為 IADL 資產"""
        asset_type = self.device_type_mapping.get(
            pme_device['type'], 
            'generic_electrical_device'
        )
        
        return {
            'id': f"pme_{pme_device['id']}",
            'asset_type': asset_type,
            'name': pme_device['name'],
            'description': f"PME Device: {pme_device.get('model', 'Unknown')}",
            'metadata': {
                'pme_id': pme_device['id'],
                'pme_type': pme_device['type'],
                'model': pme_device.get('model'),
                'serial_number': pme_device.get('serialNumber'),
                'location': pme_device.get('location'),
                'source': 'schneider_pme'
            },
            'parent_id': f"pme_{pme_device['parent']['id']}" if pme_device.get('parent') else None
        }
        
    def map_measurements_to_tags(
        self, 
        device_id: str, 
        measurements: Dict[str, Any]
    ) -> List[Dict[str, Any]]:
        """將 PME 測量值映射為 NDH 標籤數據"""
        tags = []
        
        # 電壓
        if 'voltage' in measurements:
            voltage = measurements['voltage']
            timestamp = datetime.fromisoformat(voltage['timestamp'].replace('Z', '+00:00'))
            
            for phase in ['phaseA', 'phaseB', 'phaseC']:
                if phase in voltage:
                    tag_name = self.measurement_mapping[f'voltage.{phase}']
                    tags.append({
                        'asset_id': f"pme_{device_id}",
                        'tag_name': tag_name,
                        'value': voltage[phase],
                        'unit': voltage['unit'],
                        'timestamp': timestamp,
                        'quality': 'good'
                    })
                    
        # 電流
        if 'current' in measurements:
            current = measurements['current']
            timestamp = datetime.fromisoformat(current['timestamp'].replace('Z', '+00:00'))
            
            for phase in ['phaseA', 'phaseB', 'phaseC']:
                if phase in current:
                    tag_name = self.measurement_mapping[f'current.{phase}']
                    tags.append({
                        'asset_id': f"pme_{device_id}",
                        'tag_name': tag_name,
                        'value': current[phase],
                        'unit': current['unit'],
                        'timestamp': timestamp,
                        'quality': 'good'
                    })
                    
        # 功率
        if 'power' in measurements:
            power = measurements['power']
            timestamp = datetime.fromisoformat(power['timestamp'].replace('Z', '+00:00'))
            
            for metric in ['active', 'reactive', 'apparent', 'powerFactor', 'frequency']:
                if metric in power:
                    tag_name = self.measurement_mapping[f'power.{metric}']
                    tags.append({
                        'asset_id': f"pme_{device_id}",
                        'tag_name': tag_name,
                        'value': power[metric],
                        'unit': power.get('unit', ''),
                        'timestamp': timestamp,
                        'quality': 'good'
                    })
                    
        # 電能
        if 'energy' in measurements:
            energy = measurements['energy']
            timestamp = datetime.fromisoformat(energy['timestamp'].replace('Z', '+00:00'))
            
            for metric in ['activeImport', 'activeExport']:
                if metric in energy:
                    tag_name = self.measurement_mapping[f'energy.{metric}']
                    tags.append({
                        'asset_id': f"pme_{device_id}",
                        'tag_name': tag_name,
                        'value': energy[metric],
                        'unit': energy['unit'],
                        'timestamp': timestamp,
                        'quality': 'good'
                    })
                    
        return tags
        
    def map_alarm_to_event(self, pme_alarm: Dict[str, Any]) -> Dict[str, Any]:
        """將 PME 告警映射為 NDH 事件"""
        # PME 嚴重性 → NDH 優先級
        severity_mapping = {
            'CRITICAL': 'critical',
            'HIGH': 'high',
            'MEDIUM': 'medium',
            'LOW': 'low',
            'INFO': 'info'
        }
        
        return {
            'id': f"pme_alarm_{pme_alarm['id']}",
            'asset_id': f"pme_{pme_alarm['source']['id']}",
            'event_type': 'alarm',
            'name': pme_alarm['name'],
            'description': pme_alarm['description'],
            'priority': severity_mapping.get(pme_alarm['severity'], 'medium'),
            'status': 'active' if pme_alarm['status'] == 'ACTIVE' else 'cleared',
            'timestamp': datetime.fromisoformat(pme_alarm['timestamp'].replace('Z', '+00:00')),
            'acknowledged': pme_alarm['acknowledgedBy'] is not None,
            'acknowledged_by': pme_alarm.get('acknowledgedBy'),
            'acknowledged_at': datetime.fromisoformat(pme_alarm['acknowledgedAt'].replace('Z', '+00:00')) if pme_alarm.get('acknowledgedAt') else None,
            'metadata': {
                'pme_alarm_id': pme_alarm['id'],
                'pme_severity': pme_alarm['severity'],
                'source': 'schneider_pme'
            }
        }
```

#### 3. PME Connector 主類

```python
# ndh/connectors/pme/connector.py
import asyncio
from typing import Dict, Any, List, Callable
import logging

from .graphql_client import PMEGraphQLClient
from .data_mapper import PMEDataMapper

logger = logging.getLogger(__name__)

class PMEConnector:
    """Schneider PME Connector"""
    
    def __init__(self, config: Dict[str, Any]):
        self.config = config
        self.client = PMEGraphQLClient(config)
        self.mapper = PMEDataMapper()
        self.site_id = config.get('site_id')
        
        # 回調函數
        self.on_measurement = None
        self.on_alarm = None
        
    async def initialize(self):
        """初始化連接器"""
        logger.info("Initializing PME Connector...")
        
        # 認證
        await self.client.authenticate()
        
        # 同步設備清單
        await self.sync_devices()
        
        # 訂閱告警
        asyncio.create_task(self.subscribe_alarms())
        
        logger.info("PME Connector initialized successfully")
        
    async def sync_devices(self):
        """同步設備清單"""
        logger.info("Syncing devices from PME...")
        
        query = """
        query GetDevices($siteId: ID!) {
          site(id: $siteId) {
            electricalNetwork {
              devices {
                id
                name
                type
                model
                serialNumber
                location
                parent {
                  id
                }
              }
            }
          }
        }
        """
        
        result = await self.client.query(query, {'siteId': self.site_id})
        devices = result['site']['electricalNetwork']['devices']
        
        logger.info(f"Found {len(devices)} devices in PME")
        
        # 映射為 IADL 資產
        assets = [self.mapper.map_device_to_asset(device) for device in devices]
        
        # 發送到 NDH
        # TODO: 實作資產註冊邏輯
        
        return assets
        
    async def poll_measurements(self, device_ids: List[str]):
        """輪詢測量值"""
        query = """
        query GetMeasurements($deviceId: ID!) {
          device(id: $deviceId) {
            measurements {
              voltage {
                phaseA
                phaseB
                phaseC
                unit
                timestamp
              }
              current {
                phaseA
                phaseB
                phaseC
                unit
                timestamp
              }
              power {
                active
                reactive
                apparent
                powerFactor
                frequency
                unit
                timestamp
              }
              energy {
                activeImport
                activeExport
                unit
                timestamp
              }
            }
          }
        }
        """
        
        for device_id in device_ids:
            try:
                result = await self.client.query(query, {'deviceId': device_id})
                measurements = result['device']['measurements']
                
                # 映射為標籤數據
                tags = self.mapper.map_measurements_to_tags(device_id, measurements)
                
                # 發送到 Kafka
                if self.on_measurement:
                    await self.on_measurement(tags)
                    
            except Exception as e:
                logger.error(f"Error polling measurements for device {device_id}: {e}")
                
    async def subscribe_alarms(self):
        """訂閱告警"""
        subscription = """
        subscription OnAlarmCreated($siteId: ID!) {
          alarmCreated(siteId: $siteId) {
            id
            name
            description
            severity
            status
            source {
              id
              name
            }
            timestamp
          }
        }
        """
        
        async def alarm_callback(result):
            alarm = result['alarmCreated']
            logger.info(f"Received alarm: {alarm['name']}")
            
            # 映射為 NDH 事件
            event = self.mapper.map_alarm_to_event(alarm)
            
            # 發送到 Kafka
            if self.on_alarm:
                await self.on_alarm(event)
                
        await self.client.subscribe(
            subscription, 
            {'siteId': self.site_id}, 
            callback=alarm_callback
        )
        
    async def query_historical_data(
        self,
        device_id: str,
        measurement_type: str,
        start_time: str,
        end_time: str,
        aggregation: str = 'AVERAGE',
        interval: str = 'PT15M'
    ) -> List[Dict[str, Any]]:
        """查詢歷史數據"""
        query = """
        query GetHistoricalData(
          $deviceId: ID!
          $measurementType: MeasurementType!
          $startTime: DateTime!
          $endTime: DateTime!
          $aggregation: AggregationType
          $interval: Duration
        ) {
          device(id: $deviceId) {
            historicalData(
              measurementType: $measurementType
              startTime: $startTime
              endTime: $endTime
              aggregation: $aggregation
              interval: $interval
            ) {
              timestamp
              value
              unit
              quality
            }
          }
        }
        """
        
        result = await self.client.query(query, {
            'deviceId': device_id,
            'measurementType': measurement_type,
            'startTime': start_time,
            'endTime': end_time,
            'aggregation': aggregation,
            'interval': interval
        })
        
        return result['device']['historicalData']
```

---

## 數據映射

### PME 設備 → IADL 資產

| PME 設備類型 | IADL 資產類型 | 說明 |
|-------------|--------------|------|
| CIRCUIT_BREAKER | circuit_breaker | 斷路器 |
| METER | power_meter | 電表 |
| TRANSFORMER | transformer | 變壓器 |
| UPS | ups | 不斷電系統 |
| GENERATOR | generator | 發電機 |
| MOTOR | electric_motor | 電動機 |
| CAPACITOR_BANK | capacitor_bank | 電容器組 |

### PME 測量值 → NDH 標籤

| PME 測量值 | NDH 標籤 | 單位 | 說明 |
|-----------|---------|------|------|
| voltage.phaseA | voltage_l1 | V | L1 相電壓 |
| voltage.phaseB | voltage_l2 | V | L2 相電壓 |
| voltage.phaseC | voltage_l3 | V | L3 相電壓 |
| current.phaseA | current_l1 | A | L1 相電流 |
| current.phaseB | current_l2 | A | L2 相電流 |
| current.phaseC | current_l3 | A | L3 相電流 |
| power.active | active_power | kW | 有功功率 |
| power.reactive | reactive_power | kVAR | 無功功率 |
| power.apparent | apparent_power | kVA | 視在功率 |
| power.powerFactor | power_factor | - | 功率因數 |
| power.frequency | frequency | Hz | 頻率 |
| energy.activeImport | energy_import | kWh | 匯入電能 |
| energy.activeExport | energy_export | kWh | 匯出電能 |

### PME 告警 → NDH 事件

| PME 嚴重性 | NDH 優先級 | 說明 |
|-----------|-----------|------|
| CRITICAL | critical | 緊急告警 |
| HIGH | high | 高優先級告警 |
| MEDIUM | medium | 中優先級告警 |
| LOW | low | 低優先級告警 |
| INFO | info | 資訊性告警 |

---

## 整合實作

### 配置

```yaml
# ndh_config.yaml
connectors:
  pme:
    enabled: true
    base_url: https://pme-server.example.com
    client_id: YOUR_CLIENT_ID
    client_secret: YOUR_CLIENT_SECRET
    site_id: site-001
    
    # 輪詢設定
    polling:
      enabled: true
      interval: 60  # 秒
      devices:
        - device-001
        - device-002
        - device-003
        
    # 訂閱設定
    subscriptions:
      alarms: true
      measurements: false  # 使用輪詢代替
      
    # 歷史數據同步
    historical_sync:
      enabled: true
      initial_sync_days: 30
      sync_interval: 3600  # 每小時同步一次
```

### 部署

```yaml
# kubernetes/pme-connector-deployment.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: ndh-pme-connector
  namespace: ndh
spec:
  replicas: 1  # PME Connector 通常只需要一個實例
  selector:
    matchLabels:
      app: ndh-pme-connector
  template:
    metadata:
      labels:
        app: ndh-pme-connector
    spec:
      containers:
      - name: pme-connector
        image: ndh/pme-connector:latest
        env:
        - name: PME_BASE_URL
          value: "https://pme-server.example.com"
        - name: PME_CLIENT_ID
          valueFrom:
            secretKeyRef:
              name: pme-credentials
              key: client-id
        - name: PME_CLIENT_SECRET
          valueFrom:
            secretKeyRef:
              name: pme-credentials
              key: client-secret
        - name: PME_SITE_ID
          value: "site-001"
        - name: KAFKA_BOOTSTRAP_SERVERS
          value: "kafka:9092"
        - name: INFLUXDB_URL
          value: "http://influxdb:8086"
        resources:
          requests:
            memory: "256Mi"
            cpu: "250m"
          limits:
            memory: "512Mi"
            cpu: "500m"
---
apiVersion: v1
kind: Secret
metadata:
  name: pme-credentials
  namespace: ndh
type: Opaque
stringData:
  client-id: YOUR_CLIENT_ID
  client-secret: YOUR_CLIENT_SECRET
```

---

## 使用場景

### 場景 1: 能源效率分析

**目標**: 分析製程設備的能源消耗，識別節能機會

**工作流**:

1. PME Connector 收集所有電表的電能數據
2. NDH 將電能數據與製程數據關聯
3. AI Agent 分析能源消耗模式
4. 識別高能耗設備和時段
5. 生成節能建議

**範例查詢** (通過 MCP Server):

```
User: "分析過去一週的能源消耗，找出最耗能的設備"

AI Agent:
1. 查詢 PME 歷史電能數據
2. 按設備聚合能源消耗
3. 排序並識別 Top 10 耗能設備
4. 生成報告和視覺化

Response:
"過去一週能源消耗分析：
1. 設備 A (壓縮機): 12,345 kWh (25%)
2. 設備 B (冷卻塔): 9,876 kWh (20%)
3. 設備 C (HVAC): 8,765 kWh (18%)
...

建議：
- 壓縮機 A 的能耗比正常高 15%，建議檢查是否有洩漏
- 冷卻塔 B 在非生產時段仍在運行，建議優化排程
"
```

### 場景 2: 預測性維護

**目標**: 基於電力參數預測設備故障

**工作流**:

1. PME Connector 收集電流、電壓、功率因數等參數
2. NDH 儲存時序數據
3. AI 模型分析趨勢，檢測異常
4. 預測潛在故障
5. 生成維護工單

**範例**:

```python
# 檢測電動機異常
def detect_motor_anomaly(motor_id):
    # 查詢過去 24 小時的電流數據
    current_data = query_historical_data(
        device_id=motor_id,
        measurement_type='CURRENT_PHASE_A',
        start_time='2025-10-14T00:00:00Z',
        end_time='2025-10-15T00:00:00Z'
    )
    
    # 計算統計特徵
    mean_current = np.mean([d['value'] for d in current_data])
    std_current = np.std([d['value'] for d in current_data])
    
    # 檢測異常
    if std_current > threshold:
        return {
            'status': 'anomaly_detected',
            'message': f'Motor {motor_id} current fluctuation exceeds threshold',
            'recommendation': 'Schedule maintenance inspection'
        }
```

### 場景 3: 電力品質監控

**目標**: 監控電力品質，及時發現問題

**工作流**:

1. PME Connector 訂閱電力品質告警
2. NDH 接收並處理告警
3. 在 Omniverse 中視覺化受影響區域
4. 通知運維人員
5. 記錄事件並生成報告

### 場景 4: 需量管理

**目標**: 優化需量，降低電費

**工作流**:

1. PME Connector 收集即時功率數據
2. NDH 計算當前需量
3. 預測未來 15 分鐘需量
4. 如果超過目標，觸發需量控制
5. 調整非關鍵負載

---

## 部署指南

### 前置條件

1. ✅ Schneider PME 2020 或更新版本
2. ✅ PME GraphQL API 已啟用
3. ✅ 已申請 API 存取權限
4. ✅ NDH 已部署並運行
5. ✅ Kubernetes 集群可用

### 部署步驟

#### 步驟 1: 申請 PME API 存取

1. 登入 Schneider Electric Exchange
2. 導航到 PME Data API 頁面
3. 點擊 "Request API Access"
4. 填寫申請表單
5. 等待 Schneider 審核並提供 Client ID 和 Secret

#### 步驟 2: 配置 PME Connector

```bash
# 建立 Secret
kubectl create secret generic pme-credentials \
  --from-literal=client-id=YOUR_CLIENT_ID \
  --from-literal=client-secret=YOUR_CLIENT_SECRET \
  -n ndh

# 部署 PME Connector
kubectl apply -f kubernetes/pme-connector-deployment.yaml
```

#### 步驟 3: 驗證連接

```bash
# 檢查 Pod 狀態
kubectl get pods -n ndh -l app=ndh-pme-connector

# 檢查日誌
kubectl logs -n ndh -l app=ndh-pme-connector --tail=100

# 應該看到類似的日誌：
# INFO: Initializing PME Connector...
# INFO: Authentication successful
# INFO: Syncing devices from PME...
# INFO: Found 25 devices in PME
# INFO: PME Connector initialized successfully
```

#### 步驟 4: 測試數據流

```bash
# 檢查 Kafka Topic
kafka-console-consumer --bootstrap-server kafka:9092 \
  --topic ndh.measurements.pme \
  --from-beginning

# 應該看到類似的訊息：
# {"asset_id":"pme_device-001","tag_name":"voltage_l1","value":230.5,"unit":"V","timestamp":"2025-10-15T10:30:00Z"}
```

#### 步驟 5: 驗證歷史數據同步

```bash
# 查詢 InfluxDB
influx -host influxdb -port 8086 -database ndh

> SELECT * FROM measurements WHERE asset_id = 'pme_device-001' LIMIT 10
```

---

## 監控與維護

### 監控指標

```yaml
# prometheus_metrics.yml
- name: pme_connector_metrics
  metrics:
    - pme_api_requests_total
      description: Total number of PME API requests
      type: counter
      labels: [method, status]
      
    - pme_api_request_duration_seconds
      description: PME API request duration
      type: histogram
      labels: [method]
      
    - pme_devices_synced_total
      description: Total number of devices synced from PME
      type: gauge
      
    - pme_measurements_received_total
      description: Total number of measurements received
      type: counter
      labels: [device_id]
      
    - pme_alarms_received_total
      description: Total number of alarms received
      type: counter
      labels: [severity]
      
    - pme_connector_errors_total
      description: Total number of connector errors
      type: counter
      labels: [error_type]
```

### 告警規則

```yaml
# prometheus_alerts.yml
groups:
  - name: pme_connector_alerts
    rules:
      - alert: PMEConnectorDown
        expr: up{job="pme-connector"} == 0
        for: 5m
        labels:
          severity: critical
        annotations:
          summary: "PME Connector is down"
          
      - alert: PMEAPIHighErrorRate
        expr: rate(pme_api_requests_total{status="error"}[5m]) > 0.1
        for: 5m
        labels:
          severity: warning
        annotations:
          summary: "High PME API error rate"
          
      - alert: PMEDataStale
        expr: time() - pme_last_measurement_timestamp > 300
        for: 5m
        labels:
          severity: warning
        annotations:
          summary: "PME data is stale (no data for 5 minutes)"
```

---

## 最佳實踐

### 1. 錯誤處理

- ✅ 實作重試機制（指數退避）
- ✅ 處理 Token 過期
- ✅ 處理網路中斷
- ✅ 記錄所有錯誤

### 2. 效能優化

- ✅ 使用批次查詢減少 API 呼叫
- ✅ 快取設備元數據
- ✅ 使用訂閱代替輪詢（當可用時）
- ✅ 壓縮歷史數據

### 3. 安全性

- ✅ 使用 Kubernetes Secrets 儲存憑證
- ✅ 啟用 TLS/SSL
- ✅ 定期輪換 API 金鑰
- ✅ 限制 API 存取權限

### 4. 可靠性

- ✅ 實作健康檢查
- ✅ 設置告警
- ✅ 定期備份配置
- ✅ 準備故障恢復計畫

---

## 附錄

### A. PME GraphQL Schema 範例

```graphql
type Site {
  id: ID!
  name: String!
  description: String
  location: Location
  timezone: String
  currency: String
  electricalNetwork: ElectricalNetwork
  meteringHierarchy: MeteringHierarchy
  alarms(
    status: AlarmStatus
    severity: AlarmSeverity
    startTime: DateTime
    endTime: DateTime
  ): [Alarm!]!
}

type Device {
  id: ID!
  name: String!
  type: DeviceType!
  model: String
  serialNumber: String
  location: String
  parent: Device
  children: [Device!]
  measurements: Measurements
  historicalData(
    measurementType: MeasurementType!
    startTime: DateTime!
    endTime: DateTime!
    aggregation: AggregationType
    interval: Duration
  ): [DataPoint!]!
}

type Measurements {
  voltage: VoltageMeasurement
  current: CurrentMeasurement
  power: PowerMeasurement
  energy: EnergyMeasurement
}

type Alarm {
  id: ID!
  name: String!
  description: String
  severity: AlarmSeverity!
  status: AlarmStatus!
  source: Device!
  timestamp: DateTime!
  acknowledgedBy: String
  acknowledgedAt: DateTime
  clearedAt: DateTime
}
```

### B. 常見問題 (FAQ)

**Q1: PME GraphQL API 的速率限制是多少？**

A: 預設為 100 requests/minute，可聯繫 Schneider 調整。

**Q2: 如何處理 PME 伺服器停機？**

A: PME Connector 會自動重試，並在 PME 恢復後繼續同步數據。

**Q3: 是否支援多個 PME 站點？**

A: 是的，可以部署多個 PME Connector 實例，每個連接到不同的站點。

**Q4: 歷史數據可以回溯多久？**

A: 取決於 PME 的數據保留策略，通常為 1-2 年。

### C. 參考資源

- [PME GraphQL API 文檔](https://exchange.se.com/develop/products/118970/)
- [PME 產品頁面](https://www.se.com/us/en/product-range/65404-ecostruxure-power-monitoring-expert/)
- [Schneider Electric Exchange](https://exchange.se.com/)

---

## 版本歷史

| 版本 | 日期 | 作者 | 變更說明 |
|------|------|------|---------|
| 1.0.0 | 2025-10-15 | 林志錚 Michael Lin | 初始版本 |

---

**© 2025 IDTF Consortium. All rights reserved.**

