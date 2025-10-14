# IADL 與主流工廠設計軟體整合設計文件

**版本**: IDTF V3.5  
**日期**: 2025年10月14日  
**狀態**: 設計階段

---

## 目錄

1. [概述](#概述)
2. [IADL 在整合中的角色](#iadl-在整合中的角色)
3. [從工廠設計軟體自動生成 IADL](#從工廠設計軟體自動生成-iadl)
4. [IADL 與設備庫的整合](#iadl-與設備庫的整合)
5. [IADL 編輯器增強](#iadl-編輯器增強)
6. [完整的整合工作流](#完整的整合工作流)
7. [實作細節](#實作細節)
8. [使用場景](#使用場景)
9. [實作路線圖](#實作路線圖)

---

## 1. 概述

### 1.1 為什麼需要 IADL 整合？

在之前的 FDL 整合設計中，我們主要關注如何從工廠設計軟體匯入**工廠佈局和資產實例**。但是，FDL 中的資產實例需要引用 **IADL 資產定義**。因此，完整的整合方案應該包括：

1. **IADL 整合**：從工廠設計軟體的設備庫自動生成 IADL 資產定義
2. **FDL 整合**：從工廠設計軟體的 3D 模型自動生成 FDL 工廠佈局

這樣才能實現**端到端的自動化**，從設計到數位分身的無縫轉換。

### 1.2 IADL 整合的價值

**對設計師**：
- 無需手動編寫 IADL 定義
- 直接使用工廠設計軟體中的設備庫
- 保持設備規格的一致性

**對專案**：
- 縮短 IADL 建置時間（節省 70-80%）
- 減少錯誤和不一致
- 提高設計與執行的一致性

**對系統**：
- 建立標準化的設備庫
- 支援設備規格的版本管理
- 促進設備定義的重用

### 1.3 設計原則

1. **自動化優先**：盡可能自動生成 IADL，減少手動編寫
2. **保留語義**：保留工廠設計軟體中的設備規格和屬性
3. **可擴展性**：支援自定義屬性和行為
4. **標準化**：建立標準化的設備庫
5. **版本管理**：支援 IADL 的版本控制

---

## 2. IADL 在整合中的角色

### 2.1 整合架構更新

```
┌─────────────────────────────────────────────────────────────┐
│              工廠設計軟體                                      │
│  (Plant 3D / AVEVA E3D / Smart 3D)                          │
│                                                               │
│  ┌──────────────────┐        ┌──────────────────┐          │
│  │   設備庫         │        │   3D 模型        │          │
│  │ (Equipment Lib)  │        │ (3D Model)       │          │
│  └──────────────────┘        └──────────────────┘          │
└────────┬─────────────────────────────┬───────────────────────┘
         │                             │
         │ 提取設備規格                │ 提取資產實例
         ↓                             ↓
┌─────────────────────┐       ┌─────────────────────┐
│  IADL Generator     │       │  FDL Generator      │
│  (資產定義生成器)    │       │  (工廠佈局生成器)    │
└─────────────────────┘       └─────────────────────┘
         │                             │
         ↓                             ↓
┌─────────────────────┐       ┌─────────────────────┐
│  IADL Files         │       │  FDL Files          │
│  (資產定義)          │←──────│  (工廠佈局)          │
│                     │ 引用   │                     │
└─────────────────────┘       └─────────────────────┘
         │                             │
         └──────────────┬──────────────┘
                        ↓
                ┌───────────────┐
                │  NDH          │
                │ (數位分身執行) │
                └───────────────┘
```

### 2.2 IADL 與 FDL 的關係

**IADL (Industrial Asset Definition Language)**：
- 定義**資產類型**（Asset Types）
- 描述資產的**屬性、參數、行為**
- 類似於**類別定義**（Class Definition）

**FDL (Factory Design Language)**：
- 定義**資產實例**（Asset Instances）
- 描述資產的**位置、配置、連接**
- 類似於**物件實例**（Object Instance）

**關係**：
```yaml
# IADL: 定義 Pump 資產類型
asset_types:
  - name: centrifugal_pump
    category: pump
    parameters:
      - name: flow_rate
        type: float
        unit: m3/h
      - name: head
        type: float
        unit: m

# FDL: 使用 Pump 資產類型創建實例
assets:
  - id: pump-001
    asset_type: centrifugal_pump  # 引用 IADL 定義
    instance_params:
      flow_rate: 100.0
      head: 50.0
```

---

## 3. 從工廠設計軟體自動生成 IADL

### 3.1 設備庫分析

#### 3.1.1 Autodesk Plant 3D 設備庫

**設備庫結構**：
- **Catalog Files (.pcat)**：包含設備規格
- **Spec Files**：包含管線規格
- **Equipment Templates**：設備模板

**設備屬性**：
```
Equipment:
  - Tag (標籤)
  - Type (類型)
  - Description (描述)
  - Manufacturer (製造商)
  - Model (型號)
  - Size (尺寸)
  - Weight (重量)
  - Material (材質)
  - Custom Properties (自定義屬性)
```

**提取方式**：
- 使用 Plant 3D API 讀取 Catalog
- 解析 .pcat 檔案（SQLite 格式）

#### 3.1.2 AVEVA E3D 設備庫

**設備庫結構**：
- **Catalogues**：設備目錄
- **Specifications**：規格定義
- **Component Library**：組件庫

**設備屬性**：
```
Equipment:
  - Name (名稱)
  - Type (類型)
  - Description (描述)
  - Manufacturer (製造商)
  - Model (型號)
  - Capacity (容量)
  - Operating Pressure (操作壓力)
  - Operating Temperature (操作溫度)
  - Material (材質)
  - Attributes (屬性)
```

**提取方式**：
- 使用 AVEVA Connect API
- 匯出 Catalogue 為 XML/CSV

#### 3.1.3 Intergraph Smart 3D 設備庫

**設備庫結構**：
- **Smart Reference Data**：參考數據
- **Catalog**：設備目錄
- **Specifications**：規格定義

**設備屬性**：
```
Equipment:
  - Name (名稱)
  - Type (類型)
  - Description (描述)
  - Manufacturer (製造商)
  - Model (型號)
  - Rating (額定值)
  - Properties (屬性)
```

**提取方式**：
- 使用 Smart 3D Web API
- 使用 Smart Reference Data API

### 3.2 IADL 生成策略

#### 3.2.1 設備類型映射

**映射表**：

| 工廠設計軟體設備類型 | IADL 資產類別 | IADL 資產類型 |
|-------------------|--------------|--------------|
| Centrifugal Pump | pump | centrifugal_pump |
| Positive Displacement Pump | pump | positive_displacement_pump |
| Vertical Tank | tank | vertical_tank |
| Horizontal Tank | tank | horizontal_tank |
| Shell and Tube Heat Exchanger | heat_exchanger | shell_and_tube_heat_exchanger |
| Plate Heat Exchanger | heat_exchanger | plate_heat_exchanger |
| Ball Valve | valve | ball_valve |
| Gate Valve | valve | gate_valve |
| Globe Valve | valve | globe_valve |
| Centrifugal Compressor | compressor | centrifugal_compressor |
| Reciprocating Compressor | compressor | reciprocating_compressor |
| Batch Reactor | reactor | batch_reactor |
| Continuous Reactor | reactor | continuous_reactor |
| Belt Conveyor | conveyor | belt_conveyor |
| Screw Conveyor | conveyor | screw_conveyor |
| Static Mixer | mixer | static_mixer |
| Dynamic Mixer | mixer | dynamic_mixer |

#### 3.2.2 屬性映射

**通用屬性映射**：

| 工廠設計軟體屬性 | IADL 參數 | 類型 | 單位 |
|----------------|----------|------|------|
| Manufacturer | manufacturer | string | - |
| Model | model | string | - |
| Description | description | string | - |
| Material | material | string | - |
| Weight | weight | float | kg |
| Size | dimensions | object | mm |

**泵特定屬性映射**：

| 工廠設計軟體屬性 | IADL 參數 | 類型 | 單位 |
|----------------|----------|------|------|
| Flow Rate | flow_rate | float | m3/h |
| Head | head | float | m |
| Power | power | float | kW |
| Efficiency | efficiency | float | % |
| NPSH Required | npsh_required | float | m |
| Suction Pressure | suction_pressure | float | bar |
| Discharge Pressure | discharge_pressure | float | bar |

**儲罐特定屬性映射**：

| 工廠設計軟體屬性 | IADL 參數 | 類型 | 單位 |
|----------------|----------|------|------|
| Capacity | capacity | float | m3 |
| Diameter | diameter | float | mm |
| Height | height | float | mm |
| Design Pressure | design_pressure | float | bar |
| Design Temperature | design_temperature | float | °C |
| Material | material | string | - |

#### 3.2.3 IADL 生成算法

```python
class IADLGenerator:
    """IADL 生成器"""
    
    def __init__(self, equipment_library: EquipmentLibrary):
        self.equipment_library = equipment_library
        self.type_mapper = TypeMapper()
        self.attribute_mapper = AttributeMapper()
    
    def generate_iadl_from_equipment_library(self) -> Dict[str, AssetType]:
        """從設備庫生成 IADL"""
        
        asset_types = {}
        
        # 遍歷設備庫中的所有設備類型
        for equipment_type in self.equipment_library.get_all_types():
            # 映射設備類型到 IADL 資產類型
            iadl_type_name = self.type_mapper.map_type(equipment_type.name)
            iadl_category = self.type_mapper.map_category(equipment_type.category)
            
            # 創建 IADL 資產類型
            asset_type = AssetType(
                name=iadl_type_name,
                category=iadl_category,
                description=equipment_type.description,
                version="1.0"
            )
            
            # 映射屬性到 IADL 參數
            for attr in equipment_type.attributes:
                iadl_param = self.attribute_mapper.map_attribute(attr)
                if iadl_param:
                    asset_type.add_parameter(iadl_param)
            
            # 添加預設行為
            self.add_default_behaviors(asset_type, iadl_category)
            
            # 添加連接點
            self.add_connection_points(asset_type, equipment_type)
            
            asset_types[iadl_type_name] = asset_type
        
        return asset_types
    
    def add_default_behaviors(self, asset_type: AssetType, category: str):
        """添加預設行為"""
        
        if category == "pump":
            # 泵的預設行為
            asset_type.add_behavior(Behavior(
                name="calculate_power",
                description="計算泵的功率消耗",
                inputs=["flow_rate", "head", "efficiency"],
                outputs=["power"],
                logic="power = (flow_rate * head * 9.81) / (3600 * efficiency / 100)"
            ))
            
            asset_type.add_behavior(Behavior(
                name="check_cavitation",
                description="檢查是否發生氣蝕",
                inputs=["suction_pressure", "npsh_required"],
                outputs=["cavitation_risk"],
                logic="cavitation_risk = suction_pressure < npsh_required"
            ))
        
        elif category == "tank":
            # 儲罐的預設行為
            asset_type.add_behavior(Behavior(
                name="calculate_level",
                description="計算液位",
                inputs=["volume", "capacity"],
                outputs=["level"],
                logic="level = (volume / capacity) * 100"
            ))
            
            asset_type.add_behavior(Behavior(
                name="check_overflow",
                description="檢查是否溢出",
                inputs=["volume", "capacity"],
                outputs=["overflow_risk"],
                logic="overflow_risk = volume > capacity * 0.95"
            ))
    
    def add_connection_points(self, asset_type: AssetType, equipment_type: EquipmentType):
        """添加連接點"""
        
        # 從設備類型中提取 Nozzle 資訊
        for nozzle in equipment_type.nozzles:
            connection_point = ConnectionPoint(
                name=nozzle.name,
                type=nozzle.type,  # inlet, outlet, drain, vent
                size=nozzle.size,
                rating=nozzle.rating
            )
            asset_type.add_connection_point(connection_point)
    
    def export_to_yaml(self, asset_types: Dict[str, AssetType], output_dir: str):
        """匯出為 IADL YAML 檔案"""
        
        for type_name, asset_type in asset_types.items():
            filename = f"{type_name}.iadl.yaml"
            filepath = os.path.join(output_dir, filename)
            
            yaml_content = asset_type.to_yaml()
            
            with open(filepath, 'w') as f:
                f.write(yaml_content)
```

### 3.3 IADL 生成範例

#### 3.3.1 從 Plant 3D 生成泵的 IADL

**Plant 3D 設備規格**：
```
Equipment Type: Centrifugal Pump
Manufacturer: Grundfos
Model: CR 64-2
Flow Rate: 100 m3/h
Head: 50 m
Power: 30 kW
Efficiency: 75%
NPSH Required: 3.5 m
Suction Size: DN 150
Discharge Size: DN 100
Material: Cast Iron
Weight: 250 kg
```

**生成的 IADL**：
```yaml
asset_type:
  name: centrifugal_pump
  category: pump
  version: "1.0"
  description: "離心泵"
  
  parameters:
    # 基本資訊
    - name: manufacturer
      type: string
      description: "製造商"
      default: "Grundfos"
    
    - name: model
      type: string
      description: "型號"
      default: "CR 64-2"
    
    # 性能參數
    - name: flow_rate
      type: float
      unit: m3/h
      description: "流量"
      min: 0
      max: 150
      default: 100
    
    - name: head
      type: float
      unit: m
      description: "揚程"
      min: 0
      max: 80
      default: 50
    
    - name: power
      type: float
      unit: kW
      description: "功率"
      min: 0
      max: 50
      default: 30
    
    - name: efficiency
      type: float
      unit: "%"
      description: "效率"
      min: 0
      max: 100
      default: 75
    
    - name: npsh_required
      type: float
      unit: m
      description: "必需汽蝕餘量"
      default: 3.5
    
    # 物理屬性
    - name: material
      type: string
      description: "材質"
      default: "Cast Iron"
    
    - name: weight
      type: float
      unit: kg
      description: "重量"
      default: 250
  
  # 連接點
  connection_points:
    - name: suction
      type: inlet
      size: DN 150
      description: "吸入口"
    
    - name: discharge
      type: outlet
      size: DN 100
      description: "排出口"
  
  # 行為
  behaviors:
    - name: calculate_power
      description: "計算實際功率消耗"
      inputs:
        - flow_rate
        - head
        - efficiency
      outputs:
        - actual_power
      logic: |
        actual_power = (flow_rate * head * 9.81) / (3600 * efficiency / 100)
    
    - name: check_cavitation
      description: "檢查氣蝕風險"
      inputs:
        - suction_pressure
        - npsh_required
      outputs:
        - cavitation_risk
      logic: |
        npsh_available = suction_pressure / (9.81 * 1000) + 10.33
        cavitation_risk = npsh_available < npsh_required
  
  # 視覺化
  visualization:
    3d_model: "models/centrifugal_pump.gltf"
    icon: "icons/pump.svg"
    color: "#4A90E2"
```

#### 3.3.2 從 AVEVA E3D 生成儲罐的 IADL

**E3D 設備規格**：
```
Equipment Type: Vertical Tank
Manufacturer: Custom
Capacity: 50 m3
Diameter: 3000 mm
Height: 7000 mm
Design Pressure: 2.5 bar
Design Temperature: 80°C
Material: Stainless Steel 304
Nozzles:
  - Inlet: DN 100, Top
  - Outlet: DN 80, Bottom
  - Vent: DN 50, Top
  - Drain: DN 50, Bottom
```

**生成的 IADL**：
```yaml
asset_type:
  name: vertical_tank
  category: tank
  version: "1.0"
  description: "立式儲罐"
  
  parameters:
    # 基本資訊
    - name: manufacturer
      type: string
      description: "製造商"
      default: "Custom"
    
    # 容量參數
    - name: capacity
      type: float
      unit: m3
      description: "容量"
      default: 50
    
    - name: diameter
      type: float
      unit: mm
      description: "直徑"
      default: 3000
    
    - name: height
      type: float
      unit: mm
      description: "高度"
      default: 7000
    
    # 設計參數
    - name: design_pressure
      type: float
      unit: bar
      description: "設計壓力"
      default: 2.5
    
    - name: design_temperature
      type: float
      unit: "°C"
      description: "設計溫度"
      default: 80
    
    - name: material
      type: string
      description: "材質"
      default: "Stainless Steel 304"
    
    # 運行參數
    - name: current_volume
      type: float
      unit: m3
      description: "當前容量"
      min: 0
      max: 50
      default: 0
    
    - name: current_level
      type: float
      unit: "%"
      description: "當前液位"
      min: 0
      max: 100
      default: 0
    
    - name: current_pressure
      type: float
      unit: bar
      description: "當前壓力"
      min: 0
      max: 3.0
      default: 1.0
    
    - name: current_temperature
      type: float
      unit: "°C"
      description: "當前溫度"
      min: 0
      max: 100
      default: 25
  
  # 連接點
  connection_points:
    - name: inlet
      type: inlet
      size: DN 100
      location: top
      description: "進料口"
    
    - name: outlet
      type: outlet
      size: DN 80
      location: bottom
      description: "出料口"
    
    - name: vent
      type: vent
      size: DN 50
      location: top
      description: "排氣口"
    
    - name: drain
      type: drain
      size: DN 50
      location: bottom
      description: "排放口"
  
  # 行為
  behaviors:
    - name: calculate_level
      description: "計算液位百分比"
      inputs:
        - current_volume
        - capacity
      outputs:
        - current_level
      logic: |
        current_level = (current_volume / capacity) * 100
    
    - name: check_overflow
      description: "檢查溢出風險"
      inputs:
        - current_volume
        - capacity
      outputs:
        - overflow_risk
      logic: |
        overflow_risk = current_volume > capacity * 0.95
    
    - name: check_pressure
      description: "檢查壓力超限"
      inputs:
        - current_pressure
        - design_pressure
      outputs:
        - pressure_alarm
      logic: |
        pressure_alarm = current_pressure > design_pressure * 0.9
  
  # 視覺化
  visualization:
    3d_model: "models/vertical_tank.gltf"
    icon: "icons/tank.svg"
    color: "#7ED321"
```

---

## 4. IADL 與設備庫的整合

### 4.1 建立標準化設備庫

**目標**：建立一個標準化的 IADL 設備庫，包含常用的工業設備類型。

**設備庫結構**：
```
iadl_library/
├── pumps/
│   ├── centrifugal_pump.iadl.yaml
│   ├── positive_displacement_pump.iadl.yaml
│   ├── submersible_pump.iadl.yaml
│   └── ...
├── tanks/
│   ├── vertical_tank.iadl.yaml
│   ├── horizontal_tank.iadl.yaml
│   ├── spherical_tank.iadl.yaml
│   └── ...
├── heat_exchangers/
│   ├── shell_and_tube_heat_exchanger.iadl.yaml
│   ├── plate_heat_exchanger.iadl.yaml
│   └── ...
├── valves/
│   ├── ball_valve.iadl.yaml
│   ├── gate_valve.iadl.yaml
│   ├── globe_valve.iadl.yaml
│   └── ...
├── compressors/
│   ├── centrifugal_compressor.iadl.yaml
│   ├── reciprocating_compressor.iadl.yaml
│   └── ...
├── reactors/
│   ├── batch_reactor.iadl.yaml
│   ├── continuous_reactor.iadl.yaml
│   └── ...
├── conveyors/
│   ├── belt_conveyor.iadl.yaml
│   ├── screw_conveyor.iadl.yaml
│   └── ...
└── mixers/
    ├── static_mixer.iadl.yaml
    ├── dynamic_mixer.iadl.yaml
    └── ...
```

### 4.2 設備庫管理系統

**功能**：
- 瀏覽和搜尋 IADL 設備庫
- 匯入工廠設計軟體的設備規格
- 自動生成 IADL 定義
- 編輯和自定義 IADL 定義
- 版本管理
- 匯出和分享

**實作**：
```python
class IADLLibraryManager:
    """IADL 設備庫管理器"""
    
    def __init__(self, library_path: str):
        self.library_path = library_path
        self.asset_types = {}
        self.load_library()
    
    def load_library(self):
        """載入設備庫"""
        
        for root, dirs, files in os.walk(self.library_path):
            for file in files:
                if file.endswith('.iadl.yaml'):
                    filepath = os.path.join(root, file)
                    asset_type = self.load_asset_type(filepath)
                    self.asset_types[asset_type.name] = asset_type
    
    def search(self, query: str, category: str = None) -> List[AssetType]:
        """搜尋設備"""
        
        results = []
        
        for asset_type in self.asset_types.values():
            # 按類別篩選
            if category and asset_type.category != category:
                continue
            
            # 按名稱或描述搜尋
            if query.lower() in asset_type.name.lower() or \
               query.lower() in asset_type.description.lower():
                results.append(asset_type)
        
        return results
    
    def import_from_plant_design_software(self, 
                                          software: str, 
                                          equipment_library: EquipmentLibrary):
        """從工廠設計軟體匯入設備"""
        
        generator = IADLGenerator(equipment_library)
        new_asset_types = generator.generate_iadl_from_equipment_library()
        
        # 合併到現有設備庫
        for type_name, asset_type in new_asset_types.items():
            if type_name in self.asset_types:
                # 檢查是否需要更新
                if self.should_update(self.asset_types[type_name], asset_type):
                    self.update_asset_type(type_name, asset_type)
            else:
                # 新增設備類型
                self.add_asset_type(asset_type)
    
    def export_to_directory(self, output_dir: str):
        """匯出設備庫到目錄"""
        
        for asset_type in self.asset_types.values():
            category_dir = os.path.join(output_dir, asset_type.category + 's')
            os.makedirs(category_dir, exist_ok=True)
            
            filename = f"{asset_type.name}.iadl.yaml"
            filepath = os.path.join(category_dir, filename)
            
            with open(filepath, 'w') as f:
                f.write(asset_type.to_yaml())
```

### 4.3 設備庫版本管理

**使用 Git 進行版本管理**：
```bash
# 初始化 IADL 設備庫倉庫
cd iadl_library
git init
git add .
git commit -m "Initial IADL library"

# 從工廠設計軟體匯入新設備
python import_from_plant3d.py --project "Factory_A"

# 檢查變更
git status
git diff

# 提交變更
git add pumps/centrifugal_pump_v2.iadl.yaml
git commit -m "Update centrifugal pump definition from Plant 3D"

# 標記版本
git tag -a v1.1 -m "IADL Library v1.1"

# 推送到遠端倉庫
git push origin master
git push origin v1.1
```

---

## 5. IADL 編輯器增強

### 5.1 新增功能

#### 5.1.1 從工廠設計軟體匯入

**介面設計**：
```
┌────────────────────────────────────────────────────────────┐
│  IADL Editor - 從工廠設計軟體匯入                             │
├────────────────────────────────────────────────────────────┤
│                                                              │
│  步驟 1: 選擇工廠設計軟體                                     │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  ○ Autodesk Plant 3D                                  │  │
│  │  ○ AVEVA E3D                                          │  │
│  │  ○ Intergraph Smart 3D                                │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
│  步驟 2: 連接到設備庫                                        │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  Catalog 路徑: [________________] [瀏覽]              │  │
│  │  API 端點:     [________________]                     │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
│  步驟 3: 選擇設備類型                                        │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  ☑ Centrifugal Pump (5 variants)                      │  │
│  │  ☑ Vertical Tank (3 variants)                         │  │
│  │  ☑ Heat Exchanger (4 variants)                        │  │
│  │  ☐ Ball Valve (8 variants)                            │  │
│  │  ...                                                   │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
│  步驟 4: 預覽生成的 IADL                                     │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  asset_type:                                           │  │
│  │    name: centrifugal_pump                             │  │
│  │    category: pump                                      │  │
│  │    parameters:                                         │  │
│  │      - name: flow_rate                                │  │
│  │        type: float                                     │  │
│  │        unit: m3/h                                      │  │
│  │      ...                                               │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
│  [上一步]  [匯入]  [取消]                                    │
└────────────────────────────────────────────────────────────┘
```

#### 5.1.2 智能參數建議

**功能**：
- 根據設備類型自動建議常用參數
- 根據行業標準建議參數範圍
- 根據相似設備建議參數

**實作**：
```python
class SmartParameterSuggester:
    """智能參數建議器"""
    
    def __init__(self):
        self.parameter_templates = self.load_parameter_templates()
    
    def suggest_parameters(self, asset_type: str, category: str) -> List[Parameter]:
        """建議參數"""
        
        suggestions = []
        
        # 從模板中獲取常用參數
        if category in self.parameter_templates:
            template_params = self.parameter_templates[category]
            suggestions.extend(template_params)
        
        # 根據資產類型添加特定參數
        if asset_type == "centrifugal_pump":
            suggestions.extend([
                Parameter(name="flow_rate", type="float", unit="m3/h"),
                Parameter(name="head", type="float", unit="m"),
                Parameter(name="efficiency", type="float", unit="%"),
                Parameter(name="npsh_required", type="float", unit="m")
            ])
        
        return suggestions
    
    def suggest_parameter_range(self, parameter: Parameter, asset_type: str) -> Tuple[float, float]:
        """建議參數範圍"""
        
        # 根據行業標準和歷史數據建議範圍
        if parameter.name == "flow_rate" and asset_type == "centrifugal_pump":
            return (0, 500)  # 0-500 m3/h
        
        elif parameter.name == "head" and asset_type == "centrifugal_pump":
            return (0, 200)  # 0-200 m
        
        # ...
        
        return (None, None)
```

#### 5.1.3 IADL 驗證與錯誤檢查

**功能**：
- 檢查 IADL 語法錯誤
- 檢查參數定義的完整性
- 檢查行為邏輯的正確性
- 檢查連接點定義的合理性

**實作**：
```python
class IADLValidator:
    """IADL 驗證器"""
    
    def validate(self, iadl_file: str) -> ValidationResult:
        """驗證 IADL"""
        
        result = ValidationResult()
        
        # 語法檢查
        syntax_errors = self.check_syntax(iadl_file)
        result.add_errors("syntax", syntax_errors)
        
        # 參數定義檢查
        param_errors = self.check_parameters(iadl_file)
        result.add_errors("parameters", param_errors)
        
        # 行為邏輯檢查
        behavior_errors = self.check_behaviors(iadl_file)
        result.add_errors("behaviors", behavior_errors)
        
        # 連接點檢查
        connection_errors = self.check_connection_points(iadl_file)
        result.add_errors("connections", connection_errors)
        
        return result
    
    def check_parameters(self, iadl_file: str) -> List[str]:
        """檢查參數定義"""
        
        errors = []
        
        iadl = yaml.safe_load(open(iadl_file))
        
        for param in iadl['asset_type'].get('parameters', []):
            # 檢查必要欄位
            if 'name' not in param:
                errors.append("Parameter missing 'name'")
            
            if 'type' not in param:
                errors.append(f"Parameter '{param.get('name', 'unknown')}' missing 'type'")
            
            # 檢查類型有效性
            if param.get('type') not in ['string', 'integer', 'float', 'boolean', 'object', 'array']:
                errors.append(f"Parameter '{param['name']}' has invalid type: {param['type']}")
            
            # 檢查範圍合理性
            if 'min' in param and 'max' in param:
                if param['min'] > param['max']:
                    errors.append(f"Parameter '{param['name']}' has min > max")
        
        return errors
```

#### 5.1.4 視覺化設備屬性

**功能**：
- 視覺化顯示設備的參數和連接點
- 3D 預覽設備模型
- 顯示設備的行為邏輯流程圖

---

## 6. 完整的整合工作流

### 6.1 端到端工作流

```
1. 在工廠設計軟體中設計工廠
   ├── 建立設備庫（或使用現有設備庫）
   └── 設計 3D 工廠佈局
   
2. 匯出 IADL 資產定義
   ├── 連接到工廠設計軟體的設備庫
   ├── 提取設備規格
   ├── 自動生成 IADL 定義
   ├── 在 IADL Editor 中檢查和自定義
   └── 保存到 IADL 設備庫
   
3. 匯出 FDL 工廠佈局
   ├── 連接到工廠設計軟體的 3D 模型
   ├── 提取資產實例
   ├── 自動生成 FDL 定義
   ├── 在 FDL Editor 中檢查和調整
   └── 保存 FDL 檔案
   
4. 驗證整合
   ├── 檢查 FDL 中的資產實例是否引用了有效的 IADL 定義
   ├── 檢查參數的一致性
   └── 檢查連接關係的合理性
   
5. 部署到 NDH
   ├── 上傳 IADL 定義到 NDH
   ├── 上傳 FDL 定義到 NDH
   ├── NDH 創建數位分身
   └── 開始執行
```

### 6.2 工作流範例

#### 6.2.1 場景：從 Plant 3D 建立新工廠的數位分身

**步驟 1：在 Plant 3D 中設計工廠**
- 使用 Plant 3D 設計 3D 工廠佈局
- 使用 Plant 3D 的設備庫選擇設備
- 完成管線連接和佈局

**步驟 2：匯出 IADL**
```bash
# 在 Plant 3D 中執行
Command: EXPORTIADL
Select equipment catalog: [選擇 Catalog]
Select output directory: C:\IDTF\iadl_library\
Extracting equipment types... Done (25 types)
Generating IADL definitions... Done
IADL exported to: C:\IDTF\iadl_library\
```

**步驟 3：在 IADL Editor 中檢查**
- 開啟 IADL Editor
- 載入生成的 IADL 檔案
- 檢查參數定義
- 添加自定義行為（如需要）
- 保存到 IADL 設備庫

**步驟 4：匯出 FDL**
```bash
# 在 Plant 3D 中執行
Command: EXPORTFDL
Select IADL library: C:\IDTF\iadl_library\
Select output file: C:\IDTF\projects\factory_a\factory_layout.fdl.yaml
Extracting equipment... Done (150 items)
Extracting piping... Done (320 items)
Mapping to IADL... Done
Generating FDL... Done
FDL exported to: C:\IDTF\projects\factory_a\factory_layout.fdl.yaml
```

**步驟 5：在 FDL Editor 中檢查**
- 開啟 FDL Editor
- 載入生成的 FDL 檔案
- 檢查資產實例
- 檢查 IADL 引用
- 調整參數（如需要）
- 保存 FDL 檔案

**步驟 6：部署到 NDH**
```bash
# 使用 NDH CLI
ndh-cli deploy \
  --iadl-library C:\IDTF\iadl_library\ \
  --fdl C:\IDTF\projects\factory_a\factory_layout.fdl.yaml \
  --factory-name "Factory A"

Uploading IADL definitions... Done (25 types)
Uploading FDL... Done
Creating digital twin... Done
Starting execution... Done
Digital twin is now running at: http://ndh.example.com/factory-a
```

---

## 7. 實作細節

### 7.1 IADL 生成器實作

**完整程式碼範例**：
```python
# iadl_generator.py
import yaml
from typing import Dict, List
from dataclasses import dataclass

@dataclass
class Parameter:
    name: str
    type: str
    unit: str = None
    description: str = None
    min: float = None
    max: float = None
    default: any = None

@dataclass
class ConnectionPoint:
    name: str
    type: str
    size: str = None
    location: str = None
    description: str = None

@dataclass
class Behavior:
    name: str
    description: str
    inputs: List[str]
    outputs: List[str]
    logic: str

class AssetType:
    def __init__(self, name: str, category: str, description: str = None, version: str = "1.0"):
        self.name = name
        self.category = category
        self.description = description
        self.version = version
        self.parameters = []
        self.connection_points = []
        self.behaviors = []
    
    def add_parameter(self, param: Parameter):
        self.parameters.append(param)
    
    def add_connection_point(self, cp: ConnectionPoint):
        self.connection_points.append(cp)
    
    def add_behavior(self, behavior: Behavior):
        self.behaviors.append(behavior)
    
    def to_yaml(self) -> str:
        data = {
            'asset_type': {
                'name': self.name,
                'category': self.category,
                'version': self.version,
                'description': self.description,
                'parameters': [
                    {
                        'name': p.name,
                        'type': p.type,
                        'unit': p.unit,
                        'description': p.description,
                        'min': p.min,
                        'max': p.max,
                        'default': p.default
                    }
                    for p in self.parameters
                ],
                'connection_points': [
                    {
                        'name': cp.name,
                        'type': cp.type,
                        'size': cp.size,
                        'location': cp.location,
                        'description': cp.description
                    }
                    for cp in self.connection_points
                ],
                'behaviors': [
                    {
                        'name': b.name,
                        'description': b.description,
                        'inputs': b.inputs,
                        'outputs': b.outputs,
                        'logic': b.logic
                    }
                    for b in self.behaviors
                ]
            }
        }
        
        return yaml.dump(data, default_flow_style=False, allow_unicode=True, sort_keys=False)

class IADLGenerator:
    def __init__(self):
        self.type_mapper = TypeMapper()
        self.attribute_mapper = AttributeMapper()
    
    def generate_from_plant3d_equipment(self, equipment_data: Dict) -> AssetType:
        """從 Plant 3D 設備數據生成 IADL"""
        
        # 映射設備類型
        iadl_type = self.type_mapper.map_type(equipment_data['type'])
        iadl_category = self.type_mapper.map_category(equipment_data['category'])
        
        # 創建資產類型
        asset_type = AssetType(
            name=iadl_type,
            category=iadl_category,
            description=equipment_data.get('description', ''),
            version="1.0"
        )
        
        # 添加基本參數
        asset_type.add_parameter(Parameter(
            name="manufacturer",
            type="string",
            description="製造商",
            default=equipment_data.get('manufacturer', '')
        ))
        
        asset_type.add_parameter(Parameter(
            name="model",
            type="string",
            description="型號",
            default=equipment_data.get('model', '')
        ))
        
        # 添加特定參數
        for attr_name, attr_value in equipment_data.get('attributes', {}).items():
            param = self.attribute_mapper.map_attribute(attr_name, attr_value, iadl_category)
            if param:
                asset_type.add_parameter(param)
        
        # 添加連接點
        for nozzle in equipment_data.get('nozzles', []):
            cp = ConnectionPoint(
                name=nozzle['name'],
                type=nozzle['type'],
                size=nozzle.get('size', ''),
                description=nozzle.get('description', '')
            )
            asset_type.add_connection_point(cp)
        
        # 添加預設行為
        self.add_default_behaviors(asset_type, iadl_category)
        
        return asset_type
    
    def add_default_behaviors(self, asset_type: AssetType, category: str):
        """添加預設行為"""
        
        if category == "pump":
            asset_type.add_behavior(Behavior(
                name="calculate_power",
                description="計算泵的功率消耗",
                inputs=["flow_rate", "head", "efficiency"],
                outputs=["actual_power"],
                logic="actual_power = (flow_rate * head * 9.81) / (3600 * efficiency / 100)"
            ))
        
        elif category == "tank":
            asset_type.add_behavior(Behavior(
                name="calculate_level",
                description="計算液位百分比",
                inputs=["current_volume", "capacity"],
                outputs=["current_level"],
                logic="current_level = (current_volume / capacity) * 100"
            ))

# 使用範例
if __name__ == "__main__":
    # 從 Plant 3D 提取的設備數據
    equipment_data = {
        'type': 'Centrifugal Pump',
        'category': 'Pump',
        'description': '離心泵',
        'manufacturer': 'Grundfos',
        'model': 'CR 64-2',
        'attributes': {
            'Flow Rate': {'value': 100, 'unit': 'm3/h'},
            'Head': {'value': 50, 'unit': 'm'},
            'Power': {'value': 30, 'unit': 'kW'},
            'Efficiency': {'value': 75, 'unit': '%'},
            'NPSH Required': {'value': 3.5, 'unit': 'm'}
        },
        'nozzles': [
            {'name': 'suction', 'type': 'inlet', 'size': 'DN 150'},
            {'name': 'discharge', 'type': 'outlet', 'size': 'DN 100'}
        ]
    }
    
    # 生成 IADL
    generator = IADLGenerator()
    asset_type = generator.generate_from_plant3d_equipment(equipment_data)
    
    # 輸出 YAML
    print(asset_type.to_yaml())
```

---

## 8. 使用場景

### 8.1 場景 1：建立標準化設備庫

**背景**：
- 公司有多個工廠專案
- 每個專案都使用類似的設備
- 希望建立標準化的 IADL 設備庫

**步驟**：
1. 從第一個專案的 Plant 3D 匯出設備庫
2. 生成 IADL 定義
3. 在 IADL Editor 中檢查和標準化
4. 保存到公司的 IADL 設備庫
5. 後續專案直接使用標準化設備庫

**結果**：
- 建立了可重用的設備庫
- 提高了一致性
- 減少了重複工作

### 8.2 場景 2：從 E3D 匯入複雜設備

**背景**：
- 使用 AVEVA E3D 設計大型石化工廠
- 設備規格複雜，包含大量屬性
- 手動編寫 IADL 耗時且容易出錯

**步驟**：
1. 使用 AVEVA Connect API 連接到 E3D
2. 提取設備規格
3. 自動生成 IADL 定義
4. 在 IADL Editor 中添加自定義行為
5. 保存並部署到 NDH

**結果**：
- 節省 70-80% 的 IADL 編寫時間
- 減少錯誤
- 保持與 E3D 的一致性

### 8.3 場景 3：設備庫版本管理

**背景**：
- 設備規格隨時間變化
- 需要追蹤 IADL 的變更歷史
- 需要支援多個版本的 IADL

**步驟**：
1. 使用 Git 管理 IADL 設備庫
2. 從工廠設計軟體匯入更新的設備規格
3. 檢查變更
4. 提交變更並標記版本
5. 推送到遠端倉庫

**結果**：
- 完整的變更歷史
- 支援版本回滾
- 團隊協作

---

## 9. 實作路線圖

### 9.1 階段 1：IADL 生成器（1-2 個月）

**目標**：實作 IADL 生成器

**任務**：
- [ ] 設計 IADL 生成器架構
- [ ] 實作 TypeMapper
- [ ] 實作 AttributeMapper
- [ ] 實作 IADLGenerator
- [ ] 單元測試

**交付物**：
- IADL 生成器程式碼
- 單元測試
- 文檔

### 9.2 階段 2：Plant 3D 設備庫整合（1 個月）

**目標**：從 Plant 3D 匯入設備庫

**任務**：
- [ ] 實作 Plant3DEquipmentLibraryExtractor
- [ ] 整合 IADL Generator
- [ ] 測試與驗證

**交付物**：
- Plant 3D 設備庫提取器
- 測試報告

### 9.3 階段 3：E3D 和 Smart 3D 設備庫整合（1 個月）

**目標**：從 E3D 和 Smart 3D 匯入設備庫

**任務**：
- [ ] 實作 AvevaE3DEquipmentLibraryExtractor
- [ ] 實作 Smart3DEquipmentLibraryExtractor
- [ ] 測試與驗證

**交付物**：
- E3D 和 Smart 3D 設備庫提取器
- 測試報告

### 9.4 階段 4：IADL Editor 增強（1-2 個月）

**目標**：增強 IADL Editor

**任務**：
- [ ] 實作從工廠設計軟體匯入功能
- [ ] 實作智能參數建議
- [ ] 實作 IADL 驗證與錯誤檢查
- [ ] 實作視覺化設備屬性

**交付物**：
- 增強的 IADL Editor
- 使用者手冊

### 9.5 階段 5：IADL 設備庫管理系統（1 個月）

**目標**：建立 IADL 設備庫管理系統

**任務**：
- [ ] 實作設備庫瀏覽和搜尋
- [ ] 實作設備庫匯入和匯出
- [ ] 實作版本管理
- [ ] 建立標準化設備庫

**交付物**：
- IADL 設備庫管理系統
- 標準化設備庫

### 9.6 階段 6：整合測試（1 個月）

**目標**：端到端整合測試

**任務**：
- [ ] IADL + FDL 整合測試
- [ ] 與 NDH 的整合測試
- [ ] 效能測試
- [ ] 使用者驗收測試

**交付物**：
- 測試報告
- 效能基準

**總計**：6-8 個月完整實作

---

## 總結

IADL 與主流工廠設計軟體的整合將：

1. ✅ **完善端到端的自動化工作流**：從設計軟體 → IADL + FDL → 數位分身
2. ✅ **節省 70-80% 的 IADL 編寫時間**
3. ✅ **建立標準化的設備庫**，促進重用和一致性
4. ✅ **保持設備規格的一致性**，減少錯誤
5. ✅ **支援版本管理**，追蹤變更歷史

結合 FDL 整合，IDTF V3.5 將提供**業界最完整的工廠設計軟體整合方案**，真正實現從設計到數位分身的無縫轉換。

---

**文件版本**: 1.0  
**最後更新**: 2025年10月14日  
**作者**: IDTF 技術團隊  
**聯絡**: tech@idtf.io

