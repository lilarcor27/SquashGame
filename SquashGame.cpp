#include <Turboc.h>

#define UP		72
#define	LEFT	75
#define	RIGHT	77

#define W 	119
#define A 	97
#define D	100

#define TRUE	1
#define FALSE 	0

struct PlayerData {
	char x;			// 플레이어의 X좌표 기록
	char y;			// 플레이어의 Y좌표 기록
	char charTile;	// 플레이어의 char 모양 기록
	char jumpCount;	// 점프가 몇칸 남았나 기록
};

char Map[20][81] = { // 맵
	"                                                                                ",
	"              *                                                 *               ",
	"                                  *         *                                   ",
	"         *                                                           *          ",
	"                                                                                ",
	"                                                                                ",
	"                                                                                ",
	"                                                                                ",
	"                                                                                ",
	"                         ======    =========    ======                          ",
	"                                  =         =                                   ",
	"                                 =           =                                  ",
	"                                =             =                                 ",
	"                               =               =                                ",
	"          ===== ==== ==== =====                 ===== ==== ==== =====           ",
	"        ==                                                           ==         ",
	"      ==                                                               ==       ",
	"    ==                                                                   ==     ",
	"                                                                                ",
	"================================================================================"
};

struct PlayerData p[2]; 	// 플레이어 2명 생성

char Winner = -1;

void introduce(void); // 게임 소개와 캐릭터 설정
char game_start(void); // 게임 시작
int loop(void); // 메인루프
void move(int ch); // 이동
int check_gravity(void); // 중력 체킹
char finish(); // 끝

void putchxy(char x, char y, char ch);

int main(void)
{
	introduce();			// 게임 소개, char 셋팅
	while(game_start());	// 게임_스타트 에서 0이 반환될 때까지 루프
	return 0;
}

void introduce(void)
{
	clrscr();
	setcursortype(NOCURSOR);
	
	char ch;
	
	printf("\n\n\n\t먼저 밟기 게임 - KJY\n");
	delay(500);

	printf("\t상대를 먼저 밟으세요.\n");
	delay(500);
	
	printf("\t왼쪽 플레이어는 WAD를 사용하며, 오른쪽 플레이어는 방향키를 사용합니다.\n");
	delay(500);

	printf("\t왼쪽 플레이어의 캐릭터를 입력하세요(ASCII 코드만 가능합니다.): ");
	scanf("%c", &ch);
	p[0].charTile = ch;

	fflush(stdin);
	
	printf("\n\t오른쪽 플레이어의 캐릭터를 입력하세요(ASCII 코드만 가능합니다.): ");
	scanf("%c", &ch);
	p[1].charTile = ch;
	
	fflush(stdin);

	printf("\n\t아무 키나 누르면 시작합니다...");
	getch();
	return;
}

char game_start(void)
{
	clrscr();
	int
		i = 0,
		ii = 0;
		
	for(i = 0; i < 20; i++) {
		for(ii = 0; ii < 79; ii++) {	
			printf("%c", Map[i][ii]);	// 맵 찍어내기
		}
		putch('\n');
	}
	
	p[0].x = 1;
	p[1].x = 77;

	p[0].y = 18;
	p[1].y = 18;
	
	p[0].jumpCount = 0;
	p[1].jumpCount = 0;
	
	putchxy(p[0].x, p[0].y, p[0].charTile);
	
	putchxy(p[1].x, p[1].y, p[1].charTile);

	while(loop()) // 'loop' 메소드가 참값이면 계속 돈다.
		delay(10);
	
	return finish(); // 결과를 준다.
}

int loop(void)
{
	int ch;
	
	if(kbhit()) {	// 만약 키가 눌려있다면 move 함수 실행
		ch = getch();
		move(ch);
	}
	
	return check_gravity();	// 중력검사 실행, 승자가 나올때 까진 1이 리턴되므로 계속 반복
}

void move(int ch)
{
	char isMainPlayer;
	struct PlayerData *pt;	// pt는 플레이어를 직접 건드므로 포인터 변수
	struct PlayerData pt2;	// 참조만 하므로 일반 변수
	
	if(ch == 0 || ch == 0xE0) {
		ch = getch();
		isMainPlayer = TRUE;	
	} else {					
		switch(ch) {
			case W:
				ch = UP;
				break;
			case A:
				ch = LEFT;
				break;
			case D:
				ch = RIGHT;
				break;
			default:
				break;
		}
		isMainPlayer = FALSE;
	}
	
	pt = &p[isMainPlayer]; // pt 만 건들면 되니 포인터로 한다.
	pt2 = p[!isMainPlayer]; // 참조만 할 뿐 건들지 않는다.
	
	switch(ch) {		// 핵심 부분
		case LEFT:
			if(pt->x == 0) return;		// 최 좌측일 경우 리턴.
			if((pt->y == pt2.y) && (pt->x - 1 == pt2.x)) return;	// 왼쪽에 플레이어가 있을 경우 리턴.
			if(Map[pt->y][pt->x - 1] == '=') return;	// 벽일 경우 리턴.
			
			putchxy(pt->x, pt->y, ' ');	// 원래 있던 자리에 공백 찍기
			
			pt->x--;
			
			putchxy(pt->x, pt->y, pt->charTile);	// 자리를 옮겨서 charTile 찍기
			
			break;
			
		case RIGHT:
			if(pt->x == 78) return;	// 최 우측일 경우 리턴.
			if((pt->y == pt2.y) && (pt->x + 1 == pt2.x)) return; // 상동
			if(Map[pt->y][pt->x + 1] == '=') return;	// 상동
			
			putchxy(pt->x, pt->y, ' ');	// 상동
			
			pt->x++;
			
			putchxy(pt->x, pt->y, pt->charTile);	// 상동
			
			break;
			
		case UP:
			if(Map[pt->y + 1][pt->x] != '=') return;	// 머리 위가 벽일 경우 리턴
			pt->jumpCount = 3;	// 점프에 성공하면 점프 카운트를 3으로 맞춤, check_gravity 에서 1씩 깎임.

			break;
					
		default:
			break;
	}
	return;
}

int check_gravity(void)
{
	static int Progress = 0;
	
	if(Progress != 5) {		// 5번에 1번 검사. 지나치게 빠른 점프속도 방지
		Progress++;			// 딜레이를 늦추면 되지 않나.. 아니다. 좌우로 움직이는 속도(키 인식 딜레이)도 고려해야 하기 때문이다.
		return 1;
	}
	
	int i;
	for(i = 0; i < 2; i++) {
		if(p[i].jumpCount != 0) {
			if(Map[p[i].y - 1][p[i].x] == '=') {	// 머리 위가 벽이면 점프카운트 0
				p[i].jumpCount = 0;
				continue;
			}
			putchxy(p[i].x, p[i].y, ' ');	// 위치 재설정
			
			p[i].y -= 1;
			
			putchxy(p[i].x, p[i].y, p[i].charTile);
			
			p[i].jumpCount = (p[i].jumpCount > 0 ? p[i].jumpCount - 1 : 0); // 점프 카운트가 0이면 0, 아닐 시 -1
		}
		else if(Map[p[i].y + 1][p[i].x] == ' ') {
			if((p[i].y + 1 == p[!i].y) && (p[i].x == p[!i].x)) {		// 플레이어를 밟았다면
				Winner = i;	// 위너 설정, 0 리턴.
				return 0;
			} else {
				putchxy(p[i].x, p[i].y, ' ');
				
				p[i].y += 1;
				
				putchxy(p[i].x, p[i].y, p[i].charTile);
			}
		}
	}
	Progress = 0;
	return 1;
}

char finish()
{
	clrscr();
	
	char WinnerState[8];
	if(Winner == 1)
		strcpy(WinnerState, "오른쪽");
	else
		strcpy(WinnerState, "왼쪽");
	
	printf("\n\n\n\t* %s 플레이어가 승리하였습니다! *\n\n", WinnerState);
	delay(500);
	
	printf("\t다시 한 번 게임을 하시려면 아무 키나 누르세요.\n");
	delay(500);
	
	printf("\t게임을 종료하시려면 X 를 입력하세요.\n\t");
	
	char ch;
	
	scanf("%c", &ch);
	
	if(ch == 'X' || ch == 'x') exit(0);
	
	return 1;
}

void putchxy(char x, char y, char ch)
{
	gotoxy(x, y);
	printf("%c", ch);
	
	return;
}
