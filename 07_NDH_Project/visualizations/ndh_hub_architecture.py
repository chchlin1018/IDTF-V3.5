#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
NDH 中心樞紐架構圖 (星型拓撲)
展示 NDH 作為中心連接所有系統
"""

import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.patches import FancyBboxPatch, FancyArrowPatch, Circle
import numpy as np

# 設定中文字體
plt.rcParams['font.sans-serif'] = ['Noto Sans CJK SC', 'SimHei', 'DejaVu Sans']
plt.rcParams['axes.unicode_minus'] = False

# 建立圖形
fig, ax = plt.subplots(1, 1, figsize=(14, 12))
ax.set_xlim(-8, 8)
ax.set_ylim(-8, 8)
ax.axis('off')

# 中心 NDH
center_x, center_y = 0, 0
center_radius = 1.5

# 繪製中心圓
center_circle = Circle((center_x, center_y), center_radius, 
                       facecolor='#3498DB', edgecolor='#2C3E50', 
                       linewidth=4, alpha=0.9, zorder=10)
ax.add_patch(center_circle)
ax.text(center_x, center_y, 'NDH\n中立資料中樞', 
        ha='center', va='center', fontsize=16, fontweight='bold',
        color='white', zorder=11)

# 定義外圍系統 (星型拓撲)
systems = [
    {'name': 'OPC UA', 'angle': 0, 'color': '#E74C3C', 'category': 'OT'},
    {'name': 'Sparkplug\nMQTT', 'angle': 45, 'color': '#E67E22', 'category': 'OT'},
    {'name': 'PI System', 'angle': 90, 'color': '#F39C12', 'category': 'OT'},
    {'name': 'Historian', 'angle': 135, 'color': '#F1C40F', 'category': 'OT'},
    {'name': 'Modbus', 'angle': 180, 'color': '#1ABC9C', 'category': 'OT'},
    {'name': 'SAP ERP', 'angle': 225, 'color': '#16A085', 'category': 'IT'},
    {'name': 'MES', 'angle': 270, 'color': '#27AE60', 'category': 'IT'},
    {'name': 'Kafka', 'angle': 315, 'color': '#2ECC71', 'category': 'IT'},
]

# 繪製外圍系統和連接線
radius = 5.5
for system in systems:
    angle_rad = np.radians(system['angle'])
    x = center_x + radius * np.cos(angle_rad)
    y = center_y + radius * np.sin(angle_rad)
    
    # 繪製系統方塊
    box = FancyBboxPatch(
        (x - 0.8, y - 0.5),
        1.6, 1.0,
        boxstyle="round,pad=0.1",
        facecolor=system['color'],
        edgecolor='#2C3E50',
        linewidth=2,
        alpha=0.85,
        zorder=5
    )
    ax.add_patch(box)
    ax.text(x, y, system['name'], 
            ha='center', va='center', fontsize=10, fontweight='bold',
            color='white', zorder=6)
    
    # 繪製連接線和箭頭
    # 從系統到 NDH
    start_x = x - 0.8 * np.cos(angle_rad)
    start_y = y - 0.8 * np.sin(angle_rad)
    end_x = center_x + (center_radius + 0.2) * np.cos(angle_rad)
    end_y = center_y + (center_radius + 0.2) * np.sin(angle_rad)
    
    arrow = FancyArrowPatch(
        (start_x, start_y), (end_x, end_y),
        arrowstyle='->', mutation_scale=20, linewidth=2.5,
        color='#34495E', alpha=0.7, zorder=3
    )
    ax.add_patch(arrow)
    
    # 從 NDH 到系統 (雙向)
    arrow2 = FancyArrowPatch(
        (end_x, end_y), (start_x, start_y),
        arrowstyle='->', mutation_scale=20, linewidth=2.5,
        color='#95A5A6', alpha=0.5, zorder=2, linestyle='dashed'
    )
    ax.add_patch(arrow2)

# 添加類別標籤
ax.text(-6.5, 3, 'OT 層\n(營運技術)', ha='center', fontsize=12, 
        fontweight='bold', color='#E74C3C',
        bbox=dict(boxstyle='round,pad=0.5', facecolor='white', 
                 edgecolor='#E74C3C', linewidth=2))

ax.text(-6.5, -3, 'IT 層\n(資訊技術)', ha='center', fontsize=12, 
        fontweight='bold', color='#27AE60',
        bbox=dict(boxstyle='round,pad=0.5', facecolor='white', 
                 edgecolor='#27AE60', linewidth=2))

# 添加標題
ax.text(0, 7.5, 'NDH 中心樞紐架構 (星型拓撲)', 
        ha='center', fontsize=18, fontweight='bold')

# 添加核心價值說明
values = [
    '✓ 一次連接，到處使用',
    '✓ 統一的 IADL 資料格式',
    '✓ 雙向資料流動',
]

for i, value in enumerate(values):
    ax.text(0, -7 + i*0.5, value,
            ha='center', fontsize=11, color='#27AE60', fontweight='bold')

# 添加圖例
legend_elements = [
    {'label': '實線箭頭: 資料流入 NDH', 'color': '#34495E', 'style': '-'},
    {'label': '虛線箭頭: 資料流出 NDH', 'color': '#95A5A6', 'style': '--'},
]

for i, elem in enumerate(legend_elements):
    ax.plot([5.5, 6.5], [-7.5 + i*0.5, -7.5 + i*0.5], 
           color=elem['color'], linewidth=2, linestyle=elem['style'])
    ax.text(6.7, -7.5 + i*0.5, elem['label'], 
           ha='left', va='center', fontsize=9, color='#7F8C8D')

plt.tight_layout()
plt.savefig('/home/ubuntu/ndh-test-framework/visualizations/ndh_hub_architecture.png', 
            dpi=300, bbox_inches='tight', facecolor='white')
print("NDH 中心樞紐架構圖已儲存: ndh_hub_architecture.png")

