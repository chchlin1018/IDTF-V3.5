#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
資料孤島示意圖
展示工業系統之間的資料隔離問題
"""

import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.patches import FancyBboxPatch, FancyArrowPatch
import matplotlib.font_manager as fm

# 設定中文字體
plt.rcParams['font.sans-serif'] = ['Noto Sans CJK SC', 'SimHei', 'DejaVu Sans']
plt.rcParams['axes.unicode_minus'] = False

# 建立圖形
fig, ax = plt.subplots(1, 1, figsize=(14, 8))
ax.set_xlim(0, 14)
ax.set_ylim(0, 8)
ax.axis('off')

# 定義系統和顏色
systems = [
    {'name': 'PLC\n(可程式邏輯控制器)', 'pos': (1.5, 5.5), 'color': '#FF6B6B'},
    {'name': 'SCADA\n(監控與資料擷取)', 'pos': (4.5, 5.5), 'color': '#4ECDC4'},
    {'name': 'MES\n(製造執行系統)', 'pos': (7.5, 5.5), 'color': '#45B7D1'},
    {'name': 'ERP\n(企業資源規劃)', 'pos': (10.5, 5.5), 'color': '#96CEB4'},
    {'name': 'Historian\n(歷史資料庫)', 'pos': (3, 2.5), 'color': '#FFEAA7'},
    {'name': 'Cloud\n(雲端平台)', 'pos': (9, 2.5), 'color': '#DFE6E9'},
]

# 繪製系統方塊
for system in systems:
    box = FancyBboxPatch(
        (system['pos'][0] - 1, system['pos'][1] - 0.6),
        2, 1.2,
        boxstyle="round,pad=0.1",
        facecolor=system['color'],
        edgecolor='black',
        linewidth=2,
        alpha=0.8
    )
    ax.add_patch(box)
    ax.text(system['pos'][0], system['pos'][1], system['name'],
            ha='center', va='center', fontsize=11, fontweight='bold')

# 繪製隔離牆
walls = [
    (3.5, 1.5, 3.5, 7),
    (6.5, 1.5, 6.5, 7),
    (9.5, 1.5, 9.5, 7),
]

for wall in walls:
    ax.plot([wall[0], wall[2]], [wall[1], wall[3]], 
            color='#2D3436', linewidth=4, linestyle='--', alpha=0.6)
    # 添加鎖的圖示
    lock_y = (wall[1] + wall[3]) / 2
    circle = plt.Circle((wall[0], lock_y), 0.15, color='#2D3436', alpha=0.8)
    ax.add_patch(circle)

# 繪製 X 標記表示無法連接
x_marks = [
    (3.5, 5.5), (6.5, 5.5), (9.5, 5.5),
    (6, 4), (6, 3.5),
]

for x_pos in x_marks:
    ax.plot([x_pos[0]-0.2, x_pos[0]+0.2], [x_pos[1]-0.2, x_pos[1]+0.2],
            'r', linewidth=3, alpha=0.7)
    ax.plot([x_pos[0]-0.2, x_pos[0]+0.2], [x_pos[1]+0.2, x_pos[1]-0.2],
            'r', linewidth=3, alpha=0.7)

# 添加標題
ax.text(7, 7.5, '工業 4.0 的資料孤島問題', 
        ha='center', fontsize=18, fontweight='bold')

# 添加說明文字
problems = [
    '❌ 系統異構，資料格式不相容',
    '❌ 協定專有，無法互相通訊',
    '❌ 資料被困，價值無法發揮',
]

for i, problem in enumerate(problems):
    ax.text(0.5, 0.8 - i*0.3, problem,
            ha='left', fontsize=11, color='#E74C3C', fontweight='bold')

# 添加圖例
legend_text = '虛線表示資料隔離的「牆」'
ax.text(13.5, 0.5, legend_text, ha='right', fontsize=9, 
        style='italic', color='#7F8C8D')

plt.tight_layout()
plt.savefig('/home/ubuntu/ndh-test-framework/visualizations/data_silos_diagram.png', 
            dpi=300, bbox_inches='tight', facecolor='white')
print("資料孤島示意圖已儲存: data_silos_diagram.png")

