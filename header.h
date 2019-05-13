/*****************************************************************************************
파일 이름은 header.h
프로젝트 이름: BlankLearner
제작자: 강준구, 박남규, 김민재
구성 파일: main.c, blankgenerator.c, screen.c  //각자 기능에 따라 파일로 분리해 놓았다.
*****************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h> //isalpha() isupper()
#include <stdarg.h>
#include <conio.h>
#include <time.h>

#define _WIN32_WINNT 0x0500
#include <windows.h>

// Text Color Set 텍스트 색상 설정
#define RED SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x000c);
#define BLUE SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0009);
#define GREEN SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x000a);
#define WHITE SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x000f);

//커서 좌표 이동 및 받아오기
#define GotoXY(Pos) SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos)
#define GetXY(Pos) {CONSOLE_SCREEN_BUFFER_INFO csbi; GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbi); Pos=csbi.dwCursorPosition;}

//자주 쓰게 될 system 명령어
#define PAUSE system("pause");
#define CLEAR system("cls");

#define MARK '|'        // 빈칸이 뚫릴 단어의 시작을 표시.
#define LOW 20			// Length Of Words 단어의 길이ㅡ (글자 수)

typedef struct{     				// wdif(wordinfo) will contain information about word blanked. 빈칸 단어의 정보를 저장한다.
    unsigned int loc,len;           // 위치와 길이
	COORD pos;
    char str[LOW],wrongstr[LOW];  	// 문자열을 저장한다.
    boolean show;   				// 0 Not yet // 1 space pressed
    int res;        // 0 Not yet, missed // 2 Correct // -2 wrong // -1 hint used // 1 Fixed answer, from -2 or -1
} wdif;

//getch()로 받게 될 문자, 방향키, 특수자들
enum {BACKSPACE=8,TAB,ENTER=13,ESC=27,SPACE=32,F1=59,F2=60,LEFT=75,RIGHT=77,UP=72,DOWN=80};

char Ask(int n,...);
inline void Tutorial(void);
inline void mvword(int *i,int mov,unsigned int cnt);
