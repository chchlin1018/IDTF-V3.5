

# AVEVA MES 與 NDH 雙向連接器架構

**作者**: Manus AI  
**日期**: 2025年10月14日  
**版本**: 1.0

---

## 1. 簡介

本文件旨在設計一個 AVEVA MES (原 Wonderware MES) 與 NDH (Neutral Data Hub) 之間的雙向連接器,實現 MES 與 NDH 之間的無縫數據交換。

### 1.1 核心目標

1. **雙向數據流**: 實現從 MES 到 NDH,以及從 NDH 到 MES 的雙向數據流動。
2. **即時事件同步**: 能夠即時地將 MES 的事件同步到 NDH,反之亦然。
3. **標準化介面**: 採用標準化的 API 和數據模型,簡化整合。
4. **高可靠性**: 確保數據交換的可靠性和一致性。

---

## 2. 系統架構

### 2.1 架構圖

```mermaid
graph TD
    subgraph NDH
        NDH_API[NDH API]
        Message_Queue_NDH[Message Queue (NDH)]
        Asset_Servant[Asset Servant]
    end

    subgraph AVEVA_MES_NDH_Connector
        MES_to_NDH_Service[MES to NDH Service]
        NDH_to_MES_Service[NDH to MES Service]
        Config_UI[配置介面]
    end

    subgraph AVEVA_MES
        MES_Web_API[AVEVA MES Web API]
        MES_Event_Broker[AVEVA MES Event Broker]
        MES_DB[MES Database]
    end

    MES_to_NDH_Service -- 訂閱 --> MES_Event_Broker
    MES_to_NDH_Service -- 發布 --> Message_Queue_NDH

    NDH_to_MES_Service -- 訂閱 --> Message_Queue_NDH
    NDH_to_MES_Service -- 呼叫 --> MES_Web_API

    Asset_Servant -- 讀寫 --> NDH_API
    Config_UI -- 配置 --> MES_to_NDH_Service
    Config_UI -- 配置 --> NDH_to_MES_Service
```

### 2.2 核心組件

#### 1. MES to NDH Service

- **職責**:
  - 訂閱 AVEVA MES 的事件 (如:工單創建、設備狀態變更)。
  - 將 MES 事件轉換為 NDH 的標準事件格式。
  - 將轉換後的事件發布到 NDH 的訊息佇列。
- **技術**: .NET Core Worker Service, AVEVA MES SDK。

#### 2. NDH to MES Service

- **職責**:
  - 訂閱 NDH 的事件 (如:產量回報、品質檢驗結果)。
  - 將 NDH 事件轉換為 AVEVA MES 的 API 呼叫。
  - 呼叫 AVEVA MES Web API,將數據寫入 MES。
- **技術**: Python Worker Service, RabbitMQ/Kafka Client。

#### 3. 配置介面 (Configuration UI)

- **職責**: 提供一個 Web 介面,用於配置:
  - AVEVA MES 的連接參數。
  - NDH 訊息佇列的連接參數。
  - 事件的映射規則。
- **技術**: React 或 Vue.js。

---

