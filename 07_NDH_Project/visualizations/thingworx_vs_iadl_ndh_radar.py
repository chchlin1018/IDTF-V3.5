#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import matplotlib.pyplot as plt
import matplotlib.font_manager as fm
import numpy as np

# 設定中文字體
plt.rcParams['font.sans-serif'] = ['Noto Sans CJK SC', 'DejaVu Sans']
plt.rcParams['axes.unicode_minus'] = False

# 評估維度
categories = [
    '資料連接',
    '資料標準化',
    '應用開發',
    '成本效益',
    '開放性',
    '靈活性',
    '事件處理',
    'AI/ML 整合'
]

# ThingWorx 評分 (1-5)
thingworx_scores = [5, 4, 5, 2, 2, 3, 4, 5]

# IADL+NDH 評分 (1-5)
iadl_ndh_scores = [5, 5, 2, 5, 5, 5, 5, 4]

# 計算角度
num_vars = len(categories)
angles = np.linspace(0, 2 * np.pi, num_vars, endpoint=False).tolist()

# 閉合圖形
thingworx_scores += thingworx_scores[:1]
iadl_ndh_scores += iadl_ndh_scores[:1]
angles += angles[:1]

# 建立圖表
fig, ax = plt.subplots(figsize=(12, 10), subplot_kw=dict(projection='polar'))

# 繪製 ThingWorx
ax.plot(angles, thingworx_scores, 'o-', linewidth=2, label='PTC ThingWorx', color='#E74C3C')
ax.fill(angles, thingworx_scores, alpha=0.25, color='#E74C3C')

# 繪製 IADL+NDH
ax.plot(angles, iadl_ndh_scores, 'o-', linewidth=2, label='IADL+NDH', color='#27AE60')
ax.fill(angles, iadl_ndh_scores, alpha=0.25, color='#27AE60')

# 設定軸標籤
ax.set_xticks(angles[:-1])
ax.set_xticklabels(categories, size=14)

# 設定 Y 軸範圍和標籤
ax.set_ylim(0, 5)
ax.set_yticks([1, 2, 3, 4, 5])
ax.set_yticklabels(['1', '2', '3', '4', '5'], size=12)

# 添加網格
ax.grid(True, linestyle='--', alpha=0.7)

# 添加標題
plt.title('PTC ThingWorx vs. IADL+NDH\n多維度能力對比', size=18, weight='bold', pad=30)

# 添加圖例
plt.legend(loc='upper right', bbox_to_anchor=(1.3, 1.1), fontsize=14)

# 添加說明
explanation = '評分標準: 1=非常弱, 2=弱, 3=一般, 4=強, 5=非常強'
plt.figtext(0.5, 0.02, explanation, ha='center', fontsize=12, style='italic', color='gray')

# 保存圖表
plt.tight_layout()
plt.savefig('/home/ubuntu/ndh-test-framework/visualizations/thingworx_vs_iadl_ndh_radar.png', 
            dpi=300, bbox_inches='tight')
print("雷達圖已生成: thingworx_vs_iadl_ndh_radar.png")
plt.close()

