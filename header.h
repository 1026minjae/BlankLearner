/*****************************************************************************************
���� �̸��� header.h
������Ʈ �̸�: BlankLearner
������: ���ر�, �ڳ���, �����
���� ����: main.c, blankgenerator.c, screen.c  //���� ��ɿ� ���� ���Ϸ� �и��� ���Ҵ�.
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

// Text Color Set �ؽ�Ʈ ���� ����
#define RED SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x000c);
#define BLUE SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0009);
#define GREEN SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x000a);
#define WHITE SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x000f);

//Ŀ�� ��ǥ �̵� �� �޾ƿ���
#define GotoXY(Pos) SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos)
#define GetXY(Pos) {CONSOLE_SCREEN_BUFFER_INFO csbi; GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbi); Pos=csbi.dwCursorPosition;}

//���� ���� �� system ��ɾ�
#define PAUSE system("pause");
#define CLEAR system("cls");

#define MARK '|'        // ��ĭ�� �ո� �ܾ��� ������ ǥ��.
#define LOW 20			// Length Of Words �ܾ��� ���̤� (���� ��)

typedef struct{     				// wdif(wordinfo) will contain information about word blanked. ��ĭ �ܾ��� ������ �����Ѵ�.
    unsigned int loc,len;           // ��ġ�� ����
	COORD pos;
    char str[LOW],wrongstr[LOW];  	// ���ڿ��� �����Ѵ�.
    boolean show;   				// 0 Not yet // 1 space pressed
    int res;        // 0 Not yet, missed // 2 Correct // -2 wrong // -1 hint used // 1 Fixed answer, from -2 or -1
} wdif;

//getch()�� �ް� �� ����, ����Ű, Ư���ڵ�
enum {BACKSPACE=8,TAB,ENTER=13,ESC=27,SPACE=32,F1=59,F2=60,LEFT=75,RIGHT=77,UP=72,DOWN=80};

char Ask(int n,...);
inline void Tutorial(void);
inline void mvword(int *i,int mov,unsigned int cnt);
