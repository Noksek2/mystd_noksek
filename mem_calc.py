'''PG={head(24B),check(x),data}

x=(4KB-data-24)
data_cnt=data/memsz
x=(data_cnt+7u)/8u


x= ((D/M)+7u)/8u = PG-D-PH


9개 => 2바이트

8
def PRINT(Ms):
	for M in Ms:
		R_CNT=((PG-PH)//M)
		R=R_CNT*M
		print(f'use{R}, unused{PG-PH-R}')
        
PG-PH
8 4096/8=512/8=64
16 256=32
8+64

'''

def PRINT(PG, Ms):
	for M in Ms:
		R_CNT=((PG-PH)//M)
		R=R_CNT*M
		print(f'use{R}, unused{PG-PH-R}')
        
Ms=[8<<i for i in range(0,16)]
PG=4096
PH=24

for M in Ms:
    CNT_=(PG-PH)//M
    X=(CNT_ + 7)//8
    print(f'MemSz{M} cnt:{CNT_} x:{X}')


#PRINT(PG, Ms)
#64>>pool_idx
'''
MemSz8 cnt:509 x:64
MemSz16 cnt:254 x:32
MemSz32 cnt:127 x:16
MemSz64 cnt:63 x:8
MemSz128 cnt:31 x:4
MemSz256 cnt:15 x:2
MemSz512 cnt:7 x:1
MemSz1024 cnt:3 x:1
MemSz2048 cnt:1 x:1
MemSz4096 cnt:0 x:0
MemSz8192 cnt:0 x:0
MemSz16384 cnt:0 x:0
MemSz32768 cnt:0 x:0
MemSz65536 cnt:0 x:0
MemSz131072 cnt:0 x:0
MemSz262144 cnt:0 x:0
'''

'''
N=90000 M=8 
0 0 0 Pool Time : 0.016134
0 0 0 Malloc Time : 0.052937

N=90000 M=8
0 0 0 Pool Time : 0.015764
0 0 0 Malloc Time : 0.053600


N=10000 M=16
Pool 
Malloc 
0 0 0 Time : 0.003121
0 0 0 Time : 0.007458

0 0 0 Time : 0.003069
0 0 0 Time : 0.007173

0 0 0 Pool   Time : 0.003707
0 0 0 Malloc Time : 0.007513

N=10000 M=32
0 0 0 Pool   Time : 0.003236
0 0 0 Malloc Time : 0.007443

N=10000 M=64
0 0 0 Pool   Time : 0.003589
0 0 0 Malloc Time : 0.007863

N=90000 M=64
0 0 0 Pool   Time : 0.082822
0 0 0 Malloc Time : 0.059952

N=9000 M=512
0 0 0 Pool   Time : 0.055925
0 0 0 Malloc Time : 0.013018

N=9000 M=512
0 0 0 Pool   Time : 0.056369
0 0 0 Malloc Time : 0.012907

N=9000 M=256
0 0 0 Pool   Time : 0.009875
0 0 0 Malloc Time : 0.009790

'''