#include<iostream>
#include<cstring>
#include<vector>
#include <fstream>
#include<sstream>
using namespace std;

#define	MAX_COM_LENGTH			150
#define MAX_WORD_LENGTH			100
#define	MAX_PATH_LENGTH			80
#define MAX_STOPWORD_LENGTH		20

struct Command {
	bool _c;		//是否统计字符数
	bool _w;		//否统计单词总数
	bool _l;		//是否统计总行数
	bool _o;		//是否将结果输出到指定文件
	bool _s;		//是否递归处理目录下符合条件的所有文件
	bool _a;		//是否统计代码行/空行/注释行
	bool _e;		//是否开启停用词表
	char filePath[MAX_PATH_LENGTH];		//文件路径 
	char outFile[MAX_PATH_LENGTH];		//输出结果路径 
	char stopFile[MAX_PATH_LENGTH];		//停用词路径 
	Command() {
		_c = false;
		_w = false;
		_l = false;
		_o = false;
		_s = false;
		_a = false;
		_e = false;
		strcpy(filePath, "");
		strcpy(outFile, "");
		strcpy(stopFile, "");
	}
};
struct SourceFile {
	char filePath[MAX_PATH_LENGTH];
	char fileName[MAX_PATH_LENGTH];
	int charNum;
	int wordNum;
	int lineNum;
	int blankLineNum;
	int codeLineNum;
	int noteLineNum;
	SourceFile *next;
	SourceFile() {
		strcpy(filePath, "");		//路径用于寻找文件、输出最后的文件名 
		strcpy(fileName, "");		//文件名用于进行通配符匹配 
		charNum = 0;
		wordNum = 0;
		lineNum = 1;
		blankLineNum = 0;
		codeLineNum = 0;
		noteLineNum = 0;
		next = NULL;
	}
	~SourceFile() {
		delete next;
	}
};
struct StopWord {
	char word[MAX_STOPWORD_LENGTH];
	StopWord *next;
	StopWord() {
		strcpy(word, "");
		next = NULL;
	}
	~StopWord() {
		delete next;
	}
};

void mainLoop();											//程序主循环 
void analyseCommand(char commandStr[], Command &command);	//解析用户指令 
void getFileName(char path[], SourceFile *head);			//递归得到目录下所有文件 
void wordCount(SourceFile *head, char stopPath[]);			//单词统计的预备工作 
void wordCount(SourceFile *sourceFile, StopWord *head);		//单词统计 
void outPut(SourceFile *head, Command &command);			//向文本输出 

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//本段为递归查找目录函数 
#include<io.h>
void getFiles(string path, string path2, SourceFile *head, char* pattern);
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//本段为引用的字符串匹配（带通配符）函数 
#include <ctype.h>
/** Defines and Macros */
#define MATCH      1
#define NOT_MATCH  0
/* 匹配一个字符的宏 */
#define MATCH_CHAR(c1,c2,ignore_case)  ( (c1==c2) || ((ignore_case==1) &&(tolower(c1)==tolower(c2))) )
int WildCharMatch(char *src, char *pattern, int ignore_case);
////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
	mainLoop();
	return 0;
}

void mainLoop() {
	/*使用while循环反复执行cin.
	cin之后解析用户指令，并根据解析用户指令的结果执行后续功能。
	如果cin得到的是空字符串，则退出主循环。*/
	char commandStr[MAX_COM_LENGTH] = "";
	while (true) {											//循环处理用户请求 
		gets(commandStr);									//获得用户指令 
		if (strcmp(commandStr, "") == 0) break;					//如果是空指令，程序结束 

		Command command;
		analyseCommand(commandStr, command);					//解析用户指令 

		SourceFile *head = new SourceFile();
		if (command._s) getFileName(command.filePath, head);	//递归寻找目录下的文件 
		else {												//否则直接利用相对路径查找文件 
			SourceFile *p = new SourceFile();
			p->next = head->next;
			head->next = p;
			strcpy(p->fileName, command.filePath);
			strcpy(p->filePath, command.filePath);
		}

		wordCount(head, command.stopFile);					//统计单词数 

		outPut(head, command);								//结果输出到文件 

		delete head;
	}

}

void analyseCommand(char commandStr[], Command &command) {
	/*传入一个字符串和一个指令结构体的引用。
	在该结构体内存储解析结果。*/
	for (int i = 0; true; i++) {
		char c = commandStr[i];
		if (c == 0) return;
		if (c == '-') {	//遇到'-'之后立即读取下一个字符，然后判断是何种指令，判断是否需要继续读取字符串 
			i++;
			c = commandStr[i];
			if (c == 'c') { command._c = true; continue; }
			else if (c == 'w') { command._w = true; continue; }
			else if (c == 'l') { command._l = true; continue; }
			else if (c == 's') { command._s = true; continue; }
			else if (c == 'a') { command._a = true; continue; }
			else if (c == 'e') command._e = true;
			else if (c == 'o') command._o = true;
			else continue;
			i += 2;	//跳过一个空格以后开始读取文件路径
					//这个i++语句只有-e和-o会执行到 
		}
		if (c == ' ') continue;

		char path[MAX_PATH_LENGTH] = "";
		for (int j = 0; true; j++) {
			char ch = commandStr[i];
			if (ch == 0) { i--; break; }
			/*由于大循环每次i++，这里到了命令末尾
			所以要先i--才能让程序判断出这里是命令末尾*/
			if (ch == ' ') break;
			path[j] = ch;
			i++;
		}
		if (c == 'e') strcpy(command.stopFile, path);
		else if (c == 'o') strcpy(command.outFile, path);
		else strcpy(command.filePath, path);
	}
	return;
}

void getFileName(char path[], SourceFile *head) {
	//给定文件路径和头指针，得到指定路径下所有符合条件的文件名。
	char filePath[MAX_PATH_LENGTH] = "";
	char fileName[MAX_PATH_LENGTH] = "";
	strcpy(filePath, path);
	for (int i = MAX_PATH_LENGTH - 1; i >= 0; i--) {
		if (filePath[i] == '\\') {	//从后向前寻找第一个'\\'以分割文件目录和文件名 
			i++;
			int j = 0;
			for (; filePath[i + j] != 0; j++) {
				fileName[j] = filePath[i + j];
			}
			fileName[j] = 0;
			filePath[i] = 0;
			break;
		}
	}
	string p;
	p.assign(filePath);
	//获取该路径下的所有文件，并存入SourceFile链表中 
	getFiles(p, "", head, fileName);
	return;
}

void wordCount(SourceFile *head, char stopPath[]) {
	/*首先遍历停用词表，构建停用词结构体。
	依次使用每一个源文件结构体和停用词链表进行具体的统计。*/
	StopWord *sHead = new StopWord();

	ifstream in(stopPath);
	string line, word;
	if (in)
	{
		while (getline(in, line))
		{   //读取停用词时，采用逐行读取、逐单词读取的方法 
			istringstream istrm(line);
			while (istrm >> word) {
				StopWord *pS = new StopWord();
				pS->next = sHead->next;
				strcpy(pS->word, word.c_str());
				sHead->next = pS;
			}
		}
	}
	in.close();

	SourceFile *p = head->next;
	while (p != NULL) {
		wordCount(p, sHead);
		p = p->next;
	}

	delete sHead;
}

void wordCount(SourceFile *sourceFile, StopWord *head) {
	//遍历文件内容，进行统计，并将结果保存在sourceFile中
	char c;
	ifstream in;
	in.open(sourceFile->filePath);

	bool wordFlag = false;					//是否处于单词内部 
	bool longNote = false;					//是否处于长段注释 
	int state = 1;							//当前行的状态 
	bool hasPassState2 = false;				//当前行已不可能成为空行 
	char currentWord[MAX_WORD_LENGTH] = "";	//记录当前的单词 
	int wordPosition = 0;						//记录当前游标处在单词的第几个字母 


	while (true)
	{
		c = in.get();
		if (c == EOF) {
			//在文件结尾处，还要对单词数、行数等进行最后的结算 
			if (wordFlag) {
				sourceFile->wordNum++;
			}
			if (state == 1) {
				if (hasPassState2) sourceFile->noteLineNum++;
				else sourceFile->blankLineNum++;
			}
			if (state == 2) {
				if (hasPassState2) sourceFile->noteLineNum++;
				else sourceFile->blankLineNum++;
			}
			if (state == 3) sourceFile->codeLineNum++;
			if (state == 5) {
				if (hasPassState2) sourceFile->codeLineNum++;
				else sourceFile->blankLineNum++;
			}
			if (state == 6 || state == 7 || state == 8) sourceFile->noteLineNum++;

			if (strcmp(currentWord, "") != 0) {
				StopWord *pH = head->next;
				while (pH != NULL) {
					if (strcmp(currentWord, pH->word) == 0) {
						sourceFile->wordNum--;
						break;
					}
					pH = pH->next;
				}
			}
			break;
		}

		//字符数 
		if (c != '\n') sourceFile->charNum++;
		//单词数 
		bool separator = (c == ' ' || c == ',' || c == '\n' || c == '\t');	//这个变量表示当前字符是否属于分隔符 
		if (wordFlag&&separator) sourceFile->wordNum++;		//如果字符由单词内字符变为分隔符，说明单词数+1 
		wordFlag = !separator;
		if (wordFlag) {
			currentWord[wordPosition] = c;
			wordPosition++;
			currentWord[wordPosition] = 0;					//及时更新字符串停止标志 
		}
		if (!wordFlag) {										//当一个单词读完以后，要判断停用词 
			wordPosition = 0;
			if (strcmp(currentWord, "") != 0) {
				StopWord *pH = head->next;
				while (pH != NULL) {							//逐一对照停用词表进行判断 
					if (strcmp(currentWord, pH->word) == 0) {
						sourceFile->wordNum--;				//如果属于停用词，单词数就要-1 
						break;
					}
					pH = pH->next;
				}
			}
			currentWord[wordPosition] = 0;					//这个语句是为了防止重复判断停用词 
		}

		//总行数，包括代码行、注释行、空行 
		//采用状态迁移模型，这段代码太烂了，功能基本实现了 
		if (state == 1) {
			if (c == '\n') {
				sourceFile->lineNum++;
				if (hasPassState2) sourceFile->noteLineNum++;
				else sourceFile->blankLineNum++;
				hasPassState2 = false;
			}
			else if (c == '/') state = 5;
			else if (c != ' '&&c != '\t') { state = 2; }
		}
		else if (state == 2) {
			if (c == '\n') {
				state = 1;
				sourceFile->lineNum++;
				if (hasPassState2) sourceFile->noteLineNum++;
				else sourceFile->blankLineNum++;
				hasPassState2 = false;
			}
			else if (c == '/') {
				state = 5;
				hasPassState2 = true;
			}
			else if (c != ' '&&c != '\t') {
				state = 3;
				hasPassState2 = true;
			}
		}
		else if (state == 3) {		//这里其实有一些问题，以后再改 
			if (c == '\n') {
				state = 1;
				sourceFile->lineNum++;
				sourceFile->codeLineNum++;
				hasPassState2 = false;
			}
		}
		else if (state == 5) {
			if (c == '\n') {
				state = 1;
				sourceFile->lineNum++;
				if (hasPassState2) sourceFile->codeLineNum++;
				else sourceFile->blankLineNum++;
				hasPassState2 = false;
			}
			else if (c == '/')state = 6;
			else if (c == '*') {
				state = 7;
				longNote = true;
			}
			else state = 3;
		}
		else if (state == 6) {
			if (c == '\n') {
				state = 1;
				sourceFile->lineNum++;
				sourceFile->noteLineNum++;
				hasPassState2 = false;
			}
		}
		else if (state == 7) {
			if (c == '\n') {
				state = 7;
				sourceFile->lineNum++;
				sourceFile->noteLineNum++;
				hasPassState2 = false;
			}
			else if (c == '*') state = 8;
		}
		else if (state == 8) {
			if (c == '\n') {
				state = 7;
				sourceFile->lineNum++;
				sourceFile->noteLineNum++;
				hasPassState2 = false;
			}
			else if (c == '*') state = 8;
			else if (c == '/') {
				state = 1;
				longNote = false;
				hasPassState2 = true;
			}
			else state = 7;
		}
	}
	in.close();
	return;
}

void outPut(SourceFile *head, Command &command) {
	/*给定文件结构体的头指针和指令结构体的引用，依次按格式输出结果。
	按照字符'单词'行数'代码行数/空行数/注释行的顺序，依次分行显示。*/

	ofstream outf(command.outFile);
	streambuf *default_buf = cout.rdbuf();
	if(command._o) cout.rdbuf(outf.rdbuf());		//输出重定向

	SourceFile *p = head->next;
	while (p != NULL) {
		//严格按照需求里面的顺序进行输出 
		if (command._c)
			cout << p->filePath << ", 字符数: " << p->charNum << endl;
		if (command._w)
			cout << p->filePath << ", 单词数: " << p->wordNum << endl;
		if (command._l)
			cout << p->filePath << ", 行数: " << p->lineNum << endl;
		if (command._a) {
			cout << p->filePath << ", 代码行/空行/注释行: ";
			cout << p->codeLineNum << '/';
			cout << p->blankLineNum << '/';
			cout << p->noteLineNum << endl;
		}
		p = p->next;
	}

	//恢复cout默认输出 
	cout.rdbuf(default_buf);
}

int WildCharMatch(char *src, char *pattern, int ignore_case)
{
	int result;

	while (*src)
	{
		if (*pattern == '*')
		{   /* 如果 pattern 的当前字符是 '*' */
		 /* 如果后续有多个 '*', 跳过 */
			while ((*pattern == '*') || (*pattern == '?'))
				pattern++;

			/* 如果 '*" 后没有字符了，则正确匹配 */
			if (!*pattern) return MATCH;

			/* 在 src 中查找一个与 pattern中'*"后的一个字符相同的字符*/
			while (*src && (!MATCH_CHAR(*src, *pattern, ignore_case)))
				src++;

			/* 如果找不到，则匹配失败 */
			if (!*src) return NOT_MATCH;

			/* 如果找到了，匹配剩下的字符串*/
			result = WildCharMatch(src, pattern, ignore_case);
			/* 如果剩下的字符串匹配不上，但src后一个字符等于pattern中'*"后的一个字符 */
			/* src前进一位，继续匹配 */
			while ((!result) && (*(src + 1)) && MATCH_CHAR(*(src + 1), *pattern, ignore_case))
				result = WildCharMatch(++src, pattern, ignore_case);

			return result;

		}
		else
		{
			/* 如果pattern中当前字符不是 '*' */
			/* 匹配当前字符*/
			if (MATCH_CHAR(*src, *pattern, ignore_case) || ('?' == *pattern))
			{
				/* src,pattern分别前进一位，继续匹配 */
				return WildCharMatch(++src, ++pattern, ignore_case);
			}
			else
			{
				return NOT_MATCH;
			}
		}
	}


	/* 如果src结束了，看pattern有否结束*/
	if (*pattern)
	{
		/* pattern没有结束*/
		if ((*pattern == '*') && (*(pattern + 1) == 0)) /* 如果pattern有最后一位字符且是'*' */
			return MATCH;
		else
			return NOT_MATCH;
	}
	else
		return MATCH;
}

void getFiles(string path, string path2, SourceFile *head, char* pattern)
{
	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	string p, p2;
	if ((hFile = _findfirst(p.assign(path).append(path2).append("*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			//如果不是,加入列表  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("\\"), p2.assign(fileinfo.name).append("\\"), head, pattern);
			}
			else
			{
				char fileName[MAX_PATH_LENGTH];
				strcpy(fileName, p.assign(fileinfo.name).c_str());
				char filePath[MAX_PATH_LENGTH];
				strcpy(filePath, p.assign(path2).append(fileinfo.name).c_str());

				if (WildCharMatch(filePath, pattern, 1)) {
					SourceFile *pF = new SourceFile();
					pF->next = head->next;
					head->next = pF;
					strcpy(pF->fileName, fileName);
					strcpy(pF->filePath, filePath);
				}

			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}
