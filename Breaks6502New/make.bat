set PATH=c:\lcc\bin

lc -nw -g2 test.c 6502.c -o 6502.exe
6502.exe > out.txt
