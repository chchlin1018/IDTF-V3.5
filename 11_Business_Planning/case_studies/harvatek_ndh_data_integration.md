# Harvatek LED 封裝廠 NDH 資料整合方案

**基於 IDTF V3.1 和 NDH V3.1**

**版本**: 1.0  
**日期**: 2025-10-12  
**作者**: IDTF V3.1 Team

---

## 目錄

1. [NDH 架構總覽](#1-ndh-架構總覽)
2. [數據源分析](#2-數據源分析)
3. [Kafka 數據流設計](#3-kafka-數據流設計)
4. [InfluxDB 時序數據庫設計](#4-influxdb-時序數據庫設計)
5. [PostgreSQL 關聯數據庫設計](#5-postgresql-關聯數據庫設計)
6. [Connector 配置](#6-connector-配置)
7. [實時數據處理管線](#7-實時數據處理管線)
8. [告警和事件管理](#8-告警和事件管理)
9. [API 設計](#9-api-設計)
10. [部署架構](#10-部署架構)

---

## 1. NDH 架構總覽

### 1.1 系統架構圖

```
┌─────────────────────────────────────────────────────────────────────────┐
│                    Harvatek NDH V3.1 資料中樞                             │
└─────────────────────────────────────────────────────────────────────────┘
                                    │
        ┌───────────────────────────┼───────────────────────────┐
        │                           │                           │
        ▼                           ▼                           ▼
┌─────────────────┐       ┌─────────────────┐       ┌─────────────────┐
│  Kafka Cluster  │       │  InfluxDB       │       │  PostgreSQL     │
│  (3 Brokers)    │       │  Cluster        │       │  Cluster        │
│                 │       │  (2 Nodes)      │       │  (Primary +     │
│ • 實時數據流    │◄─────►│                 │       │   Replica)      │
│ • 事件處理      │       │ • 時序數據      │       │                 │
│ • 數據緩衝      │       │ • 設備參數      │       │ • 資產定義      │
│                 │       │ • 感測器數據    │       │ • 維護記錄      │
│ Topics:         │       │ • 能源數據      │       │ • 配方數據      │
│ - equipment.*   │       │                 │       │ • 使用者權限    │
│ - facility.*    │       │ Retention:      │       │ • 生產訂單      │
│ - sensor.*      │       │ - Raw: 7 days   │       │                 │
│ - alarm.*       │       │ - Agg: 90 days  │       │                 │
│ - event.*       │       │ - Archive: 3yr  │       │                 │
└─────────────────┘       └─────────────────┘       └─────────────────┘
        ▲                           ▲                           ▲
        │                           │                           │
        └───────────────────────────┴───────────────────────────┘
                                    │
        ┌───────────────────────────┼───────────────────────────┐
        │                           │                           │
        ▼                           ▼                           ▼
┌─────────────────┐       ┌─────────────────┐       ┌─────────────────┐
│ OT Connectors   │       │ IT Connectors   │       │ External        │
│                 │       │                 │       │ Connectors      │
│ • SECS/GEM      │       │ • SAP ERP       │       │ • Weather API   │
│   (340 設備)    │       │ • MES           │       │ • Supplier      │
│                 │       │ • PLM           │       │ • Customer      │
│ • OPC UA        │       │ • WMS           │       │                 │
│   (200 點)      │       │ • QMS           │       │                 │
│                 │       │                 │       │                 │
│ • Modbus TCP    │       │                 │       │                 │
│   (150 點)      │       │                 │       │                 │
│                 │       │                 │       │                 │
│ • BACnet        │       │                 │       │                 │
│   (160 設備)    │       │                 │       │                 │
│                 │       │                 │       │                 │
│ • SNMP          │       │                 │       │                 │
│   (5 UPS)       │       │                 │       │                 │
└─────────────────┘       └─────────────────┘       └─────────────────┘
        ▲                           ▲                           ▲
        │                           │                           │
        └───────────────────────────┴───────────────────────────┘
                                    │
        ┌───────────────────────────┼───────────────────────────┐
        │                           │                           │
        ▼                           ▼                           ▼
┌─────────────────┐       ┌─────────────────┐       ┌─────────────────┐
│ 生產設備        │       │ 設施系統        │       │ IT 系統         │
│ (340 台)        │       │ (160 台)        │       │                 │
│                 │       │                 │       │                 │
│ • Die Sorter    │       │ • Chiller       │       │ • ERP (SAP)     │
│ • Die Bonder    │       │ • AHU           │       │ • MES           │
│ • Wire Bonder   │       │ • DI Water      │       │ • PLM           │
│ • Dispenser     │       │ • N2 Generator  │       │ • WMS           │
│ • Tester        │       │ • Compressor    │       │ • QMS           │
│ • Taping        │       │ • UPS           │       │ • SCADA         │
│ • AOI           │       │ • Transformer   │       │                 │
│ • X-Ray         │       │ • Scrubber      │       │                 │
└─────────────────┘       └─────────────────┘       └─────────────────┘
```

### 1.2 核心組件

#### 1.2.1 Kafka Cluster

**配置**:
- **Broker 數量**: 3
- **Replication Factor**: 3
- **Partition 數量**: 根據主題動態調整 (8-32)
- **Retention**: 7 天 (原始數據)
- **Throughput**: 100,000+ events/sec

**主要功能**:
- 實時數據流處理
- 事件驅動架構
- 數據緩衝和解耦
- 高可用性和容錯

#### 1.2.2 InfluxDB Cluster

**配置**:
- **節點數量**: 2 (Active-Active)
- **Retention Policy**:
  - Raw Data: 7 天
  - Aggregated (1 min): 90 天
  - Aggregated (1 hour): 1 年
  - Aggregated (1 day): 3 年
- **Write Throughput**: 500,000+ points/sec
- **Query Performance**: < 100ms (95th percentile)

**主要功能**:
- 時序數據儲存
- 高效數據壓縮
- 快速查詢和聚合
- 自動數據降採樣

#### 1.2.3 PostgreSQL Cluster

**配置**:
- **架構**: Primary + Replica (Streaming Replication)
- **版本**: PostgreSQL 15
- **Extensions**: PostGIS, TimescaleDB (可選)
- **Backup**: 每日全備份 + WAL 歸檔

**主要功能**:
- 關聯數據儲存
- 資產定義和元數據
- 維護記錄和工單
- 使用者權限管理

---

## 2. 數據源分析

### 2.1 OT 數據源

#### 2.1.1 SECS/GEM 設備 (340 台)

**設備類型**:
- Die Sorter: 20 台
- Die Bonder: 50 台
- Wire Bonder: 80 台
- Dispenser: 40 台
- Tester: 100 台
- Taping Machine: 30 台
- AOI System: 15 台
- X-Ray Inspection: 5 台

**數據類型**:
```yaml
secs_gem_data:
  # 狀態變數 (SV - Status Variables)
  status_variables:
    - SVID_1: Equipment Status
      data_type: string
      enum: [IDLE, RUNNING, ALARM, MAINTENANCE, SETUP]
      update_rate_ms: 1000
    
    - SVID_10: UPH (Units Per Hour)
      data_type: integer
      unit: units/hour
      update_rate_ms: 5000
    
    - SVID_11: Total Count
      data_type: integer
      unit: units
      update_rate_ms: 1000
    
    - SVID_12: Good Count
      data_type: integer
      unit: units
      update_rate_ms: 1000
    
    - SVID_13: Reject Count
      data_type: integer
      unit: units
      update_rate_ms: 1000
  
  # 設備常數 (EC - Equipment Constants)
  equipment_constants:
    - ECID_1: Recipe ID
      data_type: string
    
    - ECID_2: Process Parameters
      data_type: object
  
  # 告警 (AL - Alarms)
  alarms:
    - ALID: Alarm ID
      data_type: integer
    
    - ALTX: Alarm Text
      data_type: string
    
    - ALCD: Alarm Code
      data_type: string
  
  # 事件 (EV - Events)
  events:
    - CEID: Collection Event ID
      data_type: integer
    
    - Event Data: Associated Data
      data_type: object

# 數據量估算
data_volume:
  equipment_count: 340
  tags_per_equipment: 15
  total_tags: 5100
  average_update_rate_ms: 5000
  events_per_second: 1020
  daily_data_points: 88128000  # ~88M points/day
```

#### 2.1.2 OPC UA 設備 (200 點)

**數據源**:
- 電力監控系統: 200 個電力計
- 設備振動監控: 50 點
- 設備溫度監控: 50 點

**數據類型**:
```yaml
opcua_data:
  power_monitoring:
    - node_id: ns=2;s=PowerMeter.{equipment_id}.Voltage
      data_type: float
      unit: V
      update_rate_ms: 5000
    
    - node_id: ns=2;s=PowerMeter.{equipment_id}.Current
      data_type: float
      unit: A
      update_rate_ms: 5000
    
    - node_id: ns=2;s=PowerMeter.{equipment_id}.Power
      data_type: float
      unit: kW
      update_rate_ms: 5000
    
    - node_id: ns=2;s=PowerMeter.{equipment_id}.Energy
      data_type: float
      unit: kWh
      update_rate_ms: 60000
  
  vibration_monitoring:
    - node_id: ns=2;s=VibrationSensor.{equipment_id}.RMS
      data_type: float
      unit: mm/s
      update_rate_ms: 10000
  
  temperature_monitoring:
    - node_id: ns=2;s=TempSensor.{equipment_id}.Temperature
      data_type: float
      unit: celsius
      update_rate_ms: 10000

# 數據量估算
data_volume:
  total_tags: 1000
  average_update_rate_ms: 10000
  events_per_second: 100
  daily_data_points: 8640000  # ~8.6M points/day
```

#### 2.1.3 Modbus TCP 設備 (150 點)

**數據源**:
- 純水系統: 5 台
- 氮氣系統: 3 台
- 壓縮空氣系統: 8 台

**數據類型**:
```yaml
modbus_data:
  di_water_system:
    - register: 40001
      description: System Status
      data_type: boolean
      update_rate_ms: 5000
    
    - register: 40002
      description: Flow Rate
      data_type: float
      unit: lpm
      update_rate_ms: 10000
    
    - register: 40003
      description: Resistivity
      data_type: float
      unit: mohm_cm
      update_rate_ms: 10000
  
  nitrogen_system:
    - register: 40001
      description: System Status
      data_type: boolean
      update_rate_ms: 5000
    
    - register: 40002
      description: Flow Rate
      data_type: float
      unit: nm3/h
      update_rate_ms: 10000
    
    - register: 40003
      description: Purity
      data_type: float
      unit: percent
      update_rate_ms: 60000

# 數據量估算
data_volume:
  total_tags: 150
  average_update_rate_ms: 10000
  events_per_second: 15
  daily_data_points: 1296000  # ~1.3M points/day
```

#### 2.1.4 BACnet 設備 (160 台)

**數據源**:
- Chiller: 10 台
- AHU: 40 台
- 環境感測器: 1,100 點

**數據類型**:
```yaml
bacnet_data:
  chiller:
    - object_type: ANALOG_INPUT
      object_instance: 1
      property: present_value
      description: Cooling Load
      unit: RT
      update_rate_ms: 10000
    
    - object_type: ANALOG_INPUT
      object_instance: 2
      property: present_value
      description: CHW Supply Temperature
      unit: celsius
      update_rate_ms: 10000
  
  ahu:
    - object_type: BINARY_VALUE
      object_instance: 1
      property: present_value
      description: Status
      update_rate_ms: 5000
    
    - object_type: ANALOG_INPUT
      object_instance: 1
      property: present_value
      description: Supply Air Temperature
      unit: celsius
      update_rate_ms: 10000
  
  environmental_sensors:
    - object_type: ANALOG_INPUT
      description: Temperature
      unit: celsius
      update_rate_ms: 10000
      quantity: 400
    
    - object_type: ANALOG_INPUT
      description: Humidity
      unit: percent_rh
      update_rate_ms: 10000
      quantity: 400
    
    - object_type: ANALOG_INPUT
      description: Differential Pressure
      unit: pa
      update_rate_ms: 10000
      quantity: 200

# 數據量估算
data_volume:
  total_tags: 1200
  average_update_rate_ms: 10000
  events_per_second: 120
  daily_data_points: 10368000  # ~10.4M points/day
```

#### 2.1.5 SNMP 設備 (5 台)

**數據源**:
- UPS: 5 台

**數據類型**:
```yaml
snmp_data:
  ups:
    - oid: 1.3.6.1.4.1.318.1.1.1.4.1.1.0
      description: UPS Status
      data_type: integer
      enum: {2: ONLINE, 3: ON_BATTERY, 4: BYPASS, 5: FAULT}
      update_rate_ms: 5000
    
    - oid: 1.3.6.1.4.1.318.1.1.1.4.2.3.0
      description: Output Load
      data_type: float
      unit: percent
      update_rate_ms: 10000
    
    - oid: 1.3.6.1.4.1.318.1.1.1.2.2.1.0
      description: Battery Capacity
      data_type: float
      unit: percent
      update_rate_ms: 60000

# 數據量估算
data_volume:
  total_tags: 35
  average_update_rate_ms: 10000
  events_per_second: 3.5
  daily_data_points: 302400  # ~0.3M points/day
```

### 2.2 IT 數據源

#### 2.2.1 SAP ERP

**數據類型**:
```yaml
sap_erp_data:
  # 生產訂單
  production_orders:
    - table: AUFK
      description: Order Master Data
      sync_frequency: 5_minutes
      fields:
        - AUFNR: Order Number
        - WERKS: Plant
        - MATNR: Material Number
        - GAMNG: Total Order Quantity
        - GSTRS: Scheduled Start Date
        - GLTRS: Scheduled Finish Date
  
  # 物料主檔
  material_master:
    - table: MARA
      description: General Material Data
      sync_frequency: 1_hour
      fields:
        - MATNR: Material Number
        - MAKTX: Material Description
        - MEINS: Base Unit of Measure
  
  # 庫存
  inventory:
    - table: MARD
      description: Storage Location Data
      sync_frequency: 15_minutes
      fields:
        - MATNR: Material Number
        - WERKS: Plant
        - LGORT: Storage Location
        - LABST: Valuated Unrestricted-Use Stock
  
  # 設備主檔
  equipment_master:
    - table: EQUI
      description: Equipment Master Record
      sync_frequency: 1_hour
      fields:
        - EQUNR: Equipment Number
        - EQKTX: Description
        - ANLNR: Asset Number
  
  # 維護工單
  maintenance_orders:
    - table: AUFK
      description: Maintenance Order Header
      sync_frequency: 5_minutes
      fields:
        - AUFNR: Order Number
        - AUART: Order Type
        - EQUNR: Equipment Number
        - GSTRS: Scheduled Start
        - GLTRS: Scheduled Finish

# 數據量估算
data_volume:
  production_orders: 100 orders/day
  material_master: 5000 materials
  inventory_transactions: 500 transactions/day
  equipment_master: 500 equipment
  maintenance_orders: 50 orders/day
```

#### 2.2.2 MES (Manufacturing Execution System)

**數據類型**:
```yaml
mes_data:
  # 生產批次
  production_lots:
    - description: Lot Information
      sync_frequency: real_time
      fields:
        - lot_id: Lot ID
        - product_type: Product Type
        - quantity: Quantity
        - status: Status
        - start_time: Start Time
        - end_time: End Time
  
  # 設備狀態
  equipment_status:
    - description: Equipment Status
      sync_frequency: real_time
      fields:
        - equipment_id: Equipment ID
        - status: Status
        - current_lot: Current Lot
        - recipe_id: Recipe ID
  
  # 品質數據
  quality_data:
    - description: Quality Test Results
      sync_frequency: real_time
      fields:
        - lot_id: Lot ID
        - test_type: Test Type
        - test_result: Test Result
        - test_time: Test Time
  
  # 配方
  recipes:
    - description: Process Recipes
      sync_frequency: on_change
      fields:
        - recipe_id: Recipe ID
        - product_type: Product Type
        - parameters: Process Parameters

# 數據量估算
data_volume:
  production_lots: 500 lots/day
  equipment_status_updates: 340 equipment × 60 updates/hour = 489,600 updates/day
  quality_data: 10,000 tests/day
  recipes: 200 recipes (static)
```

### 2.3 數據量總結

```yaml
# ========================================
# Total Data Volume Summary
# ========================================

daily_data_volume:
  ot_data:
    secs_gem: 88128000  # 88.1M points/day
    opcua: 8640000  # 8.6M points/day
    modbus: 1296000  # 1.3M points/day
    bacnet: 10368000  # 10.4M points/day
    snmp: 302400  # 0.3M points/day
    total: 108734400  # 108.7M points/day
  
  it_data:
    sap_erp: 650  # records/day
    mes: 500150  # records/day
    total: 500800  # records/day
  
  total_daily_data_points: 109235200  # ~109M points/day

storage_requirements:
  influxdb:
    raw_data_7_days: 763.6M points × 50 bytes = 38.2 GB
    aggregated_1min_90_days: 1.57M points × 50 bytes = 78.5 MB
    aggregated_1hour_1_year: 26,280 points × 50 bytes = 1.3 MB
    aggregated_1day_3_years: 1,095 points × 50 bytes = 54.8 KB
    total: ~39 GB (with compression: ~10 GB)
  
  postgresql:
    asset_definitions: 2000 assets × 10 KB = 20 MB
    maintenance_records: 50 orders/day × 365 days × 5 KB = 91.25 MB
    production_orders: 100 orders/day × 365 days × 5 KB = 182.5 MB
    quality_data: 10,000 tests/day × 365 days × 2 KB = 7.3 GB
    total: ~8 GB

throughput_requirements:
  kafka:
    peak_events_per_second: 2000
    average_events_per_second: 1260
    peak_throughput_mbps: 10
    average_throughput_mbps: 6.3
  
  influxdb:
    write_points_per_second: 1260
    query_per_second: 50
  
  postgresql:
    write_transactions_per_second: 10
    read_transactions_per_second: 100
```

---

## 3. Kafka 數據流設計

### 3.1 Kafka 主題架構

```yaml
# ========================================
# Kafka Topics Design
# ========================================

kafka_topics:
  # 生產設備主題
  - topic: equipment.production.events
    description: 生產設備狀態和數據事件
    partitions: 16
    replication_factor: 3
    retention_ms: 604800000  # 7 days
    
    message_schema:
      key: equipment_id
      value:
        timestamp: long
        equipment_id: string
        equipment_type: string
        status: string
        data:
          uph: integer
          total_count: integer
          good_count: integer
          reject_count: integer
          parameters: object
    
    producers:
      - secs_gem_connector
    
    consumers:
      - influxdb_sink_connector
      - postgresql_sink_connector
      - real_time_analytics_service
      - alarm_service
  
  # 設施系統主題
  - topic: facility.hvac.events
    description: HVAC 系統數據事件
    partitions: 8
    replication_factor: 3
    retention_ms: 604800000
    
    message_schema:
      key: equipment_id
      value:
        timestamp: long
        equipment_id: string
        equipment_type: string
        data:
          status: boolean
          temperature: float
          humidity: float
          pressure: float
          flow_rate: float
    
    producers:
      - bacnet_connector
    
    consumers:
      - influxdb_sink_connector
      - energy_management_service
  
  - topic: facility.utility.events
    description: 公用系統數據事件
    partitions: 8
    replication_factor: 3
    retention_ms: 604800000
    
    message_schema:
      key: system_id
      value:
        timestamp: long
        system_id: string
        system_type: string
        data:
          flow_rate: float
          pressure: float
          purity: float
          temperature: float
    
    producers:
      - modbus_connector
    
    consumers:
      - influxdb_sink_connector
  
  - topic: facility.power.events
    description: 電力系統數據事件
    partitions: 8
    replication_factor: 3
    retention_ms: 604800000
    
    message_schema:
      key: meter_id
      value:
        timestamp: long
        meter_id: string
        location: string
        data:
          voltage: float
          current: float
          power: float
          energy: float
          power_factor: float
    
    producers:
      - opcua_connector
    
    consumers:
      - influxdb_sink_connector
      - energy_management_service
      - cost_allocation_service
  
  # 環境感測器主題
  - topic: sensor.environmental.events
    description: 環境感測器數據事件
    partitions: 16
    replication_factor: 3
    retention_ms: 604800000
    
    message_schema:
      key: sensor_id
      value:
        timestamp: long
        sensor_id: string
        sensor_type: string
        location:
          floor: string
          area: string
          coordinates: object
        data:
          temperature: float
          humidity: float
          pressure: float
          particle_count: object
    
    producers:
      - bacnet_connector
    
    consumers:
      - influxdb_sink_connector
      - environmental_monitoring_service
  
  # 告警主題
  - topic: alarm.equipment
    description: 設備告警事件
    partitions: 8
    replication_factor: 3
    retention_ms: 2592000000  # 30 days
    
    message_schema:
      key: equipment_id
      value:
        timestamp: long
        alarm_id: string
        equipment_id: string
        equipment_type: string
        alarm_code: string
        alarm_text: string
        severity: string  # CRITICAL, HIGH, MEDIUM, LOW
        status: string  # ACTIVE, ACKNOWLEDGED, CLEARED
    
    producers:
      - secs_gem_connector
      - bacnet_connector
      - snmp_connector
    
    consumers:
      - alarm_management_service
      - notification_service
      - postgresql_sink_connector
  
  # 事件主題
  - topic: event.production
    description: 生產事件
    partitions: 8
    replication_factor: 3
    retention_ms: 2592000000  # 30 days
    
    message_schema:
      key: event_id
      value:
        timestamp: long
        event_id: string
        event_type: string
        equipment_id: string
        lot_id: string
        data: object
    
    producers:
      - mes_connector
      - secs_gem_connector
    
    consumers:
      - postgresql_sink_connector
      - production_analytics_service
  
  # MES 整合主題
  - topic: mes.production.lots
    description: MES 生產批次數據
    partitions: 4
    replication_factor: 3
    retention_ms: 2592000000  # 30 days
    
    message_schema:
      key: lot_id
      value:
        timestamp: long
        lot_id: string
        product_type: string
        quantity: integer
        status: string
        equipment_id: string
        recipe_id: string
        start_time: long
        end_time: long
    
    producers:
      - mes_connector
    
    consumers:
      - postgresql_sink_connector
      - production_tracking_service
  
  # ERP 整合主題
  - topic: erp.production.orders
    description: ERP 生產訂單數據
    partitions: 4
    replication_factor: 3
    retention_ms: 2592000000  # 30 days
    
    message_schema:
      key: order_number
      value:
        timestamp: long
        order_number: string
        material_number: string
        quantity: integer
        scheduled_start: long
        scheduled_finish: long
        status: string
    
    producers:
      - sap_connector
    
    consumers:
      - postgresql_sink_connector
      - production_planning_service
```

### 3.2 Kafka Connect 配置

```yaml
# ========================================
# Kafka Connect Configuration
# ========================================

kafka_connect:
  # Source Connectors (數據採集)
  source_connectors:
    - name: secs-gem-source-connector
      connector_class: com.harvatek.kafka.connect.secsgem.SecsGemSourceConnector
      tasks_max: 10
      config:
        equipment_list: /config/equipment_list.json
        poll_interval_ms: 1000
        topic_prefix: equipment.production
    
    - name: opcua-source-connector
      connector_class: io.confluent.connect.opcua.OpcUaSourceConnector
      tasks_max: 5
      config:
        opcua_server_uri: opc.tcp://power-monitor:4840
        node_ids_file: /config/opcua_nodes.json
        poll_interval_ms: 5000
        topic: facility.power.events
    
    - name: modbus-source-connector
      connector_class: com.github.jcustenborder.kafka.connect.modbus.ModbusSourceConnector
      tasks_max: 3
      config:
        modbus_hosts: /config/modbus_hosts.json
        poll_interval_ms: 10000
        topic: facility.utility.events
    
    - name: bacnet-source-connector
      connector_class: com.harvatek.kafka.connect.bacnet.BacnetSourceConnector
      tasks_max: 5
      config:
        bacnet_devices: /config/bacnet_devices.json
        poll_interval_ms: 10000
        topic_prefix: facility
    
    - name: snmp-source-connector
      connector_class: com.github.jcustenborder.kafka.connect.snmp.SnmpSourceConnector
      tasks_max: 1
      config:
        snmp_hosts: /config/snmp_hosts.json
        poll_interval_ms: 10000
        topic: facility.power.events
  
  # Sink Connectors (數據寫入)
  sink_connectors:
    - name: influxdb-sink-connector
      connector_class: com.github.jcustenborder.kafka.connect.influxdb.InfluxDBSinkConnector
      tasks_max: 8
      config:
        influxdb_url: http://influxdb-cluster:8086
        influxdb_database: harvatek_timeseries
        topics: >
          equipment.production.events,
          facility.hvac.events,
          facility.utility.events,
          facility.power.events,
          sensor.environmental.events
        batch_size: 1000
        linger_ms: 100
    
    - name: postgresql-sink-connector
      connector_class: io.confluent.connect.jdbc.JdbcSinkConnector
      tasks_max: 4
      config:
        connection_url: jdbc:postgresql://postgresql-cluster:5432/harvatek_relational
        topics: >
          alarm.equipment,
          event.production,
          mes.production.lots,
          erp.production.orders
        auto_create: false
        insert_mode: upsert
        pk_mode: record_key
        batch_size: 500
```

---

*文件未完,繼續下一部分...*




## 4. InfluxDB 時序數據庫設計

### 4.1 數據庫架構

```yaml
# ========================================
# InfluxDB Database Design
# ========================================

influxdb_databases:
  - database: harvatek_timeseries
    description: Harvatek 時序數據主數據庫
    
    retention_policies:
      # 原始數據保留策略
      - name: raw_data
        duration: 7d
        replication: 1
        shard_duration: 1d
        default: true
        
        measurements:
          - equipment_status
          - equipment_parameters
          - facility_hvac
          - facility_utility
          - facility_power
          - environmental_sensors
      
      # 1 分鐘聚合數據
      - name: aggregated_1min
        duration: 90d
        replication: 1
        shard_duration: 7d
        default: false
        
        measurements:
          - equipment_status_1min
          - equipment_parameters_1min
          - facility_hvac_1min
          - facility_utility_1min
          - facility_power_1min
          - environmental_sensors_1min
      
      # 1 小時聚合數據
      - name: aggregated_1hour
        duration: 365d
        replication: 1
        shard_duration: 30d
        default: false
        
        measurements:
          - equipment_status_1hour
          - equipment_parameters_1hour
          - facility_hvac_1hour
          - facility_utility_1hour
          - facility_power_1hour
          - environmental_sensors_1hour
      
      # 1 天聚合數據
      - name: aggregated_1day
        duration: 1095d  # 3 years
        replication: 1
        shard_duration: 365d
        default: false
        
        measurements:
          - equipment_status_1day
          - equipment_parameters_1day
          - facility_hvac_1day
          - facility_utility_1day
          - facility_power_1day
          - environmental_sensors_1day
```

### 4.2 Measurement 設計

#### 4.2.1 生產設備 Measurements

```yaml
# ========================================
# Equipment Production Measurements
# ========================================

measurements:
  # 設備狀態
  - name: equipment_status
    description: 生產設備狀態數據
    
    tags:
      - equipment_id: 設備 ID (e.g., WB-2F-A-001)
      - equipment_type: 設備類型 (e.g., WireBonder)
      - floor: 樓層 (e.g., 2F)
      - area: 區域 (e.g., A)
      - line_id: 產線 ID (e.g., LINE-STD-01)
    
    fields:
      - status: string (IDLE, RUNNING, ALARM, MAINTENANCE, SETUP)
      - uph: integer (Units Per Hour)
      - total_count: integer
      - good_count: integer
      - reject_count: integer
      - yield_rate: float (%)
      - availability: float (%)
      - performance: float (%)
      - quality: float (%)
      - oee: float (%)
    
    indexes:
      - equipment_id
      - equipment_type
      - line_id
    
    example_query: |
      SELECT mean("oee") 
      FROM "equipment_status" 
      WHERE "equipment_type" = 'WireBonder' 
        AND time > now() - 1h 
      GROUP BY time(5m), "equipment_id"
  
  # 設備參數
  - name: equipment_parameters
    description: 生產設備工藝參數
    
    tags:
      - equipment_id: 設備 ID
      - equipment_type: 設備類型
      - recipe_id: 配方 ID
      - lot_id: 批次 ID
    
    fields:
      # Wire Bonder 參數範例
      - bonding_force: float (gf)
      - bonding_temperature: float (°C)
      - bonding_time: float (ms)
      - wire_tension: float (gf)
      - ultrasonic_power: float (%)
      
      # Die Bonder 參數範例
      - pickup_force: float (N)
      - bonding_force: float (N)
      - bonding_temperature: float (°C)
      - bonding_time: float (s)
      
      # Dispenser 參數範例
      - dispense_pressure: float (psi)
      - dispense_time: float (ms)
      - dot_size: float (mm)
      - viscosity: float (cP)
    
    example_query: |
      SELECT mean("bonding_temperature"), stddev("bonding_temperature")
      FROM "equipment_parameters"
      WHERE "equipment_id" = 'WB-2F-A-001'
        AND time > now() - 24h
      GROUP BY time(1h)
```

#### 4.2.2 設施系統 Measurements

```yaml
# ========================================
# Facility System Measurements
# ========================================

measurements:
  # HVAC 系統
  - name: facility_hvac
    description: HVAC 系統數據
    
    tags:
      - equipment_id: 設備 ID
      - equipment_type: 設備類型 (Chiller, AHU, FFU)
      - floor: 樓層
      - area: 區域
    
    fields:
      # Chiller
      - status: boolean
      - cooling_load: float (RT)
      - chw_supply_temp: float (°C)
      - chw_return_temp: float (°C)
      - cop: float
      - power_consumption: float (kW)
      
      # AHU
      - status: boolean
      - supply_air_temp: float (°C)
      - return_air_temp: float (°C)
      - supply_air_flow: float (m³/min)
      - filter_pressure_drop: float (Pa)
      - fan_speed: float (%)
      
      # FFU
      - status: boolean
      - air_velocity: float (m/s)
      - filter_pressure_drop: float (Pa)
    
    example_query: |
      SELECT mean("cop") as avg_cop, 
             sum("power_consumption") as total_power
      FROM "facility_hvac"
      WHERE "equipment_type" = 'Chiller'
        AND time > now() - 1d
      GROUP BY time(1h)
  
  # 公用系統
  - name: facility_utility
    description: 公用系統數據
    
    tags:
      - system_id: 系統 ID
      - system_type: 系統類型 (DIWater, Nitrogen, CDA, Vacuum)
      - location: 位置
    
    fields:
      # DI Water
      - flow_rate: float (lpm)
      - resistivity: float (MΩ·cm)
      - toc: float (ppb)
      - tank_level: float (%)
      
      # Nitrogen
      - flow_rate: float (Nm³/h)
      - purity: float (%)
      - output_pressure: float (bar)
      - dew_point: float (°C)
      
      # CDA
      - flow_rate: float (m³/min)
      - output_pressure: float (bar)
      - dew_point: float (°C)
      - motor_load: float (%)
    
    example_query: |
      SELECT mean("flow_rate"), max("flow_rate")
      FROM "facility_utility"
      WHERE "system_type" = 'Nitrogen'
        AND time > now() - 7d
      GROUP BY time(1h)
  
  # 電力系統
  - name: facility_power
    description: 電力系統數據
    
    tags:
      - meter_id: 電錶 ID
      - location: 位置
      - equipment_id: 設備 ID (可選)
      - cost_center: 成本中心
    
    fields:
      - voltage: float (V)
      - current: float (A)
      - power: float (kW)
      - energy: float (kWh)
      - power_factor: float
      - frequency: float (Hz)
    
    example_query: |
      SELECT sum("energy") as total_energy,
             mean("power") as avg_power
      FROM "facility_power"
      WHERE "cost_center" = '2F-Production'
        AND time > now() - 30d
      GROUP BY time(1d)
```

#### 4.2.3 環境感測器 Measurements

```yaml
# ========================================
# Environmental Sensor Measurements
# ========================================

measurements:
  - name: environmental_sensors
    description: 環境感測器數據
    
    tags:
      - sensor_id: 感測器 ID
      - sensor_type: 感測器類型 (Temperature, Humidity, Pressure, Particle)
      - floor: 樓層
      - area: 區域
      - location_x: X 座標
      - location_y: Y 座標
    
    fields:
      - temperature: float (°C)
      - humidity: float (%RH)
      - differential_pressure: float (Pa)
      - particle_0_3um: integer (particles/m³)
      - particle_0_5um: integer (particles/m³)
      - particle_1_0um: integer (particles/m³)
      - particle_5_0um: integer (particles/m³)
    
    example_query: |
      SELECT mean("temperature"), stddev("temperature"),
             mean("humidity"), stddev("humidity")
      FROM "environmental_sensors"
      WHERE "floor" = '2F' AND "area" = 'A'
        AND time > now() - 1d
      GROUP BY time(10m)
```

### 4.3 Continuous Queries (自動聚合)

```sql
-- ========================================
-- InfluxDB Continuous Queries
-- ========================================

-- 1 分鐘聚合 - 設備狀態
CREATE CONTINUOUS QUERY "cq_equipment_status_1min" ON "harvatek_timeseries"
BEGIN
  SELECT 
    mean("uph") AS "uph",
    sum("total_count") AS "total_count",
    sum("good_count") AS "good_count",
    sum("reject_count") AS "reject_count",
    mean("yield_rate") AS "yield_rate",
    mean("availability") AS "availability",
    mean("performance") AS "performance",
    mean("quality") AS "quality",
    mean("oee") AS "oee"
  INTO "harvatek_timeseries"."aggregated_1min"."equipment_status_1min"
  FROM "harvatek_timeseries"."raw_data"."equipment_status"
  GROUP BY time(1m), "equipment_id", "equipment_type", "floor", "area", "line_id"
END;

-- 1 小時聚合 - 設備狀態
CREATE CONTINUOUS QUERY "cq_equipment_status_1hour" ON "harvatek_timeseries"
BEGIN
  SELECT 
    mean("uph") AS "uph",
    sum("total_count") AS "total_count",
    sum("good_count") AS "good_count",
    sum("reject_count") AS "reject_count",
    mean("yield_rate") AS "yield_rate",
    mean("availability") AS "availability",
    mean("performance") AS "performance",
    mean("quality") AS "quality",
    mean("oee") AS "oee"
  INTO "harvatek_timeseries"."aggregated_1hour"."equipment_status_1hour"
  FROM "harvatek_timeseries"."aggregated_1min"."equipment_status_1min"
  GROUP BY time(1h), "equipment_id", "equipment_type", "floor", "area", "line_id"
END;

-- 1 天聚合 - 設備狀態
CREATE CONTINUOUS QUERY "cq_equipment_status_1day" ON "harvatek_timeseries"
BEGIN
  SELECT 
    mean("uph") AS "uph",
    sum("total_count") AS "total_count",
    sum("good_count") AS "good_count",
    sum("reject_count") AS "reject_count",
    mean("yield_rate") AS "yield_rate",
    mean("availability") AS "availability",
    mean("performance") AS "performance",
    mean("quality") AS "quality",
    mean("oee") AS "oee"
  INTO "harvatek_timeseries"."aggregated_1day"."equipment_status_1day"
  FROM "harvatek_timeseries"."aggregated_1hour"."equipment_status_1hour"
  GROUP BY time(1d), "equipment_id", "equipment_type", "floor", "area", "line_id"
END;

-- 1 分鐘聚合 - 電力數據
CREATE CONTINUOUS QUERY "cq_facility_power_1min" ON "harvatek_timeseries"
BEGIN
  SELECT 
    mean("voltage") AS "voltage",
    mean("current") AS "current",
    mean("power") AS "power",
    sum("energy") AS "energy",
    mean("power_factor") AS "power_factor"
  INTO "harvatek_timeseries"."aggregated_1min"."facility_power_1min"
  FROM "harvatek_timeseries"."raw_data"."facility_power"
  GROUP BY time(1m), "meter_id", "location", "equipment_id", "cost_center"
END;

-- 1 小時聚合 - 電力數據
CREATE CONTINUOUS QUERY "cq_facility_power_1hour" ON "harvatek_timeseries"
BEGIN
  SELECT 
    mean("voltage") AS "voltage",
    mean("current") AS "current",
    mean("power") AS "power",
    sum("energy") AS "energy",
    mean("power_factor") AS "power_factor"
  INTO "harvatek_timeseries"."aggregated_1hour"."facility_power_1hour"
  FROM "harvatek_timeseries"."aggregated_1min"."facility_power_1min"
  GROUP BY time(1h), "meter_id", "location", "equipment_id", "cost_center"
END;

-- 1 天聚合 - 電力數據
CREATE CONTINUOUS QUERY "cq_facility_power_1day" ON "harvatek_timeseries"
BEGIN
  SELECT 
    mean("voltage") AS "voltage",
    mean("current") AS "current",
    mean("power") AS "power",
    sum("energy") AS "energy",
    mean("power_factor") AS "power_factor"
  INTO "harvatek_timeseries"."aggregated_1day"."facility_power_1day"
  FROM "harvatek_timeseries"."aggregated_1hour"."facility_power_1hour"
  GROUP BY time(1d), "meter_id", "location", "equipment_id", "cost_center"
END;

-- 10 分鐘聚合 - 環境感測器
CREATE CONTINUOUS QUERY "cq_environmental_sensors_10min" ON "harvatek_timeseries"
BEGIN
  SELECT 
    mean("temperature") AS "temperature",
    stddev("temperature") AS "temperature_stddev",
    mean("humidity") AS "humidity",
    stddev("humidity") AS "humidity_stddev",
    mean("differential_pressure") AS "differential_pressure",
    mean("particle_0_3um") AS "particle_0_3um",
    mean("particle_0_5um") AS "particle_0_5um",
    mean("particle_1_0um") AS "particle_1_0um",
    mean("particle_5_0um") AS "particle_5_0um"
  INTO "harvatek_timeseries"."aggregated_1min"."environmental_sensors_1min"
  FROM "harvatek_timeseries"."raw_data"."environmental_sensors"
  GROUP BY time(10m), "sensor_id", "sensor_type", "floor", "area"
END;
```

---

## 5. PostgreSQL 關聯數據庫設計

### 5.1 數據庫架構

```sql
-- ========================================
-- PostgreSQL Database Schema
-- ========================================

CREATE DATABASE harvatek_relational;

\c harvatek_relational;

-- 啟用擴展
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";
CREATE EXTENSION IF NOT EXISTS "postgis";
```

### 5.2 資產管理表

```sql
-- ========================================
-- Asset Management Tables
-- ========================================

-- 資產主檔
CREATE TABLE assets (
    asset_id VARCHAR(50) PRIMARY KEY,
    asset_type VARCHAR(50) NOT NULL,
    asset_name VARCHAR(200) NOT NULL,
    description TEXT,
    manufacturer VARCHAR(100),
    model VARCHAR(100),
    serial_number VARCHAR(100),
    installation_date DATE,
    location_building VARCHAR(50),
    location_floor VARCHAR(10),
    location_area VARCHAR(50),
    location_coordinates GEOMETRY(POINT, 3857),
    parent_asset_id VARCHAR(50) REFERENCES assets(asset_id),
    status VARCHAR(20) DEFAULT 'ACTIVE',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    metadata JSONB
);

CREATE INDEX idx_assets_type ON assets(asset_type);
CREATE INDEX idx_assets_location ON assets(location_floor, location_area);
CREATE INDEX idx_assets_parent ON assets(parent_asset_id);
CREATE INDEX idx_assets_coordinates ON assets USING GIST(location_coordinates);

-- 資產屬性
CREATE TABLE asset_properties (
    property_id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    asset_id VARCHAR(50) REFERENCES assets(asset_id) ON DELETE CASCADE,
    property_name VARCHAR(100) NOT NULL,
    property_value TEXT,
    property_type VARCHAR(20),
    unit VARCHAR(20),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_asset_properties_asset ON asset_properties(asset_id);

-- 資產連接關係
CREATE TABLE asset_connections (
    connection_id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    source_asset_id VARCHAR(50) REFERENCES assets(asset_id) ON DELETE CASCADE,
    target_asset_id VARCHAR(50) REFERENCES assets(asset_id) ON DELETE CASCADE,
    connection_type VARCHAR(50) NOT NULL,
    connection_medium VARCHAR(50),
    properties JSONB,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_asset_connections_source ON asset_connections(source_asset_id);
CREATE INDEX idx_asset_connections_target ON asset_connections(target_asset_id);

-- 資產文件
CREATE TABLE asset_documents (
    document_id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    asset_id VARCHAR(50) REFERENCES assets(asset_id) ON DELETE CASCADE,
    document_type VARCHAR(50) NOT NULL,
    document_name VARCHAR(200) NOT NULL,
    file_path TEXT,
    file_url TEXT,
    file_size_bytes BIGINT,
    mime_type VARCHAR(100),
    uploaded_by VARCHAR(100),
    uploaded_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    metadata JSONB
);

CREATE INDEX idx_asset_documents_asset ON asset_documents(asset_id);
CREATE INDEX idx_asset_documents_type ON asset_documents(document_type);
```

### 5.3 維護管理表

```sql
-- ========================================
-- Maintenance Management Tables
-- ========================================

-- 維護計畫
CREATE TABLE maintenance_plans (
    plan_id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    asset_id VARCHAR(50) REFERENCES assets(asset_id) ON DELETE CASCADE,
    plan_name VARCHAR(200) NOT NULL,
    plan_type VARCHAR(50) NOT NULL,  -- PREVENTIVE, PREDICTIVE, CORRECTIVE
    frequency_type VARCHAR(20),  -- DAILY, WEEKLY, MONTHLY, QUARTERLY, YEARLY, HOURS
    frequency_value INTEGER,
    last_maintenance_date TIMESTAMP,
    next_maintenance_date TIMESTAMP,
    estimated_duration_hours DECIMAL(10, 2),
    priority VARCHAR(20),
    status VARCHAR(20) DEFAULT 'ACTIVE',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    metadata JSONB
);

CREATE INDEX idx_maintenance_plans_asset ON maintenance_plans(asset_id);
CREATE INDEX idx_maintenance_plans_next_date ON maintenance_plans(next_maintenance_date);

-- 維護工單
CREATE TABLE maintenance_orders (
    order_id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    order_number VARCHAR(50) UNIQUE NOT NULL,
    asset_id VARCHAR(50) REFERENCES assets(asset_id),
    plan_id UUID REFERENCES maintenance_plans(plan_id),
    order_type VARCHAR(50) NOT NULL,  -- PREVENTIVE, CORRECTIVE, EMERGENCY
    priority VARCHAR(20) NOT NULL,  -- CRITICAL, HIGH, MEDIUM, LOW
    status VARCHAR(20) NOT NULL DEFAULT 'CREATED',  -- CREATED, SCHEDULED, IN_PROGRESS, COMPLETED, CANCELLED
    description TEXT,
    scheduled_start TIMESTAMP,
    scheduled_end TIMESTAMP,
    actual_start TIMESTAMP,
    actual_end TIMESTAMP,
    assigned_to VARCHAR(100),
    created_by VARCHAR(100),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    metadata JSONB
);

CREATE INDEX idx_maintenance_orders_asset ON maintenance_orders(asset_id);
CREATE INDEX idx_maintenance_orders_status ON maintenance_orders(status);
CREATE INDEX idx_maintenance_orders_scheduled ON maintenance_orders(scheduled_start);

-- 維護工單明細
CREATE TABLE maintenance_order_items (
    item_id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    order_id UUID REFERENCES maintenance_orders(order_id) ON DELETE CASCADE,
    task_description TEXT NOT NULL,
    task_type VARCHAR(50),
    status VARCHAR(20) DEFAULT 'PENDING',
    assigned_to VARCHAR(100),
    estimated_hours DECIMAL(10, 2),
    actual_hours DECIMAL(10, 2),
    notes TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_maintenance_order_items_order ON maintenance_order_items(order_id);

-- 備品備件
CREATE TABLE spare_parts (
    part_id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    part_number VARCHAR(100) UNIQUE NOT NULL,
    part_name VARCHAR(200) NOT NULL,
    description TEXT,
    manufacturer VARCHAR(100),
    unit_price DECIMAL(15, 2),
    currency VARCHAR(10) DEFAULT 'USD',
    stock_quantity INTEGER DEFAULT 0,
    min_stock_level INTEGER,
    max_stock_level INTEGER,
    location VARCHAR(100),
    status VARCHAR(20) DEFAULT 'ACTIVE',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    metadata JSONB
);

CREATE INDEX idx_spare_parts_number ON spare_parts(part_number);

-- 維護工單備件使用
CREATE TABLE maintenance_order_parts (
    usage_id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    order_id UUID REFERENCES maintenance_orders(order_id) ON DELETE CASCADE,
    part_id UUID REFERENCES spare_parts(part_id),
    quantity_used INTEGER NOT NULL,
    unit_cost DECIMAL(15, 2),
    total_cost DECIMAL(15, 2),
    used_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    used_by VARCHAR(100)
);

CREATE INDEX idx_maintenance_order_parts_order ON maintenance_order_parts(order_id);
CREATE INDEX idx_maintenance_order_parts_part ON maintenance_order_parts(part_id);
```

### 5.4 生產管理表

```sql
-- ========================================
-- Production Management Tables
-- ========================================

-- 生產訂單
CREATE TABLE production_orders (
    order_id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    order_number VARCHAR(50) UNIQUE NOT NULL,
    erp_order_number VARCHAR(50),
    product_type VARCHAR(100) NOT NULL,
    product_name VARCHAR(200),
    quantity INTEGER NOT NULL,
    unit VARCHAR(20),
    status VARCHAR(20) NOT NULL DEFAULT 'CREATED',
    priority VARCHAR(20),
    scheduled_start TIMESTAMP,
    scheduled_end TIMESTAMP,
    actual_start TIMESTAMP,
    actual_end TIMESTAMP,
    line_id VARCHAR(50),
    created_by VARCHAR(100),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    metadata JSONB
);

CREATE INDEX idx_production_orders_number ON production_orders(order_number);
CREATE INDEX idx_production_orders_status ON production_orders(status);
CREATE INDEX idx_production_orders_scheduled ON production_orders(scheduled_start);

-- 生產批次
CREATE TABLE production_lots (
    lot_id VARCHAR(50) PRIMARY KEY,
    order_id UUID REFERENCES production_orders(order_id),
    product_type VARCHAR(100) NOT NULL,
    quantity INTEGER NOT NULL,
    status VARCHAR(20) NOT NULL DEFAULT 'CREATED',
    recipe_id VARCHAR(50),
    start_time TIMESTAMP,
    end_time TIMESTAMP,
    current_equipment_id VARCHAR(50),
    current_process_step VARCHAR(100),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    metadata JSONB
);

CREATE INDEX idx_production_lots_order ON production_lots(order_id);
CREATE INDEX idx_production_lots_status ON production_lots(status);

-- 生產事件
CREATE TABLE production_events (
    event_id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    event_type VARCHAR(50) NOT NULL,
    lot_id VARCHAR(50) REFERENCES production_lots(lot_id),
    equipment_id VARCHAR(50),
    event_time TIMESTAMP NOT NULL,
    event_data JSONB,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_production_events_lot ON production_events(lot_id);
CREATE INDEX idx_production_events_equipment ON production_events(equipment_id);
CREATE INDEX idx_production_events_time ON production_events(event_time);

-- 配方管理
CREATE TABLE recipes (
    recipe_id VARCHAR(50) PRIMARY KEY,
    recipe_name VARCHAR(200) NOT NULL,
    product_type VARCHAR(100) NOT NULL,
    version VARCHAR(20),
    status VARCHAR(20) DEFAULT 'ACTIVE',
    parameters JSONB NOT NULL,
    created_by VARCHAR(100),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    metadata JSONB
);

CREATE INDEX idx_recipes_product_type ON recipes(product_type);
CREATE INDEX idx_recipes_status ON recipes(status);
```

### 5.5 品質管理表

```sql
-- ========================================
-- Quality Management Tables
-- ========================================

-- 品質測試
CREATE TABLE quality_tests (
    test_id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    lot_id VARCHAR(50) REFERENCES production_lots(lot_id),
    test_type VARCHAR(50) NOT NULL,
    test_equipment_id VARCHAR(50),
    test_time TIMESTAMP NOT NULL,
    test_result VARCHAR(20) NOT NULL,  -- PASS, FAIL, RETEST
    test_data JSONB,
    tested_by VARCHAR(100),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_quality_tests_lot ON quality_tests(lot_id);
CREATE INDEX idx_quality_tests_type ON quality_tests(test_type);
CREATE INDEX idx_quality_tests_time ON quality_tests(test_time);

-- 缺陷記錄
CREATE TABLE defect_records (
    defect_id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    lot_id VARCHAR(50) REFERENCES production_lots(lot_id),
    equipment_id VARCHAR(50),
    defect_type VARCHAR(50) NOT NULL,
    defect_code VARCHAR(50),
    defect_description TEXT,
    quantity INTEGER,
    detected_at TIMESTAMP NOT NULL,
    detected_by VARCHAR(100),
    root_cause TEXT,
    corrective_action TEXT,
    status VARCHAR(20) DEFAULT 'OPEN',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_defect_records_lot ON defect_records(lot_id);
CREATE INDEX idx_defect_records_equipment ON defect_records(equipment_id);
CREATE INDEX idx_defect_records_type ON defect_records(defect_type);
```

### 5.6 告警管理表

```sql
-- ========================================
-- Alarm Management Tables
-- ========================================

-- 告警記錄
CREATE TABLE alarms (
    alarm_id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    alarm_code VARCHAR(50) NOT NULL,
    equipment_id VARCHAR(50),
    alarm_type VARCHAR(50) NOT NULL,
    severity VARCHAR(20) NOT NULL,  -- CRITICAL, HIGH, MEDIUM, LOW
    alarm_text TEXT NOT NULL,
    alarm_time TIMESTAMP NOT NULL,
    acknowledged_at TIMESTAMP,
    acknowledged_by VARCHAR(100),
    cleared_at TIMESTAMP,
    cleared_by VARCHAR(100),
    status VARCHAR(20) NOT NULL DEFAULT 'ACTIVE',  -- ACTIVE, ACKNOWLEDGED, CLEARED
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    metadata JSONB
);

CREATE INDEX idx_alarms_equipment ON alarms(equipment_id);
CREATE INDEX idx_alarms_time ON alarms(alarm_time);
CREATE INDEX idx_alarms_status ON alarms(status);
CREATE INDEX idx_alarms_severity ON alarms(severity);

-- 告警規則
CREATE TABLE alarm_rules (
    rule_id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    rule_name VARCHAR(200) NOT NULL,
    equipment_id VARCHAR(50),
    equipment_type VARCHAR(50),
    parameter_name VARCHAR(100) NOT NULL,
    condition_type VARCHAR(20) NOT NULL,  -- GREATER_THAN, LESS_THAN, EQUAL, NOT_EQUAL, RANGE
    threshold_value DECIMAL(15, 4),
    threshold_min DECIMAL(15, 4),
    threshold_max DECIMAL(15, 4),
    severity VARCHAR(20) NOT NULL,
    enabled BOOLEAN DEFAULT true,
    notification_channels JSONB,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_alarm_rules_equipment ON alarm_rules(equipment_id);
CREATE INDEX idx_alarm_rules_type ON alarm_rules(equipment_type);
```

### 5.7 使用者和權限管理表

```sql
-- ========================================
-- User and Permission Management Tables
-- ========================================

-- 使用者
CREATE TABLE users (
    user_id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    username VARCHAR(100) UNIQUE NOT NULL,
    email VARCHAR(200) UNIQUE NOT NULL,
    full_name VARCHAR(200),
    department VARCHAR(100),
    role VARCHAR(50),
    status VARCHAR(20) DEFAULT 'ACTIVE',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    last_login TIMESTAMP,
    metadata JSONB
);

CREATE INDEX idx_users_username ON users(username);
CREATE INDEX idx_users_email ON users(email);

-- 角色
CREATE TABLE roles (
    role_id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    role_name VARCHAR(100) UNIQUE NOT NULL,
    description TEXT,
    permissions JSONB,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 使用者角色關聯
CREATE TABLE user_roles (
    user_id UUID REFERENCES users(user_id) ON DELETE CASCADE,
    role_id UUID REFERENCES roles(role_id) ON DELETE CASCADE,
    assigned_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    assigned_by UUID REFERENCES users(user_id),
    PRIMARY KEY (user_id, role_id)
);
```

---

*文件未完,繼續下一部分...*




## 6. Connector 配置

### 6.1 SECS/GEM Connector

```yaml
# ========================================
# SECS/GEM Connector Configuration
# ========================================

secs_gem_connector:
  name: harvatek-secs-gem-connector
  version: 3.1.0
  
  connection:
    protocol: HSMS  # High-Speed SECS Message Services
    mode: passive  # Equipment as active, connector as passive
    port: 5000
    t3_timeout_ms: 45000
    t5_timeout_ms: 10000
    t6_timeout_ms: 5000
    t7_timeout_ms: 10000
    t8_timeout_ms: 5000
  
  equipment_list:
    - equipment_id: WB-2F-A-001
      ip_address: 192.168.10.101
      port: 5000
      device_id: 1
      enabled: true
      
      data_collection:
        # 狀態變數
        status_variables:
          - svid: 1
            name: EquipmentStatus
            data_type: string
            poll_interval_ms: 1000
          
          - svid: 10
            name: UPH
            data_type: integer
            poll_interval_ms: 5000
          
          - svid: 11
            name: TotalCount
            data_type: integer
            poll_interval_ms: 1000
          
          - svid: 12
            name: GoodCount
            data_type: integer
            poll_interval_ms: 1000
          
          - svid: 13
            name: RejectCount
            data_type: integer
            poll_interval_ms: 1000
        
        # 事件訂閱
        events:
          - ceid: 1
            name: ProcessStart
            enabled: true
          
          - ceid: 2
            name: ProcessEnd
            enabled: true
          
          - ceid: 10
            name: AlarmSet
            enabled: true
          
          - ceid: 11
            name: AlarmCleared
            enabled: true
        
        # 告警訂閱
        alarms:
          enabled: true
          all_alarms: true
      
      kafka_output:
        topic: equipment.production.events
        key_field: equipment_id
        partition_strategy: equipment_id_hash
  
  # 批次配置範例 (可用於所有設備)
  batch_configuration:
    - equipment_type: WireBonder
      equipment_ids: [WB-2F-A-001, WB-2F-A-002, ..., WB-2F-B-040]
      common_config:
        status_variables: [1, 10, 11, 12, 13]
        events: [1, 2, 10, 11]
        alarms_enabled: true

# Python 實現範例
python_implementation: |
  import secsgem
  from kafka import KafkaProducer
  import json
  
  class SecsGemConnector:
      def __init__(self, config):
          self.config = config
          self.producer = KafkaProducer(
              bootstrap_servers=['kafka-1:9092', 'kafka-2:9092', 'kafka-3:9092'],
              value_serializer=lambda v: json.dumps(v).encode('utf-8')
          )
      
      def connect_equipment(self, equipment_config):
          # 建立 HSMS 連接
          connection = secsgem.HsmsPassiveConnection(
              address=equipment_config['ip_address'],
              port=equipment_config['port'],
              device_id=equipment_config['device_id']
          )
          
          # 註冊事件處理器
          connection.register_callback(
              secsgem.SecsS06F11,
              self.on_event_report
          )
          
          connection.register_callback(
              secsgem.SecsS05F01,
              self.on_alarm
          )
          
          connection.enable()
          return connection
      
      def on_event_report(self, connection, packet):
          # 處理事件報告
          ceid = packet.get_value('CEID')
          reports = packet.get_value('REPORTS')
          
          event_data = {
              'timestamp': int(time.time() * 1000),
              'equipment_id': connection.equipment_id,
              'event_type': 'CEID_' + str(ceid),
              'data': self.parse_reports(reports)
          }
          
          # 發送到 Kafka
          self.producer.send(
              'equipment.production.events',
              key=connection.equipment_id.encode('utf-8'),
              value=event_data
          )
      
      def on_alarm(self, connection, packet):
          # 處理告警
          alid = packet.get_value('ALID')
          alcd = packet.get_value('ALCD')
          altx = packet.get_value('ALTX')
          
          alarm_data = {
              'timestamp': int(time.time() * 1000),
              'equipment_id': connection.equipment_id,
              'alarm_id': alid,
              'alarm_code': alcd,
              'alarm_text': altx,
              'severity': self.determine_severity(alcd),
              'status': 'ACTIVE' if alcd == 128 else 'CLEARED'
          }
          
          # 發送到 Kafka
          self.producer.send(
              'alarm.equipment',
              key=connection.equipment_id.encode('utf-8'),
              value=alarm_data
          )
```

### 6.2 OPC UA Connector

```yaml
# ========================================
# OPC UA Connector Configuration
# ========================================

opcua_connector:
  name: harvatek-opcua-connector
  version: 3.1.0
  
  servers:
    - server_id: power-monitor
      endpoint_url: opc.tcp://192.168.1.100:4840
      security_mode: SignAndEncrypt
      security_policy: Basic256Sha256
      authentication:
        type: username_password
        username: ${OPCUA_USERNAME}
        password: ${OPCUA_PASSWORD}
      
      subscriptions:
        - subscription_id: power_meters
          publishing_interval_ms: 5000
          
          monitored_items:
            # 電力計 - WB-2F-A-001
            - node_id: ns=2;s=PowerMeter.WB-2F-A-001.Voltage
              sampling_interval_ms: 5000
              queue_size: 10
              
            - node_id: ns=2;s=PowerMeter.WB-2F-A-001.Current
              sampling_interval_ms: 5000
              queue_size: 10
            
            - node_id: ns=2;s=PowerMeter.WB-2F-A-001.Power
              sampling_interval_ms: 5000
              queue_size: 10
            
            - node_id: ns=2;s=PowerMeter.WB-2F-A-001.Energy
              sampling_interval_ms: 60000
              queue_size: 10
            
            # ... (其他 340 台設備的電力計)
      
      kafka_output:
        topic: facility.power.events
        key_field: meter_id
        partition_strategy: meter_id_hash

# Python 實現範例
python_implementation: |
  from opcua import Client, ua
  from kafka import KafkaProducer
  import json
  import time
  
  class OpcUaConnector:
      def __init__(self, config):
          self.config = config
          self.producer = KafkaProducer(
              bootstrap_servers=['kafka-1:9092', 'kafka-2:9092', 'kafka-3:9092'],
              value_serializer=lambda v: json.dumps(v).encode('utf-8')
          )
      
      def connect_server(self, server_config):
          client = Client(server_config['endpoint_url'])
          client.set_security_string(
              f"{server_config['security_policy']},{server_config['security_mode']},"
              f"{server_config['authentication']['username']},"
              f"{server_config['authentication']['password']}"
          )
          client.connect()
          return client
      
      def create_subscription(self, client, subscription_config):
          handler = DataChangeHandler(self.producer, subscription_config['kafka_output'])
          subscription = client.create_subscription(
              subscription_config['publishing_interval_ms'],
              handler
          )
          
          for item in subscription_config['monitored_items']:
              node = client.get_node(item['node_id'])
              subscription.subscribe_data_change(node)
          
          return subscription
  
  class DataChangeHandler:
      def __init__(self, producer, kafka_config):
          self.producer = producer
          self.kafka_config = kafka_config
      
      def datachange_notification(self, node, val, data):
          # 解析 node_id 取得 meter_id
          node_id = str(node.nodeid)
          meter_id = self.extract_meter_id(node_id)
          parameter = self.extract_parameter(node_id)
          
          event_data = {
              'timestamp': int(time.time() * 1000),
              'meter_id': meter_id,
              'parameter': parameter,
              'value': val,
              'quality': str(data.monitored_item.Value.StatusCode)
          }
          
          # 發送到 Kafka
          self.producer.send(
              self.kafka_config['topic'],
              key=meter_id.encode('utf-8'),
              value=event_data
          )
```

### 6.3 Modbus TCP Connector

```yaml
# ========================================
# Modbus TCP Connector Configuration
# ========================================

modbus_connector:
  name: harvatek-modbus-connector
  version: 3.1.0
  
  devices:
    - device_id: DI-WATER-SYSTEM-MAIN
      ip_address: 192.168.1.101
      port: 502
      unit_id: 1
      poll_interval_ms: 10000
      
      registers:
        - register_type: holding
          start_address: 40001
          count: 1
          data_type: boolean
          parameter: Status
        
        - register_type: holding
          start_address: 40002
          count: 2
          data_type: float32
          parameter: FlowRate
          unit: lpm
        
        - register_type: holding
          start_address: 40004
          count: 2
          data_type: float32
          parameter: Resistivity
          unit: mohm_cm
        
        - register_type: holding
          start_address: 40006
          count: 2
          data_type: float32
          parameter: TOC
          unit: ppb
      
      kafka_output:
        topic: facility.utility.events
        key_field: device_id

# Python 實現範例
python_implementation: |
  from pymodbus.client import ModbusTcpClient
  from kafka import KafkaProducer
  import json
  import struct
  import time
  
  class ModbusConnector:
      def __init__(self, config):
          self.config = config
          self.producer = KafkaProducer(
              bootstrap_servers=['kafka-1:9092', 'kafka-2:9092', 'kafka-3:9092'],
              value_serializer=lambda v: json.dumps(v).encode('utf-8')
          )
      
      def connect_device(self, device_config):
          client = ModbusTcpClient(
              host=device_config['ip_address'],
              port=device_config['port']
          )
          client.connect()
          return client
      
      def poll_device(self, client, device_config):
          data = {}
          
          for register in device_config['registers']:
              # 讀取寄存器
              if register['register_type'] == 'holding':
                  result = client.read_holding_registers(
                      address=register['start_address'] - 40001,
                      count=register['count'],
                      slave=device_config['unit_id']
                  )
              
              # 解析數據
              if register['data_type'] == 'boolean':
                  value = bool(result.registers[0])
              elif register['data_type'] == 'float32':
                  # 組合兩個寄存器為 float32
                  bytes_data = struct.pack('>HH', result.registers[0], result.registers[1])
                  value = struct.unpack('>f', bytes_data)[0]
              
              data[register['parameter']] = value
          
          # 發送到 Kafka
          event_data = {
              'timestamp': int(time.time() * 1000),
              'device_id': device_config['device_id'],
              'data': data
          }
          
          self.producer.send(
              device_config['kafka_output']['topic'],
              key=device_config['device_id'].encode('utf-8'),
              value=event_data
          )
```

### 6.4 BACnet Connector

```yaml
# ========================================
# BACnet Connector Configuration
# ========================================

bacnet_connector:
  name: harvatek-bacnet-connector
  version: 3.1.0
  
  network:
    interface: eth0
    port: 47808
    device_id: 999999
  
  devices:
    - device_id: CHILLER-01
      device_instance: 201
      ip_address: 192.168.1.201
      poll_interval_ms: 10000
      
      objects:
        - object_type: BINARY_VALUE
          object_instance: 1
          property: present_value
          parameter: Status
        
        - object_type: ANALOG_INPUT
          object_instance: 1
          property: present_value
          parameter: CoolingLoad
          unit: RT
        
        - object_type: ANALOG_INPUT
          object_instance: 2
          property: present_value
          parameter: CHW_SupplyTemp
          unit: celsius
        
        - object_type: ANALOG_INPUT
          object_instance: 3
          property: present_value
          parameter: CHW_ReturnTemp
          unit: celsius
      
      kafka_output:
        topic: facility.hvac.events
        key_field: device_id

# Python 實現範例
python_implementation: |
  from bacpypes.app import BIPSimpleApplication
  from bacpypes.object import get_datatype
  from bacpypes.apdu import ReadPropertyRequest
  from kafka import KafkaProducer
  import json
  import time
  
  class BacnetConnector(BIPSimpleApplication):
      def __init__(self, config):
          super().__init__(config['network']['device_id'], config['network']['ip_address'])
          self.config = config
          self.producer = KafkaProducer(
              bootstrap_servers=['kafka-1:9092', 'kafka-2:9092', 'kafka-3:9092'],
              value_serializer=lambda v: json.dumps(v).encode('utf-8')
          )
      
      def read_property(self, device_config, object_config):
          # 建立讀取請求
          request = ReadPropertyRequest(
              objectIdentifier=(object_config['object_type'], object_config['object_instance']),
              propertyIdentifier=object_config['property']
          )
          request.pduDestination = Address(device_config['ip_address'])
          
          # 發送請求並等待回應
          iocb = IOCB(request)
          self.request_io(iocb)
          iocb.wait()
          
          if iocb.ioResponse:
              apdu = iocb.ioResponse
              datatype = get_datatype(
                  apdu.objectIdentifier[0],
                  apdu.propertyIdentifier
              )
              value = apdu.propertyValue.cast_out(datatype)
              return value
          
          return None
      
      def poll_device(self, device_config):
          data = {}
          
          for obj in device_config['objects']:
              value = self.read_property(device_config, obj)
              if value is not None:
                  data[obj['parameter']] = value
          
          # 發送到 Kafka
          event_data = {
              'timestamp': int(time.time() * 1000),
              'device_id': device_config['device_id'],
              'data': data
          }
          
          self.producer.send(
              device_config['kafka_output']['topic'],
              key=device_config['device_id'].encode('utf-8'),
              value=event_data
          )
```

---

## 7. 實時數據處理管線

### 7.1 Kafka Streams 處理

```yaml
# ========================================
# Kafka Streams Processing
# ========================================

kafka_streams_applications:
  # OEE 計算
  - application_id: oee-calculator
    description: 實時計算設備 OEE
    
    input_topics:
      - equipment.production.events
    
    output_topics:
      - equipment.production.oee
    
    processing_logic: |
      // 計算 OEE = Availability × Performance × Quality
      
      // Availability = (Operating Time / Planned Production Time) × 100%
      // Performance = (Actual Output / Theoretical Output) × 100%
      // Quality = (Good Units / Total Units) × 100%
      
      KStream<String, EquipmentEvent> events = builder.stream("equipment.production.events");
      
      KTable<Windowed<String>, OEEMetrics> oee = events
          .groupByKey()
          .windowedBy(TimeWindows.of(Duration.ofMinutes(5)))
          .aggregate(
              OEEMetrics::new,
              (key, event, metrics) -> metrics.update(event),
              Materialized.with(Serdes.String(), oeeMetricsSerde)
          );
      
      oee.toStream()
          .map((windowedKey, metrics) -> KeyValue.pair(
              windowedKey.key(),
              metrics.calculate()
          ))
          .to("equipment.production.oee");
  
  # 能源消耗聚合
  - application_id: energy-aggregator
    description: 實時聚合能源消耗數據
    
    input_topics:
      - facility.power.events
    
    output_topics:
      - facility.power.aggregated
    
    processing_logic: |
      KStream<String, PowerEvent> powerEvents = builder.stream("facility.power.events");
      
      // 按樓層聚合
      KTable<Windowed<String>, PowerAggregation> floorAggregation = powerEvents
          .map((key, event) -> KeyValue.pair(event.getFloor(), event))
          .groupByKey()
          .windowedBy(TimeWindows.of(Duration.ofMinutes(1)))
          .aggregate(
              PowerAggregation::new,
              (floor, event, agg) -> agg.add(event),
              Materialized.with(Serdes.String(), powerAggregationSerde)
          );
      
      // 按區域聚合
      KTable<Windowed<String>, PowerAggregation> areaAggregation = powerEvents
          .map((key, event) -> KeyValue.pair(event.getArea(), event))
          .groupByKey()
          .windowedBy(TimeWindows.of(Duration.ofMinutes(1)))
          .aggregate(
              PowerAggregation::new,
              (area, event, agg) -> agg.add(event),
              Materialized.with(Serdes.String(), powerAggregationSerde)
          );
      
      floorAggregation.toStream().to("facility.power.aggregated.floor");
      areaAggregation.toStream().to("facility.power.aggregated.area");
  
  # 告警檢測
  - application_id: alarm-detector
    description: 實時檢測異常並產生告警
    
    input_topics:
      - equipment.production.events
      - facility.hvac.events
      - sensor.environmental.events
    
    output_topics:
      - alarm.equipment
      - alarm.facility
      - alarm.environmental
    
    processing_logic: |
      // 載入告警規則
      GlobalKTable<String, AlarmRule> alarmRules = builder.globalTable("alarm.rules");
      
      // 處理設備事件
      KStream<String, EquipmentEvent> equipmentEvents = builder.stream("equipment.production.events");
      
      KStream<String, Alarm> equipmentAlarms = equipmentEvents
          .join(
              alarmRules,
              (eventKey, event) -> event.getEquipmentId(),
              (event, rule) -> rule.evaluate(event)
          )
          .filter((key, alarm) -> alarm != null);
      
      equipmentAlarms.to("alarm.equipment");
```

### 7.2 實時數據處理服務

```python
# ========================================
# Real-time Data Processing Service
# ========================================

from kafka import KafkaConsumer, KafkaProducer
from influxdb_client import InfluxDBClient, Point
from influxdb_client.client.write_api import SYNCHRONOUS
import json
import logging

class RealTimeDataProcessor:
    def __init__(self, config):
        self.config = config
        
        # Kafka Consumer
        self.consumer = KafkaConsumer(
            *config['input_topics'],
            bootstrap_servers=config['kafka_brokers'],
            group_id=config['consumer_group'],
            value_deserializer=lambda m: json.loads(m.decode('utf-8')),
            auto_offset_reset='latest',
            enable_auto_commit=True
        )
        
        # InfluxDB Client
        self.influx_client = InfluxDBClient(
            url=config['influxdb_url'],
            token=config['influxdb_token'],
            org=config['influxdb_org']
        )
        self.write_api = self.influx_client.write_api(write_options=SYNCHRONOUS)
        
        # Kafka Producer (for processed data)
        self.producer = KafkaProducer(
            bootstrap_servers=config['kafka_brokers'],
            value_serializer=lambda v: json.dumps(v).encode('utf-8')
        )
    
    def process_equipment_event(self, event):
        """處理設備事件"""
        # 寫入 InfluxDB
        point = Point("equipment_status") \
            .tag("equipment_id", event['equipment_id']) \
            .tag("equipment_type", event['equipment_type']) \
            .tag("floor", event.get('floor', '')) \
            .tag("area", event.get('area', '')) \
            .field("status", event['data']['status']) \
            .field("uph", event['data'].get('uph', 0)) \
            .field("total_count", event['data'].get('total_count', 0)) \
            .field("good_count", event['data'].get('good_count', 0)) \
            .field("reject_count", event['data'].get('reject_count', 0)) \
            .time(event['timestamp'], write_precision='ms')
        
        self.write_api.write(
            bucket=self.config['influxdb_bucket'],
            record=point
        )
        
        # 計算衍生指標
        if event['data'].get('total_count', 0) > 0:
            yield_rate = (event['data']['good_count'] / event['data']['total_count']) * 100
            
            # 發送衍生數據到 Kafka
            derived_event = {
                'timestamp': event['timestamp'],
                'equipment_id': event['equipment_id'],
                'yield_rate': yield_rate
            }
            
            self.producer.send(
                'equipment.production.derived',
                key=event['equipment_id'].encode('utf-8'),
                value=derived_event
            )
    
    def process_power_event(self, event):
        """處理電力事件"""
        point = Point("facility_power") \
            .tag("meter_id", event['meter_id']) \
            .tag("location", event.get('location', '')) \
            .field("voltage", event['data'].get('voltage', 0)) \
            .field("current", event['data'].get('current', 0)) \
            .field("power", event['data'].get('power', 0)) \
            .field("energy", event['data'].get('energy', 0)) \
            .field("power_factor", event['data'].get('power_factor', 0)) \
            .time(event['timestamp'], write_precision='ms')
        
        self.write_api.write(
            bucket=self.config['influxdb_bucket'],
            record=point
        )
    
    def run(self):
        """主處理循環"""
        logging.info("Starting real-time data processor...")
        
        for message in self.consumer:
            try:
                topic = message.topic
                event = message.value
                
                if topic == 'equipment.production.events':
                    self.process_equipment_event(event)
                elif topic == 'facility.power.events':
                    self.process_power_event(event)
                # ... 其他主題處理
                
            except Exception as e:
                logging.error(f"Error processing message: {e}")
                continue

# 啟動服務
if __name__ == '__main__':
    config = {
        'input_topics': [
            'equipment.production.events',
            'facility.power.events',
            'facility.hvac.events'
        ],
        'kafka_brokers': ['kafka-1:9092', 'kafka-2:9092', 'kafka-3:9092'],
        'consumer_group': 'real-time-processor',
        'influxdb_url': 'http://influxdb-cluster:8086',
        'influxdb_token': '${INFLUXDB_TOKEN}',
        'influxdb_org': 'harvatek',
        'influxdb_bucket': 'harvatek_timeseries'
    }
    
    processor = RealTimeDataProcessor(config)
    processor.run()
```

---

## 8. 告警和事件管理

### 8.1 告警規則引擎

```python
# ========================================
# Alarm Rule Engine
# ========================================

from typing import Dict, List, Any
import logging

class AlarmRuleEngine:
    def __init__(self, db_connection):
        self.db = db_connection
        self.rules = self.load_rules()
    
    def load_rules(self) -> Dict[str, List[AlarmRule]]:
        """從數據庫載入告警規則"""
        query = """
            SELECT rule_id, equipment_id, equipment_type, parameter_name,
                   condition_type, threshold_value, threshold_min, threshold_max,
                   severity, enabled, notification_channels
            FROM alarm_rules
            WHERE enabled = true
        """
        
        rules = {}
        for row in self.db.execute(query):
            rule = AlarmRule(
                rule_id=row['rule_id'],
                equipment_id=row['equipment_id'],
                equipment_type=row['equipment_type'],
                parameter_name=row['parameter_name'],
                condition_type=row['condition_type'],
                threshold_value=row['threshold_value'],
                threshold_min=row['threshold_min'],
                threshold_max=row['threshold_max'],
                severity=row['severity'],
                notification_channels=row['notification_channels']
            )
            
            key = row['equipment_id'] or row['equipment_type']
            if key not in rules:
                rules[key] = []
            rules[key].append(rule)
        
        return rules
    
    def evaluate(self, event: Dict[str, Any]) -> List[Alarm]:
        """評估事件是否觸發告警"""
        alarms = []
        
        # 取得適用的規則
        equipment_id = event.get('equipment_id')
        equipment_type = event.get('equipment_type')
        
        applicable_rules = []
        if equipment_id and equipment_id in self.rules:
            applicable_rules.extend(self.rules[equipment_id])
        if equipment_type and equipment_type in self.rules:
            applicable_rules.extend(self.rules[equipment_type])
        
        # 評估每個規則
        for rule in applicable_rules:
            parameter_value = event.get('data', {}).get(rule.parameter_name)
            if parameter_value is None:
                continue
            
            is_triggered = False
            
            if rule.condition_type == 'GREATER_THAN':
                is_triggered = parameter_value > rule.threshold_value
            elif rule.condition_type == 'LESS_THAN':
                is_triggered = parameter_value < rule.threshold_value
            elif rule.condition_type == 'EQUAL':
                is_triggered = parameter_value == rule.threshold_value
            elif rule.condition_type == 'NOT_EQUAL':
                is_triggered = parameter_value != rule.threshold_value
            elif rule.condition_type == 'RANGE':
                is_triggered = not (rule.threshold_min <= parameter_value <= rule.threshold_max)
            
            if is_triggered:
                alarm = Alarm(
                    rule_id=rule.rule_id,
                    equipment_id=equipment_id,
                    equipment_type=equipment_type,
                    parameter_name=rule.parameter_name,
                    parameter_value=parameter_value,
                    threshold_value=rule.threshold_value,
                    severity=rule.severity,
                    alarm_text=self.generate_alarm_text(rule, parameter_value),
                    notification_channels=rule.notification_channels
                )
                alarms.append(alarm)
        
        return alarms
    
    def generate_alarm_text(self, rule: AlarmRule, value: float) -> str:
        """產生告警文字"""
        return f"{rule.parameter_name} {rule.condition_type} threshold: " \
               f"current={value}, threshold={rule.threshold_value}"

class AlarmRule:
    def __init__(self, **kwargs):
        self.rule_id = kwargs['rule_id']
        self.equipment_id = kwargs.get('equipment_id')
        self.equipment_type = kwargs.get('equipment_type')
        self.parameter_name = kwargs['parameter_name']
        self.condition_type = kwargs['condition_type']
        self.threshold_value = kwargs.get('threshold_value')
        self.threshold_min = kwargs.get('threshold_min')
        self.threshold_max = kwargs.get('threshold_max')
        self.severity = kwargs['severity']
        self.notification_channels = kwargs.get('notification_channels', [])

class Alarm:
    def __init__(self, **kwargs):
        self.rule_id = kwargs['rule_id']
        self.equipment_id = kwargs.get('equipment_id')
        self.equipment_type = kwargs.get('equipment_type')
        self.parameter_name = kwargs['parameter_name']
        self.parameter_value = kwargs['parameter_value']
        self.threshold_value = kwargs.get('threshold_value')
        self.severity = kwargs['severity']
        self.alarm_text = kwargs['alarm_text']
        self.notification_channels = kwargs.get('notification_channels', [])
    
    def to_dict(self):
        return {
            'rule_id': self.rule_id,
            'equipment_id': self.equipment_id,
            'equipment_type': self.equipment_type,
            'parameter_name': self.parameter_name,
            'parameter_value': self.parameter_value,
            'threshold_value': self.threshold_value,
            'severity': self.severity,
            'alarm_text': self.alarm_text,
            'notification_channels': self.notification_channels
        }
```

### 8.2 告警通知服務

```python
# ========================================
# Alarm Notification Service
# ========================================

import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
import requests
import logging

class AlarmNotificationService:
    def __init__(self, config):
        self.config = config
    
    def send_notification(self, alarm: Alarm):
        """發送告警通知"""
        for channel in alarm.notification_channels:
            try:
                if channel['type'] == 'email':
                    self.send_email(alarm, channel)
                elif channel['type'] == 'sms':
                    self.send_sms(alarm, channel)
                elif channel['type'] == 'webhook':
                    self.send_webhook(alarm, channel)
                elif channel['type'] == 'teams':
                    self.send_teams(alarm, channel)
            except Exception as e:
                logging.error(f"Failed to send notification via {channel['type']}: {e}")
    
    def send_email(self, alarm: Alarm, channel: Dict):
        """發送 Email 通知"""
        msg = MIMEMultipart()
        msg['From'] = self.config['email_from']
        msg['To'] = channel['recipient']
        msg['Subject'] = f"[{alarm.severity}] Equipment Alarm: {alarm.equipment_id}"
        
        body = f"""
        Alarm Details:
        - Equipment ID: {alarm.equipment_id}
        - Equipment Type: {alarm.equipment_type}
        - Parameter: {alarm.parameter_name}
        - Current Value: {alarm.parameter_value}
        - Threshold: {alarm.threshold_value}
        - Severity: {alarm.severity}
        - Description: {alarm.alarm_text}
        - Time: {alarm.timestamp}
        """
        
        msg.attach(MIMEText(body, 'plain'))
        
        with smtplib.SMTP(self.config['smtp_server'], self.config['smtp_port']) as server:
            server.starttls()
            server.login(self.config['smtp_username'], self.config['smtp_password'])
            server.send_message(msg)
    
    def send_webhook(self, alarm: Alarm, channel: Dict):
        """發送 Webhook 通知"""
        payload = {
            'alarm_id': alarm.alarm_id,
            'equipment_id': alarm.equipment_id,
            'severity': alarm.severity,
            'alarm_text': alarm.alarm_text,
            'timestamp': alarm.timestamp
        }
        
        response = requests.post(
            channel['url'],
            json=payload,
            headers={'Content-Type': 'application/json'},
            timeout=10
        )
        response.raise_for_status()
    
    def send_teams(self, alarm: Alarm, channel: Dict):
        """發送 Microsoft Teams 通知"""
        card = {
            "@type": "MessageCard",
            "@context": "http://schema.org/extensions",
            "themeColor": self.get_color_by_severity(alarm.severity),
            "summary": f"Equipment Alarm: {alarm.equipment_id}",
            "sections": [{
                "activityTitle": f"[{alarm.severity}] Equipment Alarm",
                "activitySubtitle": alarm.equipment_id,
                "facts": [
                    {"name": "Equipment Type", "value": alarm.equipment_type},
                    {"name": "Parameter", "value": alarm.parameter_name},
                    {"name": "Current Value", "value": str(alarm.parameter_value)},
                    {"name": "Threshold", "value": str(alarm.threshold_value)},
                    {"name": "Time", "value": alarm.timestamp}
                ],
                "text": alarm.alarm_text
            }]
        }
        
        response = requests.post(
            channel['webhook_url'],
            json=card,
            timeout=10
        )
        response.raise_for_status()
    
    def get_color_by_severity(self, severity: str) -> str:
        """根據嚴重程度取得顏色"""
        colors = {
            'CRITICAL': 'FF0000',  # Red
            'HIGH': 'FFA500',      # Orange
            'MEDIUM': 'FFFF00',    # Yellow
            'LOW': '00FF00'        # Green
        }
        return colors.get(severity, '808080')  # Gray as default
```

---

*文件未完,繼續下一部分...*




## 9. API 設計

### 9.1 RESTful API

```python
# ========================================
# NDH RESTful API
# ========================================

from fastapi import FastAPI, HTTPException, Query, Path
from fastapi.middleware.cors import CORSMiddleware
from typing import List, Optional
from datetime import datetime, timedelta
from influxdb_client import InfluxDBClient
import psycopg2
from pydantic import BaseModel

app = FastAPI(
    title="Harvatek NDH API",
    description="Neutral Data Hub API for Harvatek LED Factory",
    version="3.1.0"
)

# CORS 設定
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# ========================================
# Data Models
# ========================================

class EquipmentStatus(BaseModel):
    equipment_id: str
    equipment_type: str
    status: str
    uph: int
    total_count: int
    good_count: int
    reject_count: int
    yield_rate: float
    oee: float
    timestamp: datetime

class PowerData(BaseModel):
    meter_id: str
    location: str
    voltage: float
    current: float
    power: float
    energy: float
    power_factor: float
    timestamp: datetime

class EnvironmentalData(BaseModel):
    sensor_id: str
    sensor_type: str
    floor: str
    area: str
    temperature: Optional[float]
    humidity: Optional[float]
    differential_pressure: Optional[float]
    timestamp: datetime

class Alarm(BaseModel):
    alarm_id: str
    equipment_id: str
    alarm_code: str
    alarm_text: str
    severity: str
    status: str
    alarm_time: datetime
    acknowledged_at: Optional[datetime]
    cleared_at: Optional[datetime]

# ========================================
# Equipment APIs
# ========================================

@app.get("/api/v1/equipment", tags=["Equipment"])
async def list_equipment(
    equipment_type: Optional[str] = Query(None, description="Filter by equipment type"),
    floor: Optional[str] = Query(None, description="Filter by floor"),
    area: Optional[str] = Query(None, description="Filter by area"),
    status: Optional[str] = Query(None, description="Filter by status")
):
    """取得設備清單"""
    # PostgreSQL 查詢
    query = "SELECT * FROM assets WHERE asset_type IN ('DieSorter', 'DieBonder', 'WireBonder', ...)"
    filters = []
    
    if equipment_type:
        filters.append(f"asset_type = '{equipment_type}'")
    if floor:
        filters.append(f"location_floor = '{floor}'")
    if area:
        filters.append(f"location_area = '{area}'")
    if status:
        filters.append(f"status = '{status}'")
    
    if filters:
        query += " AND " + " AND ".join(filters)
    
    # 執行查詢並返回結果
    # ... (實際實現)
    
    return {"equipment": []}

@app.get("/api/v1/equipment/{equipment_id}", tags=["Equipment"])
async def get_equipment(
    equipment_id: str = Path(..., description="Equipment ID")
):
    """取得設備詳細資訊"""
    # PostgreSQL 查詢
    query = f"SELECT * FROM assets WHERE asset_id = '{equipment_id}'"
    
    # 執行查詢並返回結果
    # ... (實際實現)
    
    return {"equipment_id": equipment_id}

@app.get("/api/v1/equipment/{equipment_id}/status", tags=["Equipment"])
async def get_equipment_status(
    equipment_id: str = Path(..., description="Equipment ID"),
    start_time: Optional[datetime] = Query(None, description="Start time"),
    end_time: Optional[datetime] = Query(None, description="End time"),
    aggregation: Optional[str] = Query("raw", description="Aggregation level: raw, 1m, 1h, 1d")
):
    """取得設備狀態數據"""
    if not start_time:
        start_time = datetime.now() - timedelta(hours=1)
    if not end_time:
        end_time = datetime.now()
    
    # InfluxDB 查詢
    measurement = f"equipment_status_{aggregation}" if aggregation != "raw" else "equipment_status"
    
    query = f'''
        from(bucket: "harvatek_timeseries")
            |> range(start: {start_time.isoformat()}, stop: {end_time.isoformat()})
            |> filter(fn: (r) => r["_measurement"] == "{measurement}")
            |> filter(fn: (r) => r["equipment_id"] == "{equipment_id}")
            |> pivot(rowKey:["_time"], columnKey: ["_field"], valueColumn: "_value")
    '''
    
    # 執行查詢並返回結果
    # ... (實際實現)
    
    return {"data": []}

@app.get("/api/v1/equipment/{equipment_id}/oee", tags=["Equipment"])
async def get_equipment_oee(
    equipment_id: str = Path(..., description="Equipment ID"),
    start_time: Optional[datetime] = Query(None, description="Start time"),
    end_time: Optional[datetime] = Query(None, description="End time")
):
    """取得設備 OEE 數據"""
    if not start_time:
        start_time = datetime.now() - timedelta(days=7)
    if not end_time:
        end_time = datetime.now()
    
    # InfluxDB 查詢
    query = f'''
        from(bucket: "harvatek_timeseries")
            |> range(start: {start_time.isoformat()}, stop: {end_time.isoformat()})
            |> filter(fn: (r) => r["_measurement"] == "equipment_status_1hour")
            |> filter(fn: (r) => r["equipment_id"] == "{equipment_id}")
            |> filter(fn: (r) => r["_field"] == "oee" or r["_field"] == "availability" 
                                  or r["_field"] == "performance" or r["_field"] == "quality")
            |> pivot(rowKey:["_time"], columnKey: ["_field"], valueColumn: "_value")
    '''
    
    # 執行查詢並返回結果
    # ... (實際實現)
    
    return {"data": []}

# ========================================
# Facility APIs
# ========================================

@app.get("/api/v1/facility/power", tags=["Facility"])
async def get_power_data(
    location: Optional[str] = Query(None, description="Filter by location"),
    start_time: Optional[datetime] = Query(None, description="Start time"),
    end_time: Optional[datetime] = Query(None, description="End time"),
    aggregation: Optional[str] = Query("1h", description="Aggregation: raw, 1m, 1h, 1d")
):
    """取得電力數據"""
    if not start_time:
        start_time = datetime.now() - timedelta(days=1)
    if not end_time:
        end_time = datetime.now()
    
    # InfluxDB 查詢
    measurement = f"facility_power_{aggregation}" if aggregation != "raw" else "facility_power"
    
    query = f'''
        from(bucket: "harvatek_timeseries")
            |> range(start: {start_time.isoformat()}, stop: {end_time.isoformat()})
            |> filter(fn: (r) => r["_measurement"] == "{measurement}")
    '''
    
    if location:
        query += f'''
            |> filter(fn: (r) => r["location"] == "{location}")
        '''
    
    query += '''
        |> pivot(rowKey:["_time"], columnKey: ["_field"], valueColumn: "_value")
    '''
    
    # 執行查詢並返回結果
    # ... (實際實現)
    
    return {"data": []}

@app.get("/api/v1/facility/power/summary", tags=["Facility"])
async def get_power_summary(
    group_by: str = Query("floor", description="Group by: floor, area, equipment_type"),
    start_time: Optional[datetime] = Query(None, description="Start time"),
    end_time: Optional[datetime] = Query(None, description="End time")
):
    """取得電力消耗摘要"""
    if not start_time:
        start_time = datetime.now() - timedelta(days=30)
    if not end_time:
        end_time = datetime.now()
    
    # InfluxDB 查詢
    query = f'''
        from(bucket: "harvatek_timeseries")
            |> range(start: {start_time.isoformat()}, stop: {end_time.isoformat()})
            |> filter(fn: (r) => r["_measurement"] == "facility_power_1day")
            |> filter(fn: (r) => r["_field"] == "energy")
            |> group(columns: ["{group_by}"])
            |> sum()
    '''
    
    # 執行查詢並返回結果
    # ... (實際實現)
    
    return {"summary": []}

@app.get("/api/v1/facility/hvac", tags=["Facility"])
async def get_hvac_data(
    equipment_type: Optional[str] = Query(None, description="Filter by equipment type"),
    floor: Optional[str] = Query(None, description="Filter by floor"),
    start_time: Optional[datetime] = Query(None, description="Start time"),
    end_time: Optional[datetime] = Query(None, description="End time")
):
    """取得 HVAC 系統數據"""
    if not start_time:
        start_time = datetime.now() - timedelta(hours=24)
    if not end_time:
        end_time = datetime.now()
    
    # InfluxDB 查詢
    query = f'''
        from(bucket: "harvatek_timeseries")
            |> range(start: {start_time.isoformat()}, stop: {end_time.isoformat()})
            |> filter(fn: (r) => r["_measurement"] == "facility_hvac")
    '''
    
    if equipment_type:
        query += f'''
            |> filter(fn: (r) => r["equipment_type"] == "{equipment_type}")
        '''
    
    if floor:
        query += f'''
            |> filter(fn: (r) => r["floor"] == "{floor}")
        '''
    
    query += '''
        |> pivot(rowKey:["_time"], columnKey: ["_field"], valueColumn: "_value")
    '''
    
    # 執行查詢並返回結果
    # ... (實際實現)
    
    return {"data": []}

# ========================================
# Environmental APIs
# ========================================

@app.get("/api/v1/environmental/sensors", tags=["Environmental"])
async def get_environmental_data(
    sensor_type: Optional[str] = Query(None, description="Filter by sensor type"),
    floor: Optional[str] = Query(None, description="Filter by floor"),
    area: Optional[str] = Query(None, description="Filter by area"),
    start_time: Optional[datetime] = Query(None, description="Start time"),
    end_time: Optional[datetime] = Query(None, description="End time")
):
    """取得環境感測器數據"""
    if not start_time:
        start_time = datetime.now() - timedelta(hours=24)
    if not end_time:
        end_time = datetime.now()
    
    # InfluxDB 查詢
    query = f'''
        from(bucket: "harvatek_timeseries")
            |> range(start: {start_time.isoformat()}, stop: {end_time.isoformat()})
            |> filter(fn: (r) => r["_measurement"] == "environmental_sensors")
    '''
    
    if sensor_type:
        query += f'''
            |> filter(fn: (r) => r["sensor_type"] == "{sensor_type}")
        '''
    
    if floor:
        query += f'''
            |> filter(fn: (r) => r["floor"] == "{floor}")
        '''
    
    if area:
        query += f'''
            |> filter(fn: (r) => r["area"] == "{area}")
        '''
    
    query += '''
        |> pivot(rowKey:["_time"], columnKey: ["_field"], valueColumn: "_value")
    '''
    
    # 執行查詢並返回結果
    # ... (實際實現)
    
    return {"data": []}

@app.get("/api/v1/environmental/heatmap", tags=["Environmental"])
async def get_environmental_heatmap(
    floor: str = Query(..., description="Floor"),
    parameter: str = Query(..., description="Parameter: temperature, humidity, pressure"),
    timestamp: Optional[datetime] = Query(None, description="Timestamp")
):
    """取得環境參數熱圖數據"""
    if not timestamp:
        timestamp = datetime.now()
    
    # InfluxDB 查詢
    query = f'''
        from(bucket: "harvatek_timeseries")
            |> range(start: {(timestamp - timedelta(minutes=5)).isoformat()}, 
                     stop: {timestamp.isoformat()})
            |> filter(fn: (r) => r["_measurement"] == "environmental_sensors")
            |> filter(fn: (r) => r["floor"] == "{floor}")
            |> filter(fn: (r) => r["_field"] == "{parameter}")
            |> last()
    '''
    
    # 執行查詢並返回結果
    # ... (實際實現)
    
    return {"heatmap": []}

# ========================================
# Alarm APIs
# ========================================

@app.get("/api/v1/alarms", tags=["Alarm"])
async def list_alarms(
    equipment_id: Optional[str] = Query(None, description="Filter by equipment ID"),
    severity: Optional[str] = Query(None, description="Filter by severity"),
    status: Optional[str] = Query(None, description="Filter by status"),
    start_time: Optional[datetime] = Query(None, description="Start time"),
    end_time: Optional[datetime] = Query(None, description="End time"),
    limit: int = Query(100, description="Limit"),
    offset: int = Query(0, description="Offset")
):
    """取得告警清單"""
    # PostgreSQL 查詢
    query = "SELECT * FROM alarms WHERE 1=1"
    filters = []
    
    if equipment_id:
        filters.append(f"equipment_id = '{equipment_id}'")
    if severity:
        filters.append(f"severity = '{severity}'")
    if status:
        filters.append(f"status = '{status}'")
    if start_time:
        filters.append(f"alarm_time >= '{start_time.isoformat()}'")
    if end_time:
        filters.append(f"alarm_time <= '{end_time.isoformat()}'")
    
    if filters:
        query += " AND " + " AND ".join(filters)
    
    query += f" ORDER BY alarm_time DESC LIMIT {limit} OFFSET {offset}"
    
    # 執行查詢並返回結果
    # ... (實際實現)
    
    return {"alarms": [], "total": 0}

@app.post("/api/v1/alarms/{alarm_id}/acknowledge", tags=["Alarm"])
async def acknowledge_alarm(
    alarm_id: str = Path(..., description="Alarm ID"),
    acknowledged_by: str = Query(..., description="User who acknowledged")
):
    """確認告警"""
    # PostgreSQL 更新
    query = f"""
        UPDATE alarms 
        SET status = 'ACKNOWLEDGED',
            acknowledged_at = NOW(),
            acknowledged_by = '{acknowledged_by}'
        WHERE alarm_id = '{alarm_id}'
    """
    
    # 執行更新
    # ... (實際實現)
    
    return {"message": "Alarm acknowledged successfully"}

@app.post("/api/v1/alarms/{alarm_id}/clear", tags=["Alarm"])
async def clear_alarm(
    alarm_id: str = Path(..., description="Alarm ID"),
    cleared_by: str = Query(..., description="User who cleared")
):
    """清除告警"""
    # PostgreSQL 更新
    query = f"""
        UPDATE alarms 
        SET status = 'CLEARED',
            cleared_at = NOW(),
            cleared_by = '{cleared_by}'
        WHERE alarm_id = '{alarm_id}'
    """
    
    # 執行更新
    # ... (實際實現)
    
    return {"message": "Alarm cleared successfully"}

# ========================================
# Production APIs
# ========================================

@app.get("/api/v1/production/orders", tags=["Production"])
async def list_production_orders(
    status: Optional[str] = Query(None, description="Filter by status"),
    start_date: Optional[datetime] = Query(None, description="Start date"),
    end_date: Optional[datetime] = Query(None, description="End date"),
    limit: int = Query(100, description="Limit"),
    offset: int = Query(0, description="Offset")
):
    """取得生產訂單清單"""
    # PostgreSQL 查詢
    # ... (實際實現)
    
    return {"orders": [], "total": 0}

@app.get("/api/v1/production/lots/{lot_id}", tags=["Production"])
async def get_production_lot(
    lot_id: str = Path(..., description="Lot ID")
):
    """取得生產批次詳細資訊"""
    # PostgreSQL 查詢
    # ... (實際實現)
    
    return {"lot_id": lot_id}

@app.get("/api/v1/production/lots/{lot_id}/events", tags=["Production"])
async def get_lot_events(
    lot_id: str = Path(..., description="Lot ID")
):
    """取得生產批次事件"""
    # PostgreSQL 查詢
    # ... (實際實現)
    
    return {"events": []}

# ========================================
# Analytics APIs
# ========================================

@app.get("/api/v1/analytics/oee/summary", tags=["Analytics"])
async def get_oee_summary(
    equipment_type: Optional[str] = Query(None, description="Filter by equipment type"),
    floor: Optional[str] = Query(None, description="Filter by floor"),
    start_date: Optional[datetime] = Query(None, description="Start date"),
    end_date: Optional[datetime] = Query(None, description="End date")
):
    """取得 OEE 摘要"""
    if not start_date:
        start_date = datetime.now() - timedelta(days=30)
    if not end_date:
        end_date = datetime.now()
    
    # InfluxDB 查詢
    # ... (實際實現)
    
    return {"summary": {}}

@app.get("/api/v1/analytics/energy/summary", tags=["Analytics"])
async def get_energy_summary(
    group_by: str = Query("floor", description="Group by: floor, area, equipment_type"),
    start_date: Optional[datetime] = Query(None, description="Start date"),
    end_date: Optional[datetime] = Query(None, description="End date")
):
    """取得能源消耗摘要"""
    if not start_date:
        start_date = datetime.now() - timedelta(days=30)
    if not end_date:
        end_date = datetime.now()
    
    # InfluxDB 查詢
    # ... (實際實現)
    
    return {"summary": {}}

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)
```

### 9.2 GraphQL API (可選)

```python
# ========================================
# NDH GraphQL API
# ========================================

import strawberry
from typing import List, Optional
from datetime import datetime

@strawberry.type
class Equipment:
    equipment_id: str
    equipment_type: str
    equipment_name: str
    manufacturer: str
    model: str
    location_floor: str
    location_area: str
    status: str

@strawberry.type
class EquipmentStatus:
    timestamp: datetime
    status: str
    uph: int
    total_count: int
    good_count: int
    reject_count: int
    yield_rate: float
    oee: float

@strawberry.type
class Query:
    @strawberry.field
    def equipment(self, equipment_id: str) -> Optional[Equipment]:
        # 查詢設備
        pass
    
    @strawberry.field
    def equipment_list(
        self,
        equipment_type: Optional[str] = None,
        floor: Optional[str] = None
    ) -> List[Equipment]:
        # 查詢設備清單
        pass
    
    @strawberry.field
    def equipment_status(
        self,
        equipment_id: str,
        start_time: Optional[datetime] = None,
        end_time: Optional[datetime] = None
    ) -> List[EquipmentStatus]:
        # 查詢設備狀態
        pass

schema = strawberry.Schema(query=Query)
```

---

## 10. 部署架構

### 10.1 Kubernetes 部署

```yaml
# ========================================
# Kubernetes Deployment Configuration
# ========================================

# Namespace
apiVersion: v1
kind: Namespace
metadata:
  name: harvatek-ndh

---

# Kafka Cluster (使用 Strimzi Operator)
apiVersion: kafka.strimzi.io/v1beta2
kind: Kafka
metadata:
  name: harvatek-kafka
  namespace: harvatek-ndh
spec:
  kafka:
    version: 3.5.0
    replicas: 3
    listeners:
      - name: plain
        port: 9092
        type: internal
        tls: false
      - name: tls
        port: 9093
        type: internal
        tls: true
    config:
      offsets.topic.replication.factor: 3
      transaction.state.log.replication.factor: 3
      transaction.state.log.min.isr: 2
      default.replication.factor: 3
      min.insync.replicas: 2
      inter.broker.protocol.version: "3.5"
    storage:
      type: jbod
      volumes:
      - id: 0
        type: persistent-claim
        size: 500Gi
        class: fast-ssd
        deleteClaim: false
    resources:
      requests:
        memory: 8Gi
        cpu: "2"
      limits:
        memory: 16Gi
        cpu: "4"
  zookeeper:
    replicas: 3
    storage:
      type: persistent-claim
      size: 100Gi
      class: fast-ssd
      deleteClaim: false
    resources:
      requests:
        memory: 2Gi
        cpu: "1"
      limits:
        memory: 4Gi
        cpu: "2"
  entityOperator:
    topicOperator: {}
    userOperator: {}

---

# InfluxDB StatefulSet
apiVersion: apps/v1
kind: StatefulSet
metadata:
  name: influxdb
  namespace: harvatek-ndh
spec:
  serviceName: influxdb
  replicas: 2
  selector:
    matchLabels:
      app: influxdb
  template:
    metadata:
      labels:
        app: influxdb
    spec:
      containers:
      - name: influxdb
        image: influxdb:2.7
        ports:
        - containerPort: 8086
          name: http
        env:
        - name: INFLUXDB_DB
          value: harvatek_timeseries
        - name: INFLUXDB_ADMIN_USER
          valueFrom:
            secretKeyRef:
              name: influxdb-secret
              key: admin-user
        - name: INFLUXDB_ADMIN_PASSWORD
          valueFrom:
            secretKeyRef:
              name: influxdb-secret
              key: admin-password
        volumeMounts:
        - name: influxdb-data
          mountPath: /var/lib/influxdb2
        resources:
          requests:
            memory: 8Gi
            cpu: "2"
          limits:
            memory: 16Gi
            cpu: "4"
  volumeClaimTemplates:
  - metadata:
      name: influxdb-data
    spec:
      accessModes: [ "ReadWriteOnce" ]
      storageClassName: fast-ssd
      resources:
        requests:
          storage: 1Ti

---

# PostgreSQL StatefulSet
apiVersion: apps/v1
kind: StatefulSet
metadata:
  name: postgresql
  namespace: harvatek-ndh
spec:
  serviceName: postgresql
  replicas: 2
  selector:
    matchLabels:
      app: postgresql
  template:
    metadata:
      labels:
        app: postgresql
    spec:
      containers:
      - name: postgresql
        image: postgres:15
        ports:
        - containerPort: 5432
          name: postgresql
        env:
        - name: POSTGRES_DB
          value: harvatek_relational
        - name: POSTGRES_USER
          valueFrom:
            secretKeyRef:
              name: postgresql-secret
              key: user
        - name: POSTGRES_PASSWORD
          valueFrom:
            secretKeyRef:
              name: postgresql-secret
              key: password
        - name: PGDATA
          value: /var/lib/postgresql/data/pgdata
        volumeMounts:
        - name: postgresql-data
          mountPath: /var/lib/postgresql/data
        resources:
          requests:
            memory: 4Gi
            cpu: "2"
          limits:
            memory: 8Gi
            cpu: "4"
  volumeClaimTemplates:
  - metadata:
      name: postgresql-data
    spec:
      accessModes: [ "ReadWriteOnce" ]
      storageClassName: fast-ssd
      resources:
        requests:
          storage: 500Gi

---

# Kafka Connect Deployment
apiVersion: apps/v1
kind: Deployment
metadata:
  name: kafka-connect
  namespace: harvatek-ndh
spec:
  replicas: 3
  selector:
    matchLabels:
      app: kafka-connect
  template:
    metadata:
      labels:
        app: kafka-connect
    spec:
      containers:
      - name: kafka-connect
        image: confluentinc/cp-kafka-connect:7.5.0
        ports:
        - containerPort: 8083
          name: rest-api
        env:
        - name: CONNECT_BOOTSTRAP_SERVERS
          value: harvatek-kafka-kafka-bootstrap:9092
        - name: CONNECT_GROUP_ID
          value: harvatek-connect-cluster
        - name: CONNECT_CONFIG_STORAGE_TOPIC
          value: connect-configs
        - name: CONNECT_OFFSET_STORAGE_TOPIC
          value: connect-offsets
        - name: CONNECT_STATUS_STORAGE_TOPIC
          value: connect-status
        - name: CONNECT_KEY_CONVERTER
          value: org.apache.kafka.connect.json.JsonConverter
        - name: CONNECT_VALUE_CONVERTER
          value: org.apache.kafka.connect.json.JsonConverter
        resources:
          requests:
            memory: 2Gi
            cpu: "1"
          limits:
            memory: 4Gi
            cpu: "2"

---

# NDH API Deployment
apiVersion: apps/v1
kind: Deployment
metadata:
  name: ndh-api
  namespace: harvatek-ndh
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
        image: harvatek/ndh-api:3.1.0
        ports:
        - containerPort: 8000
          name: http
        env:
        - name: KAFKA_BROKERS
          value: harvatek-kafka-kafka-bootstrap:9092
        - name: INFLUXDB_URL
          value: http://influxdb:8086
        - name: POSTGRESQL_HOST
          value: postgresql
        - name: POSTGRESQL_PORT
          value: "5432"
        - name: POSTGRESQL_DB
          value: harvatek_relational
        resources:
          requests:
            memory: 1Gi
            cpu: "500m"
          limits:
            memory: 2Gi
            cpu: "1"

---

# Service for NDH API
apiVersion: v1
kind: Service
metadata:
  name: ndh-api
  namespace: harvatek-ndh
spec:
  selector:
    app: ndh-api
  ports:
  - port: 80
    targetPort: 8000
  type: LoadBalancer
```

### 10.2 監控和告警

```yaml
# ========================================
# Prometheus Monitoring
# ========================================

# ServiceMonitor for Kafka
apiVersion: monitoring.coreos.com/v1
kind: ServiceMonitor
metadata:
  name: kafka-metrics
  namespace: harvatek-ndh
spec:
  selector:
    matchLabels:
      app: kafka
  endpoints:
  - port: metrics
    interval: 30s

---

# ServiceMonitor for InfluxDB
apiVersion: monitoring.coreos.com/v1
kind: ServiceMonitor
metadata:
  name: influxdb-metrics
  namespace: harvatek-ndh
spec:
  selector:
    matchLabels:
      app: influxdb
  endpoints:
  - port: http
    path: /metrics
    interval: 30s

---

# Grafana Dashboard ConfigMap
apiVersion: v1
kind: ConfigMap
metadata:
  name: grafana-dashboards
  namespace: harvatek-ndh
data:
  ndh-overview.json: |
    {
      "dashboard": {
        "title": "Harvatek NDH Overview",
        "panels": [
          {
            "title": "Kafka Throughput",
            "targets": [
              {
                "expr": "rate(kafka_server_brokertopicmetrics_messagesin_total[5m])"
              }
            ]
          },
          {
            "title": "InfluxDB Write Rate",
            "targets": [
              {
                "expr": "rate(influxdb_write_total[5m])"
              }
            ]
          }
        ]
      }
    }
```

---

## 總結

本文件建立了 Harvatek LED 封裝廠的完整 NDH (Neutral Data Hub) 資料整合方案,涵蓋:

### **核心組件**:
1. **Kafka Cluster**: 3 個 Broker,處理 100,000+ events/sec
2. **InfluxDB Cluster**: 2 個節點,儲存 109M+ points/day
3. **PostgreSQL Cluster**: Primary + Replica,管理關聯數據

### **數據源整合**:
1. **OT 數據**: 
   - SECS/GEM (340 台設備)
   - OPC UA (200 點)
   - Modbus TCP (150 點)
   - BACnet (160 台設備)
   - SNMP (5 台 UPS)

2. **IT 數據**:
   - SAP ERP
   - MES
   - PLM
   - WMS
   - QMS

### **數據處理**:
1. **Kafka Topics**: 10+ 主題,涵蓋設備、設施、感測器、告警、事件
2. **Kafka Streams**: OEE 計算、能源聚合、告警檢測
3. **Continuous Queries**: 自動數據降採樣 (1min, 1hour, 1day)

### **API 服務**:
1. **RESTful API**: 完整的設備、設施、環境、告警、生產 API
2. **GraphQL API**: 靈活的數據查詢介面

### **部署架構**:
1. **Kubernetes**: 容器化部署,高可用性
2. **監控**: Prometheus + Grafana
3. **擴展性**: 支援水平擴展

### **數據量**:
- **每日數據點**: 109M points
- **儲存需求**: ~10 GB (InfluxDB) + ~8 GB (PostgreSQL)
- **吞吐量**: 1,260 events/sec (平均), 2,000 events/sec (峰值)

此 NDH 方案可直接用於:
- 實時數據採集和整合
- 設備性能監控和 OEE 分析
- 能源管理和成本分析
- 預測性維護
- 生產追溯和品質管理
- Omniverse Digital Twins 視覺化

---

**文件版本**: 1.0  
**最後更新**: 2025-10-12  
**作者**: IDTF V3.1 Team

