/************************************************************************************************************
파일 이름: hell_mode.c
주요 함수: int HellGate(FILE *fp,unsigned int* pcnt,wdif** pword);
주요 기능: 1. 파일을 불러와 모든 단어에 빈칸 뚫기 2. cnt, word를 결정.
출력 인자: 파일 불러오기 및 빈칸 생성 시 발생한 오류 및 성공 여부를  0과 -1로 반환.
입력 인자: fp로 파일 받아오고, cnt와 word의 값을 cnt와 word의 포인터로 반환시켜준다.
제작자: 강준구, 박남규, 김민재
************************************************************************************************************/
#include "header.h"

typedef struct {char **str; unsigned int now;} wdlist;

wdlist exc, voca;

extern boolean fgets_abc(char *str, unsigned int siz, FILE *fp_db); //단어목록으로부터 단어들을 불러온다.

int HellGate(FILE *fp,unsigned int* pcnt,wdif** pword)
{
    FILE *fp_db;
    wdif *word;
    char c;
	unsigned int wdcnt,cnt,len,loc,i, temp;           // len: word length  / loc: location of filepointer indicating position's distance from SEEK_SET

	exc.now=voca.now=1;
	word=(wdif*)calloc(1,sizeof(wdif));

	fseek(fp,0,SEEK_SET);

	for(len=wdcnt=cnt=0;(c=fgetc(fp))!=EOF;++len){  // len is used to count length of word, while getting character from file until it reachs End Of File.
		if( isalpha(c) )
            word[cnt].str[len]=c;	                        // 알파벳이면 문자열에 저장하자.
		else
        {												// 알파벳아니면 단어 끝났으니까 '\0'삽입하고 빈칸 뚫기 시작.
			word[cnt].str[len]='\0';
			if (len==0); //공백일뿐. 단어는 아님.
			else
			{ 													// 단어에 빈칸 뚫기
			    temp=len+1;
                loc=ftell(fp)-1-len;							// fp가 가리키고 있는 위치 저장.
				if(c=='\n') --loc;								// Unknown Reason. '\n'이 2개의 공간을 차지하는듯 하더라.
				word[cnt].loc=loc; word[cnt].len=len;			// word[cnt]에 loc, len 저장.
				word=(wdif*)realloc(word,sizeof(wdif)*(++cnt+1)); // word[]를 동적으로 메모리 할당.
				word[cnt].show=word[cnt].res=word[cnt].str[0]=word[cnt].wrongstr[0]=0;
				fseek(fp,loc,SEEK_SET);
				fputc(MARK,fp);								// 빈칸단어의 시작점을 MARK '|'로 체크
				for(--len;len>0;--len) fputc('_',fp);		// 빈칸으로 뚫어버린다.
				fseek(fp,loc+temp,SEEK_SET);					// 파일포인터 읽는 위치는 다음 단어로 옮겨버린다.
            }
			len=-1; //len이 현재 0이므로 -1을 해주어야 루프가 돌면서 len이 1증가하여 다시 0이 된다.
		}
	}

	// 깔끔하게 동적 메모리 할당 해제.
	for(i=0;i<exc.now;++i) free(exc.str[i]);
	for(i=0;i<voca.now;++i) free(voca.str[i]);
	free(exc.str);
	free(voca.str);

	fp_db=fopen("Temp\\wddat.dat","wb"); // 나중에 다시 빈칸 안 뚫고 재활용 할때 재활용 할 수 있도록,
	fseek(fp_db,0,SEEK_SET);
	fwrite(&cnt,sizeof(unsigned int),1,fp_db); //cnt와
	fseek(fp_db,5,SEEK_SET);
	fwrite(word,sizeof(wdif),cnt,fp_db);	// word 구조체배열을 저장해준다.
	fclose(fp_db);

	// 결과로 얻은 cnt와 word는 main()의 cnt와 word로 포인터를 통해 반환.
	*pcnt=cnt;
	*pword=word;
	return 0;
}

