# Sparkplug 研究筆記

**日期**: 2025-10-10  
**目的**: 評估 Sparkplug 是否應該整合到 NDH 中

---

## 1. Sparkplug 基本資訊

### 1.1. 什麼是 Sparkplug?

**Sparkplug** 是一個開源的 MQTT 規範,由 Eclipse Foundation 維護,專為工業物聯網 (IIoT) 和工業 4.0 設計。

**核心定義**:
- Sparkplug 是一個**軟體規範**,為 MQTT 客戶端提供框架
- 定義了如何在 MQTT 基礎設施中無縫整合來自應用程式、感測器、設備和閘道的資料
- 提供雙向和可互操作的通訊方式

### 1.2. Sparkplug 的三大目標

1. **定義 MQTT 主題命名空間** (Topic Namespace)
2. **定義 MQTT 狀態管理** (State Management)
3. **定義 MQTT 有效負載** (Payload)

### 1.3. 版本歷史

| 版本 | 日期 | 作者 | 說明 |
|------|------|------|------|
| 1.0 | 2016-05-26 | Cirrus Link Solutions | 初始版本 |
| 2.1 | 2016-12-10 | Cirrus Link Solutions | 新增 Payload B |
| 2.2 | 2019-10-11 | Cirrus Link Solutions | 遷移到 Eclipse Foundation |
| 3.0 | 2022-10-21 | Sparkplug Specification Project Team | 遷移到 AsciiDoc,完全重組 |

**當前版本**: 3.0 (2022 年 10 月)

---

## 2. Sparkplug 的核心特性

### 2.1. 與標準 MQTT 的差異

**標準 MQTT 的問題**:
- 沒有定義主題結構
- 沒有定義有效負載格式
- 沒有定義設備行為規範
- 需要各方自行約定資料格式和主題結構
- 導致緊密耦合和資料轉換需求

**Sparkplug 的解決方案**:
- ✅ 統一的主題命名空間
- ✅ 標準化的有效負載格式
- ✅ 定義的狀態管理
- ✅ 自動設備發現
- ✅ 雙向通訊
- ✅ 跨供應商互操作性

### 2.2. 主題命名空間

Sparkplug 定義了標準的主題結構:

```
spBv1.0/{group_id}/{message_type}/{edge_node_id}/{device_id}
```

**範例**:
```
spBv1.0/Factory1/DDATA/Gateway1/Sensor001
spBv1.0/Factory1/NBIRTH/Gateway1
spBv1.0/Factory1/DBIRTH/Gateway1/PLC001
```

### 2.3. 訊息類型

| 訊息類型 | 說明 | 用途 |
|---------|------|------|
| **NBIRTH** | Node Birth | Edge Node 上線通知 |
| **NDEATH** | Node Death | Edge Node 離線通知 |
| **DBIRTH** | Device Birth | 設備上線通知 |
| **DDEATH** | Device Death | 設備離線通知 |
| **NDATA** | Node Data | Edge Node 資料 |
| **DDATA** | Device Data | 設備資料 |
| **NCMD** | Node Command | Edge Node 命令 |
| **DCMD** | Device Command | 設備命令 |
| **STATE** | State | SCADA 主機狀態 |

### 2.4. 有效負載格式

Sparkplug 定義了兩種有效負載格式:

**Sparkplug A**: 使用 Eclipse Kura 有效負載格式 (已過時)
**Sparkplug B**: 首選格式,使用 Google Protocol Buffers (Protobuf)

**Sparkplug B 有效負載結構**:
```protobuf
message Payload {
    uint64 timestamp = 1;
    repeated Metric metrics = 2;
    uint64 seq = 3;
    string uuid = 4;
    bytes body = 5;
}

message Metric {
    string name = 1;
    uint64 alias = 2;
    uint64 timestamp = 3;
    uint32 datatype = 4;
    // ... value fields
    MetaData metadata = 10;
    PropertySet properties = 11;
}
```

---

## 3. Sparkplug 的架構

### 3.1. 核心元件

```
┌─────────────────────────────────────────────────────────┐
│                    SCADA / MES / HMI                     │
│                  (Primary Application)                   │
└────────────────────┬────────────────────────────────────┘
                     │ Subscribe to all data
                     │ Send commands
┌────────────────────▼────────────────────────────────────┐
│                    MQTT Broker                           │
│                  (HiveMQ, Mosquitto)                     │
└─────┬──────────────┬──────────────┬─────────────────────┘
      │              │              │
      │ Publish      │ Publish      │ Publish
      │ NBIRTH       │ NBIRTH       │ NBIRTH
      │ NDATA        │ NDATA        │ NDATA
      │              │              │
┌─────▼──────┐ ┌─────▼──────┐ ┌─────▼──────┐
│ Edge Node  │ │ Edge Node  │ │ Edge Node  │
│ (Gateway)  │ │ (Gateway)  │ │ (Gateway)  │
└─────┬──────┘ └─────┬──────┘ └─────┬──────┘
      │              │              │
      │ DBIRTH       │ DBIRTH       │ DBIRTH
      │ DDATA        │ DDATA        │ DDATA
      │              │              │
┌─────▼──────┐ ┌─────▼──────┐ ┌─────▼──────┐
│  Device    │ │  Device    │ │  Device    │
│  (PLC,     │ │  (Sensor,  │ │  (Actuator,│
│   SCADA)   │ │   Meter)   │ │   Motor)   │
└────────────┘ └────────────┘ └────────────┘
```

### 3.2. 元件角色

**SCADA Host / Primary Application**:
- 訂閱所有設備資料
- 發送命令到 Edge Nodes 和設備
- 管理整個系統狀態

**MQTT Broker**:
- 中央訊息路由器
- 處理所有發布/訂閱
- 必須支援 MQTT 3.1.1 完整規範

**Edge Node (Gateway)**:
- 連接到 MQTT Broker
- 管理多個設備
- 發布 NBIRTH, NDATA, NDEATH
- 接收 NCMD

**Device**:
- 連接到 Edge Node
- 發布 DBIRTH, DDATA, DDEATH
- 接收 DCMD

---

## 4. Sparkplug 的優勢

### 4.1. 技術優勢

1. **輕量級**: 基於 MQTT,比 OPC UA 更輕量
2. **標準化**: 統一的主題和有效負載格式
3. **互操作性**: 跨供應商的互操作性
4. **自動發現**: Birth 訊息提供自動設備發現
5. **狀態管理**: 明確的上線/離線狀態管理
6. **雙向通訊**: 支援資料和命令雙向流動
7. **Report by Exception**: 只在資料變更時發送
8. **低延遲**: MQTT 的低延遲特性
9. **高吞吐量**: 支援大規模設備連接

### 4.2. 業務優勢

1. **降低整合成本**: 統一標準減少客製化開發
2. **加速部署**: 標準化加快專案實施
3. **提高可維護性**: 統一格式易於維護
4. **供應商中立**: 不鎖定特定供應商
5. **開源**: 免費使用,無授權費用

---

## 5. Sparkplug 的應用場景

### 5.1. 典型應用

1. **SCADA 系統**: 實時監控和控制
2. **MES 系統**: 製造執行系統整合
3. **工業 4.0**: 智慧製造和數位轉型
4. **邊緣計算**: 邊緣節點資料聚合
5. **設備監控**: 設備狀態和效能監控
6. **能源管理**: 能源消耗監控和優化

### 5.2. 產業採用

**已採用 Sparkplug 的公司**:
- Chevron (石油和天然氣)
- Inductive Automation (SCADA 軟體)
- HiveMQ (MQTT Broker)
- Opto22 (工業自動化)
- Canary Labs (工業資料歷史庫)

---

## 6. Sparkplug 的技術要求

### 6.1. MQTT Broker 要求

必須支援 MQTT 3.1.1 的以下功能:
- ✅ QoS 0 和 QoS 1
- ✅ Retained Messages (保留訊息)
- ✅ Last Will and Testament (LWT,遺囑訊息)
- ✅ 靈活的安全系統

**推薦的 MQTT Broker**:
- HiveMQ Platform (商業)
- HiveMQ Community Edition (開源)
- Eclipse Mosquitto (開源)

**不適合的平台**:
- ❌ AWS IoT Core (缺少 Retained Messages)
- ❌ Azure IoT Hub (缺少 Retained Messages)

### 6.2. 客戶端要求

- 支援 MQTT 3.1.1
- 支援 Protocol Buffers (Protobuf)
- 實作 Sparkplug 狀態管理
- 實作 Birth/Death 訊息

---

## 7. Sparkplug 與其他協定的比較

| 特性 | Sparkplug | OPC UA | Modbus | MQTT (原生) |
|------|-----------|--------|--------|-------------|
| **標準化** | ✅ 高 | ✅ 高 | ⚠️ 中 | ❌ 低 |
| **複雜度** | ⚠️ 中 | ❌ 高 | ✅ 低 | ✅ 低 |
| **輕量級** | ✅ 是 | ❌ 否 | ✅ 是 | ✅ 是 |
| **互操作性** | ✅ 高 | ✅ 高 | ⚠️ 中 | ❌ 低 |
| **自動發現** | ✅ 是 | ✅ 是 | ❌ 否 | ❌ 否 |
| **雙向通訊** | ✅ 是 | ✅ 是 | ⚠️ 有限 | ✅ 是 |
| **狀態管理** | ✅ 是 | ✅ 是 | ❌ 否 | ❌ 否 |
| **有效負載定義** | ✅ 是 | ✅ 是 | ⚠️ 有限 | ❌ 否 |
| **安全性** | ✅ 是 | ✅ 是 | ❌ 弱 | ✅ 是 |
| **延遲** | ✅ 低 | ⚠️ 中 | ✅ 低 | ✅ 低 |
| **學習曲線** | ⚠️ 中 | ❌ 陡峭 | ✅ 平緩 | ✅ 平緩 |

---

## 8. Sparkplug 的限制

### 8.1. 技術限制

1. **依賴 MQTT Broker**: 需要額外的基礎設施
2. **Protobuf 依賴**: 需要 Protocol Buffers 庫
3. **學習曲線**: 需要理解 Sparkplug 規範
4. **MQTT 3.1.1 限制**: 不支援 MQTT 5.0 的新功能

### 8.2. 生態系統限制

1. **相對較新**: 相比 OPC UA,生態系統較小
2. **工具支援**: 工具和庫相對較少
3. **人才**: 熟悉 Sparkplug 的人才較少

---

## 9. Sparkplug 與 NDH 的契合度分析

### 9.1. NDH 當前架構

NDH 目前支援:
- ✅ OPC UA (直接訂閱)
- ✅ PI System (WebSocket)
- ✅ AVEVA Historian (REST API)
- ✅ SAP ERP (BAPI/RFC, OData)
- ✅ Dynamics 365 (OData)
- ✅ Siemens Opcenter (REST API)
- ✅ Kafka (事件流)

### 9.2. Sparkplug 的潛在價值

**對 NDH 的價值**:

1. **補充 MQTT 生態系統**: NDH 目前沒有 MQTT 整合
2. **輕量級 IIoT 整合**: 提供比 OPC UA 更輕量的選擇
3. **邊緣計算支援**: 適合邊緣節點和閘道整合
4. **標準化 MQTT**: 避免客製化 MQTT 整合
5. **自動發現**: Birth 訊息提供設備自動發現
6. **與 Kafka 互補**: Sparkplug (MQTT) + Kafka 提供完整的訊息架構

**潛在應用場景**:

1. **邊緣閘道整合**: 連接邊緣閘道到 NDH
2. **輕量級設備**: 連接資源受限的設備
3. **SCADA 整合**: 整合支援 Sparkplug 的 SCADA 系統
4. **混合架構**: OPC UA (重量級) + Sparkplug (輕量級)

### 9.3. 與現有整合的比較

| 特性 | Sparkplug | OPC UA | PI System |
|------|-----------|--------|-----------|
| **延遲** | < 100ms | < 50ms | < 100ms |
| **複雜度** | 中 | 高 | 高 |
| **輕量級** | ✅ | ❌ | ❌ |
| **標準化** | ✅ | ✅ | ❌ |
| **邊緣友好** | ✅ | ⚠️ | ❌ |
| **自動發現** | ✅ | ✅ | ⚠️ |
| **生態系統** | 成長中 | 成熟 | 成熟 |

---

## 10. 初步結論

### 10.1. 建議

**建議: ✅ 應該整合 Sparkplug 到 NDH**

**理由**:

1. **填補 MQTT 空白**: NDH 目前沒有 MQTT 整合,Sparkplug 是標準化的 MQTT 解決方案
2. **輕量級選擇**: 提供比 OPC UA 更輕量的 IIoT 整合選項
3. **邊緣計算**: 非常適合邊緣節點和閘道場景
4. **產業趨勢**: Sparkplug 被越來越多的工業公司採用
5. **與 Kafka 互補**: Sparkplug (MQTT) 和 Kafka 可以很好地互補
6. **開源**: 免費使用,無授權費用

### 10.2. 優先級

**建議優先級: P1 (短期內完成)**

**時間估計**: 7-10 天

**原因**:
- Sparkplug 規範清晰,實作相對簡單
- 有現成的 Python 庫 (如 `sparkplug-client`)
- 可以快速提供價值

### 10.3. 實作範圍

**最小可行產品 (MVP)**:

1. **Sparkplug Edge Node 連接器**: 作為 Edge Node 連接到 MQTT Broker
2. **訂閱 Sparkplug 訊息**: 訂閱 DDATA, DBIRTH, DDEATH
3. **發布到 NDH**: 將 Sparkplug 資料發布為 NDH 事件
4. **基本命令支援**: 發送 DCMD 到設備

**進階功能** (可選):

5. **Sparkplug Host 應用**: 作為 Primary Application
6. **完整的狀態管理**: 實作完整的 Birth/Death 處理
7. **Sparkplug B 完整支援**: 支援所有 Metric 類型
8. **與 Kafka 橋接**: Sparkplug MQTT ↔ Kafka 橋接

---

## 11. 下一步

1. ✅ 完成 Sparkplug 研究和分析
2. ⏳ 撰寫 Sparkplug 分析報告
3. ⏳ 設計 Sparkplug 連接器架構
4. ⏳ 實作 Sparkplug 連接器
5. ⏳ 撰寫 Sparkplug 整合文件
6. ⏳ 建立使用範例

---

**研究完成日期**: 2025-10-10  
**下一步**: 撰寫完整的 Sparkplug 分析報告和實作方案

