#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import matplotlib.pyplot as plt
import matplotlib.font_manager as fm
import numpy as np

# 設定中文字體
plt.rcParams['font.sans-serif'] = ['Noto Sans CJK SC', 'DejaVu Sans']
plt.rcParams['axes.unicode_minus'] = False

# 成本數據 (單位: 千美元)
categories = ['第 1 年', '第 2 年', '第 3 年', '第 4 年', '第 5 年']
thingworx_costs = [150, 120, 120, 120, 120]  # 初始成本高,後續維護費用
iadl_ndh_costs = [30, 20, 20, 20, 20]  # 僅基礎設施和人力成本

x = np.arange(len(categories))
width = 0.35

fig, ax = plt.subplots(figsize=(12, 8))

bars1 = ax.bar(x - width/2, thingworx_costs, width, label='PTC ThingWorx', color='#E74C3C', alpha=0.8)
bars2 = ax.bar(x + width/2, iadl_ndh_costs, width, label='IADL+NDH', color='#27AE60', alpha=0.8)

# 添加數值標籤
for bars in [bars1, bars2]:
    for bar in bars:
        height = bar.get_height()
        ax.text(bar.get_x() + bar.get_width()/2., height,
                f'${int(height)}K',
                ha='center', va='bottom', fontsize=11, fontweight='bold')

# 設定標籤和標題
ax.set_xlabel('時間', fontsize=14, fontweight='bold')
ax.set_ylabel('成本 (千美元)', fontsize=14, fontweight='bold')
ax.set_title('PTC ThingWorx vs. IADL+NDH\n5 年總擁有成本 (TCO) 對比', fontsize=16, fontweight='bold', pad=20)
ax.set_xticks(x)
ax.set_xticklabels(categories, fontsize=12)
ax.legend(fontsize=13, loc='upper right')

# 添加網格
ax.grid(axis='y', linestyle='--', alpha=0.3)
ax.set_axisbelow(True)

# 添加總成本說明
thingworx_total = sum(thingworx_costs)
iadl_ndh_total = sum(iadl_ndh_costs)
savings = thingworx_total - iadl_ndh_total
savings_pct = (savings / thingworx_total) * 100

explanation = f'5 年總成本: ThingWorx = ${thingworx_total}K | IADL+NDH = ${iadl_ndh_total}K | 節省 ${savings}K ({savings_pct:.0f}%)'
plt.figtext(0.5, 0.02, explanation, ha='center', fontsize=12, style='italic', 
            bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.3))

# 保存圖表
plt.tight_layout()
plt.savefig('/home/ubuntu/ndh-test-framework/visualizations/thingworx_vs_iadl_ndh_cost.png', 
            dpi=300, bbox_inches='tight')
print("成本對比圖已生成: thingworx_vs_iadl_ndh_cost.png")
plt.close()

