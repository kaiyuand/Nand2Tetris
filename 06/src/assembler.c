#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//Max lines of the ".asm" file(except for the comments)
#define MAX_LINES 30000
//Max characters every command
#define MAX_CHAR_PER_LINE 100
//Struct of sysmboltable
struct symboltable
	{
		char name[50];
		int address;
	}symbols[1000]=
	{
//Predefined symbols
		{"SP",0},
		{"LCL",1},
		{"ARG",2},
		{"THIS",3},
		{"THAT",4},
		{"R0",0},
		{"R1",1},
		{"R2",2},
		{"R3",3},
		{"R4",4},
		{"R5",5},
		{"R6",6},
		{"R7",7},
		{"R8",8},
		{"R9",9},
		{"R10",10},
		{"R11",11},
		{"R12",12},
		{"R13",13},
		{"R14",14},
		{"R15",15},
		{"SCREEN",16384},
		{"KBD",24576}
	};
//The begining number of the new symbol
int symbolsid=23;
//The begining address of the new symbol
int symbolsrom=16;

//The function of judge the command type
char commandtype(char *command)
{
	if(command[0]=='@'&&isdigit(command[1]))
	return 'a';
	else if(command[0]=='@')
	return 'l';
	else if(command[0]=='(')
	return 's';
	else
	return 'c';
}

//The function to decode the A-command to 16bits binary
void decodeA(char str1[],char code[])
{
	int number;
	int i=0;
	char number2[17]="0000000000000000";
	char *str2=(char*)malloc(strlen(str1)*sizeof(char));
	for(i=0;i<(strlen(str1)-1);i++)
	{
		str2[i]=str1[i+1];
	}
	str2[i]='\0';
	number=atoi(str2);
	for(i=15;number>0;i--)
	{
		if((number%2)==1)
		number2[i]='1';
		number=number/2;
	}
	free(str2);
	strcpy(code,number2);
}
//The funtion to decode the 3 dest bits of C-command
void decodedest(char dest[],char destnumber[])
{
	if(strcmp(dest,"M")==0)
	strcpy(destnumber,"001");
	else if(strcmp(dest,"D")==0)
	strcpy(destnumber,"010");
        else if(strcmp(dest,"MD")==0||strcmp(dest,"DM")==0)
        strcpy(destnumber,"011");
        else if(strcmp(dest,"A")==0)
        strcpy(destnumber,"100");
        else if(strcmp(dest,"AM")==0||strcmp(dest,"MA")==0)
        strcpy(destnumber,"101");
        else if(strcmp(dest,"AD")==0||strcmp(dest,"DA")==0)
        strcpy(destnumber,"110");
        else
        strcpy(destnumber,"111");
}
//The funtion to decode the 3 jump bits of C-command
void decodejump(char jump[],char jumpnumber[])
{
	if(strcmp(jump,"JGT")==0)
        strcpy(jumpnumber,"001");
        else if(strcmp(jump,"JEQ")==0)
        strcpy(jumpnumber,"010");
        else if(strcmp(jump,"JGE")==0)
        strcpy(jumpnumber,"011");
        else if(strcmp(jump,"JLT")==0)
        strcpy(jumpnumber,"100");
        else if(strcmp(jump,"JNE")==0)
        strcpy(jumpnumber,"101");
        else if(strcmp(jump,"JLE")==0)
        strcpy(jumpnumber,"110");
        else
        strcpy(jumpnumber,"111");
}
//The funtion to decode the 7 comp bits of C-command
void decodecomp(char comp[],char compnumber[])
{
	if(strcmp(comp,"0")==0)
	strcpy(compnumber,"0101010");
        if(strcmp(comp,"1")==0)
        strcpy(compnumber,"0111111");
        if(strcmp(comp,"-1")==0)
        strcpy(compnumber,"0111010");
        if(strcmp(comp,"D")==0)
        strcpy(compnumber,"0001100");
        if(strcmp(comp,"A")==0)
        strcpy(compnumber,"0110000");
        if(strcmp(comp,"!D")==0)
        strcpy(compnumber,"0001101");
        if(strcmp(comp,"!A")==0)
        strcpy(compnumber,"0110001");
        if(strcmp(comp,"-D")==0)
        strcpy(compnumber,"0001111");
        if(strcmp(comp,"-A")==0)
        strcpy(compnumber,"0110011");
        if(strcmp(comp,"D+1")==0)
        strcpy(compnumber,"0011111");
        if(strcmp(comp,"A+1")==0)
        strcpy(compnumber,"0110111");
        if(strcmp(comp,"D-1")==0)
        strcpy(compnumber,"0001110");
        if(strcmp(comp,"A-1")==0)
        strcpy(compnumber,"0110010");
        if(strcmp(comp,"D+A")==0||strcmp(comp,"A+D")==0)
        strcpy(compnumber,"0000010");
        if(strcmp(comp,"D-A")==0)
        strcpy(compnumber,"0010011");
        if(strcmp(comp,"A-D")==0)
        strcpy(compnumber,"0000111");
        if(strcmp(comp,"D&A")==0||strcmp(comp,"A&D")==0)
        strcpy(compnumber,"0000000");
        if(strcmp(comp,"D|A")==0||strcmp(comp,"A|D")==0)
        strcpy(compnumber,"0010101");
        if(strcmp(comp,"M")==0)
        strcpy(compnumber,"1110000");
        if(strcmp(comp,"!M")==0)
        strcpy(compnumber,"1110001");
        if(strcmp(comp,"-M")==0)
        strcpy(compnumber,"1110011");
        if(strcmp(comp,"M+1")==0)
        strcpy(compnumber,"1110111");
        if(strcmp(comp,"M-1")==0)
        strcpy(compnumber,"1110010");
        if(strcmp(comp,"D+M")==0||strcmp(comp,"M+D")==0)
        strcpy(compnumber,"1000010");
        if(strcmp(comp,"D-M")==0)
        strcpy(compnumber,"1010011");
        if(strcmp(comp,"M-D")==0)
        strcpy(compnumber,"1000111");
        if(strcmp(comp,"D&M")==0||strcmp(comp,"M&D")==0)
        strcpy(compnumber,"1000000");
        if(strcmp(comp,"D|M")==0||strcmp(comp,"M|D")==0)
        strcpy(compnumber,"1010101");
}

/*The function to decode the C-command to 16bits binary.
Seperate the C-command into 3 parts(dest,comp,jump), 
and then decode them respectively*/
void decodeC(char str1[],char code[])
{
	int i,j,k,flagd=0,flagj=strlen(str1);
	char dest[4];
	char destnumber[4]="000";
	char comp[8];
	char compnumber[8];
	char jump[4];
	char jumpnumber[4]="000";
	memset(dest,'\0',sizeof(dest));
	memset(comp,'\0',sizeof(comp));
	memset(jump,'\0',sizeof(jump));
	for(i=0;i<strlen(str1);i++)
	{
		if(str1[i]=='=')
		flagd=i+1;
		if(str1[i]==';')
		flagj=i;
	}
	if(flagd!=0)
	{
		for(j=0;j<flagd-1;j++)
		{
			dest[j]=str1[j];
		}
		decodedest(dest,destnumber);
	}
	if(flagj!=strlen(str1))
	{
		k=0;
		for(j=flagj+1;j<strlen(str1);j++)
		{
			jump[k++]=str1[j];
		}
		decodejump(jump,jumpnumber);
	}
	k=0;
	for(i=flagd;i<flagj;i++)
	{
		comp[k++]=str1[i];
	}
	decodecomp(comp,compnumber);
	strcat(code,"111");
	strcat(code,compnumber);
	strcat(code,destnumber);
	strcat(code,jumpnumber);
}
//The function of adding the symbol into symboltable
void addsymbol(char var[], int address)
{
	strcpy(symbols[symbolsid].name,var);
	symbols[symbolsid].address=address;
	symbolsid++;
}
//First pass of the str[], change the whole L-command to the symbols' names
//then call the addsymbol function
void firstpass(char command[], int address)
{
	int i;
	char *symbol=(char*)malloc(strlen(command)*sizeof(char));
	for(i=0;i<(strlen(command)-2);i++)
	{
		symbol[i]=command[i+1];
	}
	symbol[i]='\0';
	addsymbol(symbol,address);
	free(symbol);
}
//The function of decoding the L-command, 
//if the symbol is already in the symboltable, get the address of it;
//if not, adds it into the symboltable
void decodeL(char str1[],char code[])
{
	int i;
	int address=-1;
	char number2[17]="0000000000000000";
	char *str2=(char*)malloc(strlen(str1)*sizeof(char));
	for(i=0;i<(strlen(str1)-1);i++)
	{
		str2[i]=str1[i+1];
	}
	str2[i]='\0';
		for(i=0;i<symbolsid;i++)
	{
		if (strcmp(str2,symbols[i].name)==0)
		{
			address=symbols[i].address;
		}
	}
	if(address==-1)
	{
		address=symbolsrom;
		addsymbol(str2,symbolsrom++);
	}
	for(i=15;address>0;i--)
	{
		if((address%2)==1)
		number2[i]='1';
		address=address/2;
	}
	free(str2);
	strcpy(code,number2);
}

//The core function of the assembler
int parser(char *input, char *output)
{
	FILE *fin;
	char line[MAX_CHAR_PER_LINE];
	int i=0,j,k;
	char type;
	char str[MAX_LINES][MAX_CHAR_PER_LINE];
	memset(str,'\0',sizeof(str));
	fin=fopen(input,"rt");
	if(fin==NULL) 
 	{ 
		printf("Error on open input file\n"); 
		exit(1); 
 	}
	printf("Please wait ...\n");
//Delete the whitespace and comments first,
// and then store the command into the array str[].
	while(fgets(line,MAX_CHAR_PER_LINE,fin)!=NULL)
	{
		if(line[0]==13)
		continue;
		k=0;
		for(j=0;line[j]!=13;j++)
		{	
			if(line[j]==' '||line[j]=='\t')
			{
				continue;
			}
			else if(line[j]=='/')
			{
				break;
			}
			else
			str[i][k++]=line[j];
		}
		if(k!=0)
		{
			i++;
		}
	}
	fclose(fin);
	// First pass of the str[], adding the symbol into the symboltable
	int rom=0;
	for(int m=0;m<i;m++)
	{
		type=commandtype(str[m]);
		if(type=='s')
		{
			firstpass(str[m],rom);
		}
		else
		rom++;
	}
	//Second pass of the str[], 
	// then decoding the A,C,L commands into 16-bits binary
	// finally printf the 16 bits into the ".hack" file
	FILE *fout;
	fout=fopen(output,"wt");
	char code[i][17];
	memset(code,'\0',sizeof(code));
	for(int m=0;m<i;m++)
	{
		type=commandtype(str[m]);
		if(type=='a')
		decodeA(str[m],code[m]);
		if(type=='c')
		decodeC(str[m],code[m]);
		if (type=='l')
		decodeL(str[m],code[m]);
		if(type=='a'||type=='c'||type=='l')
	 	fprintf(fout, "%s\n", code[m]);
	}
	fclose(fout);
	printf("Process complete\n");
	return 0;
}


//The main function to read the file name and get the output file name
//Then call the function parser
int main(int argc, char *argv[])
{
	char hack[50];
	int j;
	if(argc==1)
	{
		printf("You don't enter the file.asm\n");
		exit(1);
	}
	if(argc==2)
	{
		strcpy(hack,argv[1]);
		j=strlen(argv[1])-3;
		hack[j]='h';
		hack[j+1]='a';
		hack[j+2]='c';
		hack[j+3]='k';
		hack[j+4]='\0';
		parser(argv[1],hack);
	}
	return 0;
}
