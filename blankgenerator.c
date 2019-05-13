/************************************************************************************************************
파일 이름: blankgenerator.c
주요 함수: int BlankGenerator(FILE *fp,unsigned int* pcnt,wdif** pword);
주요 기능: 1. 파일을 불러와 예외 조건 목록을 참고하여 빈칸을 뚫을지 결정 2. 빈칸뚫기 3. cnt, word를 결정.
출력 인자: 파일 불러오기 및 빈칸 생성 시 발생한  오류 및 성공 여부를  0과 -1로 반환.
입력 인자: fp로 파일 받아오고, cnt와 word의 값을 cnt와 word의 포인터로 반환시켜준다.
제작자: 강준구, 박남규, 김민재
************************************************************************************************************/
#include "header.h"

typedef struct {char **str; unsigned int now;} wdlist;

boolean fgets_abc(char *str, unsigned int siz, FILE *fp_db); //단어목록으로부터 단어들을 불러온다.
inline void Register(FILE *fp_db,wdlist *data); // 텍스트로부터 단어목록을 불러와 wdlist형 data 에 저장한다.
int Search(char *str); // 단어가 예외목록, 필수 목록에 있는지 검색.
boolean dtblank(unsigned int wdcnt,char freq,char *str); // 빈칸을 뚫을지 말지 결정해준다.

wdlist exc, voca;
boolean dc[10]={0,1,0,1,0,1,0,1,0,1};	// dc[] 는 빈칸을 뚫을 지 말지를 결정한 결과(참,거짓)를 담게 됨.

int BlankGenerator(FILE *fp,unsigned int* pcnt,wdif** pword)
{
    FILE *fp_db; wdif *word;
    char c,freq;
	unsigned int wdcnt,cnt,len,loc,i,temp;           // len: word length  / loc: location of filepointer indicating position's distance from SEEK_SET

	srand((unsigned)time(NULL));		// srand()를 이용한 rand()의 seed 초기화
	exc.now=voca.now=1;
	word=(wdif*)calloc(1,sizeof(wdif));

	//Register()를 통해 텍스트 파일에 있는 단어 목록을 wdlist 형의 구조체의 exc,voca로 가져옴.
	if((fp_db=fopen("File\\Exception.txt","rt"))==NULL)
    {
        fprintf(stderr,"Failed to open Exception.txt\n");
        PAUSE
        return -1;
    }

	Register(fp_db,&exc);
	fclose(fp_db);

    if((fp_db=fopen("File\\VocaList.txt","rt"))==NULL)
    {
        fprintf(stderr,"Failed to open VocaList.txt\n");
        return -1;
    }

	Register(fp_db,&voca);
	fclose(fp_db);

	if((fp_db=fopen("Temp\\userdata.txt","rt"))==NULL)
    { //userdata에 접근이 불가능하면
		fp_db=fopen("Temp\\userdata.txt","wt");
		freq=4;					//userdata에 (freq=4)를 쓴다.
	}
	else
	{												//userdata에 접근 했으면
		fscanf(fp_db,"%d",&freq); fclose(fp_db);		//freq를 읽어온다.
		if(freq<1||freq>5) freq=4;
		fp_db=fopen("Temp\\userdata.txt","wt");
	}

	fprintf(fp_db,"%d:frequency\n%4s| %s\n",freq,"cnt","string"); //userdata.txt에 달아줄 장식.
	fseek(fp,0,SEEK_SET);

	for(len=wdcnt=cnt=0;(c=fgetc(fp))!=EOF;++len){				// len is used to count length of word, while getting character from file until it reachs End Of File.
		if( isalpha(c) )
            word[cnt].str[len]=c;	                        // 알파벳이면 문자열에 저장하자.
		else
        {												// 알파벳아니면 단어 끝났으니까 '\0'삽입하고 빈칸 뚫기 시작.
			word[cnt].str[len]='\0';
			if (len==0); //공백일뿐. 단어는 아님.
			else if (len==1) ++wdcnt; 			// 단어가 감지 되었지만, 너무 짧음. / wdcnt 는 본문에서 몇번째 단어인지 카운트 / cnt 는 몇번째 빈칸인지 카운트
			else
			{ 													// 적당한 길이의 단어일때,
				if(dtblank(++wdcnt%10,freq,word[cnt].str)==TRUE)
				{	// dtblank()가 결정한다. 빈칸 뚫기로 결정하면,
				    temp=len+1;
					loc=ftell(fp)-1-len;							// fp가 가리키고 있는 위치 저장.
					if(c=='\n') --loc;								// Unknown Reason. '\n'이 2개의 공간을 차지하는듯 하더라.
					word[cnt].loc=loc; word[cnt].len=len;			// word[cnt]에 loc, len 저장.
					fprintf(fp_db,"%04d: %s\n",cnt+1,word[cnt].str);
					word=(wdif*)realloc(word,sizeof(wdif)*(++cnt+1)); // word[]를 동적으로 메모리 할당.
					word[cnt].show=word[cnt].res=word[cnt].str[0]=word[cnt].wrongstr[0]=0;
					fseek(fp,loc,SEEK_SET);
					fputc(MARK,fp);								// 빈칸단어의 시작점을 MARK '|'로 체크
					for(--len;len>0;--len) fputc('_',fp);		// 빈칸으로 뚫어버린다.
					fseek(fp,loc+temp,SEEK_SET);					// 파일포인터 읽는 위치는 다음 단어로 옮겨버린다.
				}
			}
			len=-1;
		}
	}

	// 깔끔하게 동적 메모리 할당 해제.
	for(i=0;i<exc.now;++i) free(exc.str[i]);
	for(i=0;i<voca.now;++i) free(voca.str[i]);
	free(exc.str);
	free(voca.str);

	if(cnt==0)
    {
        if(BlankGenerator(fp,pcnt, pword)==-1)
            return -1;
        else
            return 0;
    }

	fprintf(fp_db,"Total %d of words were successfully replaced as blank.\n",cnt);
	fclose(fp_db); // 파일포인터도 다 쓰고나면 닫아주는 습관.

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

boolean dtblank(unsigned int wdcnt,char freq,char *str){			// Function that determine whether replace as blank or not.
    unsigned int i,sum;
    if((i=Search(str))==0) return dc[wdcnt]=FALSE;				// 해당 단어가 Exception.txt에 있으니 빈칸을 뚫지 않는다.
	else if(i==2) return dc[wdcnt]=TRUE;						// 해당 단어가 VocaList.txt에 있으니 꼭 뚫는다.
    else if(dc[(wdcnt+10-1)%10]==1) return dc[wdcnt]=FALSE;		// 이전 단어가 뚫려 있으니 고르게 빈칸을 분포시키기 위해 뚫지 않도록 한다.
    else if(isupper(str[0])) return dc[wdcnt]=FALSE;			// 대문자로 시작하면 빈칸을 뚫지 않는다. (제목이나 고유명사인 경우를 위해)
    else{											// 이러한 전제조건을 모두 만족하면, 빈칸 분포를 바탕으로 결정하자.
		for(i=sum=0;i<10;++i)
			sum+=dc[i];								// 근방 10개에 빈칸이 몇개나 있는지 확인
        if(sum>freq) dc[wdcnt]=FALSE;				// 기준치보다 많으니 안 뚫음.
        else if(sum<freq) dc[wdcnt]=TRUE;			// 기준치보다 적으니 뚫는다.
        else dc[wdcnt]=rand()%3;					// 허용치면 랜덤.
        return dc[wdcnt];							// 결과 반환, dc[wdcnt]에 저장해서 두고두고 10번 우려먹기.
    }
}

int Search(char *str){
	unsigned int i;
	for(i=0;i<exc.now;++i)
		if(strcmp(str,exc.str[i])==0) return 0;					// If str[] and exc_str[i][] containing same word, (if str[] contains exception word,) return 0.
	for(i=0;i<voca.now;++i)
		if(strcmp(str,voca.str[i])==0) return 2;
	return 1;													// If str[] does not contain any of exception word, return 1.
}

inline void Register(FILE *fp_db,wdlist *data){
	(*data).str=(char**) calloc( sizeof(char*), (*data).now );								// Exception Word list which is including the words that user doesn't want to replace as blank.
    (*data).str[0]=(char*) calloc( sizeof(char), LOW);
    while((fgets_abc((*data).str[(*data).now-1],LOW,fp_db))!=0){				//Exception registration to exc_str[][]
		(*data).str=(char**)realloc((*data).str, sizeof(char*) * ++(*data).now );				//Reallocation for exc_str, as needed.
		(*data).str[(*data).now-1]=(char*)calloc(sizeof(char),LOW); 						//New allocation for exc_str[].
	}
}

boolean fgets_abc(char *str, unsigned int siz, FILE *fp_db){	//fgets_abc는 파일스트림으로부터 알파벳으로 이루어진 단어만 가져온다.
	unsigned int i; char c;
	for(i=0;isalpha((c=fgetc(fp_db)))&&i<siz;str[i++]=c);
	str[i]='\0';												//문자열 끝에 '\0' NULL 문자 삽입
	return (c!=EOF ? TRUE : FALSE);								//파일의 끝인지를 판별.
}
