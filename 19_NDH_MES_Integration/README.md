

# NDH-MES 整合架構

**作者**: Manus AI  
**日期**: 2025年10月14日  
**版本**: 1.0

---

## 1. 簡介

本文件旨在設計如何將 MES 虛擬介面層整合到 NDH 的架構中,包括模組結構、部署方式和配置管理。

### 1.1 核心目標

1. **無縫整合**: 將 MES 虛擬介面層作為 NDH 的一個核心模組,實現無縫整合。
2. **可配置性**: 支援透過 NDH 的配置檔案來配置 MES 虛擬介面層。
3. **統一管理**: 透過 NDH 的管理介面來管理 MES 虛擬介面層。

---

## 2. 系統架構

### 2.1 架構圖

```mermaid
graph TD
    subgraph NDH
        NDH_API[NDH API]
        Message_Queue_NDH[Message Queue (NDH)]
        Asset_Servant[Asset Servant]
        MES_Integration_Module[MES 整合模組]
    end

    subgraph MES_Integration_Module
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
    Siemens_Adapter -- 發布 --> Event_dispatcher

    Event_Dispatcher -- 發布 --> Message_Queue_NDH
```

### 2.2 核心組件

#### 1. MES 整合模組 (MES Integration Module)

- **職責**: 作為 NDH 的一個核心模組,負責與 MES 虛擬介面層的整合。
- **技術**: Python 模組。

#### 2. 虛擬 MES API (Virtual MES API)

- **職責**: 提供一個統一的 RESTful API,供 NDH 的其他模組呼叫。
- **技術**: FastAPI。

#### 3. 事件分派器 (Event Dispatcher)

- **職責**: 接收來自各個 MES 適配器的事件,轉換為 NDH 標準事件格式,並發布到 NDH 的訊息佇列。
- **技術**: Python Worker Service。

#### 4. 適配器管理器 (Adapter Manager)

- **職責**: 根據 NDH 的配置檔案動態載入和管理 MES 適配器。
- **技術**: Python。

#### 5. MES 適配器 (MES Adapter)

- **職責**: 實現與特定 MES 系統的通訊。
- **技術**: Python。

---

