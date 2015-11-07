// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[3], respectively.)

// Put your code here.
@i
M=0		//i=0
@R2
M=0		//R2=0
(LOOP)
@i
D=M		//D=i(0,1,2,3,…,R0)
@R0
D=M-D		//D=R0-i(R0,R0-1,…,0)
@END
D;JLE		//D<=0 -> END
@R1
D=M		//D=R1
@R2
M=D+M		//R2=R1+R2(0,R1,2R1,3R1,…,R0*R1)
@i
M=M+1		//i=i+1
@LOOP
0;JMP
(END)
@END
0;JMP