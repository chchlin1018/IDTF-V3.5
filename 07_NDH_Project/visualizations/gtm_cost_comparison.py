import matplotlib.pyplot as plt
import matplotlib.font_manager as fm
import numpy as np

# 設定中文字體
plt.rcParams['font.sans-serif'] = ['Noto Sans CJK SC', 'DejaVu Sans']
plt.rcParams['axes.unicode_minus'] = False

# 數據
solutions = ['Siemens\nMindSphere', 'AVEVA\nAIM', 'PTC\nThingWorx', 'AWS IoT\nTwinMaker', 'IADL+NDH+\nOmniverse']
initial_cost = [250, 200, 225, 80, 30]  # 初始成本 (千美元)
annual_cost = [120, 100, 110, 60, 10]  # 年度成本 (千美元)
five_year_tco = [850, 700, 775, 380, 80]  # 5年總擁有成本 (千美元)

# 創建圖表
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 7))

# 左圖: 初始成本 vs 年度成本
x = np.arange(len(solutions))
width = 0.35

bars1 = ax1.bar(x - width/2, initial_cost, width, label='初始成本', color='#E74C3C', alpha=0.8)
bars2 = ax1.bar(x + width/2, annual_cost, width, label='年度成本', color='#3498DB', alpha=0.8)

# 添加數值標籤
for bars in [bars1, bars2]:
    for bar in bars:
        height = bar.get_height()
        ax1.text(bar.get_x() + bar.get_width()/2., height,
                f'${int(height)}K',
                ha='center', va='bottom', fontsize=11, fontweight='bold')

ax1.set_title('初始成本 vs 年度成本', fontsize=18, fontweight='bold', pad=15)
ax1.set_ylabel('成本 (千美元)', fontsize=14, fontweight='bold')
ax1.set_xticks(x)
ax1.set_xticklabels(solutions, fontsize=12)
ax1.legend(fontsize=12)
ax1.grid(True, alpha=0.3, axis='y', linestyle='--')

# 右圖: 5年總擁有成本
colors = ['#E74C3C', '#E67E22', '#F39C12', '#3498DB', '#27AE60']
bars = ax2.bar(solutions, five_year_tco, color=colors, alpha=0.8, edgecolor='black', linewidth=2)

# 添加數值標籤
for bar in bars:
    height = bar.get_height()
    ax2.text(bar.get_x() + bar.get_width()/2., height,
            f'${int(height)}K',
            ha='center', va='bottom', fontsize=13, fontweight='bold')

# 突出顯示 IADL+NDH+Omniverse
bars[-1].set_edgecolor('#27AE60')
bars[-1].set_linewidth(4)

ax2.set_title('5年總擁有成本 (TCO)', fontsize=18, fontweight='bold', pad=15)
ax2.set_ylabel('成本 (千美元)', fontsize=14, fontweight='bold')
ax2.set_xticklabels(solutions, fontsize=12)
ax2.grid(True, alpha=0.3, axis='y', linestyle='--')

# 添加節省標註
ax2.text(4, 500, '節省 90%!', fontsize=16, fontweight='bold', color='#27AE60',
         bbox=dict(boxstyle='round', facecolor='yellow', alpha=0.8))

plt.tight_layout()
plt.savefig('/home/ubuntu/ndh-test-framework/visualizations/gtm_cost_comparison.png', dpi=300, bbox_inches='tight')
print("成本對比圖表已生成: gtm_cost_comparison.png")

