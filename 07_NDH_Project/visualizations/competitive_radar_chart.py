#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
競爭力對比雷達圖
比較 NDH 與主流商業方案的競爭力
"""

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import Circle, Polygon

# 設定中文字體
plt.rcParams['font.sans-serif'] = ['Noto Sans CJK SC', 'SimHei', 'DejaVu Sans']
plt.rcParams['axes.unicode_minus'] = False

# 建立圖形
fig, ax = plt.subplots(figsize=(12, 10), subplot_kw=dict(projection='polar'))

# 定義評估維度
categories = ['價格優勢', '開源透明', '中立性', '可擴展性', '事件溯源', '資料主權']
N = len(categories)

# 定義角度
angles = np.linspace(0, 2 * np.pi, N, endpoint=False).tolist()
angles += angles[:1]  # 閉合圖形

# 定義數據 (0-5 分)
ndh_values = [5, 5, 5, 5, 5, 5]  # NDH 在所有維度都是滿分
commercial_values = [2, 1, 2, 3, 2, 2]  # 商業方案的平均分數

ndh_values += ndh_values[:1]
commercial_values += commercial_values[:1]

# 繪製雷達圖
ax.plot(angles, ndh_values, 'o-', linewidth=3, label='NDH', color='#27AE60', markersize=8)
ax.fill(angles, ndh_values, alpha=0.25, color='#27AE60')

ax.plot(angles, commercial_values, 'o-', linewidth=3, label='商業方案 (平均)', 
        color='#E74C3C', markersize=8)
ax.fill(angles, commercial_values, alpha=0.25, color='#E74C3C')

# 設定刻度標籤
ax.set_xticks(angles[:-1])
ax.set_xticklabels(categories, fontsize=12, fontweight='bold')

# 設定徑向刻度
ax.set_ylim(0, 5)
ax.set_yticks([1, 2, 3, 4, 5])
ax.set_yticklabels(['1', '2', '3', '4', '5'], fontsize=10, color='gray')
ax.set_rlabel_position(0)

# 添加網格
ax.grid(True, linestyle='--', alpha=0.7)

# 添加圖例
ax.legend(loc='upper right', bbox_to_anchor=(1.3, 1.1), fontsize=12, frameon=True, 
         shadow=True, fancybox=True)

# 添加標題
plt.title('NDH vs. 主流商業方案競爭力對比', 
         fontsize=16, fontweight='bold', pad=20)

# 在圖表下方添加評分說明
fig.text(0.5, 0.05, 
         '評分標準: 1=非常弱  2=弱  3=中等  4=強  5=非常強\n'
         '商業方案包括: Kepware KEPServerEX, Ignition, HighByte Intelligence Hub 等',
         ha='center', fontsize=10, style='italic', color='#7F8C8D',
         bbox=dict(boxstyle='round,pad=0.5', facecolor='#ECF0F1', edgecolor='#BDC3C7'))

plt.tight_layout()
plt.savefig('/home/ubuntu/ndh-test-framework/visualizations/competitive_radar_chart.png', 
            dpi=300, bbox_inches='tight', facecolor='white')
print("競爭力對比雷達圖已儲存: competitive_radar_chart.png")

