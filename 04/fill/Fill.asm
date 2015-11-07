// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, the
// program clears the screen, i.e. writes "white" in every pixel.

// Put your code here.
(BEGIN)
@i
M=0                //i=0
(LOOP)
@KBD
D=M                //judge the state of the keyboard(pressed or unpressed)
@ELSE
D;JEQ
@input	
M=-1		   //if it is pressed, let input=-1(1111111111111111)
@PART
0;JMP
(ELSE)
@input
M=0                //else, let input=0

(PART)
@i
D=M                //D=i
@SCREEN
D=D+A              
@address
M=D                //get the address of the screen word        
@input
D=M                //D=input
@address
A=M                //get the address, assign the value of RAM(address) 
M=D                //black or white determined by the input
@i
M=M+1              //i=i+1
D=M
@8192              //width of the screen RAM
D=D-A
@BEGIN
D;JGE
@LOOP
0;JMP