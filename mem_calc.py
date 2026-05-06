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
import math
Ms=[8<<i for i in range(0,16)]
PG=4096*4
PH=32
X=64


for M in Ms:
    X=((PG-PH)//M)+7
    X=max(X//8,8)
    CNT=(PG-PH-X)//M
    R=PG-PH-X-CNT*M
    M_=M
    if M>256:
        CNT=max((PG)//(M),1)
        M_=((PG-PH-X)//CNT)//8*8
        H=PH+X+(M_*CNT)
        R=PG-H
    #1<<int(math.log2(M))
    
    print(f'{M}({M_})B X={X}({CNT}/{8*X}) unused{R}')
    #print(f'MemSz{M}({M_}) cnt:{CNT_}->{CNT} x:{X} unused:{PG-(PH+X+M_*CNT)}')
exit()

for M in Ms:
    if M>PG: break
    CNT_=(PG-PH)//M
    X=max(8,(CNT_ + 7)//8)
    CNT=max((PG-PH-X)//M,1)
    M_=M
    #256
    if M>256:
        M_=(PG-PH-X)//CNT
        M_=M_//8*8
    print(f'MemSz{M}({M_}) cnt:{CNT_}->{CNT} x:{X} unused:{PG-(PH+X+M_*CNT)}')

print('after')
for M in Ms:
    CNT=(PG)//M
    X=max(CNT//8,8)
    if CNT==0: break
    #M_=(PG-PH)//CNT
    #M=((M_+7)>>3)<<3
    #X=(CNT_ + 7)//8
    print(f'MemSz{M} X:{X} cnt:{CNT} ')

#CNT=(PG/8)>>idx
#PH(32) X(8~64) 

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

N=10000 M=512
0 0 0 Pool   Time : 0.068574
0 0 0 Malloc Time : 0.013770

N=9000 M=256
0 0 0 Pool   Time : 0.009875
0 0 0 Malloc Time : 0.009790


'''

'''N=9000 M=512
Pool   Time : 99 loop(91) 0.000137
199 loop(378) 0.000435
299 loop(861) 0.000150
399 loop(1596) 0.000125
499 loop(2485) 0.000133
599 loop(3570) 0.000397
699 loop(4851) 0.000143
799 loop(6441) 0.000152
899 loop(8128) 0.000150
999 loop(10011) 0.000160
1099 loop(12246) 0.000184
1199 loop(14535) 0.000184
1299 loop(17020) 0.000197
1399 loop(19701) 0.000225
1499 loop(22791) 0.000262
1599 loop(25878) 0.000268
1699 loop(29161) 0.000309
1799 loop(32896) 0.000223
1899 loop(36585) 0.000374
1999 loop(40470) 0.000362
2099 loop(44551) 0.000373
2199 loop(49141) 0.000398
2299 loop(53628) 0.000415
2399 loop(58311) 0.000435
2499 loop(63546) 0.000481
2599 loop(68635) 0.000428
2699 loop(73920) 0.000534
2799 loop(79401) 0.000552
2899 loop(85491) 0.000559
2999 loop(91378) 0.000494
3099 loop(97461) 0.000540
3199 loop(104196) 0.000595
3299 loop(110685) 0.000518
3399 loop(117370) 0.000593
3499 loop(124251) 0.000548
3599 loop(131841) 0.000526
3699 loop(139128) 0.000586
3799 loop(146611) 0.000593
3899 loop(154846) 0.000693
3999 loop(162735) 0.000598
4099 loop(170820) 0.000691
4199 loop(179101) 0.000692
4299 loop(188191) 0.000760
4399 loop(196878) 0.000663
4499 loop(205761) 0.000740
4599 loop(215496) 0.000789
4699 loop(224785) 0.000767
4799 loop(234270) 0.000735
4899 loop(243951) 0.000804
4999 loop(254541) 0.000877
5099 loop(264628) 0.000762
5199 loop(274911) 0.000807
5299 loop(286146) 0.000873
5399 loop(296835) 0.000809
5499 loop(307720) 0.000875
5599 loop(318801) 0.000821
5699 loop(330891) 0.000966
5799 loop(342378) 0.000916
5899 loop(354061) 0.000942
5999 loop(366796) 0.001039
6099 loop(378885) 0.000918
6199 loop(391170) 0.001844
6299 loop(403651) 0.001350
6399 loop(417241) 0.001064
6499 loop(430128) 0.000935
6599 loop(443211) 0.000965
6699 loop(457446) 0.001137
6799 loop(470935) 0.001061
6899 loop(484620) 0.001107
6999 loop(498501) 0.001105
7099 loop(513591) 0.001102
7199 loop(527878) 0.001130
7299 loop(542361) 0.001138
7399 loop(558096) 0.001154
7499 loop(572985) 0.001181
7599 loop(588070) 0.001118
7699 loop(603351) 0.001191
7799 loop(619941) 0.001430
7899 loop(635628) 0.001254
7999 loop(651511) 0.001164
8099 loop(668746) 0.001350
8199 loop(685035) 0.001336
8299 loop(701520) 0.001215
8399 loop(718201) 0.001196
8499 loop(736291) 0.001287
8599 loop(753378) 0.001339
8699 loop(770661) 0.001250
8799 loop(789396) 0.001392
8899 loop(807085) 0.001261
8999 loop(824970) 0.001393
loop 824970
'''