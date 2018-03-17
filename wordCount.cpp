#include<iostream>
#include<cstring>
#include<vector>
using namespace std;

#define	MAX_COM_LENGTH	50
#define	MAX_PATH_LENGTH	50

struct Command{
	bool _c;		//是否统计字符数
	bool _w;		//否统计单词总数
	bool _l;		//是否统计总行数
	bool _o;		//是否将结果输出到指定文件
	bool _s;		//是否递归处理目录下符合条件的所有文件
	bool _a;		//是否统计代码行/空行/注释行
	bool _e;		//是否开启停用词表
	char filePath[MAX_PATH_LENGTH];
	char outFile[MAX_PATH_LENGTH];
	char stopFile[MAX_PATH_LENGTH];
	Command(){
		_c=false;
		_w=false;
		_l=false;
		_o=false;
		_s=false;
		_a=false;
		_e=false;
		strcpy(filePath,"");
		strcpy(outFile,"");
		strcpy(stopFile,"");
	}
};
struct SourceFile{
	char filePath[MAX_PATH_LENGTH];
	char fileName[MAX_PATH_LENGTH];
	int charNum;
	int wordNum;
	int lineNum;
	SourceFile *next;
	SourceFile(){
		strcpy(filePath,"");		//路径用于寻找文件、输出最后的文件名 
		strcpy(fileName,"");		//文件名用于进行通配符匹配 
		charNum=0;
		wordNum=0;
		lineNum=0;
		next=NULL;
	}
};

void mainLoop();
void analyseCommand(char commandStr[], Command &command);
void getFileName(char filePath[], SourceFile *head);
void wordCount(SourceFile *head, char stopPath[]);
void wordCount(SourceFile *sourceFile, vector<string> &stopWords);
void outPut(SourceFile *head, Command &command);

int main(){
	mainLoop();
	return 0;	
}

void mainLoop(){
	/*使用while循环反复执行cin.
	cin之后解析用户指令，并根据解析用户指令的结果执行后续功能。
	如果cin得到的是空字符串，则退出主循环。*/
	char commandStr[MAX_COM_LENGTH] = "";
	while(true){
		gets(commandStr);
		if(strcmp(commandStr,"")==0) break;
		Command command;
		analyseCommand(commandStr,command);
	}
	
}

void analyseCommand(char commandStr[], Command &command){
	/*传入一个字符串和一个指令结构体的引用。 
	在该结构体内存储解析结果。*/
	return;
}

void getFileName(char filePath[], SourceFile *head){
	//给定文件路径和头指针，得到指定路径下所有符合条件的文件名。 
	return;
}

void wordCount(SourceFile *head, char stopPath[]){
	/*首先遍历停用词表，构建停用词结构体。 
	依次使用每一个源文件结构体和停用词向量进行具体的统计。*/
}

void wordCount(SourceFile *sourceFile, vector<string> &stopWords){
	//遍历文件内容，进行统计，并将结果保存在sourceFile中 
	return;
}

void outPut(SourceFile *head, Command &command){
	/*给定文件结构体的头指针和指令结构体的引用，依次按格式输出结果。
	按照字符'单词'行数'代码行数/空行数/注释行的顺序，依次分行显示。*/
}

