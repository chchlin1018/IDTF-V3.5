#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
NDH 技術架構層次圖
展示 NDH 的四大技術支柱
"""

import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.patches import FancyBboxPatch, Rectangle, FancyArrowPatch
import numpy as np

# 設定中文字體
plt.rcParams['font.sans-serif'] = ['Noto Sans CJK SC', 'SimHei', 'DejaVu Sans']
plt.rcParams['axes.unicode_minus'] = False

# 建立圖形
fig, ax = plt.subplots(1, 1, figsize=(14, 10))
ax.set_xlim(0, 14)
ax.set_ylim(0, 10)
ax.axis('off')

# 定義四層架構
layers = [
    {
        'name': 'CORBA 啟發的 Broker 架構',
        'y': 7.5,
        'height': 1.5,
        'color': '#3498DB',
        'description': '成熟的分散式物件模型\n提供穩定的基礎架構',
        'icon': '🏗️'
    },
    {
        'name': 'Kafka 事件流核心',
        'y': 5.5,
        'height': 1.5,
        'color': '#E74C3C',
        'description': '高吞吐量、高可靠性\n完整的事件溯源能力',
        'icon': '⚡'
    },
    {
        'name': '插件式連接器系統',
        'y': 3.5,
        'height': 1.5,
        'color': '#F39C12',
        'description': '易於擴展的萬能插座\n快速支援新設備和協定',
        'icon': '🔌'
    },
    {
        'name': 'IADL 資料標準化',
        'y': 1.5,
        'height': 1.5,
        'color': '#27AE60',
        'description': '統一的資料語義模型\n消除資料異構性',
        'icon': '📊'
    },
]

# 繪製層次
for i, layer in enumerate(layers):
    # 主要方塊
    box = FancyBboxPatch(
        (1, layer['y']),
        12, layer['height'],
        boxstyle="round,pad=0.05",
        facecolor=layer['color'],
        edgecolor='#2C3E50',
        linewidth=3,
        alpha=0.85
    )
    ax.add_patch(box)
    
    # 層次名稱
    ax.text(7, layer['y'] + layer['height'] * 0.65, layer['name'],
            ha='center', va='center', fontsize=14, fontweight='bold',
            color='white')
    
    # 描述文字
    ax.text(7, layer['y'] + layer['height'] * 0.25, layer['description'],
            ha='center', va='center', fontsize=10,
            color='white', style='italic')
    
    # 繪製連接箭頭 (除了最底層)
    if i < len(layers) - 1:
        arrow = FancyArrowPatch(
            (7, layer['y']), (7, layers[i+1]['y'] + layers[i+1]['height']),
            arrowstyle='->', mutation_scale=30, linewidth=3,
            color='#34495E', alpha=0.6, zorder=1
        )
        ax.add_patch(arrow)

# 添加標題
ax.text(7, 9.5, 'NDH 核心技術架構', 
        ha='center', fontsize=18, fontweight='bold')

# 添加左側標籤
labels = [
    {'text': '分散式\n物件層', 'y': 8.25, 'color': '#3498DB'},
    {'text': '事件\n流層', 'y': 6.25, 'color': '#E74C3C'},
    {'text': '連接器\n插件層', 'y': 4.25, 'color': '#F39C12'},
    {'text': '資料\n標準層', 'y': 2.25, 'color': '#27AE60'},
]

for label in labels:
    ax.text(0.3, label['y'], label['text'],
            ha='center', va='center', fontsize=10, fontweight='bold',
            color=label['color'],
            bbox=dict(boxstyle='round,pad=0.3', facecolor='white',
                     edgecolor=label['color'], linewidth=2))

# 添加右側核心能力
capabilities = [
    {'text': '穩定可靠', 'y': 8.25},
    {'text': '事件溯源', 'y': 6.25},
    {'text': '易於擴展', 'y': 4.25},
    {'text': '語義統一', 'y': 2.25},
]

for cap in capabilities:
    ax.text(13.7, cap['y'], cap['text'],
            ha='center', va='center', fontsize=10, fontweight='bold',
            color='#2C3E50',
            bbox=dict(boxstyle='round,pad=0.3', facecolor='#ECF0F1',
                     edgecolor='#95A5A6', linewidth=2))

# 添加底部資料流說明
flow_y = 0.5
ax.text(1, flow_y, '外部系統', ha='left', fontsize=11, 
        fontweight='bold', color='#7F8C8D')
ax.annotate('', xy=(3.5, flow_y), xytext=(2.5, flow_y),
            arrowprops=dict(arrowstyle='->', lw=2, color='#34495E'))
ax.text(4.5, flow_y, '連接器轉換', ha='center', fontsize=11, 
        fontweight='bold', color='#7F8C8D')
ax.annotate('', xy=(6.5, flow_y), xytext=(5.5, flow_y),
            arrowprops=dict(arrowstyle='->', lw=2, color='#34495E'))
ax.text(7.5, flow_y, 'IADL 格式', ha='center', fontsize=11, 
        fontweight='bold', color='#7F8C8D')
ax.annotate('', xy=(9.5, flow_y), xytext=(8.5, flow_y),
            arrowprops=dict(arrowstyle='->', lw=2, color='#34495E'))
ax.text(10.5, flow_y, 'Kafka 事件', ha='center', fontsize=11, 
        fontweight='bold', color='#7F8C8D')
ax.annotate('', xy=(12.5, flow_y), xytext=(11.5, flow_y),
            arrowprops=dict(arrowstyle='->', lw=2, color='#34495E'))
ax.text(13, flow_y, '應用', ha='left', fontsize=11, 
        fontweight='bold', color='#7F8C8D')

plt.tight_layout()
plt.savefig('/home/ubuntu/ndh-test-framework/visualizations/technical_architecture_layers.png', 
            dpi=300, bbox_inches='tight', facecolor='white')
print("技術架構層次圖已儲存: technical_architecture_layers.png")

