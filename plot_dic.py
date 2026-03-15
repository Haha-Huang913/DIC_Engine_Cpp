import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import griddata

# 1. 加载c++计算结果表格
df = pd.read_csv("D:/DIC_App/FNCC+ICGN.csv")

# 2. 提取全场核心数据
x = df['X'].values
y = df['Y'].values
u = df['U'].values
v = df['V'].values
ux = df['UX'].values
vy = df['VY'].values  # 通常看主应变，X方向和Y方向的拉压最直观

# 3. 建立平滑网格 (高精度 200x200 插值)
xi = np.linspace(x.min(), x.max(), 200)
yi = np.linspace(y.min(), y.max(), 200)
xi, yi = np.meshgrid(xi, yi)

# 将离散点平滑化
zi_u = griddata((x, y), u, (xi, yi), method='cubic')
zi_v = griddata((x, y), v, (xi, yi), method='cubic')
zi_ux = griddata((x, y), ux, (xi, yi), method='cubic')
zi_vy = griddata((x, y), vy, (xi, yi), method='cubic')

# 4.  2x2 排版矩阵
fig, axs = plt.subplots(2, 2, figsize=(14, 10))
fig.suptitle('DIC Full-Field Deformation & Strain Analysis', fontsize=18, fontweight='bold')

# --- 图 1：U 位移 ---
im1 = axs[0, 0].imshow(zi_u, extent=(x.min(), x.max(), y.min(), y.max()), origin='lower', cmap='coolwarm', alpha=0.9)
axs[0, 0].set_title('U Displacement (Horizontal)', fontsize=12, fontweight='bold')
fig.colorbar(im1, ax=axs[0, 0], label='Pixels')

# --- 图 2：V 位移 ---
im2 = axs[0, 1].imshow(zi_v, extent=(x.min(), x.max(), y.min(), y.max()), origin='lower', cmap='viridis', alpha=0.9)
axs[0, 1].set_title('V Displacement (Vertical)', fontsize=12, fontweight='bold')
fig.colorbar(im2, ax=axs[0, 1], label='Pixels')

# --- 图 3：UX 应变 ---
im3 = axs[1, 0].imshow(zi_ux, extent=(x.min(), x.max(), y.min(), y.max()), origin='lower', cmap='RdBu_r', alpha=0.9)
axs[1, 0].set_title('UX Strain (Normal X)', fontsize=12, fontweight='bold')
fig.colorbar(im3, ax=axs[1, 0], label='Strain')

# --- 图 4：VY 应变 ---
im4 = axs[1, 1].imshow(zi_vy, extent=(x.min(), x.max(), y.min(), y.max()), origin='lower', cmap='Spectral', alpha=0.9)
axs[1, 1].set_title('VY Strain (Normal Y)', fontsize=12, fontweight='bold')
fig.colorbar(im4, ax=axs[1, 1], label='Strain')

# 5. 批量翻转 Y 轴并添加网格点
for ax in axs.flat:
    ax.scatter(x, y, c='black', s=5, alpha=0.3) # 画出你的计算网格点
    ax.invert_yaxis() # 图像坐标系Y轴朝下
    ax.set_xlabel('X (Pixels)')
    ax.set_ylabel('Y (Pixels)')

# 6. 完美自适应布局并保存高清大图
plt.tight_layout()
fig.subplots_adjust(top=0.92) # 给主标题留点空间
plt.savefig('DIC_Full_Report.png', dpi=300, bbox_inches='tight')
print(">>> (DIC_Full_Report.png)已生成 ")
plt.show()
