import matplotlib.pyplot as plt
import matplotlib.font_manager as fm
import numpy as np

# 設定中文字體
plt.rcParams['font.sans-serif'] = ['Noto Sans CJK SC', 'DejaVu Sans']
plt.rcParams['axes.unicode_minus'] = False

# 數據
years = [2024, 2025, 2026, 2027, 2028, 2029, 2030]
market_size = [12.5, 15.2, 18.6, 22.8, 27.9, 34.2, 41.9]  # 單位: 十億美元

# 創建圖表
fig, ax = plt.subplots(figsize=(12, 7))

# 繪製柱狀圖
bars = ax.bar(years, market_size, color='#3498DB', alpha=0.8, edgecolor='#2874A6', linewidth=2)

# 在柱子上方添加數值標籤
for bar in bars:
    height = bar.get_height()
    ax.text(bar.get_x() + bar.get_width()/2., height,
            f'${height:.1f}B',
            ha='center', va='bottom', fontsize=14, fontweight='bold')

# 添加趨勢線
z = np.polyfit(years, market_size, 2)
p = np.poly1d(z)
ax.plot(years, p(years), "r--", alpha=0.8, linewidth=2, label='趨勢線')

# 設定標題和標籤
ax.set_title('工業數位分身市場規模預測 (2024-2030)', fontsize=20, fontweight='bold', pad=20)
ax.set_xlabel('年份', fontsize=16, fontweight='bold')
ax.set_ylabel('市場規模 (十億美元)', fontsize=16, fontweight='bold')

# 設定 Y 軸範圍
ax.set_ylim(0, 50)

# 添加網格
ax.grid(True, alpha=0.3, linestyle='--')

# 添加圖例
ax.legend(fontsize=12, loc='upper left')

# 添加註釋
ax.text(2027, 45, 'CAGR: 22.5%', fontsize=14, fontweight='bold',
        bbox=dict(boxstyle='round', facecolor='#F39C12', alpha=0.8))

plt.tight_layout()
plt.savefig('/home/ubuntu/ndh-test-framework/visualizations/gtm_market_size.png', dpi=300, bbox_inches='tight')
print("市場規模圖表已生成: gtm_market_size.png")

