#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#define MAX_LINES 30000
#define MAX_CHAR_PER_LINE 200


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
	char word[MAX_CHAR_PER_LINE];
	int i=0,j,size;
	char type[20];
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
	
	//printf the begin of the token
	fprintf(fout,"<tokens>\n");

	while(!feof(fin))
	{
		memset(getword,'\0',sizeof(getword));
		memset(type,'\0',sizeof(type));
		memset(word,'\0',sizeof(word));
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
			strcpy(type,"stringConstant");
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
		strcpy(word,getword);
		if (strlen(type)==0)
		judgetype(word,type);
		//fprintf(stdout,"<%s>%s</%s>\n",type,word,type);
		fprintf(fout,"<%s>%s</%s>\n",type,word,type);
	}

	fprintf(fout,"</tokens>\n");

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
					xmlf[sized-5]='T';
					xmlf[sized-4]='.';
					xmlf[sized-3]='x';
					xmlf[sized-2]='m';
					xmlf[sized-1]='l';
					xmlf[sized]='\0';
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
				xmlf[j]='T';
				xmlf[j+1]='.';
				xmlf[j+2]='x';
				xmlf[j+3]='m';
				xmlf[j+4]='l';
				xmlf[j+5]='\0';
				parser(argv[1],xmlf);
			}
			else
			printf("The file must be a .jack file\n");
		}
	}
	return 0;
}
