import matplotlib.pyplot as plt
import numpy as np
import io
import pandas as pd

data1 = """
8.000   0.0011  0.0002
12.018  0.0012  0.0004
16.026  0.0013  0.0005
19.960  0.0014  0.0005
25.562  0.0015  0.0006
32.167  0.0016  0.0007
41.467  0.0017  0.0006
51.787  0.0017  0.0008
67.754  0.0021  0.0008
85.872  0.0028  0.0009
110.829 0.0022  0.0009
144.994 0.0027  0.0010
214.004 0.0031  0.0012
341.240 0.0037  0.0016
"""


data2="""
1000    8       0.0000  0.0001  0.0000
1000    16      0.0000  0.0000  0.0000
1000    24      0.0000  0.0000  0.0000
1000    32      0.0000  0.0001  0.0000
1000    48      0.0001  0.0001  0.0001
1000    64      0.0001  0.0001  0.0001
1000    96      0.0001  0.0001  0.0001
1000    128     0.0001  0.0001  0.0001
1000    192     0.0001  0.0001  0.0002
1000    256     0.0002  0.0001  0.0003
1000    384     0.0003  0.0002  0.0004
1000    512     0.0003  0.0002  0.0005
1000    1024    0.0006  0.0008  0.0011
1000    1024    0.0006  0.0006  0.0011
6000    8       0.0002  0.0002  0.0001
6000    16      0.0002  0.0002  0.0002
6000    24      0.0002  0.0001  0.0002
6000    32      0.0002  0.0003  0.0002
6000    48      0.0003  0.0003  0.0004
6000    64      0.0004  0.0003  0.0005
6000    96      0.0005  0.0004  0.0006
6000    128     0.0006  0.0005  0.0011
6000    192     0.0009  0.0007  0.0012
6000    256     0.0011  0.0010  0.0016
6000    384     0.0017  0.0014  0.0033
6000    512     0.0024  0.0020  0.0038
6000    1024    0.0044  0.0041  0.0075
6000    1024    0.0040  0.0043  0.0075
11000   8       0.0003  0.0008  0.0003
11000   16      0.0004  0.0003  0.0003
11000   24      0.0004  0.0003  0.0004
11000   32      0.0004  0.0007  0.0006
11000   48      0.0006  0.0006  0.0007
11000   64      0.0007  0.0007  0.0009
11000   96      0.0010  0.0007  0.0014
11000   128     0.0012  0.0009  0.0018
11000   192     0.0016  0.0017  0.0026
11000   256     0.0021  0.0022  0.0035
11000   384     0.0032  0.0034  0.0052
11000   512     0.0044  0.0042  0.0069
11000   1024    0.0102  0.0087  0.0139
11000   1024    0.0077  0.0088  0.0137
"""

def plot2(raw_data):
    data = np.loadtxt(io.StringIO(raw_data))
    N, Malloc, Pool = data[:, 0], data[:, 1], data[:, 2]

    # 2. 그래프 생성
    plt.figure(figsize=(10, 6))

    plt.plot(N, Pool, 'o-', label='Pool', linewidth=2)
    plt.plot(N, Malloc, 's--', label='Malloc', alpha=0.7)

    # 3. 그래프 설정
    plt.xscale('log')
    plt.yscale('log')
    plt.xlabel('N (Size)', fontweight='bold')
    plt.ylabel('Time (sec)', fontweight='bold')
    plt.title('Memory Performance: Malloc vs Pool', fontsize=14, fontweight='bold')
    plt.grid(True, which="both", ls="--", alpha=0.3)
    plt.legend()

    plt.tight_layout()
    plt.show()
    
def plot1(raw_data): 
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
    ax.set_title('Pool vs SlowPool vs Malloc vs Arena 성능 비교', fontsize=14, fontweight='bold')
    ax.set_xscale('log')
    ax.set_yscale('log')
    ax.grid(True, alpha=0.3, linestyle='--')
    ax.legend(loc='best', fontsize=10, framealpha=0.9)

    plt.tight_layout()
    plt.savefig('./plot_linux_1', dpi=300, bbox_inches='tight')
    print("그래프가 저장되었습니다: memory_performance_comparison.png")
    plt.show()