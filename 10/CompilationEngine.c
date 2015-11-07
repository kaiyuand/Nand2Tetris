#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#define MAX_LINES 30000
#define MAX_CHAR_PER_LINE 200
#define MAX_WORDS 30000
//tag of the expression
int tag=0;
//the number of tabes before every line
int tabindex=0;
//class='c',classvardec='v',subroutinedec='f',vardec='x',statements='s'
//let='l',if='i',while='w',para='p',return='r',do='d',subroutinebody='b'
//expression='e',expressionlist='t',term='m'
char keywords[MAX_WORDS];
int keyindex=0;
// compile all the keywords
void CompileKeyword(char* word,char* wordnext,FILE* fout)
{
	//keyword 'class' -> class
	if (!strcmp(word,"class"))
	{
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<class>\n");
		tabindex++;
		keywords[keyindex++]='c';	
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<keyword>class</keyword>\n");
	}
	//keyword 'field' & 'static' ->classVarDec
	else if (!strcmp(word,"field")||!strcmp(word,"static"))
	{
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<classVarDec>\n");
		tabindex++;
		keywords[keyindex++]='v';
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<keyword>%s</keyword>\n",word);
	}
	//keyword 'constructor' &'function' & 'method'
	//->subroutineDec
	else if (!strcmp(word,"constructor")||!strcmp(word,"function")||!strcmp(word,"method"))
	{
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<subroutineDec>\n");
		tabindex++;
		keywords[keyindex++]='f';
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<keyword>%s</keyword>\n",word);
	}
	//keyword 'var'->varDec
	else if (!strcmp(word,"var"))
	{
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<varDec>\n");
		tabindex++;
		keywords[keyindex++]='x';
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<keyword>%s</keyword>\n",word);
	}
	//keyword 'let'->letstatement
	else if (!strcmp(word,"let"))
	{
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<letStatement>\n");
		tabindex++;
		keywords[keyindex++]='l';
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<keyword>%s</keyword>\n",word);
	}
	//keyword 'do' ->dostatement
	else if (!strcmp(word,"do"))
	{
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<doStatement>\n");
		tabindex++;
		keywords[keyindex++]='d';
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<keyword>%s</keyword>\n",word);
	}
	//keyword 'if' ->ifstatement
	else if (!strcmp(word,"if"))
	{
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<ifStatement>\n");
		tabindex++;
		keywords[keyindex++]='i';
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<keyword>%s</keyword>\n",word);
	}
	//keyword 'while' ->whilestatement
	else if (!strcmp(word,"while"))
	{
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<whileStatement>\n");
		tabindex++;
		keywords[keyindex++]='w';
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<keyword>%s</keyword>\n",word);
	}
	//keyword 'return' ->returnstatement->expression
	else if (!strcmp(word,"return"))
	{
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<returnStatement>\n");
		tabindex++;
		keywords[keyindex++]='r';
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<keyword>%s</keyword>\n",word);
		if (strcmp(wordnext,";")!=0)
		{
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"<expression>\n");
			tabindex++;
			keywords[keyindex++]='e';
		}
	}
	//keyword 'this' &'true' &'false' & 'null'
	//->(expression)
	else if (!strcmp(word,"this")||!strcmp(word,"true")||!strcmp(word,"false")||!strcmp(word,"null"))
	{
		if (keywords[keyindex-1]=='e')
		{
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"<term>\n");
			tabindex++;
			keywords[keyindex++]='m';
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"<keyword>%s</keyword>\n",word);
		}
		else
		{
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"<keyword>%s</keyword>\n",word);
		}
	}
	else
	{
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<keyword>%s</keyword>\n",word);
	}
}

//compile all the symbols
void Compilesymbol(char* word,char* wordnext,FILE* fout)
{
	//symbol '{'
	//1)in a f=subroutineBody
	if (!strcmp(word,"{"))
	{
		if (keywords[keyindex-1]=='f')
		{
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"<subroutineBody>\n");
			tabindex++;
			keywords[keyindex++]='b';
		}

		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<symbol>%s</symbol>\n",word);

		//2)in subroutineBody and nextword is the part of statements
		//->statements 
		if (keywords[keyindex-1]=='b'&&strcmp(wordnext,"var")!=0)
		{
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"<statements>\n");
			tabindex++;
			keywords[keyindex++]='s';
		}
		//3)in if and while statement -> statements
		if (keywords[keyindex-1]=='i'||keywords[keyindex-1]=='w')
		{
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"<statements>\n");
			tabindex++;
			keywords[keyindex++]='s';
		}
	}
	////symbol '}'
	else if (!strcmp(word,"}"))
	{
		//end statements ->/statements
		if (keywords[keyindex-1]=='s')
		{
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</statements>\n");
		}
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<symbol>%s</symbol>\n",word);
		//end of if statements ->/ifstatements
		if (keywords[keyindex-1]=='i')
		{
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</ifStatement>\n");
		}
		// end of the while statements ->/whilestatements
		if (keywords[keyindex-1]=='w')
		{
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</whileStatement>\n");
		}
		//end the class ->/class
		if (keywords[keyindex-1]=='c')
		{
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</class>\n");
		}
		//end the subroutinebBody ->/subroutinebody
		if (keywords[keyindex-1]=='b')
		{
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</subroutineBody>\n");
		}
		//end the subroutineDec ->/subroutineDec
		if (keywords[keyindex-1]=='f')
		{
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</subroutineDec>\n");
		}
	}
	//symbol ';'
	else if (!strcmp(word,";"))
	{
		//end a term
		if (keywords[keyindex-1]=='m')
		{
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</term>\n");
		}
		//end a expression
		if (keywords[keyindex-1]=='e')
		{
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</expression>\n");
		}
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<symbol>%s</symbol>\n",word);
		//end the classvardec
		if (keywords[keyindex-1]=='v')
		{
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</classVarDec>\n");
		}
		//end the returnstatement
		if (keywords[keyindex-1]=='r')
		{
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</returnStatement>\n");
		}
		//end the vardec
		if (keywords[keyindex-1]=='x')
		{
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</varDec>\n");
			if (strcmp(wordnext,"var")!=0)
			{
				for (int i = 0; i < tabindex; ++i)
				{
					fprintf(fout,"  ");
				}
				fprintf(fout,"<statements>\n");
				tabindex++;
				keywords[keyindex++]='s';
			}
		}
		//end the do statement
		if (keywords[keyindex-1]=='d')
		{
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</doStatement>\n");
		}
		//end the let statment
		if (keywords[keyindex-1]=='l')
		{
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</letStatement>\n");
		}
	}
	//symbol '('
	else if (!strcmp(word,"("))
	{
		//begin a term in an expression
		if (keywords[keyindex-1]=='e')
		{
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"<term>\n");
			tabindex++;
			keywords[keyindex++]='m';
		}
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<symbol>%s</symbol>\n",word);
		//begin a parameterlist in a function
		if (keywords[keyindex-1]=='f')
		{
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"<parameterList>\n");
			tabindex++;
			keywords[keyindex++]='p';
		}
		//if in a expression that need a expressionlist
		if (keywords[keyindex-1]=='d'||tag==1)
		{
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"<expressionList>\n");
			tabindex++;
			keywords[keyindex++]='t';
			if (strcmp(wordnext,")")!=0)
			{
				for (int i = 0; i < tabindex; ++i)
				{
					fprintf(fout,"  ");
				}
				fprintf(fout,"<expression>\n");
				tabindex++;
				keywords[keyindex++]='e';
			}
		}
		//begin a expression in if,while,term
		if (keywords[keyindex-1]=='i'||keywords[keyindex-1]=='w'||keywords[keyindex-1]=='m')
		{
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"<expression>\n");
			tabindex++;
			keywords[keyindex++]='e';
		}
		//begin a term in expression
		if (keywords[keyindex-1]=='e')
		{
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"<term>\n");
			tabindex++;
			keywords[keyindex++]='m';
		}
	}
	//symbol ')'
	else if (!strcmp(word,")"))
	{
		//end terms
		while (keywords[keyindex-1]=='m')
		{
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</term>\n");
		}
		if (keywords[keyindex-1]=='m')
		{
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</term>\n");
		}
		//end a expression
		if (keywords[keyindex-1]=='e')
		{
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</expression>\n");
		}
		//end a expressionlist
		if (keywords[keyindex-1]=='t')
		{
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</expressionList>\n");
		}
		//end a parameterlist
		if (keywords[keyindex-1]=='p')
		{
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</parameterList>\n");
		}
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<symbol>%s</symbol>\n",word);
		
		//end a term
		if (keywords[keyindex-1]=='m')
		{
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</term>\n");
		}
		/*
		if (keywords[keyindex-1]=='e')
		{
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</expression>\n");
		}
		*/
		//end a expressionlist
		if (keywords[keyindex-1]=='t')
		{
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</expressionList>\n");
		}
		//end a parameterlist
		if (keywords[keyindex-1]=='p')
		{
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</parameterList>\n");
		}
	}
	//symbol '['
	//begin a expression
	else if (!strcmp(word,"["))
	{
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<symbol>%s</symbol>\n",word);
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<expression>\n");
		tabindex++;
		keywords[keyindex++]='e';
	}
	//symbol ']'
	//end a expression
	else if (!strcmp(word,"]"))
	{
		if (keywords[keyindex-1]=='m')
		{
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</term>\n");
		}
		tabindex--;
		keyindex--;
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"</expression>\n");
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<symbol>%s</symbol>\n",word);
	}
	//symbol '='
	else if (!strcmp(word,"="))
	{
		//end a term
		if (keywords[keyindex-1]=='m')
		{	
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</term>\n");
		}
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<symbol>%s</symbol>\n",word);
		// begin a expression in a let statement
		if (keywords[keyindex-1]=='l')
		{
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"<expression>\n");
			tabindex++;
			keywords[keyindex++]='e';
		}
	}
	//nary symbols end a term
	else if (!strcmp(word,"+")||!strcmp(word,"-")||!strcmp(word,"*")||!strcmp(word,"/")||!strcmp(word,"&amp;")||!strcmp(word,"&lt;")||!strcmp(word,"&gt;"))
	{
		if (keywords[keyindex-1]=='m')
		{	
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</term>\n");
		}
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<symbol>%s</symbol>\n",word);
	}
	//unary symbol '~' begin a term
	else if (!strcmp(word,"~"))
	{
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<symbol>%s</symbol>\n",word);
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<term>\n");
		tabindex++;
		keywords[keyindex++]='m';
	}
	//symbol ','
	else if (!strcmp(word,","))
	{
		//end a term
		if (keywords[keyindex-1]=='m')
		{	
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</term>\n");
		}
		//end a expression
		if (keywords[keyindex-1]=='e')
		{	
			tabindex--;
			keyindex--;
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"</expression>\n");
		}
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<symbol>%s</symbol>\n",word);
		//begin a expression in list
		if (keywords[keyindex-1]=='t')
		{
			for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"<expression>\n");
			tabindex++;
			keywords[keyindex++]='e';
		}
	}
	else
	{
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<symbol>%s</symbol>\n",word);
	}
}

//compile the constant
//include integer, string, and identifier
//if in expression ->term
void Compileconstant(char* word,char* type,FILE* fout)
{
	if (keywords[keyindex-1]=='e')
	{
		for (int i = 0; i < tabindex; ++i)
			{
				fprintf(fout,"  ");
			}
			fprintf(fout,"<term>\n");
			tabindex++;
			keywords[keyindex++]='m';
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<%s>%s</%s>\n",type,word,type);
	}
	else
	{
		for (int i = 0; i < tabindex; ++i)
		{
			fprintf(fout,"  ");
		}
		fprintf(fout,"<%s>%s</%s>\n",type,word,type);
	}
}


//judge the input char whether is a legal char in jack
int judgesymbol(char c)
{
	if (c=='.'||c==';'||c==','||c=='~')
	{
		return 1;
	}
	if (c=='/'||c=='+'||c=='-'||c=='*'||c=='&'||c=='|'||c=='>'||c=='<'||c=='=')
	{
		return 1;
	}
	if (c=='['||c==']'||c=='('||c==')'||c=='{'||c=='}')
	{
		return 1;
	}
	return 0;
}

//judge whether it is a legal keyword in jack
int judgekeyword(char* word)
{
	if (!strcmp(word,"class"))
	return 1;
	else if (!strcmp(word,"constructor"))
	return 1;
	else if (!strcmp(word,"function"))
	return 1;
	else if (!strcmp(word,"method"))
	return 1;
	else if (!strcmp(word,"field"))
	return 1;
	else if (!strcmp(word,"static"))
	return 1;
	else if (!strcmp(word,"var"))
	return 1;
	else if (!strcmp(word,"int"))
	return 1;
	else if (!strcmp(word,"char"))
	return 1;
	else if (!strcmp(word,"boolean"))
	return 1;
	else if (!strcmp(word,"void"))
	return 1;
	else if (!strcmp(word,"true"))
	return 1;
	else if (!strcmp(word,"false"))
	return 1;
	else if (!strcmp(word,"null"))
	return 1;
	else if (!strcmp(word,"this"))
	return 1;
	else if (!strcmp(word,"let"))
	return 1;
	else if (!strcmp(word,"do"))
	return 1;
	else if (!strcmp(word,"if"))
	return 1;
	else if (!strcmp(word,"else"))
	return 1;
	else if (!strcmp(word,"while"))
	return 1;
	else if (!strcmp(word,"return"))
	return 1;
	else
		return 0;
}

//judge the input whether is a integer in jack
int judgeinteger(char* word)
{
	for (int i = 0; i < strlen(word); ++i)
	{
		if(!isdigit(word[i]))
			return 0;
	}
	return 1;
}

//judge the type of the input argument
//only one word?or str
void judgetype(char* word,char* type)
{
	int size=strlen(word);
	if (size==1)
	{
		int c=word[0];
		if(judgesymbol(c))
		{
			strcpy(type,"symbol");
			if (c=='<')
			{
				strcpy(word,"&lt;");
			}
			if (c=='>')
			{
				strcpy(word,"&gt;");
			}
			if (c=='&')
			{
				strcpy(word,"&amp;");
			}
		}
		else if (c>='0'&&c<='9')
			strcpy(type,"integerConstant");
		else
			strcpy(type,"identifier");
	}
	else if (judgekeyword(word))
	{
		strcpy(type,"keyword");
	}
	else if (judgeinteger(word))
	{
		strcpy(type,"integerConstant");
	}
	else
		strcpy(type,"identifier");
}

int parser(char *input, char *output)
{
	FILE *fin;
	FILE *fout;
	char c;
	char getword[MAX_CHAR_PER_LINE];
	char word[MAX_WORDS][MAX_CHAR_PER_LINE];
	int i=0,j,k=0,size;
	char type[MAX_WORDS][20];
	char line[MAX_CHAR_PER_LINE];
	//open the two files
	fin=fopen(input,"rt");
	fout=fopen(output,"wt");
	if(fin==NULL) 
 	{ 
		printf("Error on open input file\n"); 
		exit(1); 
 	}
	printf("Please wait ...\n");

	//fprintf(fout,"<tokens>\n");
	memset(type,'\0',sizeof(type));
	memset(word,'\0',sizeof(word));
	while(!feof(fin))
	{
		memset(getword,'\0',sizeof(getword));
		fscanf(fin,"%s",getword);
		size=strlen(getword);
		//delete the comments
		//begin with '//'
		if (getword[0]=='/'&&getword[1]=='/')
		{
			fgets(line,MAX_CHAR_PER_LINE,fin);
			continue;
		}
		//comments begin with '/*'
		else if (getword[0]=='/'&&getword[1]=='*')
		{
			while(1)
			{
				c=fgetc(fin);
				if (c=='*')
				{
					c=fgetc(fin);
					if (c=='/')
					{
						break;
					}
				}
			}
			continue;
		}
		//get the string between ""
		else if (getword[0]=='"')
		{
			strcpy(type[k],"stringConstant");
			for (i = 0; i < size-1; i++)
			{
				getword[i]=getword[i+1];
			}
			for (i=size-1;(c=fgetc(fin))!='"';i++)
			{
				getword[i]=c;
			}
			getword[i]='\0';
		}
		//divide the char and command that together
		else
		{
			for (i = 0; i < size; ++i)
			{
				if(judgesymbol(getword[i])&&size!=1)
				{
					for (j = size; j>i; j--)
					{
						ungetc(getword[j],fin);
					}
					ungetc(' ',fin);
					ungetc(getword[i],fin);
					getword[i]='\0';
					break;
				}
			}
		}
		if (strlen(getword)==0)
		continue;
		strcpy(word[k],getword);
		if (strlen(type[k])==0)
		judgetype(word[k],type[k]);
		k++;
	}



	//compile 
	for(i=0;i<k;i++)
	{
		tag=0;
		//printf("word=%s,type=%s\n",word[i],type[i]);
		if (i>=3)
		{
			if ((!strcmp(word[i],"("))&&(!strcmp(type[i-3],"identifier"))&&(!strcmp(type[i-1],"identifier"))&&(!strcmp(word[i-2],".")))
			{
				tag=1;
			}
		}
		if (!strcmp(type[i],"keyword"))
			CompileKeyword(word[i],word[i+1],fout);
		else if (!strcmp(type[i],"symbol"))
			Compilesymbol(word[i],word[i+1],fout);
		else
			Compileconstant(word[i],type[i],fout);
	}
	//fprintf(fout,"</tokens>\n");

	//close the two files
	fclose(fin);
	fclose(fout);
	printf("Process complete\n");
	return 0;
}

//main function, get the name of the input file and output file
int main(int argc, char *argv[])
{
	char input[200];
	char xmlf[200];
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
		//judge the input is a dir or a single file
		if ((dir=opendir(argv[1]))!=NULL)
		{
			printf("Input is a dir\n");
			//if it is a dir , read the file in it one by one
			while((ptr = readdir(dir)) != NULL)
    		{
    			strcpy(input,argv[1]);
    			strcpy(xmlf,argv[1]);
    			size=strlen(ptr->d_name);
    			if(strcmp( ( ptr->d_name + (size - 5) ) , ".jack") == 0)
        		{
					strcat(input,ptr->d_name);
					strcat(xmlf,ptr->d_name);
					sized=strlen(xmlf);
					xmlf[sized-4]='x';
					xmlf[sized-3]='m';
					xmlf[sized-2]='l';
					xmlf[sized-1]='\0';
					//xmlf[sized]='\0';
					printf("outputfilename=%s\n", xmlf);
					parser(input,xmlf);
    			}
    		}
  		closedir(dir);
		}
		//if it is a single file
		else
		{
			printf("Input is a file\n");
			size=strlen(argv[1]);
			if(strcmp( ( argv[1] + (size - 5) ) , ".jack") == 0)  
			{	
				strcpy(xmlf,argv[1]);
				printf("%s\n", xmlf);
				j=strlen(argv[1])-5;
				//xmlf[j]='T';
				//xmlf[j+1]='.';
				xmlf[j+1]='x';
				xmlf[j+2]='m';
				xmlf[j+3]='l';
				xmlf[j+4]='\0';
				parser(argv[1],xmlf);
			}
			else
			printf("The file must be a .jack file\n");
		}
	}
	return 0;
}
