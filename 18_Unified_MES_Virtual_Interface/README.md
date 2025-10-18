

# NDH-MES 雙向連接器虛擬介面架構

**作者**: Manus AI  
**日期**: 2025年10月14日  
**版本**: 1.0

---

## 1. 簡介

本文件旨在設計一個通用的 NDH-MES 雙向連接器虛擬介面架構,提供一個統一的抽象層,讓不同品牌的 MES 都能透過相同的介面與 NDH 整合。

### 1.1 核心目標

1. **抽象化**: 提供統一的、與具體 MES 品牌無關的 API 介面。
2. **標準化**: 採用 ISA-95 標準作為數據模型基礎。
3. **可擴展**: 透過「適配器模式」輕鬆支援新的 MES 品牌。
4. **簡化整合**: 大大降低將新 MES 系統接入 NDH 的成本和時間。

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

    subgraph MES_Virtual_Interface
        Virtual_MES_API[虛擬 MES API]
        Event_Dispatcher[事件分派器]
        Adapter_Manager[適配器管理器]
    end

    subgraph Adapters
        AVEVA_Adapter[AVEVA MES 適配器]
        FactoryTalk_Adapter[FactoryTalk MES 適配器]
        Siemens_Adapter[Siemens Opcenter 適配器]
    end

    NDH_API -- 呼叫 --> Virtual_MES_API
    Virtual_MES_API -- 透過 --> Adapter_Manager -- 呼叫 --> AVEVA_Adapter
    Virtual_MES_API -- 透過 --> Adapter_Manager -- 呼叫 --> FactoryTalk_Adapter
    Virtual_MES_API -- 透過 --> Adapter_Manager -- 呼叫 --> Siemens_Adapter

    AVEVA_Adapter -- 發布 --> Event_Dispatcher
    FactoryTalk_Adapter -- 發布 --> Event_Dispatcher
    Siemens_Adapter -- 發布 --> Event_Dispatcher

    Event_Dispatcher -- 發布 --> Message_Queue_NDH
```

### 2.2 核心組件

#### 1. 虛擬 MES API (Virtual MES API)

- **職責**: 提供一個統一的 RESTful API,供 NDH 呼叫。
- **技術**: FastAPI 或 .NET Core Web API。

#### 2. 事件分派器 (Event Dispatcher)

- **職責**: 接收來自各個 MES 適配器的事件,轉換為 NDH 標準事件格式,並發布到 NDH 的訊息佇列。
- **技術**: Python Worker Service 或 .NET Core Worker Service。

#### 3. 適配器管理器 (Adapter Manager)

- **職責**: 根據配置動態載入和管理 MES 適配器。
- **技術**: Python 或 .NET Core。

#### 4. MES 適配器 (MES Adapter)

- **職責**: 實現與特定 MES 系統的通訊,包括:
  - 訂閱 MES 的事件 (透過 Message Queue, OPC UA 或其他方式)。
  - 呼叫 MES 的 API。
  - 將 MES 的數據模型轉換為通用數據模型。
- **技術**: 根據 MES 的 API 和協議選擇,如: .NET Core, Python, Java。

---

