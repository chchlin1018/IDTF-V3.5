import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.patches import FancyBboxPatch
import matplotlib.font_manager as fm

# 設定中文字體
plt.rcParams['font.sans-serif'] = ['Noto Sans CJK SC', 'DejaVu Sans']
plt.rcParams['axes.unicode_minus'] = False

# 創建圖表
fig, ax = plt.subplots(figsize=(16, 10))

# 定義時間線
phases = [
    {
        'name': '第一階段: 產品完善\n(0-3個月)',
        'y': 4,
        'color': '#3498DB',
        'tasks': [
            '✓ 簡化部署流程',
            '✓ 建立快速開始指南',
            '✓ 開發應用範本',
            '✓ 完善文件'
        ]
    },
    {
        'name': '第二階段: 市場驗證\n(3-6個月)',
        'y': 3,
        'color': '#F39C12',
        'tasks': [
            '✓ 建立 2-3 個標竿案例',
            '✓ 收集客戶反饋',
            '✓ 優化產品功能',
            '✓ 準備行銷材料'
        ]
    },
    {
        'name': '第三階段: 市場推廣\n(6-9個月)',
        'y': 2,
        'color': '#E74C3C',
        'tasks': [
            '✓ 啟動內容行銷',
            '✓ 參加行業展會',
            '✓ 建立合作夥伴網路',
            '✓ 開展線上研討會'
        ]
    },
    {
        'name': '第四階段: 規模化\n(9-12個月)',
        'y': 1,
        'color': '#27AE60',
        'tasks': [
            '✓ 擴大銷售團隊',
            '✓ 建立渠道合作',
            '✓ 推出企業版',
            '✓ 全球市場擴展'
        ]
    }
]

# 繪製時間線
for i, phase in enumerate(phases):
    # 繪製階段框
    box = FancyBboxPatch((0.5, phase['y']-0.3), 15, 0.6,
                          boxstyle="round,pad=0.05",
                          facecolor=phase['color'],
                          edgecolor='black',
                          linewidth=2,
                          alpha=0.7)
    ax.add_patch(box)
    
    # 添加階段名稱
    ax.text(1, phase['y'], phase['name'],
            fontsize=16, fontweight='bold', color='white',
            va='center')
    
    # 添加任務列表
    tasks_text = '\n'.join(phase['tasks'])
    ax.text(6, phase['y'], tasks_text,
            fontsize=12, color='white',
            va='center')

# 繪製連接箭頭
for i in range(len(phases)-1):
    ax.annotate('', xy=(8, phases[i+1]['y']+0.3), xytext=(8, phases[i]['y']-0.3),
                arrowprops=dict(arrowstyle='->', lw=3, color='black'))

# 設定標題
ax.set_title('Go-to-Market 實施路線圖', fontsize=24, fontweight='bold', pad=20)

# 設定座標軸
ax.set_xlim(0, 16)
ax.set_ylim(0.5, 4.8)
ax.axis('off')

# 添加時間軸
ax.text(8, 0.7, '時間軸: 12 個月', fontsize=18, fontweight='bold',
        ha='center',
        bbox=dict(boxstyle='round', facecolor='#ECF0F1', alpha=0.8))

plt.tight_layout()
plt.savefig('/home/ubuntu/ndh-test-framework/visualizations/gtm_strategy_timeline.png', dpi=300, bbox_inches='tight')
print("GTM 策略時間線圖表已生成: gtm_strategy_timeline.png")

