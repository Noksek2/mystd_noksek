import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# 데이터 입력
data = {
    'N': [
        1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000,
        6000, 6000, 6000, 6000, 6000, 6000, 6000, 6000, 6000,
        11000, 11000, 11000, 11000, 11000, 11000, 11000, 11000, 11000,
        16000, 16000, 16000, 16000, 16000, 16000, 16000, 16000, 16000
    ],
    'M': [
        8, 16, 32, 64, 128, 256, 512, 1024, 2048,
        8, 16, 32, 64, 128, 256, 512, 1024, 2048,
        8, 16, 32, 64, 128, 256, 512, 1024, 2048,
        8, 16, 32, 64, 128, 256, 512, 1024, 2048
    ],
    'Pool': [
        0.0000, 0.0000, 0.0001, 0.0001, 0.0001, 0.0002, 0.0007, 0.0014, 0.0021,
        0.0002, 0.0002, 0.0003, 0.0004, 0.0006, 0.0011, 0.0043, 0.0111, 0.0167,
        0.0004, 0.0004, 0.0005, 0.0008, 0.0012, 0.0021, 0.0078, 0.0180, 0.0305,
        0.0005, 0.0006, 0.0008, 0.0011, 0.0019, 0.0032, 0.0116, 0.0229, 0.0436
    ],
    'Malloc': [
        0.0001, 0.0000, 0.0001, 0.0001, 0.0001, 0.0001, 0.0002, 0.0006, 0.0013,
        0.0002, 0.0001, 0.0003, 0.0004, 0.0006, 0.0011, 0.0022, 0.0043, 0.0088,
        0.0004, 0.0003, 0.0006, 0.0007, 0.0012, 0.0024, 0.0042, 0.0083, 0.0167,
        0.0006, 0.0004, 0.0009, 0.0011, 0.0019, 0.0036, 0.0064, 0.0125, 0.0253
    ]
}

df = pd.DataFrame(data)

# N 값에 따른 색상 설정
colors = {1000: '#1f77b4', 6000: '#ff7f0e', 11000: '#2ca02c', 16000: '#d62728'}
n_values = sorted(df['N'].unique())

# 그래프 생성
fig, ax = plt.subplots(figsize=(12, 7))

# N별로 Pool 데이터 그리기
for n in n_values:
    subset = df[df['N'] == n]
    ax.plot(subset['M'], subset['Pool'], marker='o', linestyle='-', 
            color=colors[n], linewidth=2, markersize=6, 
            label=f'Pool (N={n})', alpha=0.8)

# N별로 Malloc 데이터 그리기
for n in n_values:
    subset = df[df['N'] == n]
    ax.plot(subset['M'], subset['Malloc'], marker='s', linestyle='--', 
            color=colors[n], linewidth=2, markersize=6, 
            label=f'Malloc (N={n})', alpha=0.8)

# 그래프 설정
ax.set_xlabel('M', fontsize=12, fontweight='bold')
ax.set_ylabel('소요시간 (초)', fontsize=12, fontweight='bold')
ax.set_title('Pool vs Malloc 성능 비교', fontsize=14, fontweight='bold')
ax.set_xscale('log')
ax.set_yscale('log')
ax.grid(True, alpha=0.3, linestyle='--')
ax.legend(loc='best', fontsize=10, framealpha=0.9)

plt.tight_layout()
plt.savefig('./plot.png', dpi=300, bbox_inches='tight')
print("그래프가 저장되었습니다: memory_performance_comparison.png")
plt.show()