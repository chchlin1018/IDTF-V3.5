# 從 NVIDIA Omniverse 建立 FDL 模型設計文件

**版本**: IDTF V3.5  
**日期**: 2025年10月14日  
**狀態**: 設計階段

---

## 目錄

1. [概述](#概述)
2. [為什麼需要從 Omniverse 生成 FDL](#為什麼需要從-omniverse-生成-fdl)
3. [Omniverse USD 場景結構分析](#omniverse-usd-場景結構分析)
4. [FDL 生成架構](#fdl-生成架構)
5. [USD 到 FDL 的映射策略](#usd-到-fdl-的映射策略)
6. [Omniverse 擴展實作](#omniverse-擴展實作)
7. [完整的雙向工作流](#完整的雙向工作流)
8. [使用場景](#使用場景)
9. [實作路線圖](#實作路線圖)

---

## 1. 概述

### 1.1 目標

設計並實作從 **NVIDIA Omniverse** 場景自動生成 **FDL (Factory Design Language)** 的功能，使 IDTF V3.5 能夠：

1. **從 Omniverse 場景提取工廠佈局**
2. **自動生成 FDL 定義**
3. **支援雙向同步**（Omniverse ↔ FDL）
4. **整合到現有的工作流**

### 1.2 整合範圍

本文件描述如何從 Omniverse 生成 FDL，與現有的整合方案形成完整的生態系統：

```
工廠設計軟體 (Plant 3D/E3D/Smart 3D)
    ↓
IADL + FDL
    ↓
NDH (數位分身執行)
    ↓
Omniverse (視覺化)
    ↑
FDL 生成 (本文件)
```

**新增功能**：
- **Omniverse → FDL**：從 Omniverse 場景生成 FDL
- **FDL → Omniverse**：已有（透過 NDH Omniverse Connector）

### 1.3 價值主張

**對設計師**：
- 在 Omniverse 中視覺化設計工廠佈局
- 自動生成 FDL，無需手動編寫
- 利用 Omniverse 的協作功能

**對專案**：
- 提供另一種 FDL 建立方式
- 支援視覺化優先的設計流程
- 縮短設計到部署的時間

**對系統**：
- 完善 Omniverse 整合
- 支援雙向工作流
- 擴展 IDTF 的應用場景

### 1.4 設計原則

1. **視覺化優先**：利用 Omniverse 的視覺化能力
2. **非侵入性**：作為 Omniverse 擴展，不修改核心
3. **語義保留**：保留場景中的語義資訊
4. **雙向同步**：支援 Omniverse 與 FDL 的雙向同步
5. **標準化**：使用 USD 標準和 IDTF 標準

---

## 2. 為什麼需要從 Omniverse 生成 FDL

### 2.1 補充現有的整合方案

**現有整合**：
- **工廠設計軟體 → FDL**：從 Plant 3D/E3D/Smart 3D 生成 FDL
- **FDL → Omniverse**：透過 NDH Omniverse Connector 視覺化

**新增整合**：
- **Omniverse → FDL**：從 Omniverse 場景生成 FDL

**完整的工作流**：
```
方案 A: 工廠設計軟體優先
Plant 3D/E3D/Smart 3D → IADL + FDL → NDH → Omniverse

方案 B: Omniverse 優先 (新增)
Omniverse → FDL → NDH → Omniverse (同步)

方案 C: 混合
Plant 3D/E3D/Smart 3D → IADL + FDL → NDH → Omniverse
                                              ↓
                                        調整佈局
                                              ↓
                                        Omniverse → FDL (更新)
```

### 2.2 使用場景

#### 場景 1：視覺化優先的設計流程

**背景**：
- 設計團隊習慣使用視覺化工具
- 希望在 3D 環境中直接設計工廠佈局
- 利用 Omniverse 的協作功能

**流程**：
1. 在 Omniverse 中創建工廠場景
2. 放置設備、定義連接
3. 自動生成 FDL
4. 部署到 NDH

#### 場景 2：佈局調整與同步

**背景**：
- 已有從工廠設計軟體生成的 FDL
- 在 Omniverse 中調整佈局
- 需要同步回 FDL

**流程**：
1. FDL → NDH → Omniverse（初始視覺化）
2. 在 Omniverse 中調整設備位置
3. Omniverse → FDL（更新 FDL）
4. 重新部署到 NDH

#### 場景 3：快速原型設計

**背景**：
- 需要快速驗證工廠佈局
- 沒有詳細的工廠設計軟體模型
- 使用 Omniverse 快速搭建原型

**流程**：
1. 在 Omniverse 中快速搭建原型
2. 生成 FDL
3. 部署到 NDH 驗證
4. 迭代優化

---

## 3. Omniverse USD 場景結構分析

### 3.1 USD (Universal Scene Description) 基礎

**USD 是什麼**：
- Pixar 開發的開源 3D 場景描述格式
- Omniverse 的核心數據格式
- 支援場景組合、層次結構、變體

**USD 場景結構**：
```
Stage (場景)
└── Root Layer
    ├── Prim (基元)
    │   ├── Properties (屬性)
    │   ├── Relationships (關係)
    │   └── Metadata (元數據)
    └── Prim
        └── Child Prim
```

### 3.2 USD Prim 類型

**常用 Prim 類型**：

| USD Prim 類型 | 描述 | 對應 FDL 元素 |
|--------------|------|--------------|
| `Xform` | 變換節點，用於組織層次結構 | `zone`, `area` |
| `Mesh` | 幾何網格 | 視覺化，不直接映射 |
| `Scope` | 組織節點，無變換 | `zone`, `area` |
| `UsdGeom.Cube` | 立方體 | 可能是設備的簡化表示 |
| `UsdGeom.Sphere` | 球體 | 可能是設備的簡化表示 |
| `UsdGeom.Cylinder` | 圓柱體 | 可能是管線或設備 |
| `UsdPhysics.RigidBody` | 剛體 | 可能是設備 |
| `Custom Prim` | 自定義 Prim | 設備實例 |

### 3.3 USD 屬性與元數據

**常用屬性**：
```python
# 變換屬性
xformOp:translate  # 位置
xformOp:rotateXYZ  # 旋轉
xformOp:scale      # 縮放

# 自定義屬性
custom:asset_type  # 資產類型
custom:asset_id    # 資產 ID
custom:parameters  # 參數
```

**元數據**：
```python
# Prim 元數據
assetInfo         # 資產資訊
customData        # 自定義數據
documentation     # 文檔
```

### 3.4 Omniverse 場景組織建議

為了方便生成 FDL，建議在 Omniverse 中按以下方式組織場景：

```
/World (Root)
├── /Factory (工廠)
│   ├── /Building_A (建築物 A)
│   │   ├── /Floor_1 (樓層 1)
│   │   │   ├── /Zone_Production (生產區)
│   │   │   │   ├── /Asset_Pump_001 (泵 001)
│   │   │   │   ├── /Asset_Tank_001 (儲罐 001)
│   │   │   │   └── /Asset_HeatExchanger_001 (熱交換器 001)
│   │   │   └── /Zone_Storage (儲存區)
│   │   │       └── /Asset_Tank_002 (儲罐 002)
│   │   └── /Floor_2 (樓層 2)
│   └── /Building_B (建築物 B)
└── /Connections (連接)
    ├── /Connection_Pump001_Tank001 (連接：泵001 → 儲罐001)
    └── /Connection_Tank001_HX001 (連接：儲罐001 → 熱交換器001)
```

**命名約定**：
- **工廠**：`/Factory` 或 `/World/Factory`
- **建築物**：`/Building_{name}`
- **樓層**：`/Floor_{number}`
- **區域**：`/Zone_{name}`
- **資產**：`/Asset_{type}_{id}`
- **連接**：`/Connection_{source}_{target}`

---

## 4. FDL 生成架構

### 4.1 整體架構

```
Omniverse Stage (USD 場景)
    ↓
USD Scene Traverser (場景遍歷器)
    ↓
Semantic Extractor (語義提取器)
    ↓
USD to FDL Mapper (USD 到 FDL 映射器)
    ↓
FDL Generator (FDL 生成器)
    ↓
FDL YAML Files
```

### 4.2 核心組件

#### 4.2.1 USD Scene Traverser（場景遍歷器）

**職責**：
- 遍歷 USD 場景樹
- 提取 Prims 和屬性
- 識別場景結構

**實作**：
```python
from pxr import Usd, UsdGeom

class USDSceneTraverser:
    """USD 場景遍歷器"""
    
    def __init__(self, stage: Usd.Stage):
        self.stage = stage
    
    def traverse(self, root_path: str = "/") -> Dict:
        """遍歷場景並提取結構"""
        
        root_prim = self.stage.GetPrimAtPath(root_path)
        
        scene_data = {
            'hierarchy': self.extract_hierarchy(root_prim),
            'assets': self.extract_assets(root_prim),
            'connections': self.extract_connections(root_prim)
        }
        
        return scene_data
    
    def extract_hierarchy(self, prim: Usd.Prim, level: int = 0) -> Dict:
        """提取層次結構"""
        
        hierarchy = {
            'path': str(prim.GetPath()),
            'name': prim.GetName(),
            'type': prim.GetTypeName(),
            'children': []
        }
        
        for child in prim.GetChildren():
            hierarchy['children'].append(
                self.extract_hierarchy(child, level + 1)
            )
        
        return hierarchy
    
    def extract_assets(self, root_prim: Usd.Prim) -> List[Dict]:
        """提取資產"""
        
        assets = []
        
        # 遍歷所有 Prims
        for prim in Usd.PrimRange(root_prim):
            # 檢查是否是資產
            if self.is_asset(prim):
                asset_data = self.extract_asset_data(prim)
                assets.append(asset_data)
        
        return assets
    
    def is_asset(self, prim: Usd.Prim) -> bool:
        """判斷 Prim 是否是資產"""
        
        # 方法 1: 檢查命名約定
        if prim.GetName().startswith('Asset_'):
            return True
        
        # 方法 2: 檢查自定義屬性
        if prim.HasAttribute('custom:asset_type'):
            return True
        
        # 方法 3: 檢查元數據
        if prim.HasCustomDataKey('idtf:is_asset'):
            return True
        
        return False
    
    def extract_asset_data(self, prim: Usd.Prim) -> Dict:
        """提取資產數據"""
        
        # 獲取變換
        xform = UsdGeom.Xformable(prim)
        transform = xform.ComputeLocalToWorldTransform(Usd.TimeCode.Default())
        
        # 提取位置和旋轉
        position = transform.ExtractTranslation()
        rotation = transform.ExtractRotation()
        
        asset_data = {
            'path': str(prim.GetPath()),
            'name': prim.GetName(),
            'type': self.get_asset_type(prim),
            'position': {
                'x': position[0],
                'y': position[1],
                'z': position[2]
            },
            'rotation': {
                'x': rotation.GetAxis()[0] * rotation.GetAngle(),
                'y': rotation.GetAxis()[1] * rotation.GetAngle(),
                'z': rotation.GetAxis()[2] * rotation.GetAngle()
            },
            'parameters': self.extract_parameters(prim)
        }
        
        return asset_data
    
    def get_asset_type(self, prim: Usd.Prim) -> str:
        """獲取資產類型"""
        
        # 方法 1: 從自定義屬性獲取
        if prim.HasAttribute('custom:asset_type'):
            return prim.GetAttribute('custom:asset_type').Get()
        
        # 方法 2: 從名稱推斷
        name = prim.GetName()
        if '_' in name:
            parts = name.split('_')
            if parts[0] == 'Asset' and len(parts) >= 2:
                return parts[1].lower()
        
        return 'unknown'
    
    def extract_parameters(self, prim: Usd.Prim) -> Dict:
        """提取參數"""
        
        parameters = {}
        
        # 提取所有自定義屬性
        for attr in prim.GetAttributes():
            attr_name = attr.GetName()
            
            # 只提取 custom: 命名空間的屬性
            if attr_name.startswith('custom:param:'):
                param_name = attr_name.replace('custom:param:', '')
                param_value = attr.Get()
                parameters[param_name] = param_value
        
        return parameters
    
    def extract_connections(self, root_prim: Usd.Prim) -> List[Dict]:
        """提取連接"""
        
        connections = []
        
        # 遍歷所有 Prims
        for prim in Usd.PrimRange(root_prim):
            # 檢查是否是連接
            if self.is_connection(prim):
                connection_data = self.extract_connection_data(prim)
                connections.append(connection_data)
        
        return connections
    
    def is_connection(self, prim: Usd.Prim) -> bool:
        """判斷 Prim 是否是連接"""
        
        # 方法 1: 檢查命名約定
        if prim.GetName().startswith('Connection_'):
            return True
        
        # 方法 2: 檢查自定義屬性
        if prim.HasAttribute('custom:connection_source') and \
           prim.HasAttribute('custom:connection_target'):
            return True
        
        return False
    
    def extract_connection_data(self, prim: Usd.Prim) -> Dict:
        """提取連接數據"""
        
        connection_data = {
            'name': prim.GetName(),
            'source': None,
            'target': None,
            'type': 'pipe'  # 預設類型
        }
        
        # 從自定義屬性獲取
        if prim.HasAttribute('custom:connection_source'):
            connection_data['source'] = prim.GetAttribute('custom:connection_source').Get()
        
        if prim.HasAttribute('custom:connection_target'):
            connection_data['target'] = prim.GetAttribute('custom:connection_target').Get()
        
        if prim.HasAttribute('custom:connection_type'):
            connection_data['type'] = prim.GetAttribute('custom:connection_type').Get()
        
        # 從名稱推斷（如果屬性不存在）
        if not connection_data['source'] or not connection_data['target']:
            name = prim.GetName()
            if '_' in name:
                parts = name.split('_')
                if parts[0] == 'Connection' and len(parts) >= 3:
                    connection_data['source'] = parts[1]
                    connection_data['target'] = parts[2]
        
        return connection_data
```

#### 4.2.2 Semantic Extractor（語義提取器）

**職責**：
- 從 USD 場景中提取語義資訊
- 識別工廠結構（建築物、樓層、區域）
- 識別資產和連接

**實作**：
```python
class SemanticExtractor:
    """語義提取器"""
    
    def __init__(self, scene_data: Dict):
        self.scene_data = scene_data
    
    def extract_factory_structure(self) -> Dict:
        """提取工廠結構"""
        
        factory_structure = {
            'factory_name': self.extract_factory_name(),
            'buildings': self.extract_buildings(),
            'zones': self.extract_zones(),
            'assets': self.extract_assets_with_hierarchy()
        }
        
        return factory_structure
    
    def extract_factory_name(self) -> str:
        """提取工廠名稱"""
        
        # 從場景根節點或元數據中提取
        hierarchy = self.scene_data['hierarchy']
        
        # 查找名為 Factory 的節點
        for child in hierarchy.get('children', []):
            if 'Factory' in child['name']:
                return child['name']
        
        return "Factory"
    
    def extract_buildings(self) -> List[Dict]:
        """提取建築物"""
        
        buildings = []
        hierarchy = self.scene_data['hierarchy']
        
        # 遍歷層次結構，查找建築物節點
        for child in hierarchy.get('children', []):
            if self.is_building(child):
                building = {
                    'id': self.extract_id_from_name(child['name']),
                    'name': child['name'],
                    'floors': self.extract_floors(child)
                }
                buildings.append(building)
        
        return buildings
    
    def is_building(self, node: Dict) -> bool:
        """判斷節點是否是建築物"""
        return 'Building' in node['name']
    
    def extract_floors(self, building_node: Dict) -> List[Dict]:
        """提取樓層"""
        
        floors = []
        
        for child in building_node.get('children', []):
            if self.is_floor(child):
                floor = {
                    'id': self.extract_id_from_name(child['name']),
                    'name': child['name']
                }
                floors.append(floor)
        
        return floors
    
    def is_floor(self, node: Dict) -> bool:
        """判斷節點是否是樓層"""
        return 'Floor' in node['name']
    
    def extract_zones(self) -> List[Dict]:
        """提取區域"""
        
        zones = []
        hierarchy = self.scene_data['hierarchy']
        
        # 遞歸查找所有區域節點
        self._find_zones_recursive(hierarchy, zones)
        
        return zones
    
    def _find_zones_recursive(self, node: Dict, zones: List[Dict]):
        """遞歸查找區域"""
        
        if self.is_zone(node):
            zone = {
                'id': self.extract_id_from_name(node['name']),
                'name': node['name'],
                'parent': self.extract_parent_from_path(node['path'])
            }
            zones.append(zone)
        
        for child in node.get('children', []):
            self._find_zones_recursive(child, zones)
    
    def is_zone(self, node: Dict) -> bool:
        """判斷節點是否是區域"""
        return 'Zone' in node['name']
    
    def extract_assets_with_hierarchy(self) -> List[Dict]:
        """提取資產及其層次結構"""
        
        assets = []
        
        for asset_data in self.scene_data['assets']:
            # 從路徑中提取層次結構資訊
            path = asset_data['path']
            hierarchy_info = self.parse_hierarchy_from_path(path)
            
            asset = {
                'id': self.extract_id_from_name(asset_data['name']),
                'type': asset_data['type'],
                'position': asset_data['position'],
                'rotation': asset_data['rotation'],
                'parameters': asset_data['parameters'],
                'building': hierarchy_info.get('building'),
                'floor': hierarchy_info.get('floor'),
                'zone': hierarchy_info.get('zone')
            }
            
            assets.append(asset)
        
        return assets
    
    def parse_hierarchy_from_path(self, path: str) -> Dict:
        """從路徑解析層次結構"""
        
        parts = path.split('/')
        hierarchy = {}
        
        for part in parts:
            if 'Building' in part:
                hierarchy['building'] = self.extract_id_from_name(part)
            elif 'Floor' in part:
                hierarchy['floor'] = self.extract_id_from_name(part)
            elif 'Zone' in part:
                hierarchy['zone'] = self.extract_id_from_name(part)
        
        return hierarchy
    
    def extract_id_from_name(self, name: str) -> str:
        """從名稱中提取 ID"""
        
        # 移除前綴
        for prefix in ['Asset_', 'Building_', 'Floor_', 'Zone_', 'Connection_']:
            if name.startswith(prefix):
                name = name.replace(prefix, '')
                break
        
        return name.lower().replace(' ', '_')
    
    def extract_parent_from_path(self, path: str) -> str:
        """從路徑中提取父節點"""
        
        parts = path.split('/')
        if len(parts) > 1:
            return parts[-2]
        
        return None
```

#### 4.2.3 USD to FDL Mapper（映射器）

**職責**：
- 將 USD 場景元素映射到 FDL 元素
- 處理座標系轉換
- 處理單位轉換

**實作**：
```python
class USDToFDLMapper:
    """USD 到 FDL 映射器"""
    
    def __init__(self):
        self.coordinate_system = 'z_up'  # USD 預設是 Z-up
    
    def map_factory_structure(self, factory_structure: Dict) -> Dict:
        """映射工廠結構"""
        
        fdl_structure = {
            'factory': {
                'name': factory_structure['factory_name'],
                'coordinate_system': self.coordinate_system
            },
            'layout': {
                'buildings': self.map_buildings(factory_structure['buildings']),
                'zones': self.map_zones(factory_structure['zones'])
            },
            'assets': self.map_assets(factory_structure['assets']),
            'connections': self.map_connections(factory_structure.get('connections', []))
        }
        
        return fdl_structure
    
    def map_buildings(self, buildings: List[Dict]) -> List[Dict]:
        """映射建築物"""
        
        fdl_buildings = []
        
        for building in buildings:
            fdl_building = {
                'id': building['id'],
                'name': building['name'],
                'floors': [
                    {
                        'id': floor['id'],
                        'name': floor['name']
                    }
                    for floor in building['floors']
                ]
            }
            fdl_buildings.append(fdl_building)
        
        return fdl_buildings
    
    def map_zones(self, zones: List[Dict]) -> List[Dict]:
        """映射區域"""
        
        fdl_zones = []
        
        for zone in zones:
            fdl_zone = {
                'id': zone['id'],
                'name': zone['name'],
                'parent': zone.get('parent')
            }
            fdl_zones.append(fdl_zone)
        
        return fdl_zones
    
    def map_assets(self, assets: List[Dict]) -> List[Dict]:
        """映射資產"""
        
        fdl_assets = []
        
        for asset in assets:
            fdl_asset = {
                'id': asset['id'],
                'asset_type': asset['type'],
                'location': {
                    'building': asset.get('building'),
                    'floor': asset.get('floor'),
                    'zone': asset.get('zone'),
                    'position': self.convert_position(asset['position']),
                    'rotation': self.convert_rotation(asset['rotation'])
                },
                'instance_params': asset['parameters']
            }
            fdl_assets.append(fdl_asset)
        
        return fdl_assets
    
    def convert_position(self, position: Dict) -> Dict:
        """轉換位置（單位轉換和座標系轉換）"""
        
        # USD 預設單位是 cm，FDL 使用 m
        # USD 預設是 Z-up，FDL 可能是 Y-up（根據需求）
        
        converted = {
            'x': position['x'] / 100.0,  # cm 到 m
            'y': position['y'] / 100.0,
            'z': position['z'] / 100.0
        }
        
        # 如果需要座標系轉換（Z-up 到 Y-up）
        if self.coordinate_system == 'y_up':
            converted = {
                'x': converted['x'],
                'y': converted['z'],
                'z': -converted['y']
            }
        
        return converted
    
    def convert_rotation(self, rotation: Dict) -> Dict:
        """轉換旋轉"""
        
        # 根據需要進行座標系轉換
        converted = {
            'x': rotation['x'],
            'y': rotation['y'],
            'z': rotation['z']
        }
        
        return converted
    
    def map_connections(self, connections: List[Dict]) -> List[Dict]:
        """映射連接"""
        
        fdl_connections = []
        
        for conn in connections:
            fdl_conn = {
                'source_asset': conn['source'],
                'target_asset': conn['target'],
                'connection_type': conn.get('type', 'pipe')
            }
            fdl_connections.append(fdl_conn)
        
        return fdl_connections
```

#### 4.2.4 FDL Generator（FDL 生成器）

**職責**：
- 生成 FDL YAML 檔案
- 驗證 FDL 語法
- 優化 FDL 結構

**實作**：
```python
import yaml

class FDLGenerator:
    """FDL 生成器"""
    
    def generate_fdl(self, fdl_structure: Dict, output_path: str):
        """生成 FDL YAML 檔案"""
        
        # 構建 FDL 文檔
        fdl_doc = {
            'fdl_version': '3.5',
            'factory': fdl_structure['factory'],
            'layout': fdl_structure['layout'],
            'assets': fdl_structure['assets'],
            'connections': fdl_structure['connections']
        }
        
        # 寫入 YAML 檔案
        with open(output_path, 'w') as f:
            yaml.dump(fdl_doc, f, default_flow_style=False, allow_unicode=True, sort_keys=False)
        
        print(f"FDL generated: {output_path}")
```

---

## 5. USD 到 FDL 的映射策略

### 5.1 場景結構映射

| USD 結構 | FDL 結構 | 映射規則 |
|---------|---------|---------|
| `/Factory` | `factory.name` | 工廠根節點 |
| `/Building_{name}` | `layout.buildings[].id` | 建築物節點 |
| `/Floor_{number}` | `layout.buildings[].floors[].id` | 樓層節點 |
| `/Zone_{name}` | `layout.zones[].id` | 區域節點 |
| `/Asset_{type}_{id}` | `assets[].id` | 資產實例 |
| `/Connection_{source}_{target}` | `connections[]` | 連接 |

### 5.2 屬性映射

| USD 屬性 | FDL 屬性 | 轉換 |
|---------|---------|------|
| `xformOp:translate` | `location.position` | cm → m, 座標系轉換 |
| `xformOp:rotateXYZ` | `location.rotation` | 座標系轉換 |
| `custom:asset_type` | `asset_type` | 直接映射 |
| `custom:param:{name}` | `instance_params.{name}` | 直接映射 |

### 5.3 座標系轉換

**USD 座標系**：
- 預設：Z-up, 右手座標系
- 單位：cm（預設）

**FDL 座標系**：
- 可配置：Z-up 或 Y-up
- 單位：m

**轉換公式**：
```python
# USD (Z-up, cm) → FDL (Z-up, m)
fdl_x = usd_x / 100.0
fdl_y = usd_y / 100.0
fdl_z = usd_z / 100.0

# USD (Z-up, cm) → FDL (Y-up, m)
fdl_x = usd_x / 100.0
fdl_y = usd_z / 100.0
fdl_z = -usd_y / 100.0
```

---

## 6. Omniverse 擴展實作

### 6.1 Omniverse 擴展架構

**擴展結構**：
```
omni.idtf.fdl_exporter/
├── config/
│   └── extension.toml
├── data/
│   └── icon.png
├── docs/
│   └── README.md
├── omni/
│   └── idtf/
│       └── fdl_exporter/
│           ├── __init__.py
│           ├── extension.py
│           ├── ui/
│           │   ├── __init__.py
│           │   └── export_window.py
│           ├── core/
│           │   ├── __init__.py
│           │   ├── scene_traverser.py
│           │   ├── semantic_extractor.py
│           │   ├── usd_to_fdl_mapper.py
│           │   └── fdl_generator.py
│           └── utils/
│               ├── __init__.py
│               └── helpers.py
└── README.md
```

### 6.2 擴展配置

**extension.toml**：
```toml
[package]
title = "IDTF FDL Exporter"
description = "Export Omniverse scene to IDTF FDL format"
version = "1.0.0"
authors = ["IDTF Team"]
repository = "https://github.com/idtf/omniverse-fdl-exporter"
keywords = ["idtf", "fdl", "export", "digital twin"]

[dependencies]
"omni.kit.uiapp" = {}
"omni.usd" = {}
"omni.kit.window.file_exporter" = {}

[[python.module]]
name = "omni.idtf.fdl_exporter"
```

### 6.3 擴展主類

**extension.py**：
```python
import omni.ext
import omni.ui as ui
from .ui.export_window import ExportWindow

class FDLExporterExtension(omni.ext.IExt):
    """IDTF FDL Exporter Extension"""
    
    def on_startup(self, ext_id):
        print("[omni.idtf.fdl_exporter] FDL Exporter Extension startup")
        
        # 創建菜單項
        self._window = None
        self._menu = ui.Menu("IDTF")
        self._menu_item = ui.MenuItem("Export to FDL", triggered_fn=self._on_export_clicked)
    
    def on_shutdown(self):
        print("[omni.idtf.fdl_exporter] FDL Exporter Extension shutdown")
        
        if self._window:
            self._window.destroy()
            self._window = None
        
        if self._menu:
            self._menu.destroy()
            self._menu = None
    
    def _on_export_clicked(self):
        """處理匯出按鈕點擊"""
        
        if not self._window:
            self._window = ExportWindow()
        
        self._window.show()
```

### 6.4 匯出視窗

**export_window.py**：
```python
import omni.ui as ui
import omni.usd
from ..core.scene_traverser import USDSceneTraverser
from ..core.semantic_extractor import SemanticExtractor
from ..core.usd_to_fdl_mapper import USDToFDLMapper
from ..core.fdl_generator import FDLGenerator

class ExportWindow:
    """FDL 匯出視窗"""
    
    def __init__(self):
        self._window = ui.Window("Export to FDL", width=400, height=300)
        
        with self._window.frame:
            with ui.VStack(spacing=10):
                ui.Label("Export Omniverse Scene to IDTF FDL")
                
                ui.Spacer(height=10)
                
                # 輸出路徑
                with ui.HStack():
                    ui.Label("Output Path:", width=100)
                    self._output_path = ui.StringField()
                    ui.Button("Browse", clicked_fn=self._on_browse_clicked, width=80)
                
                ui.Spacer(height=10)
                
                # 選項
                ui.Label("Options:")
                self._include_hierarchy = ui.CheckBox("Include Hierarchy", checked=True)
                self._include_connections = ui.CheckBox("Include Connections", checked=True)
                
                ui.Spacer()
                
                # 按鈕
                with ui.HStack():
                    ui.Spacer()
                    ui.Button("Export", clicked_fn=self._on_export_clicked, width=100)
                    ui.Button("Cancel", clicked_fn=self._on_cancel_clicked, width=100)
    
    def show(self):
        """顯示視窗"""
        self._window.visible = True
    
    def destroy(self):
        """銷毀視窗"""
        self._window.destroy()
    
    def _on_browse_clicked(self):
        """處理瀏覽按鈕點擊"""
        # 打開檔案選擇對話框
        pass
    
    def _on_export_clicked(self):
        """處理匯出按鈕點擊"""
        
        output_path = self._output_path.model.get_value_as_string()
        
        if not output_path:
            print("Please specify output path")
            return
        
        # 獲取當前 USD Stage
        stage = omni.usd.get_context().get_stage()
        
        # 遍歷場景
        traverser = USDSceneTraverser(stage)
        scene_data = traverser.traverse()
        
        # 提取語義
        extractor = SemanticExtractor(scene_data)
        factory_structure = extractor.extract_factory_structure()
        
        # 映射到 FDL
        mapper = USDToFDLMapper()
        fdl_structure = mapper.map_factory_structure(factory_structure)
        
        # 生成 FDL
        generator = FDLGenerator()
        generator.generate_fdl(fdl_structure, output_path)
        
        print(f"FDL exported to: {output_path}")
        
        self._window.visible = False
    
    def _on_cancel_clicked(self):
        """處理取消按鈕點擊"""
        self._window.visible = False
```

---

## 7. 完整的雙向工作流

### 7.1 工作流 A：Omniverse 優先

```
1. 在 Omniverse 中設計工廠佈局
   ├── 創建場景結構（建築物、樓層、區域）
   ├── 放置資產
   └── 定義連接

2. 匯出 FDL
   ├── IDTF → Export to FDL
   ├── 自動生成 FDL YAML
   └── 保存 FDL 檔案

3. 部署到 NDH
   ├── ndh-cli deploy --fdl factory_layout.fdl.yaml
   └── 創建數位分身

4. 同步回 Omniverse
   ├── NDH Omniverse Connector
   └── 視覺化運行狀態
```

### 7.2 工作流 B：混合工作流

```
1. 從工廠設計軟體匯入
   ├── Plant 3D/E3D/Smart 3D → IADL + FDL
   └── 部署到 NDH

2. 在 Omniverse 中視覺化
   ├── NDH → Omniverse
   └── 顯示初始佈局

3. 在 Omniverse 中調整
   ├── 調整設備位置
   ├── 添加新設備
   └── 修改連接

4. 匯出更新的 FDL
   ├── Omniverse → FDL (更新)
   └── 保存 FDL 檔案

5. 重新部署到 NDH
   ├── ndh-cli deploy --fdl factory_layout_v2.fdl.yaml
   └── 更新數位分身
```

### 7.3 工作流 C：快速原型

```
1. 在 Omniverse 中快速搭建原型
   ├── 使用預設資產
   ├── 快速佈局
   └── 簡化連接

2. 匯出 FDL
   └── 生成初始 FDL

3. 部署到 NDH 驗證
   └── 驗證佈局和連接

4. 迭代優化
   ├── 在 Omniverse 中調整
   ├── 重新匯出 FDL
   └── 重新部署驗證
```

---

## 8. 使用場景

### 8.1 場景 1：視覺化設計新工廠

**背景**：
- 設計團隊習慣使用 3D 視覺化工具
- 沒有現有的工廠設計軟體模型
- 希望快速驗證佈局

**步驟**：
1. 在 Omniverse 中創建工廠場景
2. 從 IADL 設備庫拖放設備
3. 調整設備位置和方向
4. 定義設備連接
5. 匯出 FDL
6. 部署到 NDH

**結果**：
- 快速驗證工廠佈局
- 視覺化設計過程
- 團隊協作設計

### 8.2 場景 2：從工廠設計軟體匯入後調整

**背景**：
- 已有從 Plant 3D 生成的 FDL
- 需要在 Omniverse 中調整佈局
- 保持與 FDL 的同步

**步驟**：
1. FDL → NDH → Omniverse（初始視覺化）
2. 在 Omniverse 中調整設備位置
3. 匯出更新的 FDL
4. 比較新舊 FDL 的差異
5. 重新部署到 NDH

**結果**：
- 靈活調整佈局
- 保持 FDL 的最新狀態
- 視覺化驗證調整結果

### 8.3 場景 3：多人協作設計

**背景**：
- 多個設計師協作設計工廠
- 利用 Omniverse 的協作功能
- 需要統一生成 FDL

**步驟**：
1. 在 Omniverse Nucleus 上創建共享場景
2. 多個設計師同時編輯不同區域
3. 定期匯出 FDL
4. 部署到 NDH 驗證
5. 迭代優化

**結果**：
- 高效的協作設計
- 即時視覺化反饋
- 統一的 FDL 輸出

---

## 9. 實作路線圖

### 9.1 階段 1：核心功能開發（2-3 個月）

**目標**：實作基本的 FDL 匯出功能

**任務**：
- [ ] 設計整體架構
- [ ] 實作 USDSceneTraverser
- [ ] 實作 SemanticExtractor
- [ ] 實作 USDToFDLMapper
- [ ] 實作 FDLGenerator
- [ ] 單元測試

**交付物**：
- 核心組件程式碼
- 單元測試
- 技術文檔

### 9.2 階段 2：Omniverse 擴展開發（1-2 個月）

**目標**：開發 Omniverse 擴展

**任務**：
- [ ] 設計擴展架構
- [ ] 實作擴展主類
- [ ] 實作匯出視窗 UI
- [ ] 整合核心組件
- [ ] 測試擴展

**交付物**：
- Omniverse 擴展
- 使用者介面
- 使用者手冊

### 9.3 階段 3：雙向同步功能（1-2 個月）

**目標**：實作 Omniverse 與 FDL 的雙向同步

**任務**：
- [ ] 設計同步機制
- [ ] 實作 FDL 到 Omniverse 的匯入
- [ ] 實作增量更新
- [ ] 實作衝突解決
- [ ] 測試同步功能

**交付物**：
- 雙向同步功能
- 同步文檔

### 9.4 階段 4：整合測試與優化（1 個月）

**目標**：整合測試並優化效能

**任務**：
- [ ] 端到端整合測試
- [ ] 效能測試和優化
- [ ] 使用者驗收測試
- [ ] 修復 Bug

**交付物**：
- 測試報告
- 效能基準
- Bug 修復

### 9.5 階段 5：文檔與發布（1 個月）

**目標**：完善文檔並發布

**任務**：
- [ ] 撰寫使用者手冊
- [ ] 撰寫開發者文檔
- [ ] 製作教學影片
- [ ] 準備發布包
- [ ] 發布到 Omniverse Extension Manager

**交付物**：
- 完整文檔
- 教學影片
- 發布包

**總計**：6-9 個月完整實作

---

## 總結

從 Omniverse 建立 FDL 模型的功能將：

1. ✅ **補充現有的整合方案**，提供視覺化優先的設計流程
2. ✅ **支援雙向工作流**，Omniverse ↔ FDL ↔ NDH
3. ✅ **利用 Omniverse 的協作功能**，支援多人協作設計
4. ✅ **提供快速原型設計能力**，加速設計驗證
5. ✅ **完善 IDTF 生態系統**，打通設計、執行、視覺化的完整閉環

結合工廠設計軟體整合（Plant 3D/E3D/Smart 3D → IADL + FDL）和 Omniverse 整合（Omniverse ↔ FDL），IDTF V3.5 將提供**業界最完整的工廠數位分身解決方案**。

---

**文件版本**: 1.0  
**最後更新**: 2025年10月14日  
**作者**: IDTF 技術團隊  
**聯絡**: tech@idtf.io

