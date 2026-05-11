import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
raw_data="""
1000    8       0.0000  0.0001  0.0000
1000    16      0.0001  0.0000  0.0000
1000    32      0.0000  0.0001  0.0001
1000    64      0.0001  0.0001  0.0001
1000    128     0.0001  0.0001  0.0002
1000    256     0.0002  0.0001  0.0003
1000    504     0.0003  0.0002  0.0006
1000    1016    0.0007  0.0006  0.0013
1000    2040    0.0013  0.0013  0.0026
1000    4080    0.0025  0.0030  0.0056
6000    8       0.0001  0.0002  0.0003
6000    16      0.0001  0.0002  0.0002
6000    32      0.0002  0.0004  0.0003
6000    64      0.0003  0.0004  0.0005
6000    128     0.0006  0.0006  0.0010
6000    256     0.0010  0.0012  0.0020
6000    504     0.0020  0.0023  0.0049
6000    1016    0.0055  0.0045  0.0093
6000    2040    0.0125  0.0096  0.0184
6000    4080    0.0192  0.0189  0.0397
11000   8       0.0002  0.0005  0.0002
11000   16      0.0003  0.0003  0.0003
11000   32      0.0004  0.0007  0.0005
11000   64      0.0006  0.0015  0.0010
11000   128     0.0011  0.0012  0.0020
11000   256     0.0025  0.0024  0.0044
11000   504     0.0038  0.0044  0.0086
11000   1016    0.0074  0.0085  0.0174
11000   2040    0.0177  0.0162  0.0357
11000   4080    0.0348  0.0350  0.0741
"""
data = {
    'N': [],
    'M': [],
    'Pool': [],
    'Malloc': [],
    'Arena': []
}
# 3. 줄 단위로 읽어서 처리
lines = raw_data.strip().split('\n')
for line in lines:
    parts = line.split()  # 공백 기준으로 분리
    if len(parts) >= 5:
        data['N'].append(int(parts[0]))
        data['M'].append(int(parts[1]))
        data['Pool'].append(float(parts[2]))
        data['Malloc'].append(float(parts[3]))
        data['Arena'].append(float(parts[4]))

df = pd.DataFrame(data)

# N 값에 따른 색상 설정
colors = {1000: '#1f77b4', 6000: '#ff7f0e', 11000: '#2ca02c', 16000: '#d62728', 21000: '#80e030'}
n_values = sorted(df['N'].unique())

# 그래프 생성
fig, ax = plt.subplots(figsize=(12, 7))

# N별로 Pool 데이터 그리기
for n in n_values:
    subset = df[df['N'] == n]
    ax.plot(subset['M'], subset['Pool'], marker='o', linestyle='-', 
            color=colors[n], linewidth=2, markersize=6, 
            label=f'Pool (N={n})', alpha=1.0)

# N별로 Malloc 데이터 그리기
for n in n_values:
    subset = df[df['N'] == n]
    ax.plot(subset['M'], subset['Malloc'], marker='s', linestyle='--', 
            color=colors[n], linewidth=2, markersize=6, 
            label=f'Malloc (N={n})', alpha=0.6)

# N별로 Malloc 데이터 그리기
# for n in n_values:
#     subset = df[df['N'] == n]
#     ax.plot(subset['M'], subset['Arena'], marker='x', linestyle='-', 
#             color=colors[n], linewidth=2, markersize=6, 
#             label=f'Arena (N={n})', alpha=0.8)

# 그래프 설정
ax.set_xlabel('M', fontsize=12, fontweight='bold')
ax.set_ylabel('소요시간 (초)', fontsize=12, fontweight='bold')
ax.set_title('Pool vs Malloc vs Arena 성능 비교', fontsize=14, fontweight='bold')
ax.set_xscale('log')
ax.set_yscale('log')
ax.grid(True, alpha=0.3, linestyle='--')
ax.legend(loc='best', fontsize=10, framealpha=0.9)

plt.tight_layout()
plt.savefig('./plot_linux_32KB_2_1', dpi=300, bbox_inches='tight')
print("그래프가 저장되었습니다: memory_performance_comparison.png")
plt.show()