#!/usr/bin/env python3
"""
角色互動流程圖
展示 6 個角色在數位分身生命週期中的互動關係
"""

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.patches import FancyBboxPatch, FancyArrowPatch
import matplotlib.font_manager as fm

# 設定中文字體
plt.rcParams['font.sans-serif'] = ['Noto Sans CJK SC', 'SimHei', 'Microsoft YaHei']
plt.rcParams['axes.unicode_minus'] = False

# 創建圖表
fig, ax = plt.subplots(1, 1, figsize=(16, 12))
ax.set_xlim(0, 10)
ax.set_ylim(0, 12)
ax.axis('off')

# 定義顏色
colors = {
    'asset_creator': '#3498DB',  # 藍色
    'factory_designer': '#9B59B6',  # 紫色
    'factory_maker': '#E67E22',  # 橙色
    'factory_operator': '#27AE60',  # 綠色
    'factory_monitor': '#F39C12',  # 黃色
    'enterprise_monitor': '#E74C3C',  # 紅色
    'ndh': '#2C3E50',  # 深灰色
}

# 角色位置
roles = [
    {'name': '1. Asset Creator\n資產創建者', 'pos': (1.5, 10), 'color': colors['asset_creator']},
    {'name': '2. Factory Designer\n工廠設計者', 'pos': (5, 10), 'color': colors['factory_designer']},
    {'name': '3. Factory Maker\n工廠建造組裝者', 'pos': (8.5, 10), 'color': colors['factory_maker']},
    {'name': '4. Factory Operator\n工廠營運者', 'pos': (1.5, 6), 'color': colors['factory_operator']},
    {'name': '5. Factory Monitor\n工廠稽核者', 'pos': (5, 6), 'color': colors['factory_monitor']},
    {'name': '6. Enterprise Monitor\n企業檢測者', 'pos': (8.5, 6), 'color': colors['enterprise_monitor']},
]

# 繪製角色方塊
role_boxes = []
for role in roles:
    box = FancyBboxPatch(
        (role['pos'][0] - 1, role['pos'][1] - 0.6),
        2, 1.2,
        boxstyle="round,pad=0.1",
        facecolor=role['color'],
        edgecolor='white',
        linewidth=2,
        alpha=0.9
    )
    ax.add_patch(box)
    ax.text(role['pos'][0], role['pos'][1], role['name'],
            ha='center', va='center', fontsize=11, fontweight='bold', color='white')
    role_boxes.append((role['name'], role['pos']))

# NDH 中心
ndh_box = FancyBboxPatch(
    (4, 2.4), 2, 1.2,
    boxstyle="round,pad=0.1",
    facecolor=colors['ndh'],
    edgecolor='white',
    linewidth=3,
    alpha=0.9
)
ax.add_patch(ndh_box)
ax.text(5, 3, 'NDH\n中立資料中樞',
        ha='center', va='center', fontsize=13, fontweight='bold', color='white')

# 繪製箭頭和標籤
arrows = [
    # 設計階段
    {'from': (1.5, 9.4), 'to': (5, 9.4), 'label': 'IADL 資產定義', 'color': colors['asset_creator']},
    {'from': (5, 9.4), 'to': (8.5, 9.4), 'label': 'USD 設計圖', 'color': colors['factory_designer']},
    
    # 建造階段
    {'from': (8.5, 9.4), 'to': (1.5, 6.6), 'label': '實體資產綁定', 'color': colors['factory_maker'], 'style': 'arc3,rad=0.3'},
    
    # 運營資料流
    {'from': (1.5, 5.4), 'to': (5, 3.6), 'label': '即時運營資料', 'color': colors['factory_operator']},
    {'from': (5, 3.6), 'to': (5, 5.4), 'label': '分析報告', 'color': colors['factory_monitor']},
    {'from': (5, 5.4), 'to': (8.5, 5.4), 'label': '效能資料', 'color': colors['factory_monitor']},
    
    # 企業級資料流
    {'from': (8.5, 5.4), 'to': (6, 3.6), 'label': '業務資料', 'color': colors['enterprise_monitor']},
    {'from': (6, 3.6), 'to': (1.5, 5.4), 'label': '生產計畫', 'color': colors['enterprise_monitor'], 'style': 'arc3,rad=-0.3'},
]

for arrow in arrows:
    style = arrow.get('style', 'arc3,rad=0')
    arr = FancyArrowPatch(
        arrow['from'], arrow['to'],
        arrowstyle='->', mutation_scale=20, linewidth=2.5,
        color=arrow['color'], alpha=0.7,
        connectionstyle=style
    )
    ax.add_patch(arr)
    
    # 計算標籤位置 (箭頭中點)
    mid_x = (arrow['from'][0] + arrow['to'][0]) / 2
    mid_y = (arrow['from'][1] + arrow['to'][1]) / 2
    ax.text(mid_x, mid_y + 0.2, arrow['label'],
            ha='center', va='bottom', fontsize=9, 
            bbox=dict(boxstyle='round,pad=0.3', facecolor='white', alpha=0.8, edgecolor=arrow['color']))

# 標題
ax.text(5, 11.5, '工業數位分身生命週期角色互動圖',
        ha='center', va='top', fontsize=18, fontweight='bold')

# 階段標籤
ax.text(0.2, 10, '設計階段', ha='left', va='center', fontsize=12, fontweight='bold', 
        bbox=dict(boxstyle='round,pad=0.5', facecolor='lightblue', alpha=0.7))
ax.text(0.2, 6, '運營階段', ha='left', va='center', fontsize=12, fontweight='bold',
        bbox=dict(boxstyle='round,pad=0.5', facecolor='lightgreen', alpha=0.7))
ax.text(0.2, 3, '資料中樞', ha='left', va='center', fontsize=12, fontweight='bold',
        bbox=dict(boxstyle='round,pad=0.5', facecolor='lightgray', alpha=0.7))

# 說明文字
description = """
IADL 提供統一的資料語言,NDH 作為中央資料中樞,
連接所有角色和系統,實現從設計到運營的完整資料流動。
"""
ax.text(5, 0.8, description,
        ha='center', va='top', fontsize=10, style='italic',
        bbox=dict(boxstyle='round,pad=0.5', facecolor='lightyellow', alpha=0.7))

plt.tight_layout()
plt.savefig('/home/ubuntu/ndh-test-framework/visualizations/roles_interaction_diagram.png', 
            dpi=300, bbox_inches='tight', facecolor='white')
print("角色互動流程圖已生成: roles_interaction_diagram.png")

