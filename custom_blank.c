/************************************************************************************************************
파일 이름: custom_blank.c
주요 함수: void custom_blank(FILE *fp,unsigned int* pcnt,wdif** pword);
주요 기능: 1. 파일을 불러와 콘솔에 띄움.  2. SPACE로 사용자에게 빈칸 여부를 받음.
입력 출력: fp로 파일 받아오고, cnt와 word의 값을 cnt와 word의 포인터로 반환시켜준다.
특 징 : blankgenerator와 screen의 짬뽕 개조판.
제작자: 강준구, 박남규, 김민재
************************************************************************************************************/

#include "header.h"

enum {Out=0,In}; // 포인터가 현재 단어를 가리키고 있는지 아닌지.

typedef struct{ // wdif와 대략적으로 유사하지만 필요한 정보가 사실 조금씩 달라서 새 구조체 자료형 만듬.
	char str[LOW]; // 단어 저장
	unsigned int len,loc; // 단어의 길이와 파일포인터 상의 단어 위치
	boolean bool_blank; // 단어의 빈 칸 유무
	COORD Pos; // 단어의 콘솔상 좌표.
}wordinfo;

void SetBlank(FILE* fp,unsigned int n,wordinfo* word); // 콘솔상에서 단어 표시를 재설정. (뚫기 또는 뚫기 취소.)

void custom_blank(FILE *fp,unsigned int* pcnt,wdif** pword){
    FILE *fp_db;
	unsigned int cnt,len,cnt_,cnt_b;
	char c; int i;
	boolean STATE=Out;

	wdif *word_=(wdif*)calloc(1,sizeof(wdif));
	wordinfo *word=(wordinfo*)calloc(1,sizeof(wordinfo));

	CLEAR puts("사용자 지정 빈칸 생성기.\n왼쪽 방향키(←), 오른쪽 방향키(→)를 눌러 빈칸으로 만들고 싶은 단어로 이동하세요.\nSPACE를 눌러 빈칸을 뚫으세요.\n빈칸을 다 뚫고나서 ESC를 누르면 자동으로 학습이 시작됩니다.\n"); PAUSE CLEAR
	fseek(fp,0,SEEK_SET);
	for(cnt=len=0;(c=fgetc(fp))!=EOF;){//처음에 텍스트 파일을 출력하는 용도이다.
		if(isalpha(c)){
			if(STATE==Out) { STATE=In; GetXY(word[cnt].Pos);}
			word[cnt].str[len++]=c;
		}
		else if(STATE==In){
			STATE=Out;
			word[cnt].str[len]='\0';
			word[cnt].len=len;
			word[cnt].loc=ftell(fp)-len-1;
			word=(wordinfo*)realloc(word,sizeof(wordinfo)*(++cnt+1));
			word[cnt].bool_blank=FALSE;
			len=0;
		}
		if (c==MARK) c='_';
		putc(c,stdout);
	}
	GotoXY(word[(i=0)].Pos);

	while((c=getch())!=ESC){//텍스트 파일 출력 후 키 감지
		switch (c){
			case LEFT: mvword(&i,-1,cnt); break; //만약 왼쪽 화살표 키를 눌렀을 경우 커서를 전 단어로 이동시킨다.
			case RIGHT: mvword(&i,1,cnt); break; //만약 오른쪽 화살표 키를 눌렀을 경우 커서를 뒤에 있는 단어로 이동시킨다.
			case TAB: mvword(&i,+10,cnt); break; //탭 키를 눌렀을 경우 10개 단위로 이동한다.
			case SPACE: SetBlank(fp,i,word); break; //스페이스키를 눌렀을 경우 Blank 상태 변수에 따라 동작하고 변수를 반전시킨다.
		}
		GotoXY(word[i].Pos);
	}//ESC키 누르면 즉시 커스텀 모드를 종료한다.

	for(i=cnt_=cnt_b=0;i<cnt;++i){//옮긴다.
		if(word[i].bool_blank){
			word_[cnt_].len=word[i].len;
			word_[cnt_].loc=word[i].loc;
			word_[cnt_].pos.X=word[i].Pos.X;
			word_[cnt_].pos.Y=word[i].Pos.Y;
			strcpy(word_[cnt_].str,word[i].str);
			word_[cnt_].wrongstr[0]=word_[cnt_].show=word_[cnt_].res=0;
			word_=(wdif*)realloc(word_,sizeof(wdif)*(++cnt_+1));
			cnt_b++;
		}
	}
	free(word);

	if(!cnt_b){
        system("copy /Y Temp\\Temp2.txt Temp\\Temp.txt");
        return;
	}

	fp_db=fopen("Temp\\wddat.dat","wb"); // 나중에 다시 빈칸 안 뚫고 재활용 할때 재활용 할 수 있도록,
	fseek(fp_db,0,SEEK_SET);
	fwrite(&cnt_,sizeof(unsigned int),1,fp_db); //cnt와
	fseek(fp_db,5,SEEK_SET);
	fwrite(word_,sizeof(wdif),cnt_,fp_db);	// word 구조체배열을 저장해준다.
	fclose(fp_db);

	*pcnt=cnt_;//빈칸을 뚫고 난 후 빈칸을 뚫은 단어의 수를 포인터로 리턴합니다.
	*pword=word_;//빈칸을 뚫은 단어의 정보를 제출합니다.
	return;
}

void SetBlank(FILE* fp, unsigned int n,wordinfo* word){
	unsigned int i;
	fseek(fp,word[n].loc,SEEK_SET);
	GotoXY(word[n].Pos);
	if(word[n].bool_blank){					// 이미 뚫어놨으면
    	fputs(word[n].str,fp);
		fputs(word[n].str,stdout);			// 다시 원래대로 복구시킨다.
	}
	else{									// 안 뚫려 있으면
		putc(MARK,fp); putc('_',stdout);
		for(i=word[n].len-1;i!=0;--i){
			putc('_',fp);
			putc('_',stdout);				// 뚫자.
		}
	}
	GotoXY(word[n].Pos);					// 콘솔 위치 설정.
	word[n].bool_blank=!word[n].bool_blank;	// 뚫린 상태 다시 반영.
}
