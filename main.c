/*************************************************************************************************************
파일 이름: main.c
주요 함수: int main(int argc, char *argv[]);
주요 기능: BlankLearner의 제어부분으로서, BlankGenerator()와 Screen()을 제어하여 학습기를 구현.
처리 개요: 뚫린 빈칸을 불러와서 screen()으로 학습기를 구현하되, Menu()로 다양한 기능을 실행할 수 있도록 제어.
제작자: 강준구, 박남규, 김민재
*************************************************************************************************************/
#include "header.h"
#include <dirent.h>

inline void Information(void);
int choose_File(void);
char Menu(unsigned int cnt); // Menu()는 Screen()을 나온 후 어떤 기능을 실행할지 입력받을 함수.
inline void StarLine(unsigned int i);
void Review(FILE *fp,unsigned int *pcnt,unsigned int *esc_i); // 복습 모드로 셋팅해주는 함수.

extern int Screen(FILE *fp,unsigned int cnt, unsigned int *esc_i,wdif* word_); // 콘솔 창에 본문 텍스트를 불러들여와 출력해주는 함수
extern int BlankGenerator(FILE *fp,unsigned int* pcnt,wdif** pword); // 본문 파일을 읽어 빈칸을 생성해주는 함수
extern void custom_blank(FILE *fp,unsigned int* pcnt,wdif** pword); // 수동으로 사용자가 빈칸을 생성하도록 돕는 함수.
extern int HellGate(FILE *fp,unsigned int* pcnt,wdif** pword); // 헬게이트 오픈

char txtfiles[100][70];//제목의 최대 크기는 70바이트, 최대 파일 수 100개
char query[135];
char output_file_name[32];
struct tm *t;
time_t timer;

wdif *word;
unsigned int sco_stat=0; // Blanked word count 뚫린 빈칸 카운트

int main(int argc, char *argv[])
{
    FILE *fp,*fp_db;
    unsigned int cnt=0,esc_i=0;
    int c=0;

    WHITE
    START:
    Information(); // 안내사항 및 도움말. main함수 아래쪽에 구현되어 있다.

    if((fp=fopen("Temp\\Temp.txt","r+"))==NULL)
    {							                 // 만약 이전에 쓰던 Temp.txt가 없으면
    	system("mkdir Temp");
    	CLEAR                    //Temp.txt가 없다는 것은 Temp 폴더를 통째로 지운 경우일 것이라고 생각해서 이런 코드를 쓴 듯하다.
    	if(choose_File()==-1)
            goto START;
    	system("copy /Y Temp\\Article.txt Temp\\Temp.txt"); //Article에서 새로 불러오고
		fp=fopen("Temp\\Temp.txt","r+");
		if(BlankGenerator(fp,&cnt,&word)==-1)   //blankgenerator.c 안에서 오류가 생기면 -1이 반환됩니다.
		{
		    fclose(fp);

            printf("* BlankLearner를 이용해주셔서 감사합니다. 현재 프로그램에 문제가 생겨 정상화하고자 하니 Temp 디렉토리 삭제에 동의해주시길 바랍니다. Y 혹은 y를 입력하시면 됩니다. *\n");
            system("rmdir /s Temp");
            goto START;
		}
	}
	else
	{											    // Temp.txt를 성공적으로 불려오면
		fp_db=fopen("Temp\\wddat.dat","rb");		// log.dat으로부터
		fread(&cnt,sizeof(unsigned int),1,fp_db);	// cnt를 읽어오고,
		if(cnt==0)                                  // 프로그램이 튕기면서 (Exception.txt를 찾을 수 없다고 한다. 이전까지 아주 잘되고 있었는데도 불구하고!) 아무런 단어에도 빈칸이 안 뚫리는 버그가 존재한다. 그것을 막아보려는 것이다.
        {
            fclose(fp);
            fclose(fp_db);
            printf("* BlankLearner를 이용해주셔서 감사합니다. 현재 프로그램에 문제가 생겨 정상화하고자 하니 Temp 디렉토리 삭제에 동의해주시길 바랍니다. Y 혹은 y를 입력하시면 됩니다. *\n");
            system("rmdir /s Temp");
            goto START;
        }
		word=(wdif*)calloc(sizeof(wdif),cnt);
		fseek(fp_db,5,SEEK_SET);
		fread(word,sizeof(wdif),cnt,fp_db);			// word[cnt]를 가져온다.
		fclose(fp_db);
	}

	if( (sco_stat=Screen(fp,cnt,&esc_i,word)) == 1 )
        Review(fp,&cnt,&esc_i); 	// 답을 다 맞추면 복습할지 여부를 물어봐서 복습할 거임.
    else if(sco_stat==2)
        goto START; 								// 단어를 틀리지 않고 모두 맞았거나 복습을 거부했을 땐 시작화면으로 보낸다.

	while((c=Menu(cnt))!=ENTER){					//Menu()에서 ENTER가 눌리면 프로그램 종료
        if(c=='f'||c=='F'){
            CLEAR fputs("빈도 수 입력 (1~5의 정수): ",stdout); // 빈칸의 빈도수를 다시 설정.
            c=(Ask(5,'1','2','3','4','5')-'0');
            fp_db=fopen("Temp\\userdata.txt","r+");
            fprintf(fp_db,"%d",c);
            fclose(fp_db);
			c='g'; 					//빈도수가 변경되면 다시 빈칸을 뚫어야 적용되므로 빈칸 재생성...
        }
        if(c=='t'||c=='T'){
            CLEAR
            if(choose_File()!=-1)
                c='g';
        }
        if(c=='g'||c=='G'){			//빈칸을 재 생성.
            fclose(fp);
            system("copy /Y Temp\\Article.txt Temp\\Temp.txt");  //Article.txt로부터 다시 불러들여와서
            fp=fopen("Temp\\Temp.txt","r+");
            if(BlankGenerator(fp,&cnt,&word)==-1) // 빈칸 뚫기
            {
                fclose(fp);
                fclose(fp_db);
                return 0;
            }
            esc_i=0; // 커서 위치도 기본 위치로 초기화
        }
        else if(c=='c'||c=='C'){
            CLEAR
            if(choose_File()!=-1){
                system("copy /Y Temp\\Temp.txt Temp\\Temp2.txt");
                system("copy /Y Temp\\Article.txt Temp\\Temp.txt");  //Article.txt로부터 다시 불러들여와서
                fp=fopen("Temp\\Temp.txt","r+");

                custom_blank(fp,&cnt,&word);
                esc_i=0;
            }
		}
        else if(c=='w'||c=='W'){ // 뚫린 빈칸이 마음에 들면 텍스트 파일로 출력한다.
            fclose(fp);
            CLEAR
            strcpy(query,"");
            strcat(query,"copy /Y Temp\\Temp.txt ");
            timer=time(NULL);
            t=localtime(&timer);
            sprintf(output_file_name, "Output\\Output%04d%02d%02d%02d%02d%02d.txt",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
            strcat(query,output_file_name);
            system("mkdir Output");
            system(query);
            fp=fopen(output_file_name,"r+");
            while((c=fgetc(fp))!=EOF)
                if(c==MARK) {				//Temp.txt에는 단어의 시작점을 알기위해 MARK '|'를 사용했는데
                    fseek(fp,-1,SEEK_CUR);
                    putc('_',fp);			//출력해줄 땐 보기 흉하고 필요도 없으니, 없애버린다.
                    fseek(fp,0,SEEK_CUR);
                }
            puts("\nOutput 폴더를 확인해주세요.");
            fclose(fp);
            fp=fopen("Temp\\Temp.txt","r+");
            PAUSE
            continue; // 다시 메뉴로 돌아가기.
        }
        else if(c=='r'||c=='R') {
            Review(fp,&cnt,&esc_i);
        } 							// 복습모드. Review()로 셋팅.
        else if(c==F1) {
            Tutorial_Long();
            continue;
        }                           // 도움말 보기. 도움말 본 이후론 다시 메뉴료 돌아간다.
        if(c==F2) {
            CLEAR
            if(choose_File()!=-1){
                system("copy /Y Temp\\Article.txt Temp\\Temp.txt");  //Article.txt로부터 다시 불러들여와서
                fp=fopen("Temp\\Temp.txt","r+");
                HellGate(fp,&cnt,&word);
                esc_i=0;
            }
        }
        if( (sco_stat=Screen(fp,cnt,&esc_i,word)) == 1 )
            Review(fp,&cnt,&esc_i); 	// 답을 다 맞추면 복습할지 여부를 물어봐서 복습할 거임.
        else if(sco_stat==2)
            goto START; 								// 단어를 틀리지 않고 모두 맞았거나 복습을 거부했을 땐 그냥 시작화면으로 보낸다.
    }
    END:
    free(word); fclose(fp);
    puts("\n프로그램을 종료합니다."); PAUSE
	return 0;
}

void Review(FILE *fp,unsigned int* pcnt,unsigned int* esc_i){
    unsigned int i,count;
    wdif *nword; //new word info
    sco_stat=*esc_i=0;
    nword=(wdif*)calloc(sizeof(wdif),1);
    for(i=count=0;i<*pcnt;++i)
    {
        if(word[i].res==2)
        {					// 한번에 맞은 단어들은 빈칸을 뚫어 다시 문제낼 필요가 없어서
            fseek(fp,word[i].loc,SEEK_SET); // 그냥 Temp.txt의 빈칸을 채워넣는다.
            fputs(word[i].str,fp);
        }
        else
        {								// 한번이라도 틀렸거나 힌트를 본 단어라면,
            nword[count].loc=word[i].loc;	// word[cnt]의 정보를 복사한다.
            nword[count].len=word[i].len;
            nword[count].pos=word[i].pos;
            strcpy(nword[count].str,word[i].str);
            nword[count].show=nword[count].res=nword[count].wrongstr[0]=0;
            nword=(wdif*)realloc(nword,sizeof(wdif)*(++count+1));
        }
    }

    *pcnt=count;
    free(word); 									// 기존의 word는 소멸시키고 nword의 값을 word에.
    word=nword; CLEAR
	printf("복습할 %d개의 단어들:\n",count); 		//복습할 단어들도 표시하고

    for(i=0;i<count;++i)
        printf("%s\n",word[i].str);

    printf(":복습할 %d개의 단어들\n",count);

    FILE *fp_db=fopen("Temp\\wddat.dat","wb"); 		// wddat에 word 구조체의 정보도 넣는다.
	fseek(fp_db,0,SEEK_SET);
	fwrite(&count,sizeof(unsigned int),1,fp_db);
	fseek(fp_db,5,SEEK_SET);
	fwrite(word,sizeof(wdif),count,fp_db);
	fclose(fp_db);

    PAUSE
    return;
}

int choose_File()
{
    struct dirent *de;
    char file_cnt=0;
    char point=0;
    char c;
    COORD cursor={0,2};
    DIR *dr = opendir("\File");

    if (dr == NULL)  // 디렉토리가 안 열리면 오류를 표시한다.
    {
        CLEAR
        printf("File 폴더가 없습니다. 다시 한번 확인해주세요.\n" );
        PAUSE
        return -1;
    }

    while ((de = readdir(dr)) != NULL)
    {
        if(!strcmp(de->d_name, "."))
            continue;
        else if(!strcmp(de->d_name, ".."))
            continue;
        else if(!strcmp(de->d_name, "Exception.txt"))
            continue;
        else if(!strcmp(de->d_name, "VocaList.txt"))
            continue;
        strcpy(txtfiles[file_cnt++], de->d_name);
    }
    closedir(dr);

    if(file_cnt==0)
    {
        printf("* 학습할 수 있는 파일이 File 폴더 안에 존재하지 않습니다. 파일을 제대로 넣었는지 다시 확인해주세요. *\n"); PAUSE
        return -1;
    }

    StarLine(80);
    printf("* 학습할 파일을 선택해주세요 *\n");
    for(c=0;c<file_cnt;c++)
        printf("\t%s\n",txtfiles[c]);

    PRINT:
    GotoXY(cursor);
    printf("        ");
    cursor.Y=point+2;
    GotoXY(cursor);
    printf("[학습>");
    while((c=getch())!=ENTER)
    {
        switch(c)
        {
        case UP:
            if(point!=0)
                point--;
            break;
        case DOWN:
            if(point!=file_cnt-1)
                point++;
            break;
        }
        goto PRINT;
    }

    strcpy(query,"");
    strcat(query, "copy /Y \"File\\"); strcat(query, txtfiles[point]); strcat(query, "\" Temp\\Article.txt");
	//Article.txt에 본문을 넣어놓는다.
	system(query);

    return 0;
}

char Menu(unsigned int cnt){
    unsigned int i,cor,fix;

    if(sco_stat==2)
        return ENTER;
	CLEAR

	for(i=cor=fix=0;i<cnt;++i)
    {
        if(word[i].res==2) ++cor;
        else if(word[i].res==1) ++fix;
    }

    printf("맞은 갯수 %d개, 수정한 갯수 %d개, 틀린 단어 또는 미제출 갯수 %d개\n",cor,fix,cnt-cor-fix);

    fputs("\n메뉴\n- ESC : 학습기로 다시 돌아갑니다.\n- ENTER : 프로그램을 종료합니다.\n- F1 혹은 ; : 도움말을 봅니다.\n- F2 : 헬게이트 오픈\n- T : 학습할 본문 파일을 다시 고릅니다.\n- C : 사용자가 빈칸을 생성할 단어를 직접 지정합니다.\n- F : 빈칸이 생성되는 빈도를 재설정합니다.\n- G : 빈칸을 재생성합니다.\n- W : 빈칸이 생성된 본문을 텍스트 파일에 출력합니다.\n- R : 틀린 단어를 복습합니다.\n",stdout);
    printf("[키를 입력하세요> : ");
    return Ask(16,ESC,ENTER,F1,F2,'t','T','c','C','f','F','g','G','w','W','r','R');
}

inline void Tutorial(void)
{
    CLEAR
    puts("[학습창에서는 창 크기 조절이 불가능합니다]");
	printf("%7s : %15s\n%7s : %15s\n", "←", "왼쪽 단어로 이동","TAB, →", "오른쪽 단어로 이동");
    printf("%7s : %15s\n%7s : %15s\n%7s : %15s\n", "ENTER", "채점", "SPACE", "힌트 On/Off", "ESC", "메뉴");
    PAUSE
}

void Tutorial_Long(void)
{
    CLEAR
    RED
    puts("* 기본 조작법 *");
    WHITE
	printf("%7s : %15s\n%7s : %15s\n", "←", "왼쪽 단어로 이동","TAB, →", "오른쪽 단어로 이동");
    printf("%7s : %15s\n%7s : %15s\n%7s : %15s\n\n", "ENTER", "채점", "SPACE", "힌트 On/Off", "ESC", "메뉴");

    RED
    puts("* 세부 기능 설명 *");
    GREEN puts("1. 일반 모드"); WHITE
    puts("BlankLearner의 알파이자 오메가! 그냥 Enter만 계속 누르면 바로 나옵니다. 설정된 빈도에 따라 빈칸을 형성합니다. 조작법은 위에 '기본 조작법'에 나와있습니다. 대문자로 시작하는 단어, 한글자짜리 단어, 숫자 등등은 빈칸이 될 수 없습니다. 문제를 힌트 없이 바로 맞추시면 2점, 힌트 보고 맞추면 1점을 얻습니다. 힌트 없이 전부 입력한다면 더이상 공부할 필요가 없다고 판단하여 프로그램이 바로 종료된다고 하니 이야 이것 참 대단한 기능입니다!\n");
    GREEN puts("2. 커스텀 모드"); WHITE
    puts("빈칸을 인공지능으로 뚫을리 없겠죠? 그냥 무작위로 뚫습니다. 그런데 뭔가 마음에 들지 않을 수 있습니다. 뭔가 나의 니-즈를 알아서 만족시켜줄 수는 없는걸까? 이렇게 생각하고 계십니까? 더이상 망설이지 말고 커스텀 모드를 이용합시다! 자신이 빈칸을 생성하고 싶은 단어로 가서 SPACE키를 살포시 눌러주면 세상에! 빈칸이 생성됩니다!\n");
    GREEN puts("3. 헬 모드"); WHITE
    puts("쉽게 말해서 알파벳은 다 빈칸입니다. 다시 말해 정말 행복한 시간을 보내실 수 있습니다. 아 생각만 해도 너무 상쾌한 기분! 한번 기분 전환할 겸 바로 해봅시다!\n");
    GREEN puts("4. 복습 모드"); WHITE
    puts("힌트 안 보고 맞춘 단어를 제외하고 나머지 단어를 다시 공부할 수 있는 기능입니다. 학습 프로그램이라면 당연히 복습 기능 정도는 갖춰야 한다- 이 말입니다.\n");
    GREEN puts("5. 빈도 재설정"); WHITE
    puts("1부터 5까지의 정수를 입력하여 빈칸이 생성되는 빈도를 조절할 수 있습니다. 다른 값을 입력하면 프로그램이 망가질 수도 있으니 그냥 애초에 숫자키를 누르면 바로 넘어가는 최신 알고리즘을 탑재하였습니다.\n");
    GREEN puts("6. 빈칸 재생성"); WHITE
    puts("빈칸을 새로 뚫습니다. 그외에는 딱히 할 말이 없습니다.\n");
    GREEN puts("7. 빈칸 파일 출력"); WHITE
    puts("현재 학습 중인 본문을 빈칸이 생성된 상태로 Output 폴더 안에 텍스트 파일 형태로 출력합니다. 요긴하게 사용합시다.\n");

    RED
    puts("* 불상사에 대처하는 방법 *"); WHITE
    puts("- 빈칸이 이상하게 뚫려서 나와요!");
    puts("ex) _________________________________________________________ieve that blanklearner is the best program.");
    puts("->프로그램이 작동하긴 한다면 ESC키를 누른 다음 G키를 살포시 눌러 빈칸을 재생성해봅시다. 프로그램이 그냥 멈춰버린다면 Temp 폴더 안에 있는 Temp.txt를 삭제해버립시다.");
    puts("- 뭐가뭔지 알 수 없고 가슴이 답답하고 매우 무력감이 느껴져요!");
    puts("->Temp 폴더 안에 있는 Temp.txt를 삭제해봅시다. 문제도 해결되고 스트레스도 풀리고 좋습니다.\n");

    GREEN
    puts("BlankLearner 2.0을 이용해주셔서 감사드립니다.\n만약 도무지 해결할 수 없는 문제가 발생하거나, 뭔가 추가하고 싶은 기능이 있다면 지나가던 바다 부원을 밧줄로 묶어 고문하시면 제가 그 소리를 듣고 갈 수도 있고 가지 않을 수도 있습니다.");
    WHITE
    PAUSE
}

inline void Information(void)
{
	CLEAR
	StarLine(80);
    puts("/BlankLearner 2.01\n\n\t/제작 : 강준구, 박남규, 김민재(2.0)\n\n<사용법>\nBlankLearner.exe와 같은 경로에 File폴더를 만든 뒤 다음 파일들을 폴더 안에 저장해 주세요.");
    puts("학습할 본문 파일들\nException.txt(이 파일에는 절대 빈칸화 시키지 않을 단어를 입력하세요)\nVocaList.txt(이 파일에는 무조건 빈칸화 시킬 단어를 입력하세요(대소문자 구분))");
    StarLine(80);
    PAUSE
    Tutorial();
}

inline void StarLine(unsigned int i)
{
    for(; i!=0; --i)
        putchar('*');
    putchar('\n');
} // i개의 별을 한 줄에  출력한다.

char Ask(int n,...)
{					// 가변 인자함수이며, 변수 n은 뒤의 가변인자의 갯수를 받는다.
    char c;
    unsigned int i;

    va_list ap;//가변인자 함수 포인터를 선언
    va_start(ap,n);//가변인자의 시작점을 설정

    while(1)
    {							//getch()로 키보드로부터 매개변수로 입력받은 문자가 입력될 때까지 계속 입력받음.
        va_start(ap,n);
        c=getch();

        if(c<=0)
            c=getch();
        for(i=n;i!=0;--i)
            if(c==(char)va_arg(ap,int))
            {
                va_end(ap);
                return c;				// 매개변수로 입력받은 문자가 걸리면 그 문자를 반환.
            }
        va_end(ap);
    }
}
