#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int count=0;              /*分解的产生式的个数*/
int number;               /*所有终结符和非终结符的总数*/
char start;               /*开始符号*/
char termin[50];          /*终结符号*/
char non_ter[50];         /*非终结符号*/
char v[50];               /*所有符号*/
char str[50][50];		  /*读取的所有字符*/ 
char left[50];            /*左部*/
char right[50][50];       /*右部*/
char first[50][50],follow[50][50];       /*各产生式右部的FIRST和左部的FOLLOW集合*/
char first1[50][50];      /*所有单个符号的FIRST集合*/
char select[50][50];      /*各单个产生式的SELECT集合*/
char f[50],F[50];         /*记录各符号的FIRST和FOLLOW是否已求过*/
char empty[20];           /*记录可直接推出e的符号*/
char TEMP[50];            /*求FOLLOW时存放某一符号串的FIRST集合*/
int validity=1;           /*表示输入文法是否有效*/
int ll=1;                 /*表示输入文法是否为LL(1)文法*/
int M[20][20];            /*分析表*/
char choose;              /*用户输入时使用*/
char empt[20];            /*求_emp()时使用*/
char fo[20];              /*求FOLLOW集合时使用*/

typedef struct node{ 
    char data;		
    struct node *next; 
}NODE; 
typedef struct snode{ 
	int sum;
    NODE *top; 
    NODE *bottom; 
}SNODE; 
void *InitStack(SNODE *S)
{
	NODE *p=(NODE*)malloc(sizeof(NODE)); 	
	p->data='#';
	S->top=p; 
    S->top->next=NULL; 
    S->bottom=S->top; 
	S->sum=0;
    printf( "The Stack's Init is complete!\n "); 
    return 0; 
} 
void *StackPush(SNODE *S,char symbol)
{ 
	NODE *p;
	p=(NODE*)malloc(sizeof(NODE));  
    p->data=symbol;
    p->next=S->top->next; 
	S->top->next=p;
    S->top=p; 
	S->sum++;
    //printf( "%2c",symbol); 
    return 0; 
} 
int StackPop(SNODE *S,NODE *t)
{ 
	NODE *p; 
	int i=0;
	if(S->sum==-1)  return 1; 
	p=S->bottom;
	//printf("S->sum=%d",S->sum);
	while(S->sum!=0 && i!=(S->sum-1))
	{	i++;
		p=p->next;
	} 
	t->data=S->top->data;
	free(S->top);
	S->top=p; 
	p->next=NULL;
	S->sum--; 
	return 0; 
}  
void *PrintStack(SNODE *S)
{ 
    NODE *p;
	int i;
	i=S->sum;
    p=S->bottom; 
    while(i!=-1)
	{ 
        printf("%c",p->data);
        p=p->next;
		i--;	
	} 
    return 0; 
} 
//删除重复字符
void deleteRepeatChar(char *s){
    int i,j,n;
    int length=strlen(s);
    for(i=0;i<length;i++){//将重复的字母标记为0
        for(j=i+1;j<length;j++){
            if(s[i]==0)
                continue;
            if(s[i]==s[j])
                s[j]=0;
        }
    }
    for(i=0,n=0;i<length;i++){
        if(s[i]!=0)
            s[n++]=s[i];
    }
    s[n]='\0';
}
//读入一个文法
void grammer(FILE *inf){
    int i=0,j=0;
    while(!feof(inf)){
        char c=fgetc(inf);
        if(c>='A' && c<='Z'){
            non_ter[i]=c;
            i++;
        }
        else if(c=='-' || c=='>' || c==' ' || c=='\t'){}
        else if(c=='\n' || c=='\377') count++;
        else if(c!='e'){
            termin[j]=c;
            j++;
        }
    }
    non_ter[i]='\0';
    termin[j]='\0';
    deleteRepeatChar(non_ter);
    deleteRepeatChar(termin);
    start=non_ter[0];
    number=strlen(non_ter)+strlen(termin);
    strcpy(v,non_ter);
    strcat(v,termin);
    //printf("非终结符为%s\n",non_ter);
    //printf("终结符为%s\n",termin);
    //printf("开始符号为%c\n",start);
    //printf("符号为%s\n",v);
    //printf("产生式的总数为%d\n",count);
    //printf("终+非终总数为%d\n",number);
}
//将左部存储在left中，右部存储在right中 
void lr(FILE *inf){
	int i=0,j=0,k,m=0,n=0;
	while(!feof(inf)){
		char c=fgetc(inf);
		if(c!='\n' && c!='\377'){
			str[i][j]=c;
			j++;
		}
		else{
			str[i][j]='\0';
			i++;
			j=0;
		}
	}
	for(j=0;j<=i;j++){
		for(k=0;str[j][k]!='\0';k++){
			if(str[j][k]=='-' && str[j][k+1]=='>'){
				left[m]=str[j][k-1];
				m++;
			}
		}
	}
	for(j=0;j<=i;j++){
		m=0;
		for(k=0;str[j][k]!='\0';k++){
			if(str[j][k]=='-' && str[j][k+1]=='>'){
				k=k+2;
				while(str[j][k]!='\0'){
					right[j][m]=str[j][k];
					m++;
					k++;
				}
				str[j][m]='\0';	
			}
		}
	}
	//printf("left为%s\n",left);
} 
//判断一个字符是否在指定字符串中  
int in(char c,char *p)   
{   
 	int i;   
 	if(strlen(p)==0)   
  		return 0;   
 	for(i=0;;i++)   
 	{    
  		if(p[i]==c)   
   			return 1;       /*若在，返回1*/   
  		if(i==strlen(p))   
      		return 0;       /*若不在，返回0*/   
 	}   
}
//将单个符号或符号串并入另一符号串,e为空 
//d是目标符号串，s是源串，type＝1，源串中的'e'一并并入目串；  
//type＝2，源串中的'e'不并入目标串
void merge(char *d,char *s,int type){
	int i,j;   
 	for(i=0;i<=strlen(s)-1;i++)   
 	{   
        if(type==2 && s[i]=='e')   
   			break;   
  		else   
  		{   
   			for(j=0;;j++)   
   			{   
       			if(j<strlen(d) && s[i]==d[j]) 
       				break;  
                if(j==strlen(d))   
    			{   
        			d[j]=s[i];   
        			d[j+1]='\0';   
        			break;   
    			}   
   			}   
  		}   
 	}   
}
//求所有能直接推出空的符号，用e表示空  
void emp(char c)   
{                      
 	char temp[10];   
 	int i;   
 	for(i=0;i<=count-1;i++)   
 	{   
  		if(right[i][0]==c && strlen(right[i])==1)   
  		{   
  			temp[0]=left[i];   
   			temp[1]='\0';   
   			merge(empty,temp,1);   
    		emp(left[i]);   
  		}   
 	} 
}
//求某一符号能否推出空 
int _emp(char c)
{                  /*若能推出，返回1；否则，返回0*/
	int i,j,k,result=1,mark=0;
	char temp[20];
	temp[0]=c;
	temp[1]='\0';
	merge(empt,temp,1);
	if(in(c,empty)==1)
		return 1;
	for(i=0;;i++)
	{
		if(i==count)
            return 0;
		if(left[i]==c)//找一个左部为c的产生式
		{
            j=strlen(right[i]);//j为右部的长度
			if(j==1 && in(right[i][0],empty)==1)
			    return 1;
			else if(j==1 && in(right[i][0],termin)==1)
				return 0;
			else 
			{
                for(k=0;k<=j-1;k++)
                    if(in(right[i][k],empt)==1)
						mark=1;
				if(mark==1)
					continue;
				else
                {
					for(k=0;k<=j-1;k++)
					{
						result*=_emp(right[i][k]);
						temp[0]=right[i][k];
						temp[1]='\0';
						merge(empt,temp,1);
					}
				}
			}
		    if(result==0 && i<count)
			    continue;
		    else if(result==1 && i<count)
			    return 1;
		}
	}
}
//求单个符号的FIRST
void first2(int i)//i为符号在所有输入符号中的序号
{                 
    char c,temp[20];
	int j,k,m;
	c=v[i];
	char ch='e';
	emp(ch);
	if(in(c,termin)==1)//若为终结符
    {
        first1[i][0]=c;
	    first1[i][1]='\0';
	}   
	else if(in(c,non_ter)==1)//若为非终结符
	{
		for(j=0;j<=count-1;j++)
		{
            if(left[j]==c)//找一个左部为c的产生式
			{
                if(in(right[j][0],termin)==1 || right[j][0]=='e')//右部第一个符号为终结符或空 
				{
                    temp[0]=right[j][0];
				    temp[1]='\0';
					merge(first1[i],temp,1);
				}
		      	else if(in(right[j][0],non_ter)==1)//右部第一个符号为非终结符
				{
					for(k=0;;k++)
					{
					    if(v[k]==right[j][0])
							break;
					}//找到右部第一个符号在v中的位置 
					if(f[k]=='0')//判断此非终结符是否求了first集
					{   
						first2(k);//求此非终结符的first集
					    f[k]='1';
					}
					merge(first1[i],first1[k],2); 
                    for(k=0;k<=strlen(right[j])-1;k++)
					{
						empt[0]='\0';
						if(_emp(right[j][k])==1 && k<strlen(right[j])-1)//从左往右判断每个能否推出空 
						{
                            for(m=0;;m++){
                            	if(v[m]==right[j][k+1])
									break;
							}
							if(f[m]=='0')
							{
								first2(m);
								f[m]='1';
							}
							merge(first1[i],first1[m],2);
						}
						else if(_emp(right[j][k])==1 && k==strlen(right[j])-1)//都能推出空，则空包含在first中 
						{
							temp[0]='e';
							temp[1]='\0';
							merge(first1[i],temp,1);
						}
						else 
							break;
					}
				}
			}
		}
	}
	f[i]='1';
}
//求各产生式右部的first
void FIRST(int i,char *p)
{//i表示第i条产生式 
	int length;
	int j,k,m;
	char temp[20];
	length=strlen(p);
	if(length==1)//如果右部为单个符号 
	{
		if(p[0]=='e')//右部只有一个空 
        {   
			if(i>=0)
            {
			    first[i][0]='e';
			    first[i][1]='\0';
			}
			else
			{
				TEMP[0]='e';
				TEMP[1]='\0';
			}
		}
		else
		{	
			for(j=0;;j++)
				if(v[j]==p[0])
					break;
			if(i>=0)
			{
			    memcpy(first[i],first1[j],strlen(first1[j]));
			    first[i][strlen(first1[j])]='\0';
			}
			else
			{
				memcpy(TEMP,first1[j],strlen(first1[j]));
				TEMP[strlen(first1[j])]='\0';
			}
        }
	}
	else//如果右部为符号串
	{
		for(j=0;;j++)
			if(v[j]==p[0])
				break;
		if(i>=0)
            merge(first[i],first1[j],2);
		else
			merge(TEMP,first1[j],2);
		for(k=0;k<=length-1;k++)
		{
			empt[0]='\0';
			if(_emp(p[k])==1 && k<length-1)
			{ 
                for(m=0;;m++)
					if(v[m]==right[i][k+1])
						break;
                if(i>=0)
				    merge(first[i],first1[m],2);
				else
					merge(TEMP,first1[m],2);
			}
            else if(_emp(p[k])==1 && k==length-1)
			{
                
				temp[0]='e';
				temp[1]='\0';
				if(i>=0)
				    merge(first[i],temp,1);   
				else
					merge(TEMP,temp,1);
			}
			else if(_emp(p[k])==0)
				break;
		}
	}
}
// 求各产生式左部的FOLLOW
void FOLLOW(int i)
{
	int j,k,m,n,result=1;
	char c,temp[20];
	c=non_ter[i];//c为待求的非终结符
	temp[0]=c;
	temp[1]='\0';
	merge(fo,temp,1);
	if(c==start)//若为开始符号
	{                      
		temp[0]='#';
		temp[1]='\0';
		merge(follow[i],temp,1);
	}
    for(j=0;j<=count-1;j++)
	{
		if(in(c,right[j])==1)//找一个右部含有c的产生式
		{
			for(k=0;;k++)
				if(right[j][k]==c)
					break;//k为c在该产生式右部的序号
            for(m=0;;m++)
				if(v[m]==left[j])
					break;//m为产生式左部非终结符在所有符号中的序号
			if(k==strlen(right[j])-1)//如果c在产生式右部的最后
			{             
				if(in(v[m],fo)==1)
				{
					merge(follow[i],follow[m],1);
					continue;
                }
				if(F[m]=='0')
				{
					FOLLOW(m);
					F[m]='1';
				}
				merge(follow[i],follow[m],1);
			}
			else 
			{              /*如果c不在产生式右部的最后*/
				for(n=k+1;n<=strlen(right[j])-1;n++)
				{	
					empt[0]='\0';
					result*=_emp(right[j][n]);
				}
				if(result==1)
				{ 	//如果右部c后面的符号串能推出空 
                    if(in(v[m],fo)==1)
					{  
						merge(follow[i],follow[m],1);
						continue;
					}
					if(F[m]=='0')
					{
					    FOLLOW(m);
					    F[m]='1';
					}
				    merge(follow[i],follow[m],1);
				}
				for(n=k+1;n<=strlen(right[j])-1;n++)
                    temp[n-k-1]=right[j][n];       
				temp[strlen(right[j])-k-1]='\0';
				FIRST(-1,temp);
				merge(follow[i],TEMP,2);
			}
		}
	}
	F[i]='1';
}
//构造分析表M
void MM(){
	int i,j,k,m;
	for(i=0;i<=19;i++)
		for(j=0;j<=19;j++)
			M[i][j]=-1;
    i=strlen(termin);
    termin[i]='#';     /*将#加入终结符数组*/
    termin[i+1]='\0';
	for(i=0;i<=count-1;i++)
	{
        for(m=0;;m++)
		if(non_ter[m]==left[i])
			break;      /*m为产生式左部非终结符的序号*/
		for(j=0;j<=strlen(select[i])-1;j++)
		{
			if(in(select[i][j],termin)==1)
			{
				for(k=0;;k++)
					if(termin[k]==select[i][j])
						break;        /*k为产生式右部终结符的序号*/
                M[m][k]=i;
			}
		}
	}
}
void error()
{

	printf("\n你输入的句子不符合当前的文法！\n请确认后重新输入一遍。\n");
	exit(1);
}
int main()
{
    FILE *input,*input1;
	if((input=fopen("input.txt","r"))==NULL){
		printf("Cannot find the file!\nStrike any key to exit!\n");
        system("pause");
        exit(1);
	}
	else{
		int i,j,x,length,result=1;
		input1=fopen("input.txt","r");
        grammer(input);
        lr(input1);
        for(j=0;j<=49;j++)
		{/*初始化*/
			first[j][0]='\0';
	        follow[j][0]='\0';
			first1[j][0]='\0';
			select[j][0]='\0';
			TEMP[j]='\0';
			f[j]='0';
			F[j]='0';
		}	
        for(j=0;j<=strlen(v)-1;j++)
	    	first2(j);//求单个符号的FIRST集合
	    for(i=0;i<=count-1;i++)
	    	FIRST(i,right[i]);//求FIRST
		for(j=0;j<=strlen(non_ter)-1;j++)
    	{//求FOLLOW
			if(fo[j]==0)
			{
				fo[0]='\0';
		    	FOLLOW(j);
			}
    	}
    	printf("\nfirst:");
		for(i=0;i<=count-1;i++)
	    	printf("%s ",first[i]);
		printf("\nfollow:");
    	for(i=0;i<=strlen(non_ter)-1;i++)
	    	printf("%s ",follow[i]);
		for(i=0;i<=count-1;i++)
		{//求每一产生式的SELECT集合
        	memcpy(select[i],first[i],strlen(first[i]));
        	select[i][strlen(first[i])]='\0';
			for(j=0;j<=strlen(right[i])-1;j++)
				result*=_emp(right[i][j]);
			if(strlen(right[i])==1&&right[i][0]=='e')
				result=1;
			if(result==1)
			{		
				for(j=0;;j++)
					if(v[j]==left[i])
						break;
				merge(select[i],follow[j],1);
			}
		}	
		printf("\nselect:");
		for(i=0;i<=count-1;i++)
			printf("%s ",select[i]);
		printf("\n预测分析表为:\n");
		MM();
		for(x=0;x<=strlen(termin);x++)
            printf("\t%c",termin[x]);
        printf("\n");                        
		for(i=0;i<=strlen(non_ter);i++)
		{  
		    printf("%c",non_ter[i]);
		    for(j=0;j<20;j++)
		    {	 	
			    if(M[i][j]>=0)
				{
				    printf("\t%s",right[M[i][j]]);
				    continue;
				}
				printf("\t");
			}
			printf("\n");
		}
		char s[15],tmp;
		NODE *tt;
		int tno=0,flag=0,rno=0;
		printf("请输入要匹配句子(以'#'结束)：");
		gets(s);
		SNODE *T;
		T=(SNODE *)malloc(sizeof(SNODE)); 
		tt=(NODE *)malloc(sizeof(NODE)); 
		InitStack(T);
		StackPush(T,start);
		printf("\n");
		printf("\t\t\t输入串%s的分析过程\n",s);
		printf("符号串\t当前输入符号\t输入串\t\t说明\n");
		for(i=0;i<15;i++)
		{
			PrintStack(T);
			printf("\t%c\t\t%s\t   ",s[i],s+i+1);
			StackPop(T,tt);
			tmp=tt->data;
			if(in(tmp,termin))
			{
				if(tmp=='#'&&s[i]=='#')	
					break; 				
				else if(tmp==s[i])
		    	{
					printf("匹配，弹出%c 并读入下一个字符.\n",tmp);
		    		continue;
				}
		    	else error();
			}
			else 	     
			{	
				for(j=0;j<strlen(non_ter);j++)
					if(tmp==non_ter[j]){
						flag=1;
						break;
					}
					if(flag==1){	
						flag=0;
					for(tno=0;tno<strlen(termin);tno++)
						if(s[i]==termin[tno]){
							flag=1;
							break;
						}
					}
					if(M[j][tno]>=0&&flag==1){      
						rno=M[j][tno];			 
						if(right[rno][0]=='e')
							printf("由%c->e仅弹出栈%c\n",tmp,tmp);
						else
							printf("弹出%c,并将%c->%s反序压栈.\n",tmp,tmp,right[rno]);
						if(right[rno][0]!='e')
							for(tno=strlen(right[rno])-1;tno>=0;tno--)
							{	
								StackPush(T,right[rno][tno]); 		
							}
							i--;					    
					}
					else error();	
			}
		}	
		printf("成功匹配！\n");
		printf("此符号串是此文法的句子\n");
		//free(T);
		//free(tt);
		//system("PAUSE");	
    	//first2(0);
    	//printf("first:%s\n",first1[0]); 
	}
	return 0;
}
