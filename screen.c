/************************************************************************************************************
���� �̸�: screen.c
�ֿ� �Լ�: int Screen(FILE *fp, unsigned int cnt, unsigned int *esc_i,wdif* word_);
�ֿ� ���: �н��⿡�� ���� ���� ������ �ֿܼ� ����ְ� �������� �˷��ִ� �κ�.
��� ����: sco_stat�� ����� ��. (��ĭ�� ��� ���߾����� main() ����ο��� Ȯ���ϴ� �뵵.)
ó�� ����: fp�� �ִ� ������ �޾ƿ� cnt�� word�� ���ؼ� �м��Ͽ� ǥ��/ esc_i�� Ŀ�� ��ġ ���� �� �ҷ����� �뵵
������: ���ر�, �ڳ���, �����
************************************************************************************************************/
#include "header.h"

void InitBlank(unsigned int n); // ��ĭ�� �����ִ� ������ �������� �����Ѵ�.
inline void Adjust(unsigned int n);	// ���߿� Screen()�� �ٽ� �ҷ�������, ������ �Է��ߴ� ���� ���� ��Ȳ�� ǥ�����ش�.
inline void mvword(int *i,int mov,unsigned int cnt); // Ŀ���� �ܾ������ �̵��� ��.
void Backspace(char* str, unsigned int* i); //Scanner���� backspace�� ��������, Ŀ���� �̵���Ŵ�� ���ÿ� �ѱ��ھ� ����� ����.
char Scanner(char *str, unsigned int siz,int n); //��ĭ�� �ܾ �޴� ����.

wdif *word; //wdif�� ��ĭ �ܾ� ���� ���� header.h ����
unsigned int width; //Console width �ܼ� â �ʺ�

int Screen(FILE *fp, unsigned int cnt, unsigned int *esc_i,wdif* word_){
	char c,get[LOW];
	unsigned int tot=0;
    int i,mov=1;
    COORD Pos, set={0,0};

    HWND consoleWindow = GetConsoleWindow();
    SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX); // ���� ����ϱ��� ȭ�� ũ�� ���� ����� ���´�.

    CONSOLE_SCREEN_BUFFER_INFO csbi; GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	width=csbi.dwSize.X; // Backspace���� �� �ܼ�â �ʺ� �޾Ƶд�.(Backspace���� �ۿ� �� ������ �� ���� ������� �Լ� �ҷ��ö����� �������� �δ㽺����.)

	word=word_; //word�� ������ ���� ���� �б⸸ �ϴ� �׳� �ּҰ� �޾ƿͼ� �� ����.
    CLEAR
    fseek(fp,0,SEEK_SET);
   	for(i=0;(c=fgetc(fp))!=EOF; putc(c,stdout))
   	{ // Temp���� ��ĭ �ո� ������ �� �����´�.
		if(c==MARK)
        {
			GetXY(word[i].pos);
			if(i++==0)
                c=word[i].str[0];
            else
                c='_';
		}
	}
	GetXY(Pos); // ������ ���� ���� �ϴ� ��ǥ ���.
	GotoXY(set);
	for(i=cnt-1;i>=0;--i)
        Adjust(i); //���߿� Screen()�� �ٽ� �ҷ�������, ������ �Է��ߴ� ���� ���� ��Ȳ�� ǥ�����ش�.

	//Hell Gate Opened.
    for(i=*esc_i;c!=ESC;)
    { // ���� �ܾ ���������� ���´� (esc_i��°) ��ġ�� ����. ESC ���������� �н��� ȭ�鿡 �ӹ�����.
        if(word[i].res>0)
        {
            mvword(&i,mov,cnt);
            continue;
        } // �̹� ���� �ܾ���� �Ѿ��.

		GotoXY(word[i].pos);				// i��° �ܾ� �Է��Ҳ� ������ ��ǥ �Űܾ���.

        c=Scanner(get,word[i].len,i); // i��°�� �� �Է��� ���� �ҷ��´�. �Է��� ������ ESC,ENTER,ȭ��ǥ ��� �� �޾Ҵ��� ��ȯ��.

        if(c==LEFT||c==RIGHT||c==TAB) { 			// Ŀ�� �̵� Ű�� �޾�����,
        	if (word[i].show==1) word[i].show=0; 	// ��Ʈ �þ����� ���ְ� ����. �޷�
			Adjust(i);					// ����ϰ� �����ϰ� ������.
			mov= c==LEFT ? -1 : 1;		// ���� ������ �������ְ�
			mvword(&i,mov,cnt);			// mov�� ���� mvword�� �̵� �����ش�.
			continue;					// ��� �ؾ���?
		}
		else if(c==ENTER) {
			GotoXY(word[i].pos); // �̸� �ٽ� ������ ó����ġ�� �� �ִ´�.

        	if(strcmp(word[i].str,get)==0) { // �¾�����
            	GREEN puts(get); WHITE		// �ʷϻ�����
        	    if(word[i].res==0) word[i].res=2; // �ѹ��� �¾����� 2��
            	else if(word[i].res<0) word[i].res=1; // �ܿ� �¾����� 1��
	            Adjust(i); mvword(&i,(mov=1),cnt); // �ٽ� �������ְ� ����ĭ���� �̵����ش�.
	            ++tot;
			}
			else{	//Ʋ������
				InitBlank(i); // �ѹ� û���ϰ�
    	      	RED puts(get); WHITE // ���������� Ʋ���ܾ� ���ֱ�.
        	  	strcpy(word[i].wrongstr,get); // wrongstr�� �������ְ� �ΰ�ΰ� ����ϱ�
          		word[i].res=-2; // Ʋ�����ϱ� -2��.
			}
			if(tot==cnt){ //�� �ʷϻ� ������,
                ++Pos.Y;  // (�Ǹ����� ���� �� ������ �ǹ�)
                GotoXY(Pos); // ������ ����
                for(i=tot=0;i<cnt;++i)
                    if(word[i].res==2)
                        ++tot; // �ѹ��� ����ܾ� ��� Ȯ���ؼ�
                if(tot==cnt)
                {
                    puts("\n��� �����̽��ϴ�! ����� õ�缺�� �����մϴ�!");
                    PAUSE
                    return 2;
                } //õ������ ����Ǹ� �������ְ�

                puts("\n������ ��� �Է��ϼ̽��ϴ�. �����Ͻðڽ��ϱ�? (y/n): "); // �װ� �ƴϸ� �׳� ����Ȯ��
                return ((c=Ask(4,'y','Y','n','N'))=='y'||c=='Y') ? 1 : 2;  //sco_stat�� ����� ��� ��ȯ
            }					//sco_stat==1�� �ѹ��̻� Ʋ�� �ܾ �����ؼ� ������ ��, sco_stat==2�� õ���̰ų� �����ϱ� ������.
		}
		else if(c==SPACE) {
            if(word[i].res==0) word[i].res=-1; // space �ѹ� ������ 1���� ���̰� ����. / res>0�϶� space�� ������ ����, res<0�϶� ���� �ǹ� ������...
            GotoXY(word[i].pos);				// �ܾ� ������ġ�� ���� �� �غ�
			if(word[i].show==0){ // state 0: Not yet.
                word[i].show=1; // state 1: Hint enabled.
                BLUE puts(word[i].str); WHITE // �Ķ������� ���� �����ֱ�
            }
			else{				//�̹� ��Ʈ Ȱ��ȭ �Ǿ� �ִµ� space ������
                word[i].show=0;	// �ǵ����ְ�
                InitBlank(i);	// ��Ʈ �ٽ� ���ְ�
                RED puts(word[i].wrongstr); WHITE // ������ ġ���� �ǻ�� �ش�.
            }
            continue; //�ܾ�� ��� �Է��ؾ���?
		}
	}
	*esc_i=i; // �������� ���° (i��°) �ܾ������ (esc_i��)�����صд�. ���߿� �ٽ� �ö� �ҷ�����.
    return 0; // �� �� �������� �׳� sco_stat=0.
}

char Scanner(char *str, unsigned int siz,int n){
	unsigned int i=0; char c; COORD Pos;

	while(1) {
        GetXY(Pos); // ���� ��ġ ����Ѵ�.
        if(word[n].pos.X==width-1) Pos.X = (Pos.X + i) % width;
		c=getch();	// ���� ���� ���� �޴´�.

		if(c<=0)
            return getch(); // RIGHT, LEFT Ű�� 2����Ʈ �����̰� ù ����Ʈ�� ������ �׳� ������ �ι�° ����Ʈ�� ������ �Ǻ��Ѵ�.
		else if(c==BACKSPACE)
            Backspace(str,&i); // Backspace ������ ���� �� �Լ��� ��.
		else if(c==ESC||c==SPACE||c==TAB||c==ENTER)
		{
		    str[i]='\0';
            return c;
        } // ESC,SPACE,TAB,ENTER�� Screen()���� ó��. ���ڿ� ���� '\0'
		else if(isalpha(c))
        {
			if(word[n].pos.X==Pos.X && word[n].pos.Y==Pos.Y)
                InitBlank(n); //�Է� ���������ϱ� ������ ������.
			if(0<=i&&i<siz)
            { // ��� ���� ���̸�,
				str[i++]=c;	// str���� �ְ�
				printf("%c",c);	// ��µ� �Ѵ�.
				word[n].show=0;
			}
		}
	}
}
void Backspace(char* str, unsigned int* i){  // Backspace Function in Scanner(). Scanner�Լ����� Backspace�� ������ ����ϴ� �Լ�.
	COORD Pos;
	GetXY(Pos);
	if(*i==0) return;               // ���� ĭ�� ������ �״�� �д�.
	else if(Pos.X>0) Pos.X--;       // Ŀ���� �������� �̵�.
	else {                          // �ܼ��� ���� ���� �����ϸ� �� ���� ������ ������ �̵��Ѵ�.
		if(Pos.Y<1)return;
		Pos.X=width-1;
		Pos.Y--;
	}
	str[--*i]='\0';			// �齺���̽��� ���ڱ��̰� ���ھ� �ش�. / �ܾ��� ���� ���Ⱑ ��.
	GotoXY(Pos);
	putchar('_');                   // ������ ������ ����ٸ� ����.
	GotoXY(Pos);                    // Ŀ���� ����ġ.
	return;
}

void InitBlank(unsigned int n){	//Initialize Blank space in console.
    unsigned int i;
    GotoXY(word[n].pos);
    for(i=word[n].len;i!=0;--i) putc('_',stdout);
    GotoXY(word[n].pos);
}

inline void Adjust(unsigned int n){
	InitBlank(n);
    if(word[n].res>0) {GREEN puts(word[n].str); WHITE}
    else if(word[n].show==1) {BLUE puts(word[n].str); WHITE}
    else if(word[n].res<0) {RED puts(word[n].wrongstr); WHITE}
}

inline void mvword(int *i,int mov,unsigned int cnt){ //move location between word and other word.
    *i+=mov;						//mov�� ����� ���� ���� i�� �Ű��ִµ�,
    if(*i<0) *i+=cnt;				//i�� 0���� ���� �� ������ ������ �ܾ�� �ű��.
    else if(*i>=cnt) *i-=cnt;		//������ �ܾ� ���� �ܾ�� ������ ó������ ����.
}//Screen()���� ���� mov ������ �������� ���� ȭ��ǥ�� ������ ����� �ֱ� �����̴�. (�̹� ������ ���� �� ���ܾ �ǳ� �پ �� ȭ��ǥ ������ �����ϵ���.)

