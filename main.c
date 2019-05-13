/*************************************************************************************************************
���� �̸�: main.c
�ֿ� �Լ�: int main(int argc, char *argv[]);
�ֿ� ���: BlankLearner�� ����κ����μ�, BlankGenerator()�� Screen()�� �����Ͽ� �н��⸦ ����.
ó�� ����: �ո� ��ĭ�� �ҷ��ͼ� screen()���� �н��⸦ �����ϵ�, Menu()�� �پ��� ����� ������ �� �ֵ��� ����.
������: ���ر�, �ڳ���, �����
*************************************************************************************************************/
#include "header.h"
#include <dirent.h>

inline void Information(void);
int choose_File(void);
char Menu(unsigned int cnt); // Menu()�� Screen()�� ���� �� � ����� �������� �Է¹��� �Լ�.
inline void StarLine(unsigned int i);
void Review(FILE *fp,unsigned int *pcnt,unsigned int *esc_i); // ���� ���� �������ִ� �Լ�.

extern int Screen(FILE *fp,unsigned int cnt, unsigned int *esc_i,wdif* word_); // �ܼ� â�� ���� �ؽ�Ʈ�� �ҷ��鿩�� ������ִ� �Լ�
extern int BlankGenerator(FILE *fp,unsigned int* pcnt,wdif** pword); // ���� ������ �о� ��ĭ�� �������ִ� �Լ�
extern void custom_blank(FILE *fp,unsigned int* pcnt,wdif** pword); // �������� ����ڰ� ��ĭ�� �����ϵ��� ���� �Լ�.
extern int HellGate(FILE *fp,unsigned int* pcnt,wdif** pword); // �����Ʈ ����

char txtfiles[100][70];//������ �ִ� ũ��� 70����Ʈ, �ִ� ���� �� 100��
char query[135];
char output_file_name[32];
struct tm *t;
time_t timer;

wdif *word;
unsigned int sco_stat=0; // Blanked word count �ո� ��ĭ ī��Ʈ

int main(int argc, char *argv[])
{
    FILE *fp,*fp_db;
    unsigned int cnt=0,esc_i=0;
    int c=0;

    WHITE
    START:
    Information(); // �ȳ����� �� ����. main�Լ� �Ʒ��ʿ� �����Ǿ� �ִ�.

    if((fp=fopen("Temp\\Temp.txt","r+"))==NULL)
    {							                 // ���� ������ ���� Temp.txt�� ������
    	system("mkdir Temp");
    	CLEAR                    //Temp.txt�� ���ٴ� ���� Temp ������ ��°�� ���� ����� ���̶�� �����ؼ� �̷� �ڵ带 �� ���ϴ�.
    	if(choose_File()==-1)
            goto START;
    	system("copy /Y Temp\\Article.txt Temp\\Temp.txt"); //Article���� ���� �ҷ�����
		fp=fopen("Temp\\Temp.txt","r+");
		if(BlankGenerator(fp,&cnt,&word)==-1)   //blankgenerator.c �ȿ��� ������ ����� -1�� ��ȯ�˴ϴ�.
		{
		    fclose(fp);

            printf("* BlankLearner�� �̿����ּż� �����մϴ�. ���� ���α׷��� ������ ���� ����ȭ�ϰ��� �ϴ� Temp ���丮 ������ �������ֽñ� �ٶ��ϴ�. Y Ȥ�� y�� �Է��Ͻø� �˴ϴ�. *\n");
            system("rmdir /s Temp");
            goto START;
		}
	}
	else
	{											    // Temp.txt�� ���������� �ҷ�����
		fp_db=fopen("Temp\\wddat.dat","rb");		// log.dat���κ���
		fread(&cnt,sizeof(unsigned int),1,fp_db);	// cnt�� �о����,
		if(cnt==0)                                  // ���α׷��� ƨ��鼭 (Exception.txt�� ã�� �� ���ٰ� �Ѵ�. �������� ���� �ߵǰ� �־��µ��� �ұ��ϰ�!) �ƹ��� �ܾ�� ��ĭ�� �� �ո��� ���װ� �����Ѵ�. �װ��� ���ƺ����� ���̴�.
        {
            fclose(fp);
            fclose(fp_db);
            printf("* BlankLearner�� �̿����ּż� �����մϴ�. ���� ���α׷��� ������ ���� ����ȭ�ϰ��� �ϴ� Temp ���丮 ������ �������ֽñ� �ٶ��ϴ�. Y Ȥ�� y�� �Է��Ͻø� �˴ϴ�. *\n");
            system("rmdir /s Temp");
            goto START;
        }
		word=(wdif*)calloc(sizeof(wdif),cnt);
		fseek(fp_db,5,SEEK_SET);
		fread(word,sizeof(wdif),cnt,fp_db);			// word[cnt]�� �����´�.
		fclose(fp_db);
	}

	if( (sco_stat=Screen(fp,cnt,&esc_i,word)) == 1 )
        Review(fp,&cnt,&esc_i); 	// ���� �� ���߸� �������� ���θ� ������� ������ ����.
    else if(sco_stat==2)
        goto START; 								// �ܾ Ʋ���� �ʰ� ��� �¾Ұų� ������ �ź����� �� ����ȭ������ ������.

	while((c=Menu(cnt))!=ENTER){					//Menu()���� ENTER�� ������ ���α׷� ����
        if(c=='f'||c=='F'){
            CLEAR fputs("�� �� �Է� (1~5�� ����): ",stdout); // ��ĭ�� �󵵼��� �ٽ� ����.
            c=(Ask(5,'1','2','3','4','5')-'0');
            fp_db=fopen("Temp\\userdata.txt","r+");
            fprintf(fp_db,"%d",c);
            fclose(fp_db);
			c='g'; 					//�󵵼��� ����Ǹ� �ٽ� ��ĭ�� �վ�� ����ǹǷ� ��ĭ �����...
        }
        if(c=='t'||c=='T'){
            CLEAR
            if(choose_File()!=-1)
                c='g';
        }
        if(c=='g'||c=='G'){			//��ĭ�� �� ����.
            fclose(fp);
            system("copy /Y Temp\\Article.txt Temp\\Temp.txt");  //Article.txt�κ��� �ٽ� �ҷ��鿩�ͼ�
            fp=fopen("Temp\\Temp.txt","r+");
            if(BlankGenerator(fp,&cnt,&word)==-1) // ��ĭ �ձ�
            {
                fclose(fp);
                fclose(fp_db);
                return 0;
            }
            esc_i=0; // Ŀ�� ��ġ�� �⺻ ��ġ�� �ʱ�ȭ
        }
        else if(c=='c'||c=='C'){
            CLEAR
            if(choose_File()!=-1){
                system("copy /Y Temp\\Temp.txt Temp\\Temp2.txt");
                system("copy /Y Temp\\Article.txt Temp\\Temp.txt");  //Article.txt�κ��� �ٽ� �ҷ��鿩�ͼ�
                fp=fopen("Temp\\Temp.txt","r+");

                custom_blank(fp,&cnt,&word);
                esc_i=0;
            }
		}
        else if(c=='w'||c=='W'){ // �ո� ��ĭ�� ������ ��� �ؽ�Ʈ ���Ϸ� ����Ѵ�.
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
                if(c==MARK) {				//Temp.txt���� �ܾ��� �������� �˱����� MARK '|'�� ����ߴµ�
                    fseek(fp,-1,SEEK_CUR);
                    putc('_',fp);			//������� �� ���� ���ϰ� �ʿ䵵 ������, ���ֹ�����.
                    fseek(fp,0,SEEK_CUR);
                }
            puts("\nOutput ������ Ȯ�����ּ���.");
            fclose(fp);
            fp=fopen("Temp\\Temp.txt","r+");
            PAUSE
            continue; // �ٽ� �޴��� ���ư���.
        }
        else if(c=='r'||c=='R') {
            Review(fp,&cnt,&esc_i);
        } 							// �������. Review()�� ����.
        else if(c==F1) {
            Tutorial_Long();
            continue;
        }                           // ���� ����. ���� �� ���ķ� �ٽ� �޴��� ���ư���.
        if(c==F2) {
            CLEAR
            if(choose_File()!=-1){
                system("copy /Y Temp\\Article.txt Temp\\Temp.txt");  //Article.txt�κ��� �ٽ� �ҷ��鿩�ͼ�
                fp=fopen("Temp\\Temp.txt","r+");
                HellGate(fp,&cnt,&word);
                esc_i=0;
            }
        }
        if( (sco_stat=Screen(fp,cnt,&esc_i,word)) == 1 )
            Review(fp,&cnt,&esc_i); 	// ���� �� ���߸� �������� ���θ� ������� ������ ����.
        else if(sco_stat==2)
            goto START; 								// �ܾ Ʋ���� �ʰ� ��� �¾Ұų� ������ �ź����� �� �׳� ����ȭ������ ������.
    }
    END:
    free(word); fclose(fp);
    puts("\n���α׷��� �����մϴ�."); PAUSE
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
        {					// �ѹ��� ���� �ܾ���� ��ĭ�� �վ� �ٽ� ������ �ʿ䰡 ���
            fseek(fp,word[i].loc,SEEK_SET); // �׳� Temp.txt�� ��ĭ�� ä���ִ´�.
            fputs(word[i].str,fp);
        }
        else
        {								// �ѹ��̶� Ʋ�Ȱų� ��Ʈ�� �� �ܾ���,
            nword[count].loc=word[i].loc;	// word[cnt]�� ������ �����Ѵ�.
            nword[count].len=word[i].len;
            nword[count].pos=word[i].pos;
            strcpy(nword[count].str,word[i].str);
            nword[count].show=nword[count].res=nword[count].wrongstr[0]=0;
            nword=(wdif*)realloc(nword,sizeof(wdif)*(++count+1));
        }
    }

    *pcnt=count;
    free(word); 									// ������ word�� �Ҹ��Ű�� nword�� ���� word��.
    word=nword; CLEAR
	printf("������ %d���� �ܾ��:\n",count); 		//������ �ܾ�鵵 ǥ���ϰ�

    for(i=0;i<count;++i)
        printf("%s\n",word[i].str);

    printf(":������ %d���� �ܾ��\n",count);

    FILE *fp_db=fopen("Temp\\wddat.dat","wb"); 		// wddat�� word ����ü�� ������ �ִ´�.
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

    if (dr == NULL)  // ���丮�� �� ������ ������ ǥ���Ѵ�.
    {
        CLEAR
        printf("File ������ �����ϴ�. �ٽ� �ѹ� Ȯ�����ּ���.\n" );
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
        printf("* �н��� �� �ִ� ������ File ���� �ȿ� �������� �ʽ��ϴ�. ������ ����� �־����� �ٽ� Ȯ�����ּ���. *\n"); PAUSE
        return -1;
    }

    StarLine(80);
    printf("* �н��� ������ �������ּ��� *\n");
    for(c=0;c<file_cnt;c++)
        printf("\t%s\n",txtfiles[c]);

    PRINT:
    GotoXY(cursor);
    printf("        ");
    cursor.Y=point+2;
    GotoXY(cursor);
    printf("[�н�>");
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
	//Article.txt�� ������ �־���´�.
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

    printf("���� ���� %d��, ������ ���� %d��, Ʋ�� �ܾ� �Ǵ� ������ ���� %d��\n",cor,fix,cnt-cor-fix);

    fputs("\n�޴�\n- ESC : �н���� �ٽ� ���ư��ϴ�.\n- ENTER : ���α׷��� �����մϴ�.\n- F1 Ȥ�� ; : ������ ���ϴ�.\n- F2 : �����Ʈ ����\n- T : �н��� ���� ������ �ٽ� ���ϴ�.\n- C : ����ڰ� ��ĭ�� ������ �ܾ ���� �����մϴ�.\n- F : ��ĭ�� �����Ǵ� �󵵸� �缳���մϴ�.\n- G : ��ĭ�� ������մϴ�.\n- W : ��ĭ�� ������ ������ �ؽ�Ʈ ���Ͽ� ����մϴ�.\n- R : Ʋ�� �ܾ �����մϴ�.\n",stdout);
    printf("[Ű�� �Է��ϼ���> : ");
    return Ask(16,ESC,ENTER,F1,F2,'t','T','c','C','f','F','g','G','w','W','r','R');
}

inline void Tutorial(void)
{
    CLEAR
    puts("[�н�â������ â ũ�� ������ �Ұ����մϴ�]");
	printf("%7s : %15s\n%7s : %15s\n", "��", "���� �ܾ�� �̵�","TAB, ��", "������ �ܾ�� �̵�");
    printf("%7s : %15s\n%7s : %15s\n%7s : %15s\n", "ENTER", "ä��", "SPACE", "��Ʈ On/Off", "ESC", "�޴�");
    PAUSE
}

void Tutorial_Long(void)
{
    CLEAR
    RED
    puts("* �⺻ ���۹� *");
    WHITE
	printf("%7s : %15s\n%7s : %15s\n", "��", "���� �ܾ�� �̵�","TAB, ��", "������ �ܾ�� �̵�");
    printf("%7s : %15s\n%7s : %15s\n%7s : %15s\n\n", "ENTER", "ä��", "SPACE", "��Ʈ On/Off", "ESC", "�޴�");

    RED
    puts("* ���� ��� ���� *");
    GREEN puts("1. �Ϲ� ���"); WHITE
    puts("BlankLearner�� �������� ���ް�! �׳� Enter�� ��� ������ �ٷ� ���ɴϴ�. ������ �󵵿� ���� ��ĭ�� �����մϴ�. ���۹��� ���� '�⺻ ���۹�'�� �����ֽ��ϴ�. �빮�ڷ� �����ϴ� �ܾ�, �ѱ���¥�� �ܾ�, ���� ����� ��ĭ�� �� �� �����ϴ�. ������ ��Ʈ ���� �ٷ� ���߽ø� 2��, ��Ʈ ���� ���߸� 1���� ����ϴ�. ��Ʈ ���� ���� �Է��Ѵٸ� ���̻� ������ �ʿ䰡 ���ٰ� �Ǵ��Ͽ� ���α׷��� �ٷ� ����ȴٰ� �ϴ� �̾� �̰� �� ����� ����Դϴ�!\n");
    GREEN puts("2. Ŀ���� ���"); WHITE
    puts("��ĭ�� �ΰ��������� ������ ������? �׳� �������� �ս��ϴ�. �׷��� ���� ������ ���� ���� �� �ֽ��ϴ�. ���� ���� ��-� �˾Ƽ� ���������� ���� ���°ɱ�? �̷��� �����ϰ� ��ʴϱ�? ���̻� �������� ���� Ŀ���� ��带 �̿��սô�! �ڽ��� ��ĭ�� �����ϰ� ���� �ܾ�� ���� SPACEŰ�� ������ �����ָ� ����! ��ĭ�� �����˴ϴ�!\n");
    GREEN puts("3. �� ���"); WHITE
    puts("���� ���ؼ� ���ĺ��� �� ��ĭ�Դϴ�. �ٽ� ���� ���� �ູ�� �ð��� ������ �� �ֽ��ϴ�. �� ������ �ص� �ʹ� ������ ���! �ѹ� ��� ��ȯ�� �� �ٷ� �غ��ô�!\n");
    GREEN puts("4. ���� ���"); WHITE
    puts("��Ʈ �� ���� ���� �ܾ �����ϰ� ������ �ܾ �ٽ� ������ �� �ִ� ����Դϴ�. �н� ���α׷��̶�� �翬�� ���� ��� ������ ����� �Ѵ�- �� ���Դϴ�.\n");
    GREEN puts("5. �� �缳��"); WHITE
    puts("1���� 5������ ������ �Է��Ͽ� ��ĭ�� �����Ǵ� �󵵸� ������ �� �ֽ��ϴ�. �ٸ� ���� �Է��ϸ� ���α׷��� ������ ���� ������ �׳� ���ʿ� ����Ű�� ������ �ٷ� �Ѿ�� �ֽ� �˰����� ž���Ͽ����ϴ�.\n");
    GREEN puts("6. ��ĭ �����"); WHITE
    puts("��ĭ�� ���� �ս��ϴ�. �׿ܿ��� ���� �� ���� �����ϴ�.\n");
    GREEN puts("7. ��ĭ ���� ���"); WHITE
    puts("���� �н� ���� ������ ��ĭ�� ������ ���·� Output ���� �ȿ� �ؽ�Ʈ ���� ���·� ����մϴ�. ����ϰ� ����սô�.\n");

    RED
    puts("* �һ�翡 ��ó�ϴ� ��� *"); WHITE
    puts("- ��ĭ�� �̻��ϰ� �շ��� ���Ϳ�!");
    puts("ex) _________________________________________________________ieve that blanklearner is the best program.");
    puts("->���α׷��� �۵��ϱ� �Ѵٸ� ESCŰ�� ���� ���� GŰ�� ������ ���� ��ĭ�� ������غ��ô�. ���α׷��� �׳� ��������ٸ� Temp ���� �ȿ� �ִ� Temp.txt�� �����ع����ô�.");
    puts("- �������� �� �� ���� ������ ����ϰ� �ſ� ���°��� ��������!");
    puts("->Temp ���� �ȿ� �ִ� Temp.txt�� �����غ��ô�. ������ �ذ�ǰ� ��Ʈ������ Ǯ���� �����ϴ�.\n");

    GREEN
    puts("BlankLearner 2.0�� �̿����ּż� ����帳�ϴ�.\n���� ������ �ذ��� �� ���� ������ �߻��ϰų�, ���� �߰��ϰ� ���� ����� �ִٸ� �������� �ٴ� �ο��� ���ٷ� ���� ���Ͻø� ���� �� �Ҹ��� ��� �� ���� �ְ� ���� ���� ���� �ֽ��ϴ�.");
    WHITE
    PAUSE
}

inline void Information(void)
{
	CLEAR
	StarLine(80);
    puts("/BlankLearner 2.01\n\n\t/���� : ���ر�, �ڳ���, �����(2.0)\n\n<����>\nBlankLearner.exe�� ���� ��ο� File������ ���� �� ���� ���ϵ��� ���� �ȿ� ������ �ּ���.");
    puts("�н��� ���� ���ϵ�\nException.txt(�� ���Ͽ��� ���� ��ĭȭ ��Ű�� ���� �ܾ �Է��ϼ���)\nVocaList.txt(�� ���Ͽ��� ������ ��ĭȭ ��ų �ܾ �Է��ϼ���(��ҹ��� ����))");
    StarLine(80);
    PAUSE
    Tutorial();
}

inline void StarLine(unsigned int i)
{
    for(; i!=0; --i)
        putchar('*');
    putchar('\n');
} // i���� ���� �� �ٿ�  ����Ѵ�.

char Ask(int n,...)
{					// ���� �����Լ��̸�, ���� n�� ���� ���������� ������ �޴´�.
    char c;
    unsigned int i;

    va_list ap;//�������� �Լ� �����͸� ����
    va_start(ap,n);//���������� �������� ����

    while(1)
    {							//getch()�� Ű����κ��� �Ű������� �Է¹��� ���ڰ� �Էµ� ������ ��� �Է¹���.
        va_start(ap,n);
        c=getch();

        if(c<=0)
            c=getch();
        for(i=n;i!=0;--i)
            if(c==(char)va_arg(ap,int))
            {
                va_end(ap);
                return c;				// �Ű������� �Է¹��� ���ڰ� �ɸ��� �� ���ڸ� ��ȯ.
            }
        va_end(ap);
    }
}
