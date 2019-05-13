/************************************************************************************************************
���� �̸�: custom_blank.c
�ֿ� �Լ�: void custom_blank(FILE *fp,unsigned int* pcnt,wdif** pword);
�ֿ� ���: 1. ������ �ҷ��� �ֿܼ� ���.  2. SPACE�� ����ڿ��� ��ĭ ���θ� ����.
�Է� ���: fp�� ���� �޾ƿ���, cnt�� word�� ���� cnt�� word�� �����ͷ� ��ȯ�����ش�.
Ư ¡ : blankgenerator�� screen�� «�� ������.
������: ���ر�, �ڳ���, �����
************************************************************************************************************/

#include "header.h"

enum {Out=0,In}; // �����Ͱ� ���� �ܾ ����Ű�� �ִ��� �ƴ���.

typedef struct{ // wdif�� �뷫������ ���������� �ʿ��� ������ ��� ���ݾ� �޶� �� ����ü �ڷ��� ����.
	char str[LOW]; // �ܾ� ����
	unsigned int len,loc; // �ܾ��� ���̿� ���������� ���� �ܾ� ��ġ
	boolean bool_blank; // �ܾ��� �� ĭ ����
	COORD Pos; // �ܾ��� �ֻܼ� ��ǥ.
}wordinfo;

void SetBlank(FILE* fp,unsigned int n,wordinfo* word); // �ֻܼ󿡼� �ܾ� ǥ�ø� �缳��. (�ձ� �Ǵ� �ձ� ���.)

void custom_blank(FILE *fp,unsigned int* pcnt,wdif** pword){
    FILE *fp_db;
	unsigned int cnt,len,cnt_,cnt_b;
	char c; int i;
	boolean STATE=Out;

	wdif *word_=(wdif*)calloc(1,sizeof(wdif));
	wordinfo *word=(wordinfo*)calloc(1,sizeof(wordinfo));

	CLEAR puts("����� ���� ��ĭ ������.\n���� ����Ű(��), ������ ����Ű(��)�� ���� ��ĭ���� ����� ���� �ܾ�� �̵��ϼ���.\nSPACE�� ���� ��ĭ�� ��������.\n��ĭ�� �� �հ��� ESC�� ������ �ڵ����� �н��� ���۵˴ϴ�.\n"); PAUSE CLEAR
	fseek(fp,0,SEEK_SET);
	for(cnt=len=0;(c=fgetc(fp))!=EOF;){//ó���� �ؽ�Ʈ ������ ����ϴ� �뵵�̴�.
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

	while((c=getch())!=ESC){//�ؽ�Ʈ ���� ��� �� Ű ����
		switch (c){
			case LEFT: mvword(&i,-1,cnt); break; //���� ���� ȭ��ǥ Ű�� ������ ��� Ŀ���� �� �ܾ�� �̵���Ų��.
			case RIGHT: mvword(&i,1,cnt); break; //���� ������ ȭ��ǥ Ű�� ������ ��� Ŀ���� �ڿ� �ִ� �ܾ�� �̵���Ų��.
			case TAB: mvword(&i,+10,cnt); break; //�� Ű�� ������ ��� 10�� ������ �̵��Ѵ�.
			case SPACE: SetBlank(fp,i,word); break; //�����̽�Ű�� ������ ��� Blank ���� ������ ���� �����ϰ� ������ ������Ų��.
		}
		GotoXY(word[i].Pos);
	}//ESCŰ ������ ��� Ŀ���� ��带 �����Ѵ�.

	for(i=cnt_=cnt_b=0;i<cnt;++i){//�ű��.
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

	fp_db=fopen("Temp\\wddat.dat","wb"); // ���߿� �ٽ� ��ĭ �� �հ� ��Ȱ�� �Ҷ� ��Ȱ�� �� �� �ֵ���,
	fseek(fp_db,0,SEEK_SET);
	fwrite(&cnt_,sizeof(unsigned int),1,fp_db); //cnt��
	fseek(fp_db,5,SEEK_SET);
	fwrite(word_,sizeof(wdif),cnt_,fp_db);	// word ����ü�迭�� �������ش�.
	fclose(fp_db);

	*pcnt=cnt_;//��ĭ�� �հ� �� �� ��ĭ�� ���� �ܾ��� ���� �����ͷ� �����մϴ�.
	*pword=word_;//��ĭ�� ���� �ܾ��� ������ �����մϴ�.
	return;
}

void SetBlank(FILE* fp, unsigned int n,wordinfo* word){
	unsigned int i;
	fseek(fp,word[n].loc,SEEK_SET);
	GotoXY(word[n].Pos);
	if(word[n].bool_blank){					// �̹� �վ������
    	fputs(word[n].str,fp);
		fputs(word[n].str,stdout);			// �ٽ� ������� ������Ų��.
	}
	else{									// �� �շ� ������
		putc(MARK,fp); putc('_',stdout);
		for(i=word[n].len-1;i!=0;--i){
			putc('_',fp);
			putc('_',stdout);				// ����.
		}
	}
	GotoXY(word[n].Pos);					// �ܼ� ��ġ ����.
	word[n].bool_blank=!word[n].bool_blank;	// �ո� ���� �ٽ� �ݿ�.
}
