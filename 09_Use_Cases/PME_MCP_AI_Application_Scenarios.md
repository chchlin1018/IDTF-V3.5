# PME + MCP Server + AI 智能電力管理應用場景

**版本**: 1.0.0  
**日期**: 2025-10-15  
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) Michael Lin (Chih Cheng Lin)

---

## 目錄

1. [概述](#概述)
2. [整合架構](#整合架構)
3. [MCP Server 工具設計](#mcp-server-工具設計)
4. [核心應用場景](#核心應用場景)
5. [進階應用場景](#進階應用場景)
6. [AI 分析能力](#ai-分析能力)
7. [實作範例](#實作範例)
8. [價值分析](#價值分析)
9. [未來展望](#未來展望)

---

## 概述

### 背景

結合 **Schneider PME**、**NDH MCP Server** 和 **AI Agent**，創造出一個革命性的智能電力管理系統，使用者可以透過自然語言與電力系統互動，獲得即時洞察、智能建議和自動化決策支援。

### 核心價值

```
PME (電力數據) + MCP Server (標準化介面) + AI (智能分析) = 智能電力管理
```

**三者結合的獨特優勢**:

1. **PME**: 提供完整、準確、即時的電力數據
2. **MCP Server**: 提供標準化的 AI 整合介面
3. **AI Agent**: 提供自然語言理解、智能分析和決策建議

### 應用領域

- 🏭 **製造工廠**: 能源優化、設備維護、生產排程
- 🏢 **商業建築**: 能源管理、舒適度優化、成本控制
- 🏥 **醫院**: 關鍵負載管理、備援電力、能源合規
- 💾 **數據中心**: PUE 優化、冷卻管理、容量規劃
- 🏗️ **工業園區**: 多租戶能源管理、需量控制、碳排放追蹤

---

## 整合架構

### 高層架構

```
┌─────────────────────────────────────────────────────────────┐
│                    使用者介面層                              │
│                                                              │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │  Claude      │  │  ChatGPT     │  │  Manus       │      │
│  │  Desktop     │  │  Enterprise  │  │  AI Agent    │      │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘      │
│         │                 │                 │               │
│         └─────────────────┼─────────────────┘               │
│                           │                                 │
│                    MCP Protocol                             │
│                           │                                 │
└───────────────────────────┼─────────────────────────────────┘
                            │
┌───────────────────────────┼─────────────────────────────────┐
│                    NDH MCP Server                            │
│                                                              │
│  ┌────────────────────────────────────────────────────────┐ │
│  │         PME 專用 MCP Tools                             │ │
│  │                                                        │ │
│  │  Resources:                                            │ │
│  │  - pme://devices - 電力設備清單                        │ │
│  │  - pme://measurements/{id} - 即時電力數據              │ │
│  │  - pme://alarms - 電力告警                             │ │
│  │  - pme://energy-report - 能源報告                      │ │
│  │                                                        │ │
│  │  Tools:                                                │ │
│  │  - query_power_consumption - 查詢電力消耗              │ │
│  │  - analyze_energy_efficiency - 分析能源效率            │ │
│  │  - predict_demand - 預測需量                           │ │
│  │  - optimize_load_distribution - 優化負載分配           │ │
│  │  - detect_power_quality_issues - 檢測電力品質問題      │ │
│  │  - generate_energy_report - 生成能源報告               │ │
│  │  - recommend_energy_savings - 推薦節能措施             │ │
│  │  - simulate_load_scenario - 模擬負載情境               │ │
│  │                                                        │ │
│  │  Prompts:                                              │ │
│  │  - energy_efficiency_analysis - 能源效率分析提示        │ │
│  │  - predictive_maintenance - 預測性維護提示             │ │
│  │  - demand_management - 需量管理提示                    │ │
│  └────────────────┬───────────────────────────────────────┘ │
│                   │                                          │
│  ┌────────────────┴───────────────────────────────────────┐ │
│  │         NDH PME Connector                              │ │
│  │  - GraphQL Client                                      │ │
│  │  - Data Mapper                                         │ │
│  │  - Alarm Handler                                       │ │
│  └────────────────┬───────────────────────────────────────┘ │
└───────────────────┼─────────────────────────────────────────┘
                    │
┌───────────────────┼─────────────────────────────────────────┐
│          Schneider Electric PME                              │
│                                                              │
│  - 即時電力監控                                              │
│  - 能源管理                                                  │
│  - 電力品質分析                                              │
│  - 告警和事件                                                │
└─────────────────────────────────────────────────────────────┘
```

### 數據流

```
1. 使用者自然語言查詢
   ↓
2. AI Agent 理解意圖
   ↓
3. MCP Server 調用相應的 Tools
   ↓
4. NDH PME Connector 查詢 PME
   ↓
5. PME 返回電力數據
   ↓
6. AI Agent 分析數據
   ↓
7. 生成洞察和建議
   ↓
8. 自然語言回應使用者
```

---

## MCP Server 工具設計

### Resources (資源)

#### 1. `pme://devices`

**描述**: 電力設備清單

**URI**: `pme://devices?site={site_id}&type={device_type}`

**回應範例**:

```json
{
  "devices": [
    {
      "id": "device-001",
      "name": "Main Incomer",
      "type": "CIRCUIT_BREAKER",
      "model": "Masterpact MTZ",
      "location": "MCC Room 1",
      "status": "online",
      "load": 87.5,
      "capacity": 100.0
    }
  ]
}
```

#### 2. `pme://measurements/{device_id}`

**描述**: 設備即時電力數據

**URI**: `pme://measurements/device-001`

**回應範例**:

```json
{
  "device_id": "device-001",
  "timestamp": "2025-10-15T10:30:00Z",
  "voltage": {
    "l1": 230.5,
    "l2": 231.2,
    "l3": 229.8,
    "unit": "V"
  },
  "current": {
    "l1": 125.3,
    "l2": 128.7,
    "l3": 126.1,
    "unit": "A"
  },
  "power": {
    "active": 87.5,
    "reactive": 12.3,
    "apparent": 88.4,
    "power_factor": 0.99,
    "unit": "kW"
  }
}
```

#### 3. `pme://alarms`

**描述**: 電力告警清單

**URI**: `pme://alarms?status=active&severity=high`

**回應範例**:

```json
{
  "alarms": [
    {
      "id": "alarm-001",
      "name": "High Current",
      "description": "Phase A current exceeded 150A threshold",
      "severity": "HIGH",
      "status": "ACTIVE",
      "device": "device-001",
      "timestamp": "2025-10-15T10:25:00Z"
    }
  ]
}
```

#### 4. `pme://energy-report`

**描述**: 能源報告

**URI**: `pme://energy-report?start=2025-10-01&end=2025-10-15&format=summary`

**回應範例**:

```json
{
  "period": {
    "start": "2025-10-01T00:00:00Z",
    "end": "2025-10-15T23:59:59Z"
  },
  "total_consumption": 125000.0,
  "unit": "kWh",
  "cost": 375000.0,
  "currency": "TWD",
  "breakdown": [
    {
      "category": "Production",
      "consumption": 87500.0,
      "percentage": 70.0
    },
    {
      "category": "HVAC",
      "consumption": 25000.0,
      "percentage": 20.0
    }
  ]
}
```

---

### Tools (工具)

#### 1. `query_power_consumption`

**描述**: 查詢電力消耗

**參數**:

```json
{
  "device_ids": ["device-001", "device-002"],
  "start_time": "2025-10-14T00:00:00Z",
  "end_time": "2025-10-15T00:00:00Z",
  "aggregation": "hourly"
}
```

**回應**:

```json
{
  "data": [
    {
      "device_id": "device-001",
      "timestamp": "2025-10-14T00:00:00Z",
      "consumption": 85.2,
      "unit": "kWh"
    }
  ],
  "summary": {
    "total": 2045.6,
    "average": 85.2,
    "peak": 125.3,
    "peak_time": "2025-10-14T14:00:00Z"
  }
}
```

#### 2. `analyze_energy_efficiency`

**描述**: 分析能源效率

**參數**:

```json
{
  "scope": "factory",
  "period": "last_7_days",
  "baseline": "industry_average"
}
```

**回應**:

```json
{
  "overall_efficiency": 0.85,
  "baseline_efficiency": 0.75,
  "improvement": 0.10,
  "insights": [
    {
      "category": "Compressed Air",
      "efficiency": 0.65,
      "baseline": 0.70,
      "gap": -0.05,
      "recommendation": "檢查壓縮空氣系統是否有洩漏"
    }
  ],
  "savings_potential": {
    "annual_kwh": 125000,
    "annual_cost": 375000,
    "currency": "TWD"
  }
}
```

#### 3. `predict_demand`

**描述**: 預測需量

**參數**:

```json
{
  "forecast_horizon": "next_24_hours",
  "confidence_level": 0.95
}
```

**回應**:

```json
{
  "predictions": [
    {
      "timestamp": "2025-10-15T11:00:00Z",
      "predicted_demand": 450.5,
      "confidence_interval": [425.0, 476.0],
      "unit": "kW"
    }
  ],
  "peak_demand": {
    "value": 520.3,
    "timestamp": "2025-10-15T14:00:00Z",
    "probability": 0.85
  },
  "demand_target": 500.0,
  "risk_of_exceeding": 0.65,
  "recommendations": [
    "考慮在 13:00-15:00 減少非關鍵負載",
    "啟動需量控制策略"
  ]
}
```

#### 4. `optimize_load_distribution`

**描述**: 優化負載分配

**參數**:

```json
{
  "objective": "minimize_cost",
  "constraints": {
    "max_demand": 500.0,
    "critical_loads": ["device-001", "device-002"]
  },
  "time_horizon": "next_4_hours"
}
```

**回應**:

```json
{
  "optimized_schedule": [
    {
      "device_id": "device-010",
      "action": "reduce_load",
      "from": 100.0,
      "to": 80.0,
      "start_time": "2025-10-15T13:00:00Z",
      "duration": 120,
      "unit": "minutes"
    }
  ],
  "expected_savings": {
    "demand_reduction": 50.0,
    "cost_savings": 15000.0,
    "currency": "TWD"
  },
  "impact_analysis": {
    "production_impact": "minimal",
    "comfort_impact": "none"
  }
}
```

#### 5. `detect_power_quality_issues`

**描述**: 檢測電力品質問題

**參數**:

```json
{
  "scope": "all_devices",
  "issue_types": ["harmonics", "voltage_sag", "voltage_swell", "flicker"]
}
```

**回應**:

```json
{
  "issues": [
    {
      "type": "harmonics",
      "severity": "medium",
      "device_id": "device-005",
      "thd": 8.5,
      "threshold": 5.0,
      "unit": "%",
      "impact": "可能影響敏感設備",
      "recommendation": "安裝諧波濾波器"
    }
  ],
  "overall_power_quality": "good",
  "compliance": {
    "ieee_519": "compliant",
    "iec_61000": "compliant"
  }
}
```

#### 6. `generate_energy_report`

**描述**: 生成能源報告

**參數**:

```json
{
  "report_type": "iso_50001",
  "period": "monthly",
  "month": "2025-10",
  "format": "pdf"
}
```

**回應**:

```json
{
  "report_id": "report-2025-10",
  "report_url": "https://ndh.example.com/reports/report-2025-10.pdf",
  "summary": {
    "total_consumption": 500000.0,
    "unit": "kWh",
    "cost": 1500000.0,
    "currency": "TWD",
    "yoy_change": -5.2,
    "mom_change": -2.1
  },
  "compliance_status": "compliant",
  "recommendations": [
    "持續監控壓縮空氣系統效率",
    "考慮安裝太陽能板以減少電網依賴"
  ]
}
```

#### 7. `recommend_energy_savings`

**描述**: 推薦節能措施

**參數**:

```json
{
  "target_savings": 0.15,
  "budget": 5000000,
  "currency": "TWD",
  "payback_period": 24
}
```

**回應**:

```json
{
  "recommendations": [
    {
      "measure": "安裝 VFD 於冷卻塔風扇",
      "estimated_savings": {
        "annual_kwh": 75000,
        "annual_cost": 225000,
        "percentage": 0.06
      },
      "investment": 1500000,
      "payback_period": 6.7,
      "roi": 0.15,
      "priority": "high"
    },
    {
      "measure": "升級照明為 LED",
      "estimated_savings": {
        "annual_kwh": 50000,
        "annual_cost": 150000,
        "percentage": 0.04
      },
      "investment": 800000,
      "payback_period": 5.3,
      "roi": 0.19,
      "priority": "high"
    }
  ],
  "total_savings": {
    "annual_kwh": 125000,
    "annual_cost": 375000,
    "percentage": 0.10
  },
  "total_investment": 2300000
}
```

#### 8. `simulate_load_scenario`

**描述**: 模擬負載情境

**參數**:

```json
{
  "scenario": "add_new_production_line",
  "new_loads": [
    {
      "name": "Production Line 3",
      "power": 150.0,
      "unit": "kW",
      "operating_hours": 16
    }
  ],
  "simulation_period": "typical_day"
}
```

**回應**:

```json
{
  "baseline": {
    "peak_demand": 450.0,
    "total_consumption": 7200.0,
    "unit": "kWh"
  },
  "simulated": {
    "peak_demand": 600.0,
    "total_consumption": 9600.0,
    "unit": "kWh"
  },
  "impact": {
    "demand_increase": 150.0,
    "consumption_increase": 2400.0,
    "cost_increase": 7200.0,
    "currency": "TWD"
  },
  "capacity_analysis": {
    "transformer_capacity": 800.0,
    "utilization": 0.75,
    "headroom": 200.0,
    "status": "adequate"
  },
  "recommendations": [
    "現有變壓器容量足夠",
    "建議優化生產排程以避免需量尖峰重疊"
  ]
}
```

---

### Prompts (提示)

#### 1. `energy_efficiency_analysis`

**描述**: 能源效率分析提示

**提示內容**:

```
你是一位能源效率專家。請分析以下電力數據，並提供詳細的能源效率評估：

1. 計算整體能源效率指標
2. 識別高能耗設備和時段
3. 與行業基準對比
4. 提供具體的節能建議
5. 估算節能潛力和投資回報

請使用以下工具：
- query_power_consumption: 查詢電力消耗數據
- analyze_energy_efficiency: 分析能源效率
- recommend_energy_savings: 推薦節能措施

請以專業、清晰的方式呈現分析結果。
```

#### 2. `predictive_maintenance`

**描述**: 預測性維護提示

**提示內容**:

```
你是一位電力設備維護專家。請基於電力參數（電流、電壓、功率因數、諧波等）分析設備健康狀況：

1. 檢測異常模式和趨勢
2. 識別潛在故障風險
3. 預測設備剩餘壽命
4. 提供維護建議和優先級
5. 估算維護成本和停機影響

請使用以下工具：
- query_power_consumption: 查詢歷史電力數據
- detect_power_quality_issues: 檢測電力品質問題

請提供可操作的維護建議。
```

#### 3. `demand_management`

**描述**: 需量管理提示

**提示內容**:

```
你是一位需量管理專家。請協助優化電力需量，降低電費：

1. 預測未來需量
2. 識別需量尖峰時段
3. 評估超過目標需量的風險
4. 提供需量控制策略
5. 模擬不同情境的影響

請使用以下工具：
- predict_demand: 預測需量
- optimize_load_distribution: 優化負載分配
- simulate_load_scenario: 模擬負載情境

請提供具體的需量控制行動計畫。
```

---

## 核心應用場景

### 場景 1: 智能能源效率分析

**使用者**: 能源經理

**目標**: 快速了解工廠能源效率狀況，識別節能機會

**對話流程**:

```
User: "分析過去一個月的能源效率，找出節能機會"

AI Agent:
[調用 query_power_consumption 查詢過去 30 天的電力數據]
[調用 analyze_energy_efficiency 分析能源效率]
[調用 recommend_energy_savings 生成節能建議]

Response:
"📊 能源效率分析報告（2025-09-15 至 2025-10-15）

整體表現：
• 總能耗：500,000 kWh
• 總成本：NT$ 1,500,000
• 整體效率：85%（行業平均：75%）
• 表現：優於行業平均 10%

高能耗設備 Top 5：
1. 壓縮空氣系統：125,000 kWh（25%）⚠️ 效率 65%，低於基準 70%
2. HVAC 系統：100,000 kWh（20%）✅ 效率 82%
3. 生產線 A：87,500 kWh（17.5%）✅ 效率 88%
4. 照明系統：50,000 kWh（10%）⚠️ 仍使用傳統燈具
5. 冷卻塔：37,500 kWh（7.5%）✅ 效率 80%

節能機會：

1. 🔧 壓縮空氣系統優化
   • 問題：效率低於基準 5%
   • 建議：檢查洩漏、安裝 VFD
   • 預估節省：25,000 kWh/年（NT$ 75,000）
   • 投資：NT$ 300,000
   • 回收期：4 個月

2. 💡 照明系統升級為 LED
   • 問題：仍使用傳統 T8 燈管
   • 建議：全面升級為 LED
   • 預估節省：30,000 kWh/年（NT$ 90,000）
   • 投資：NT$ 500,000
   • 回收期：5.6 個月

3. ❄️ 冷卻塔風扇安裝 VFD
   • 問題：風扇固定轉速運行
   • 建議：安裝變頻器
   • 預估節省：15,000 kWh/年（NT$ 45,000）
   • 投資：NT$ 200,000
   • 回收期：4.4 個月

總節能潛力：
• 年節省：70,000 kWh（14%）
• 年成本節省：NT$ 210,000
• 總投資：NT$ 1,000,000
• 整體回收期：4.8 個月
• 20 年總價值：NT$ 4,200,000

建議行動：
1. 立即進行壓縮空氣系統洩漏檢測
2. 啟動 LED 照明升級專案
3. 評估冷卻塔 VFD 安裝可行性

需要我提供詳細的實施計畫嗎？"
```

---

### 場景 2: 預測性維護

**使用者**: 維護經理

**目標**: 基於電力參數預測設備故障，優化維護排程

**對話流程**:

```
User: "檢查所有電動機的健康狀況，預測是否有故障風險"

AI Agent:
[調用 query_power_consumption 查詢所有電動機的電流、電壓數據]
[調用 detect_power_quality_issues 檢測異常]
[使用 AI 模型分析趨勢]

Response:
"🔧 電動機健康狀況分析

檢查範圍：25 台電動機
檢查時間：2025-10-15 11:00

健康狀況總覽：
• ✅ 正常：20 台（80%）
• ⚠️ 需關注：3 台（12%）
• 🚨 高風險：2 台（8%）

高風險設備：

1. 🚨 電動機 M-005（冷卻塔風扇）
   • 狀態：高風險
   • 問題：電流波動異常增加
   • 數據：
     - 電流標準差：15.2A（正常：< 5A）
     - 電流增加趨勢：+8% 過去 7 天
     - 功率因數：0.82（正常：0.92）
   • 可能原因：軸承磨損、不平衡
   • 預測：7-14 天內可能故障
   • 建議：立即排程停機檢查
   • 停機影響：中等（可使用備用冷卻塔）
   • 建議時間：週末維護窗口

2. 🚨 電動機 M-012（壓縮機）
   • 狀態：高風險
   • 問題：啟動電流過高
   • 數據：
     - 啟動電流：650A（正常：500A）
     - 啟動時間：3.5s（正常：2.0s）
     - 溫度：85°C（正常：70°C）
   • 可能原因：電容器老化、繞組問題
   • 預測：14-30 天內可能故障
   • 建議：2 週內排程維護
   • 停機影響：高（關鍵設備）
   • 建議：準備備用壓縮機

需關注設備：

3. ⚠️ 電動機 M-018（輸送帶）
   • 狀態：需關注
   • 問題：功率因數下降
   • 數據：功率因數：0.88（正常：0.92）
   • 建議：1 個月內檢查

維護排程建議：

本週末（2025-10-19）：
• M-005 冷卻塔風扇電動機停機檢查
• 預估工時：4 小時
• 備件：軸承套件

下週（2025-10-21-25）：
• M-012 壓縮機電動機維護
• 預估工時：8 小時
• 備件：啟動電容器

下個月（2025-11-15）：
• M-018 輸送帶電動機檢查
• 預估工時：2 小時

預估成本：
• 備件：NT$ 50,000
• 人工：NT$ 30,000
• 總計：NT$ 80,000

避免損失：
• M-005 故障停機損失：NT$ 500,000
• M-012 故障停機損失：NT$ 2,000,000
• 總計：NT$ 2,500,000

ROI：31.25 倍

是否需要我生成維護工單？"
```

---

### 場景 3: 即時需量管理

**使用者**: 工廠經理

**目標**: 避免超過契約需量，降低電費

**對話流程**:

```
User: "現在的需量是多少？今天會超過契約需量嗎？"

AI Agent:
[調用 pme://measurements 查詢即時功率]
[調用 predict_demand 預測未來需量]
[調用 optimize_load_distribution 生成控制策略]

Response:
"⚡ 即時需量監控（2025-10-15 11:00）

當前狀況：
• 即時需量：485 kW
• 契約需量：500 kW
• 餘裕：15 kW（3%）
• 狀態：⚠️ 接近上限

今日需量預測：

11:00-12:00：
• 預測需量：490 kW ± 10 kW
• 超限風險：15%
• 狀態：✅ 安全

12:00-13:00（午休）：
• 預測需量：420 kW ± 8 kW
• 超限風險：0%
• 狀態：✅ 安全

13:00-14:00：
• 預測需量：510 kW ± 12 kW
• 超限風險：65% 🚨
• 狀態：高風險

14:00-15:00：
• 預測需量：520 kW ± 15 kW
• 超限風險：85% 🚨
• 狀態：極高風險

15:00-16:00：
• 預測需量：495 kW ± 10 kW
• 超限風險：25%
• 狀態：⚠️ 需關注

今日最高需量預測：
• 時間：14:15
• 需量：520 kW
• 超限：20 kW（4%）
• 超限罰款：NT$ 6,000

建議需量控制策略：

立即行動（11:00-13:00）：
1. 無需行動，需量在安全範圍內

13:00 前準備：
2. 通知生產部門 13:00-15:00 需量控制
3. 準備減少非關鍵負載

13:00-15:00 需量控制：
4. 減少空調負載 10 kW（調高溫度 1°C）
   • 影響：舒適度輕微下降
   • 節省需量：10 kW
   
5. 延後充電站充電 15 kW
   • 影響：無（可在 15:00 後充電）
   • 節省需量：15 kW
   
6. 暫停非關鍵壓縮機 10 kW
   • 影響：無（儲氣罐可支撐 2 小時）
   • 節省需量：10 kW

總需量削減：35 kW

實施後預測：
• 13:00-14:00：475 kW（✅ 低於上限 25 kW）
• 14:00-15:00：485 kW（✅ 低於上限 15 kW）
• 超限風險：< 1%

經濟效益：
• 避免超限罰款：NT$ 6,000
• 控制成本：NT$ 0（僅排程調整）
• 淨收益：NT$ 6,000

是否需要我自動執行需量控制策略？"

User: "是的，請執行"

AI Agent:
[調用 optimize_load_distribution 執行負載調整]
[發送控制指令到 BMS 和設備]

Response:
"✅ 需量控制策略已啟動

執行動作：
• 13:00：空調溫度調高 1°C
• 13:00：延後充電站充電
• 13:00：暫停壓縮機 C-003

監控中...

我會持續監控需量，並在 15:00 後恢復正常運行。
如有異常，我會立即通知您。"
```

---

### 場景 4: 電力品質問題診斷

**使用者**: 電氣工程師

**目標**: 診斷電力品質問題，保護敏感設備

**對話流程**:

```
User: "最近有些電腦經常重啟，可能是電力品質問題嗎？"

AI Agent:
[調用 detect_power_quality_issues 檢測電力品質]
[調用 pme://alarms 查詢相關告警]
[分析電壓驟降/驟升事件]

Response:
"🔍 電力品質診斷報告

檢測範圍：辦公區配電系統
檢測時間：過去 7 天

發現問題：

1. 🚨 電壓驟降事件（Voltage Sag）
   • 頻率：每天 3-5 次
   • 嚴重程度：高
   • 典型特徵：
     - 電壓下降：15-25%
     - 持續時間：50-200 ms
     - 影響相位：L1, L2
   • 發生時間：主要在 09:00-17:00
   • 可能原因：大型負載啟動（電梯、空調壓縮機）
   • 影響：導致電腦重啟、UPS 切換

2. ⚠️ 諧波污染（Harmonics）
   • THD（總諧波失真）：8.5%
   • 標準限值：5%（IEEE 519）
   • 超標：3.5%
   • 主要諧波：3 次、5 次、7 次
   • 來源：變頻器、開關電源
   • 影響：設備發熱、效率下降

3. ⚠️ 電壓不平衡（Voltage Unbalance）
   • 不平衡度：2.8%
   • 標準限值：2%（IEC 61000）
   • 超標：0.8%
   • 可能原因：單相負載分配不均
   • 影響：電動機過熱、壽命縮短

事件時間線（過去 24 小時）：

2025-10-15 09:15 - 電壓驟降
• 電壓：L1 195V（-15%）
• 持續：150 ms
• 來源：電梯啟動

2025-10-15 11:30 - 電壓驟降
• 電壓：L2 185V（-20%）
• 持續：100 ms
• 來源：空調壓縮機啟動

2025-10-15 14:20 - 電壓驟降
• 電壓：L1 190V（-17%）
• 持續：120 ms
• 來源：電梯啟動

解決方案：

短期措施（1 週內）：
1. 為敏感設備安裝 UPS
   • 範圍：所有辦公電腦
   • 成本：NT$ 200,000
   • 效果：完全消除重啟問題

2. 調整電梯軟啟動參數
   • 延長啟動時間至 5 秒
   • 成本：NT$ 0
   • 效果：減少電壓驟降 50%

中期措施（1 個月內）：
3. 安裝動態電壓調節器（DVR）
   • 位置：辦公區配電盤
   • 成本：NT$ 500,000
   • 效果：消除電壓驟降影響

4. 重新平衡三相負載
   • 調整單相負載分配
   • 成本：NT$ 50,000
   • 效果：消除電壓不平衡

長期措施（3 個月內）：
5. 安裝有源諧波濾波器（AHF）
   • 容量：50A
   • 成本：NT$ 800,000
   • 效果：THD 降至 < 3%

總成本：NT$ 1,550,000

預期效益：
• 消除電腦重啟問題
• 延長設備壽命 20%
• 提升電力品質至優良等級
• 符合 IEEE 519 和 IEC 61000 標準

建議優先級：
1. 高：UPS 安裝（立即）
2. 高：電梯軟啟動調整（本週）
3. 中：DVR 安裝（1 個月）
4. 中：負載平衡（1 個月）
5. 低：AHF 安裝（3 個月）

是否需要我生成詳細的實施計畫和採購清單？"
```

---

## 進階應用場景

### 場景 5: 多廠區能源對比分析

**使用者**: 集團能源總監

**目標**: 對比多個廠區的能源效率，識別最佳實踐

**對話流程**:

```
User: "對比台北、台中、高雄三個廠區的能源效率"

AI Agent:
[調用 query_power_consumption 查詢三個廠區的數據]
[調用 analyze_energy_efficiency 分析各廠區效率]
[進行對比分析]

Response:
"📊 多廠區能源效率對比分析（2025-10 月）

廠區概覽：

| 廠區 | 產能 | 能耗 | 單位能耗 | 成本 | 效率 | 排名 |
|------|------|------|---------|------|------|------|
| 台北 | 10,000 | 500,000 kWh | 50 kWh/單位 | NT$ 1.5M | 85% | 🥇 1 |
| 台中 | 12,000 | 720,000 kWh | 60 kWh/單位 | NT$ 2.16M | 75% | 🥈 2 |
| 高雄 | 8,000 | 560,000 kWh | 70 kWh/單位 | NT$ 1.68M | 68% | 🥉 3 |

關鍵洞察：

1. 台北廠區表現最佳
   • 單位能耗最低：50 kWh/單位
   • 效率最高：85%
   • 最佳實踐：
     - 全面使用 LED 照明
     - 壓縮空氣系統定期維護
     - 實施需量管理

2. 台中廠區中等表現
   • 單位能耗：60 kWh/單位（比台北高 20%）
   • 效率：75%
   • 改進機會：
     - 照明系統仍有 40% 傳統燈具
     - 壓縮空氣系統洩漏率高

3. 高雄廠區需改進
   • 單位能耗：70 kWh/單位（比台北高 40%）
   • 效率：68%
   • 主要問題：
     - 設備老舊（平均 15 年）
     - HVAC 系統效率低
     - 缺乏能源管理系統

節能潛力分析：

如果台中、高雄廠區達到台北水平：

台中廠區：
• 目標單位能耗：50 kWh/單位
• 可節省：120,000 kWh/月（20%）
• 年成本節省：NT$ 4.32M
• 建議投資：NT$ 5M
• 回收期：13.9 個月

高雄廠區：
• 目標單位能耗：50 kWh/單位
• 可節省：160,000 kWh/月（28.6%）
• 年成本節省：NT$ 5.76M
• 建議投資：NT$ 8M
• 回收期：16.7 個月

總節能潛力：
• 年節省：3,360,000 kWh
• 年成本節省：NT$ 10.08M
• 總投資：NT$ 13M
• 整體回收期：15.5 個月

最佳實踐推廣計畫：

第一階段（1-3 個月）：
1. 台北廠區經驗分享會
2. 制定集團能源管理標準
3. 建立能源效率 KPI

第二階段（3-6 個月）：
4. 台中廠區照明升級
5. 台中廠區壓縮空氣系統優化
6. 高雄廠區能源審計

第三階段（6-12 個月）：
7. 高雄廠區設備更新
8. 高雄廠區 HVAC 系統升級
9. 全集團能源管理系統部署

是否需要我生成詳細的實施路線圖？"
```

---

### 場景 6: 碳排放追蹤與報告

**使用者**: 永續發展經理

**目標**: 追蹤碳排放，生成 ESG 報告

**對話流程**:

```
User: "計算本月的碳排放，並與去年同期對比"

AI Agent:
[調用 query_power_consumption 查詢電力消耗]
[計算碳排放（使用電力排放係數）]
[調用 generate_energy_report 生成報告]

Response:
"🌍 碳排放追蹤報告（2025-10 月）

總排放量：
• 本月：225 公噸 CO2e
• 去年同期：250 公噸 CO2e
• 變化：-25 公噸（-10%）✅
• 狀態：優於目標

排放來源分解：

1. 外購電力（Scope 2）：
   • 電力消耗：500,000 kWh
   • 排放係數：0.45 kg CO2e/kWh（台電 2025）
   • 排放量：225 公噸 CO2e（100%）

2. 按設備類別：
   • 生產設備：157.5 公噸（70%）
   • HVAC 系統：45 公噸（20%）
   • 照明：13.5 公噸（6%）
   • 其他：9 公噸（4%）

年度進度：

• 年度目標：2,400 公噸 CO2e（-15% vs 2024）
• 年初至今：2,250 公噸 CO2e
• 剩餘預算：150 公噸 CO2e
• 預測年度總量：2,370 公噸 CO2e
• 狀態：✅ 預計達成目標

減排措施成效：

1. LED 照明升級（Q2 實施）
   • 減排：5 公噸 CO2e/月
   • 年度減排：60 公噸 CO2e
   • 貢獻：24%

2. 壓縮空氣系統優化（Q3 實施）
   • 減排：10 公噸 CO2e/月
   • 年度減排：40 公噸 CO2e（剩餘 4 個月）
   • 貢獻：16%

3. 需量管理（全年）
   • 減排：8 公噸 CO2e/月
   • 年度減排：96 公噸 CO2e
   • 貢獻：38.4%

4. 生產效率提升
   • 減排：2 公噸 CO2e/月
   • 年度減排：24 公噸 CO2e
   • 貢獻：9.6%

碳中和路徑（2025-2030）：

2025（基準年）：
• 排放：2,370 公噸 CO2e

2026（-20%）：
• 目標：1,896 公噸 CO2e
• 措施：太陽能板安裝（500 kW）

2027（-35%）：
• 目標：1,541 公噸 CO2e
• 措施：設備電氣化、效率提升

2028（-50%）：
• 目標：1,185 公噸 CO2e
• 措施：購買綠電

2029（-70%）：
• 目標：711 公噸 CO2e
• 措施：擴大綠電採購

2030（-90%，接近碳中和）：
• 目標：237 公噸 CO2e
• 措施：碳抵換

ESG 報告摘要：

環境績效：
• ✅ 碳排放強度：22.5 kg CO2e/單位產品（-12% YoY）
• ✅ 能源強度：50 kWh/單位產品（-10% YoY）
• ✅ 再生能源比例：5%（目標：10% by 2026）

合規狀態：
• ✅ ISO 14001 認證
• ✅ ISO 50001 認證
• ✅ CDP 氣候變遷問卷：A- 級

是否需要我生成完整的 ESG 報告（PDF）？"
```

---

### 場景 7: 容量規劃與擴建分析

**使用者**: 設施經理

**目標**: 評估新增生產線對電力系統的影響

**對話流程**:

```
User: "我們計畫新增一條生產線，功率 200 kW，現有電力系統能支撐嗎？"

AI Agent:
[調用 pme://devices 查詢現有設備容量]
[調用 query_power_consumption 分析當前負載]
[調用 simulate_load_scenario 模擬新增負載]

Response:
"🔌 電力容量分析與擴建建議

現有系統容量：

變壓器：
• 容量：1,000 kVA
• 當前峰值負載：750 kVA
• 利用率：75%
• 餘裕：250 kVA

配電系統：
• 主斷路器：1,600A
• 當前峰值電流：1,200A
• 利用率：75%
• 餘裕：400A

契約需量：
• 契約：500 kW
• 當前峰值：450 kW
• 利用率：90%
• 餘裕：50 kW

新增負載模擬：

情境：新增生產線（200 kW，16 小時/天）

模擬結果：

變壓器負載：
• 新峰值負載：950 kVA
• 新利用率：95%
• 餘裕：50 kVA
• 狀態：⚠️ 接近上限

配電系統：
• 新峰值電流：1,520A
• 新利用率：95%
• 餘裕：80A
• 狀態：⚠️ 接近上限

契約需量：
• 新峰值需量：650 kW
• 超過契約：150 kW（30%）
• 狀態：🚨 超限

每月超限罰款：NT$ 45,000

能源成本：
• 新增能耗：3,200 kWh/天
• 新增成本：NT$ 288,000/月
• 年度成本：NT$ 3,456,000

容量評估結論：

1. 變壓器容量：⚠️ 勉強足夠
   • 建議：可暫時使用，但需密切監控
   • 風險：夏季高溫時可能過載

2. 配電系統：⚠️ 勉強足夠
   • 建議：可暫時使用
   • 風險：餘裕不足，難以應對突發負載

3. 契約需量：🚨 不足
   • 建議：必須增加契約需量或實施需量管理
   • 風險：每月罰款 NT$ 45,000

解決方案：

方案 A：增加契約需量（推薦）
• 新契約需量：700 kW（+200 kW）
• 基本電費增加：NT$ 60,000/月
• 避免超限罰款：NT$ 45,000/月
• 淨成本：NT$ 15,000/月
• 優點：簡單、可靠
• 缺點：長期成本較高

方案 B：需量管理
• 實施需量控制系統
• 在生產線運行時減少非關鍵負載
• 投資：NT$ 500,000
• 避免罰款：NT$ 45,000/月
• 回收期：11 個月
• 優點：長期成本低
• 缺點：需要運營配合

方案 C：混合方案（最佳）
• 增加契約需量至 600 kW（+100 kW）
• 實施需量管理系統
• 基本電費增加：NT$ 30,000/月
• 投資：NT$ 500,000
• 總成本：NT$ 30,000/月 + NT$ 500,000 一次性
• 優點：平衡成本和風險
• 缺點：需要初期投資

長期建議（1-2 年內）：

1. 變壓器升級
   • 新容量：1,500 kVA
   • 投資：NT$ 2,000,000
   • 時機：下次大修或進一步擴建時

2. 配電系統升級
   • 新主斷路器：2,000A
   • 投資：NT$ 500,000
   • 時機：與變壓器升級同步

3. 太陽能板安裝
   • 容量：500 kW
   • 投資：NT$ 15,000,000
   • 減少電網依賴：30%
   • 回收期：7-8 年

建議行動計畫：

立即（1 個月內）：
1. 申請增加契約需量至 600 kW
2. 啟動需量管理系統採購

短期（3 個月內）：
3. 部署需量管理系統
4. 培訓操作人員

中期（6-12 個月）：
5. 評估太陽能板可行性
6. 規劃變壓器升級

是否需要我生成詳細的投資分析和實施時間表？"
```

---

## AI 分析能力

### 1. 異常檢測

**技術**: 時間序列分析、統計過程控制（SPC）

**應用**:
- 檢測電流、電壓、功率因數異常
- 識別設備性能下降
- 發現電力品質問題

**範例**:

```python
def detect_anomaly(device_id, metric, threshold=3.0):
    """
    使用 Z-score 檢測異常
    
    Args:
        device_id: 設備 ID
        metric: 測量指標（如 'current_l1'）
        threshold: Z-score 閾值（預設 3.0，即 3 個標準差）
    """
    # 查詢過去 30 天的數據
    data = query_historical_data(
        device_id=device_id,
        measurement_type=metric,
        start_time='2025-09-15T00:00:00Z',
        end_time='2025-10-15T00:00:00Z'
    )
    
    values = [d['value'] for d in data]
    mean = np.mean(values)
    std = np.std(values)
    
    # 計算最新值的 Z-score
    latest_value = values[-1]
    z_score = (latest_value - mean) / std
    
    if abs(z_score) > threshold:
        return {
            'anomaly_detected': True,
            'value': latest_value,
            'mean': mean,
            'std': std,
            'z_score': z_score,
            'severity': 'high' if abs(z_score) > 4 else 'medium'
        }
    else:
        return {'anomaly_detected': False}
```

### 2. 趨勢預測

**技術**: ARIMA、Prophet、LSTM

**應用**:
- 預測需量
- 預測能源消耗
- 預測設備剩餘壽命

**範例**:

```python
from prophet import Prophet

def predict_demand(forecast_horizon='24h'):
    """
    使用 Prophet 預測需量
    
    Args:
        forecast_horizon: 預測時間範圍（如 '24h', '7d'）
    """
    # 查詢歷史需量數據
    data = query_historical_data(
        measurement_type='ACTIVE_POWER',
        start_time='2025-09-01T00:00:00Z',
        end_time='2025-10-15T00:00:00Z',
        aggregation='AVERAGE',
        interval='PT15M'
    )
    
    # 準備 Prophet 數據格式
    df = pd.DataFrame({
        'ds': [d['timestamp'] for d in data],
        'y': [d['value'] for d in data]
    })
    
    # 訓練模型
    model = Prophet(
        yearly_seasonality=True,
        weekly_seasonality=True,
        daily_seasonality=True
    )
    model.fit(df)
    
    # 生成預測
    future = model.make_future_dataframe(periods=96, freq='15min')  # 24 小時
    forecast = model.predict(future)
    
    return forecast[['ds', 'yhat', 'yhat_lower', 'yhat_upper']].tail(96)
```

### 3. 根因分析

**技術**: 因果推斷、關聯規則挖掘

**應用**:
- 分析能源浪費根因
- 診斷電力品質問題來源
- 識別設備故障原因

**範例**:

```python
def root_cause_analysis(alarm_id):
    """
    分析告警根因
    
    Args:
        alarm_id: 告警 ID
    """
    # 查詢告警詳情
    alarm = get_alarm(alarm_id)
    
    # 查詢告警發生前後的數據
    device_id = alarm['device_id']
    alarm_time = alarm['timestamp']
    
    # 查詢多個相關指標
    metrics = ['voltage_l1', 'current_l1', 'power_factor', 'frequency']
    data = {}
    
    for metric in metrics:
        data[metric] = query_historical_data(
            device_id=device_id,
            measurement_type=metric,
            start_time=(alarm_time - timedelta(hours=1)),
            end_time=(alarm_time + timedelta(hours=1))
        )
    
    # 分析相關性
    correlations = {}
    for metric in metrics:
        values = [d['value'] for d in data[metric]]
        # 計算與告警發生時間的相關性
        # ...
    
    # 識別最可能的根因
    root_causes = sorted(correlations.items(), key=lambda x: x[1], reverse=True)
    
    return {
        'primary_cause': root_causes[0],
        'contributing_factors': root_causes[1:3],
        'recommendation': generate_recommendation(root_causes[0])
    }
```

### 4. 優化建議

**技術**: 線性規劃、遺傳算法、強化學習

**應用**:
- 優化負載分配
- 優化需量控制
- 優化維護排程

**範例**:

```python
from scipy.optimize import linprog

def optimize_load_distribution(target_demand, devices):
    """
    使用線性規劃優化負載分配
    
    Args:
        target_demand: 目標需量（kW）
        devices: 可控設備清單
    """
    # 目標函數：最小化舒適度影響
    c = [device['comfort_impact'] for device in devices]
    
    # 不等式約束：總負載削減 >= 需要削減的量
    current_demand = get_current_demand()
    required_reduction = current_demand - target_demand
    
    A_ub = [[-device['max_reduction'] for device in devices]]
    b_ub = [-required_reduction]
    
    # 變數邊界：0 <= x <= 1（削減比例）
    bounds = [(0, 1) for _ in devices]
    
    # 求解
    result = linprog(c, A_ub=A_ub, b_ub=b_ub, bounds=bounds, method='highs')
    
    # 生成優化方案
    optimized_schedule = []
    for i, device in enumerate(devices):
        reduction_ratio = result.x[i]
        if reduction_ratio > 0.01:  # 只包含有意義的削減
            optimized_schedule.append({
                'device_id': device['id'],
                'action': 'reduce_load',
                'reduction': device['max_reduction'] * reduction_ratio,
                'comfort_impact': device['comfort_impact'] * reduction_ratio
            })
    
    return {
        'schedule': optimized_schedule,
        'total_reduction': sum([s['reduction'] for s in optimized_schedule]),
        'total_comfort_impact': result.fun
    }
```

---

## 實作範例

### 完整的 MCP Server 實作

```python
# ndh/mcp_server/pme_tools.py
from mcp import Tool, Resource, Prompt
from typing import Dict, Any, List

class PMETools:
    """PME 專用 MCP Tools"""
    
    def __init__(self, pme_connector):
        self.pme = pme_connector
        
    def get_resources(self) -> List[Resource]:
        """定義 Resources"""
        return [
            Resource(
                uri="pme://devices",
                name="PME Devices",
                description="電力設備清單",
                mime_type="application/json"
            ),
            Resource(
                uri="pme://measurements/{device_id}",
                name="PME Measurements",
                description="設備即時電力數據",
                mime_type="application/json"
            ),
            Resource(
                uri="pme://alarms",
                name="PME Alarms",
                description="電力告警清單",
                mime_type="application/json"
            ),
            Resource(
                uri="pme://energy-report",
                name="Energy Report",
                description="能源報告",
                mime_type="application/json"
            )
        ]
        
    def get_tools(self) -> List[Tool]:
        """定義 Tools"""
        return [
            Tool(
                name="query_power_consumption",
                description="查詢電力消耗",
                input_schema={
                    "type": "object",
                    "properties": {
                        "device_ids": {
                            "type": "array",
                            "items": {"type": "string"},
                            "description": "設備 ID 列表"
                        },
                        "start_time": {
                            "type": "string",
                            "description": "開始時間（ISO 8601 格式）"
                        },
                        "end_time": {
                            "type": "string",
                            "description": "結束時間（ISO 8601 格式）"
                        },
                        "aggregation": {
                            "type": "string",
                            "enum": ["hourly", "daily", "weekly"],
                            "description": "聚合方式"
                        }
                    },
                    "required": ["device_ids", "start_time", "end_time"]
                }
            ),
            Tool(
                name="analyze_energy_efficiency",
                description="分析能源效率",
                input_schema={
                    "type": "object",
                    "properties": {
                        "scope": {
                            "type": "string",
                            "enum": ["factory", "building", "zone", "device"],
                            "description": "分析範圍"
                        },
                        "period": {
                            "type": "string",
                            "enum": ["last_7_days", "last_30_days", "last_year"],
                            "description": "分析期間"
                        },
                        "baseline": {
                            "type": "string",
                            "enum": ["industry_average", "historical", "target"],
                            "description": "基準線"
                        }
                    },
                    "required": ["scope", "period"]
                }
            ),
            # ... 其他 Tools
        ]
        
    def get_prompts(self) -> List[Prompt]:
        """定義 Prompts"""
        return [
            Prompt(
                name="energy_efficiency_analysis",
                description="能源效率分析提示",
                arguments=[
                    {
                        "name": "scope",
                        "description": "分析範圍",
                        "required": True
                    }
                ]
            ),
            # ... 其他 Prompts
        ]
        
    async def handle_tool_call(self, tool_name: str, arguments: Dict[str, Any]) -> Dict[str, Any]:
        """處理 Tool 調用"""
        if tool_name == "query_power_consumption":
            return await self._query_power_consumption(**arguments)
        elif tool_name == "analyze_energy_efficiency":
            return await self._analyze_energy_efficiency(**arguments)
        # ... 其他 Tools
        
    async def _query_power_consumption(
        self,
        device_ids: List[str],
        start_time: str,
        end_time: str,
        aggregation: str = "hourly"
    ) -> Dict[str, Any]:
        """查詢電力消耗實作"""
        data = []
        
        for device_id in device_ids:
            historical_data = await self.pme.query_historical_data(
                device_id=device_id,
                measurement_type='ACTIVE_POWER',
                start_time=start_time,
                end_time=end_time,
                aggregation='AVERAGE',
                interval='PT1H' if aggregation == 'hourly' else 'PT24H'
            )
            
            data.extend([
                {
                    'device_id': device_id,
                    'timestamp': d['timestamp'],
                    'consumption': d['value'],
                    'unit': d['unit']
                }
                for d in historical_data
            ])
        
        # 計算摘要
        total = sum([d['consumption'] for d in data])
        average = total / len(data) if data else 0
        peak = max([d['consumption'] for d in data]) if data else 0
        
        return {
            'data': data,
            'summary': {
                'total': total,
                'average': average,
                'peak': peak,
                'peak_time': max(data, key=lambda x: x['consumption'])['timestamp'] if data else None
            }
        }
```

---

## 價值分析

### 定量價值

| 價值類別 | 年度價值 | 計算基礎 |
|---------|---------|---------|
| **能源成本節省** | NT$ 5,000,000 | 10% 能源效率提升 × NT$ 50M 年度電費 |
| **需量管理節省** | NT$ 2,000,000 | 避免超限罰款 + 優化契約需量 |
| **預測性維護** | NT$ 10,000,000 | 避免計畫外停機損失 |
| **電力品質改善** | NT$ 3,000,000 | 減少設備故障、延長壽命 |
| **人力成本節省** | NT$ 2,000,000 | 自動化報告、分析、決策 |
| **總計** | **NT$ 22,000,000** | |

### 定性價值

1. ✅ **提升決策速度**: 從數天縮短到數分鐘
2. ✅ **降低專業門檻**: 非專家也能進行複雜分析
3. ✅ **增強可見性**: 即時洞察電力系統狀況
4. ✅ **改善合規性**: 自動化 ESG 報告
5. ✅ **提升競爭力**: 數位化轉型領先者

---

## 未來展望

### 短期（6-12 個月）

1. **多語言支援**: 支援英文、日文、韓文
2. **語音互動**: 整合語音助理（Alexa、Google Assistant）
3. **移動應用**: 開發移動端 App
4. **更多 AI 模型**: 整合更先進的預測模型

### 中期（1-2 年）

5. **自主決策**: AI 自動執行需量控制、負載優化
6. **多系統整合**: 整合 BMS、MES、ERP
7. **數位分身**: 完整的電力系統數位分身
8. **協作 AI**: 多個 AI Agent 協同工作

### 長期（2-5 年）

9. **通用能源 AI**: 支援所有能源管理系統（不限 PME）
10. **自我學習**: 持續學習優化策略
11. **生態系統**: 建立開放的能源 AI 平台
12. **碳中和助手**: 全面的碳中和規劃和執行支援

---

## 總結

結合 **Schneider PME**、**NDH MCP Server** 和 **AI Agent**，創造出一個革命性的智能電力管理系統，實現：

1. ✅ **自然語言互動**: 使用者可以用自然語言與電力系統對話
2. ✅ **即時洞察**: 秒級獲得電力系統狀況和分析
3. ✅ **智能建議**: AI 提供專業的節能、維護、優化建議
4. ✅ **自動化決策**: 自動執行需量控制、負載優化
5. ✅ **全面整合**: 電力、製程、建築、環境數據統一管理
6. ✅ **巨大價值**: 年度價值 NT$ 22,000,000+

這是工業數位化轉型的重要里程碑，為智能製造、智能建築、智能能源管理開啟了新的可能性。

---

**© 2025 IDTF Consortium. All rights reserved.**

