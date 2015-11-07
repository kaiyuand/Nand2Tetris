#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#define MAX_LINES 30000
#define MAX_CHAR_PER_LINE 100
//labelindex
int labelindex=0;
//filename
static char filename[MAX_CHAR_PER_LINE];
//function name array
static char* funcname;
//function number
int funcnum=0;
//translate the arthimatic and logic statement
void funcback(FILE *fout);
void translateA(char *str,FILE *fout)
{
	if(!strcmp(str,"add"))
	{
		fputs("@SP\n",fout);
		fputs("AM=M-1\n",fout);
		fputs("D=M\n",fout);
		fputs("A=A-1\n",fout);
		fputs("M=M+D\n",fout);
	}
	//translate sub
	else if (!strcmp(str,"sub"))
	{
		fputs("@SP\n",fout);
		fputs("AM=M-1\n",fout);
		fputs("D=M\n",fout);
		fputs("A=A-1\n",fout);
		fputs("M=M-D\n",fout);
	}
	//translate eq
	else if (!strcmp(str,"eq"))
	{
		fputs("@SP\n",fout);
		fputs("AM=M-1\n",fout);
		fputs("D=M\n",fout);
		fputs("A=A-1\n",fout);
		fputs("D=M-D\n",fout);
		fprintf(fout,"@ELSE%d\n",labelindex);
		fputs("D;JEQ\n",fout);
		fputs("@SP\n",fout);
		fputs("A=M-1\n",fout);
		fputs("M=0\n",fout);
		fprintf(fout,"@CONTINUE%d\n",labelindex);
		fputs("0;JMP\n",fout);
		fprintf(fout,"(ELSE%d)\n",labelindex);
		fputs("@SP\n",fout);
		fputs("A=M-1\n",fout);
		fputs("M=-1\n",fout);
		fprintf(fout,"(CONTINUE%d)\n",labelindex);
		labelindex++;
	}
	//translate neg
	else if (!strcmp(str,"neg"))
	{
		fputs("D=0\n",fout);
		fputs("@SP\n",fout);
		fputs("A=M-1\n",fout);
		fputs("M=D-M\n",fout);	
	}
	//translate gt
	else if (!strcmp(str,"gt"))
	{
		fputs("@SP\n",fout);
		fputs("AM=M-1\n",fout);
		fputs("D=M\n",fout);
		fputs("A=A-1\n",fout);
		fputs("D=M-D\n",fout);
		fprintf(fout,"@ELSE%d\n",labelindex);
		fputs("D;JGT\n",fout);
		fputs("@SP\n",fout);
		fputs("A=M-1\n",fout);
		fputs("M=0\n",fout);
		fprintf(fout,"@CONTINUE%d\n",labelindex);
		fputs("0;JMP\n",fout);
		fprintf(fout,"(ELSE%d)\n",labelindex);
		fputs("@SP\n",fout);
		fputs("A=M-1\n",fout);
		fputs("M=-1\n",fout);
		fprintf(fout,"(CONTINUE%d)\n",labelindex);
		labelindex++;
	}
	//translate lt
	else if (!strcmp(str,"lt"))
	{
		fputs("@SP\n",fout);
		fputs("AM=M-1\n",fout);
		fputs("D=M\n",fout);
		fputs("A=A-1\n",fout);
		fputs("D=M-D\n",fout);
		fprintf(fout,"@ELSE%d\n",labelindex);
		fputs("D;JLT\n",fout);
		fputs("@SP\n",fout);
		fputs("A=M-1\n",fout);
		fputs("M=0\n",fout);
		fprintf(fout,"@CONTINUE%d\n",labelindex);
		fputs("0;JMP\n",fout);
		fprintf(fout,"(ELSE%d)\n",labelindex);
		fputs("@SP\n",fout);
		fputs("A=M-1\n",fout);
		fputs("M=-1\n",fout);
		fprintf(fout,"(CONTINUE%d)\n",labelindex);
		labelindex++;
	}
	//translate and
	else if (!strcmp(str,"and"))
	{
		fputs("@SP\n",fout);
		fputs("AM=M-1\n",fout);
		fputs("D=M\n",fout);
		fputs("A=A-1\n",fout);
		fputs("M=M&D\n",fout);
	}
	//translate or
	else if (!strcmp(str,"or"))
	{
		fputs("@SP\n",fout);
		fputs("AM=M-1\n",fout);
		fputs("D=M\n",fout);
		fputs("A=A-1\n",fout);
		fputs("M=M|D\n",fout);
	}
	//translate not
	else if (!strcmp(str,"not"))
	{
		fputs("@SP\n",fout);
		fputs("A=M-1\n",fout);
		fputs("M=!M\n",fout);	
	}
	//return
	else if (!strcmp(str,"return"))
	funcback(fout);
}

//translate the "push constant ..." 
void pushconstant(char *str,FILE *fout)
{
	char number[50]="@";
	strcat(number,str);
	fprintf(fout,"%s\n",number);
	fputs("D=A\n",fout);
	fputs("@SP\n",fout);
	fputs("A=M\n",fout);
	fputs("M=D\n",fout);
	fputs("@SP\n",fout);
	fputs("M=M+1\n",fout);
}
//translate the "push local ..." 
void pushlocal(char *str,FILE *fout)
{
	char number[50]="@";
	strcat(number,str);
	fputs("@LCL\n",fout);
	fputs("D=M\n",fout);
	fprintf(fout,"%s\n",number);
	fputs("A=D+A\n",fout);
	fputs("D=M\n",fout);
	fputs("@SP\n",fout);
	fputs("A=M\n",fout);
	fputs("M=D\n",fout);
	fputs("@SP\n",fout);
	fputs("M=M+1\n",fout);
}
//translate the "push argument ..." 
void pushargument(char *str,FILE *fout)
{
	char number[50]="@";
	strcat(number,str);
	fputs("@ARG\n",fout);
	fputs("D=M\n",fout);
	fprintf(fout,"%s\n",number);
	fputs("A=D+A\n",fout);
	fputs("D=M\n",fout);
	fputs("@SP\n",fout);
	fputs("A=M\n",fout);
	fputs("M=D\n",fout);
	fputs("@SP\n",fout);
	fputs("M=M+1\n",fout);
}
//translate the "push this ..." 
void pushthis(char *str,FILE *fout)
{
	char number[50]="@";
	strcat(number,str);
	fputs("@THIS\n",fout);
	fputs("D=M\n",fout);
	fprintf(fout,"%s\n",number);
	fputs("A=D+A\n",fout);
	fputs("D=M\n",fout);
	fputs("@SP\n",fout);
	fputs("A=M\n",fout);
	fputs("M=D\n",fout);
	fputs("@SP\n",fout);
	fputs("M=M+1\n",fout);
}
//translate the "push that ..." 
void pushthat(char *str,FILE *fout)
{
	char number[50]="@";
	strcat(number,str);
	fputs("@THAT\n",fout);
	fputs("D=M\n",fout);
	fprintf(fout,"%s\n",number);
	fputs("A=D+A\n",fout);
	fputs("D=M\n",fout);
	fputs("@SP\n",fout);
	fputs("A=M\n",fout);
	fputs("M=D\n",fout);
	fputs("@SP\n",fout);
	fputs("M=M+1\n",fout);
}
//translate the "push pointer ..." 
void pushpointer(char *str,FILE *fout)
{
	//pointer 0 -> RAM[3]
	//pointer 1 -> RAM[4]
	char number0[5]="@3";
	char number1[5]="@4";
	char number[5];
	if (!strcmp(str,"0"))
		strcpy(number,number0);
	else if(!strcmp(str,"1"))
		strcpy(number,number1);
	fprintf(fout,"%s\n",number);
	fputs("D=M\n",fout);
	fputs("@SP\n",fout);
	fputs("A=M\n",fout);
	fputs("M=D\n",fout);
	fputs("@SP\n",fout);
	fputs("M=M+1\n",fout);
}
//translate the "push temp ..." 
void pushtemp(char *str,FILE *fout)
{
	//temp 0-7 -> RAM[5]-[12]
	char number0[5]="@5";
	char number1[5]="@6";
	char number2[5]="@7";
	char number3[5]="@8";
	char number4[5]="@9";
	char number5[5]="@10";
	char number6[5]="@11";
	char number7[5]="@12";
	char number[5];
	if (!strcmp(str,"0"))
		strcpy(number,number0);
	else if(!strcmp(str,"1"))
		strcpy(number,number1);
	else if(!strcmp(str,"2"))
		strcpy(number,number2);
	else if(!strcmp(str,"3"))
		strcpy(number,number3);
	else if(!strcmp(str,"4"))
		strcpy(number,number4);
	else if(!strcmp(str,"5"))
		strcpy(number,number5);
	else if(!strcmp(str,"6"))
		strcpy(number,number6);
	else if(!strcmp(str,"7"))
		strcpy(number,number7);
	fprintf(fout,"%s\n",number);
	fputs("D=M\n",fout);
	fputs("@SP\n",fout);
	fputs("A=M\n",fout);
	fputs("M=D\n",fout);
	fputs("@SP\n",fout);
	fputs("M=M+1\n",fout);
}
//translate the "push static ..." 
void pushstatic(char *str,FILE *fout)
{
	//the static 0 will be stored in RAM[16]
	//the address of static x is 16+x
	//int base=16;
	//int add;
	//char address[10]="@";
	//char number[10];
	//add=atoi(str);
	//base=base+add;
	//sprintf(number,"%d",base);
	//strcat(address,number);
	fprintf(fout,"@%s.%s\n",filename,str);
	fputs("D=M\n",fout);
	fputs("@SP\n",fout);
	fputs("A=M\n",fout);
	fputs("M=D\n",fout);
	fputs("@SP\n",fout);
	fputs("M=M+1\n",fout);
}
//POP functions(no pop constant)
//---------------------------------------

//translate the "pop local ..." 
void poplocal(char *str,FILE *fout)
{
	char number[50]="@";
	strcat(number,str);
	fputs("@LCL\n",fout);
	fputs("D=M\n",fout);
	fprintf(fout,"%s\n",number);
	fputs("D=D+A\n",fout);
	fputs("@R15\n",fout);
	fputs("M=D\n",fout);
	fputs("@SP\n",fout);
	fputs("AM=M-1\n",fout);
	fputs("D=M\n",fout);
	fputs("@R15\n",fout);
	fputs("A=M\n",fout);
	fputs("M=D\n",fout);
}
//translate the "pop argument ..." 
void popargument(char *str,FILE *fout)
{
	char number[50]="@";
	strcat(number,str);
	fputs("@ARG\n",fout);
	fputs("D=M\n",fout);
	fprintf(fout,"%s\n",number);
	fputs("D=D+A\n",fout);
	fputs("@R15\n",fout);
	fputs("M=D\n",fout);
	fputs("@SP\n",fout);
	fputs("AM=M-1\n",fout);
	fputs("D=M\n",fout);
	fputs("@R15\n",fout);
	fputs("A=M\n",fout);
	fputs("M=D\n",fout);
}
//translate the "pop this ..." 
void popthis(char *str,FILE *fout)
{
	char number[50]="@";
	strcat(number,str);
	fputs("@THIS\n",fout);
	fputs("D=M\n",fout);
	fprintf(fout,"%s\n",number);
	fputs("D=D+A\n",fout);
	fputs("@R15\n",fout);
	fputs("M=D\n",fout);
	fputs("@SP\n",fout);
	fputs("AM=M-1\n",fout);
	fputs("D=M\n",fout);
	fputs("@R15\n",fout);
	fputs("A=M\n",fout);
	fputs("M=D\n",fout);
}
//translate the "pop that ..." 
void popthat(char *str,FILE *fout)
{
	char number[50]="@";
	strcat(number,str);
	fputs("@THAT\n",fout);
	fputs("D=M\n",fout);
	fprintf(fout,"%s\n",number);
	fputs("D=D+A\n",fout);
	fputs("@R15\n",fout);
	fputs("M=D\n",fout);
	fputs("@SP\n",fout);
	fputs("AM=M-1\n",fout);
	fputs("D=M\n",fout);
	fputs("@R15\n",fout);
	fputs("A=M\n",fout);
	fputs("M=D\n",fout);
}
//translate the "pop pointer ..." 
void poppointer(char *str,FILE *fout)
{
	//pointer 0 -> RAM[3]
	//pointer 1 -> RAM[4]
	char number0[5]="@3";
	char number1[5]="@4";
	char number[5];
	if (!strcmp(str,"0"))
		strcpy(number,number0);
	else if(!strcmp(str,"1"))
		strcpy(number,number1);
	fputs("@SP\n",fout);
	fputs("AM=M-1\n",fout);
	fputs("D=M\n",fout);
	fprintf(fout,"%s\n",number);
	fputs("M=D\n",fout);
}
//translate the "pop temp ..." 
void poptemp(char *str,FILE *fout)
{
	//temp 0-7 -> RAM[5]-[12]
	char number0[5]="@5";
	char number1[5]="@6";
	char number2[5]="@7";
	char number3[5]="@8";
	char number4[5]="@9";
	char number5[5]="@10";
	char number6[5]="@11";
	char number7[5]="@12";
	char number[5];
	if (!strcmp(str,"0"))
		strcpy(number,number0);
	else if(!strcmp(str,"1"))
		strcpy(number,number1);
	else if(!strcmp(str,"2"))
		strcpy(number,number2);
	else if(!strcmp(str,"3"))
		strcpy(number,number3);
	else if(!strcmp(str,"4"))
		strcpy(number,number4);
	else if(!strcmp(str,"5"))
		strcpy(number,number5);
	else if(!strcmp(str,"6"))
		strcpy(number,number6);
	else if(!strcmp(str,"7"))
		strcpy(number,number7);
	fputs("@SP\n",fout);
	fputs("AM=M-1\n",fout);
	fputs("D=M\n",fout);
	fprintf(fout,"%s\n",number);
	fputs("M=D\n",fout);
}

//translate the "pop static ..."
void popstatic(char *str,FILE *fout)
{
	//the base address of static is 16
	//the address of static x is 16+x
	//int base=16;
	//int add;
	//char address[10]="@";
	//char number[10];
	//add=atoi(str);
	//base=base+add;
	//sprintf(number,"%d",base);
	//strcat(address,number);
	fputs("@SP\n",fout);
	fputs("AM=M-1\n",fout);
	fputs("D=M\n",fout);
	fprintf(fout,"@%s.%s\n",filename,str);
	fputs("M=D\n",fout);
}

void writefunction(char *functionname, char* numvariable,FILE *fout)
{
	funcname=(char*)realloc(funcname,sizeof(functionname));
	strcpy(funcname,functionname);
	//printf("funcname=%s\n", funcname);
	int number=atoi(numvariable);
	fprintf(fout, "(%s)\n",functionname);
	fputs("@0\nD=A\n",fout);
	fputs("@R13\n",fout);
	fputs("M=D\n",fout);
	fprintf(fout, "(ArgsLoop%d)\n",labelindex);
	fprintf(fout, "@%d\n",number);
	fputs("D=A-D\n",fout);
	fprintf(fout,"@CONTINUE%d\n",labelindex);
	fputs("D;JEQ\n", fout);	
	fputs("@0\n", fout);
	fputs("D=A\n", fout);
	fputs("@SP\n", fout);
	fputs("A=M\n", fout);
	fputs("M=D\n", fout);
	fputs("@SP\n", fout);
	fputs("M=M+1\n", fout);
	fputs("@R13\n", fout);
	fputs("MD=M+1\n", fout);
	fprintf(fout,"@ArgsLoop%d\n",labelindex);
	fputs("0;JMP\n", fout);
	fprintf(fout,"(CONTINUE%d)\n",labelindex);
	labelindex++;
}
//return the function
void funcback(FILE *fout)
{
	fputs("@LCL\n",fout);
	fputs("D=M\n",fout);
	//Store the LCL value in R13
	fputs("@R13\n",fout);
	fputs("M=D\n",fout);
	fputs("@5\n",fout);
	fputs("A=D-A\n",fout);
	fputs("D=M\n",fout);
	//Store the return address in R14
	fputs("@R14\n",fout);
	fputs("M=D\n",fout);
	//Reposition the return value for the caller
	fputs("@SP\n",fout);
	fputs("A=M-1\n",fout);
	fputs("D=M\n",fout);
	fputs("@ARG\n",fout);
	fputs("A=M\n",fout);
	fputs("M=D\n",fout);
	//Restore the SP of the caller
	fputs("@ARG\n",fout);
	fputs("D=M+1\n",fout);
	fputs("@SP\n",fout);
	fputs("M=D\n",fout);
	//Restore the appropriate variables of the caller
	fputs("@R13\n",fout);
	fputs("D=M\n",fout);
	fputs("@1\n",fout);
	fputs("A=D-A\n",fout);
	fputs("D=M\n",fout);
	fputs("@THAT\n",fout);
	fputs("M=D\n",fout);
	fputs("@R13\n",fout);
	fputs("D=M\n",fout);
	fputs("@2\n",fout);
	fputs("A=D-A\n",fout);
	fputs("D=M\n",fout);
	fputs("@THIS\n",fout);
	fputs("M=D\n",fout);
	fputs("@R13\n",fout);
	fputs("D=M\n",fout);
	fputs("@3\n",fout);
	fputs("A=D-A\n",fout);
	fputs("D=M\n",fout);
	fputs("@ARG\n",fout);
	fputs("M=D\n",fout);
	fputs("@R13\n",fout);
	fputs("D=M\n",fout);
	fputs("@4\n",fout);
	fputs("A=D-A\n",fout);
	fputs("D=M\n",fout);
	fputs("@LCL\n",fout);
	fputs("M=D\n",fout);
	//Goto the returnaddress int he caller's code
	fputs("@R14\n",fout);
	fputs("A=M\n",fout);
	fputs("0;JMP\n",fout);
}
void pushLATT(FILE *fout)
{
	fputs("D=M\n",fout);
	fputs("@SP\n",fout);
	fputs("A=M\n",fout);
	fputs("M=D\n",fout);
	fputs("@SP\n",fout);
	fputs("M=M+1\n",fout);
}

void callfunction(char *functionname, char*numarg, FILE *fout)
{
	int number=atoi(numarg);
	fprintf(fout,"@returnaddress%d\n",labelindex);
	fputs("D=A\n",fout);
	fputs("@SP\n",fout);
	fputs("A=M\n",fout);
	fputs("M=D\n",fout);
	fputs("@SP\n",fout);
	fputs("M=M+1\n",fout);
	//Push lcl,arg,this,that
	fputs("@LCL\n",fout);
	pushLATT(fout);
	fputs("@ARG\n",fout);
	pushLATT(fout);
	fputs("@THIS\n",fout);
	pushLATT(fout);
	fputs("@THAT\n",fout);
	pushLATT(fout);
	fputs("@SP\n",fout);
	fputs("D=M\n",fout);
	fprintf(fout,"@%d\n",number);
	fputs("D=D-A\n",fout);
	fputs("@5\n",fout);
	fputs("D=D-A\n",fout);
	fputs("@ARG\n",fout);
	fputs("M=D\n",fout);
	fputs("@SP\n",fout);
	fputs("D=M\n",fout);
	fputs("@LCL\n",fout);
	fputs("M=D\n",fout);
	fprintf(fout,"@%s\n" ,functionname);
	fputs("0; JMP\n",fout);	
	fprintf(fout,"(returnaddress%d)\n",labelindex);
	labelindex++;
}

void writeinit(FILE *fout)
{
	char *str="Sys.init";
	fputs("@256\n",fout);
	fputs("D=A\n",fout);
	fputs("@SP\n",fout);
	fputs("M=D\n",fout);
	callfunction(str,"0",fout);
}
//distinguish the pop/push statements by the argu1
void translateP(char *argu0,char *argu1,char *argu2,FILE *fout)
{
	if (!strcmp(argu0,"push"))
	{
		if (!strcmp(argu1,"constant"))
		pushconstant(argu2,fout);
		else if (!strcmp(argu1,"local"))
		pushlocal(argu2,fout);
		else if (!strcmp(argu1,"argument"))
		pushargument(argu2,fout);
		else if (!strcmp(argu1,"this"))
		pushthis(argu2,fout);
		else if (!strcmp(argu1,"that"))
		pushthat(argu2,fout);
		else if (!strcmp(argu1,"pointer"))
		pushpointer(argu2,fout);
		else if (!strcmp(argu1,"temp"))
		pushtemp(argu2,fout);
		else if (!strcmp(argu1,"static"))
		pushstatic(argu2,fout);
	}
	else if (!strcmp(argu0,"pop"))
	{
		if (!strcmp(argu1,"local"))
		poplocal(argu2,fout);
		else if (!strcmp(argu1,"argument"))
		popargument(argu2,fout);
		else if (!strcmp(argu1,"this"))
		popthis(argu2,fout);
		else if (!strcmp(argu1,"that"))
		popthat(argu2,fout);
		else if (!strcmp(argu1,"pointer"))
		poppointer(argu2,fout);
		else if (!strcmp(argu1,"temp"))
		poptemp(argu2,fout);
		else if (!strcmp(argu1,"static"))
		popstatic(argu2,fout);
	}
	else if (!strcmp(argu0,"function"))
	{
		if (funcnum==0)
		{
			writeinit(fout);
		}
		writefunction(argu1,argu2,fout);
		funcnum++;
	}
	else if (!strcmp(argu0,"call"))
	{
		callfunction(argu1,argu2,fout);
	}
}

void translateL(char *argu0, char*argu1, FILE *fout)
{
	if (!strcmp(argu0,"label"))
	{
		//printf("%s\n", funcname);
		fprintf(fout,"(%s$%s)\n",funcname,argu1);
	}
	else if (!strcmp(argu0,"goto"))
	{
		//printf("%s\n", funcname);
		fprintf(fout,"@%s$%s\n",funcname,argu1);
		fputs("0;JMP\n",fout);
	}
	else if (!strcmp(argu0,"if-goto"))
	{
		//printf("%s\n", funcname);
		fputs("@SP\n",fout);
		fputs("AM=M-1\n",fout);
		fputs("D=M\n",fout);
		fprintf(fout,"@%s$%s\n",funcname,argu1);
		fputs("D;JNE\n",fout);
	}
}

//judge the type of the command(Arthimatic/logic or push/pop)
void judgetype(char *command,FILE *fout)
{
	char argu[3][MAX_CHAR_PER_LINE];
	int m=0,k=0;
	memset(argu,'\0',sizeof(argu));
	//delete the whitespace before the first character everyline
	for(int i=0;i<strlen(command);i++)
	{
		if (command[i]!=' ')
		{
			argu[m][k++]=command[i];
		}
		else if (m<=1)
		{
			if (command[i+1]==' '||command[i+1]=='\0')
			{
				continue;
			}
			argu[m][k]='\0';
			m++;
			k=0;
		}
	}
	argu[m][k]='\0';
	// if m=0,it is the arthimatic and logic command or return
	if (m==0)
	translateA(argu[0],fout);
	//label,goto,if-goto
	else if(m==1)
	translateL(argu[0],argu[1],fout);
	// dont think over other command, pop and push must have 3 arguments
	else if (m==2)
	translateP(argu[0],argu[1],argu[2],fout);
}

int parser(char *input, char *output)
{
	static int times=0;
	FILE *fin;
	FILE *fout;
	char line[MAX_CHAR_PER_LINE];
	int i=0,j,k;
	//char type;
	char str[MAX_LINES][MAX_CHAR_PER_LINE];
	memset(str,'\0',sizeof(str));
	//open the two files
	fin=fopen(input,"rt");
	if (times==0)
		fout=fopen(output,"wt");
	else
		fout=fopen(output,"at");
	if(fin==NULL) 
 	{ 
		printf("Error on open input file\n"); 
		exit(1); 
 	}
	printf("Please wait ...\n");

	//just delete the comments and white line(not include all the whitespace)
	//get every command into the line[]
	//then store them into str[][]
	while(fgets(line,MAX_CHAR_PER_LINE,fin)!=NULL)
	{
		if(line[0]==13)
		continue;
		k=0;
		int flag=0;
		for(j=0;line[j]!=13;j++)
		{	
			if (flag==0&&line[j]==' ')
			{
				continue;
			}
			if(line[j]=='/')
			{
				break;
			}
			else
			{
				str[i][k++]=line[j];
				flag=1;
			}
		}
		if(k!=0)
		{
			i++;
		}
	}
	for(int m=0;m<i;m++)
	{
		//printf("funcname=%s\n", funcname);
		//fprintf(stdout, "%s\n", str[m]);
		judgetype(str[m],fout);
	}

	//close the two files
	fclose(fin);
	fclose(fout);
	printf("Process complete\n");
	times++;
	return 0;
}

//main function, get the name of the input file and output file
int main(int argc, char *argv[])
{
	char input[50];
	char asmf[50];
	int j;
	if(argc==1)
	{
		printf("You don't enter the file or dir\n");
		exit(1);
	}
	if(argc==2)
	{
		DIR *dir;
		int size;
		int sized;
		struct  dirent *ptr;
		int i=0;
		//judge the input is a dir or a single file
		if ((dir=opendir(argv[1]))!=NULL)
		{
			printf("Input is a dir\n");
			//if it is a dir , read the file in it one by one
			while((ptr = readdir(dir)) != NULL)
    		{
    			strcpy(input,argv[1]);
    			size=strlen(ptr->d_name);
    			if(strcmp( ( ptr->d_name + (size - 3) ) , ".vm") == 0)
        		{
        			memcpy(filename,ptr->d_name,size-3);
        			filename[size-3]='\0';
        			//get the output filname
        			if (i==0)
        			{
        				strcpy(asmf,argv[1]);
        				sized=strlen(argv[1]);
						asmf[sized-1]='.';
						asmf[sized]='a';
						asmf[sized+1]='s';
						asmf[sized+2]='m';
						asmf[sized+3]='\0';
					}
					strcat(input,ptr->d_name);
					printf("outputfilename=%s\n", asmf);
					parser(input,asmf);
					i++;
    			}
    		}
  		closedir(dir);
		}
		//if it is a single file
		else
		{
			printf("Input is a file\n");
			size=strlen(argv[1]);
			if(strcmp( ( argv[1] + (size - 3) ) , ".vm") == 0)  
			{	
				strcpy(asmf,argv[1]);
				printf("%s\n", asmf);
				j=strlen(argv[1])-2;
				asmf[j]='a';
				asmf[j+1]='s';
				asmf[j+2]='m';
				asmf[j+3]='\0';
				parser(argv[1],asmf);
			}
			else
			printf("The file must be a .vm file\n");
		}
	}
	return 0;
}