        lw      0   3   mplier
        lw      0   4   mask
        lw      0   5   maxbit
        lw      0   6   mcand
L1      nor     3   4   2       비트추출
        beq     0   2   calc    
        beq     0   0   next
calc    add     1   6   1       곱셈을 위한 덧셈수행
next    lw      0   2   neg1
        add     2   5   5       결과값비트범위계산
        beq     0   5   done    결과값비트범위계산 15비트이상이면 종료
        add     4   4   4       maskin조정
        lw      0   2   pos1    maskin조정
        add     4   2   4       maskin조정
        add     6   6   6       
        beq     0   0   L1  
	noop    
done    halt
mcand   .fill   32766
mplier  .fill   12383
mask .fill   -2
pos1    .fill   1
neg1    .fill   -1
maxbit  .fill   15
