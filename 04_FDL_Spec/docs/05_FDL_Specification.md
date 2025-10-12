# FDL (Factory Design Language) 規範

**文件編號**: IDTF-V3.3-05  
**版本**: 1.0  
**日期**: 2025-10-12  
**作者**: C.C. Lin

## 概述

FDL (Factory Design Language) 是用於定義工廠佈局和配置的聲明式語言。它描述資產實例的數量、位置和連接關係。

## FDL 語法

### 基本結構

\`\`\`yaml
factory_layout:
  - area: <區域名稱>
    building: <建築名稱>
    floor: <樓層>
    instances:
      - type: <IADL 資產類型>
        count: <數量>
        naming_prefix: "<命名前綴>"
        location: "<x:y:z 座標>"
        initial_params:
          <參數名>: <參數值>
    relationships:
      - from: "<資產 ID>"
        to: "<資產 ID>"
        type: "<關係類型>"
        properties:
          <屬性名>: <屬性值>
\`\`\`

### 範例

\`\`\`yaml
factory_layout:
  - area: ProductionFloor1
    building: MainBuilding
    floor: 1
    instances:
      - type: StandardPump
        count: 5
        naming_prefix: "Pump_"
        location: "x:10, y:20, z:0"
        initial_params:
          power: 15
          status: "Stopped"
    relationships:
      - from: "Pump_001"
        to: "Tank_001"
        type: "pipe"
        properties:
          diameter: 50mm
\`\`\`

## FDL 與 IADL 的關係

- IADL 定義「資產是什麼」(藍圖)
- FDL 定義「工廠如何佈局」(實例)

---

**參考**: [Factory Designer](04_Factory_Designer.md) | [IADL 規範](02_IADL_Specification.md)
