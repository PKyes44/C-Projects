#define MAX 20
#define XYSIZE 13

#include <stdio.h>
#include <windows.h>
#include "Console.h"
#include <time.h>
#include <conio.h>
#include <process.h>

// 격자판에 일정한 갯수의 지뢰가 깔림
// 블럭을 탐색하면 주위에 지뢰가 몇 개 있는지 알려주기

int Map[XYSIZE][XYSIZE] = { NULL };
int MineCnt[XYSIZE][XYSIZE] = { NULL };

struct OBJ {
	int x = 0;
	int y = 0;
};

OBJ Mine[MAX] = { NULL };
OBJ Player;
OBJ Loading[6];

struct PROCESS {
	bool flag = false;
	bool active = false;
};

PROCESS Front_map[XYSIZE][XYSIZE] = { false ,  };

CONSOLE_CURSOR_INFO cursorInfo;

bool MCheck = false;

void Init();
void CreateMap();
void CreateMine();
void CreateMinecnt();
void DrawMap();
void InputProcess();
void DrawPlayer();
void GameOver(int N);
void MineCntFind(int Playerx, int Playery);
void StartMenu();
void GameStart();
void Check_FlagToMine();
void GUI();
int randx();

int main() {
	
	// 게임 세팅
	Init();

	// 게임 시작
	StartMenu();

	// 맵 초기화 및 생성
 	CreateMap();

	// 맵에 지뢰 생성

	CreateMine();

	// MineCnt 생성
	CreateMinecnt();

	// 게임 스타트
	GameStart();
}

void Init() {
	// 흰색 커서 지우기
	cursorInfo.bVisible = 0;
	cursorInfo.dwSize = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
	srand(time(NULL));

	// 화면 크기 조정
	system("mode con cols=70 lines=13");

	// 플레이어좌표 초기화
	Player.x = 0;
	Player.y = 0;
}

void CreateMap() {
	for (int i = 0; i < XYSIZE; i++) {
		for (int j = 0; j < XYSIZE; j++) {
			Map[i][j] = 0;
			Front_map[i][j].flag = false;
			Front_map[i][j].active = false;
			MineCnt[i][j] = 0;
		}
	}
}

void CreateMine() {
	for (int i = 0,Cnt = 0; i < MAX; i++) {
		while(1) {
			MCheck = false;

			Mine[i].x = randx();
			Mine[i].y = randx();
			
			// 중복체크 체크
			for (int j = 0; j < i; j++) {
				if (Mine[i].x == Mine[j].x && Mine[i].y == Mine[j].y) {
					MCheck = true;
					break;
				}
			}

			// 중복이 아닐 경우
			if (MCheck == false) {
				break;
			}
		}
		// 맵에 적용
		Map[Mine[i].y][Mine[i].x] = -5;
	}
}

void CreateMinecnt() {
	// 지뢰개수 만들기
	for (int y = 0; y < XYSIZE; y++) {
		for (int x = 0; x < XYSIZE; x++) {
			if (Map[y][x] != -5) {
				for (int i = y - 1; i < y + 2; i++) {
					if (i >= 0 && i < XYSIZE) {
						for (int j = x - 1; j < x + 2; j++) {
							if (j >= 0 && j < XYSIZE) {
								if (Map[i][j] == -5) {
									MineCnt[y][x]++;
								}
							}
						}
					}
				}
  				Map[y][x] = MineCnt[y][x]; 
			}
		} 
	}
}

void DrawMap() {
	for (int i = 0; i < XYSIZE; i++) {
		for (int j = 0; j < XYSIZE; j++) {
			if (Front_map[i][j].active) {
				if (Map[i][j] >= 1) {
					GotoXY(j * 2, i);
					SetColor(0, 14);
					switch (Map[i][j]) {
					case 1: printf("①"); break;
					case 2: printf("②"); break;
					case 3: printf("③"); break;
					case 4: printf("④"); break;
					case 5: printf("⑤"); break;
					case 6: printf("⑥"); break;
					case 7: printf("⑦"); break;
					case 8: printf("⑧"); break;
					}
				}
				else if (Map[i][j] == 0) {
					GotoXY(j * 2, i);
					SetColor(0, 15);
					printf("□");
				}
				else if (Map[i][j] == -5) {
					GotoXY(j * 2, i);
					SetColor(0, 12);
					printf("★");
				}
			}
			else {
				if (Front_map[i][j].flag) {
					GotoXY(j * 2, i);
					SetColor(0, 9);
					printf("¶");
				}
				else {
					GotoXY(j * 2, i);
					SetColor(0, 8);
					printf("■");
				}
			}
		}
	}
}

void DrawPlayer() {
	// 흰색 커서 활성화
	cursorInfo.bVisible = 1;
	cursorInfo.dwSize = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
	srand(time(NULL));

	// 커서 위치
	SetColor(0, 15);
	GotoXY(Player.x, Player.y);
}

void InputProcess() {
	// 왼쪽으로 이동
	if (GetAsyncKeyState(VK_LEFT) && 0x8000) {
		if (Player.x - 2 >= 0) {
			Player.x -= 2;
		}
	}
	// 오른쪽으로 이동
	if (GetAsyncKeyState(VK_RIGHT) && 0x8000) {
		if (Player.x + 2 < XYSIZE * 2) {
			Player.x += 2;
		}
	}
	// 위로 이동
	if (GetAsyncKeyState(VK_UP) && 0x8000) {
		if (Player.y - 1 >= 0) {
			Player.y -= 1;
		}
	}
	// 아래로 이동
	if (GetAsyncKeyState(VK_DOWN) && 0x8000) {
		if (Player.y + 1 < XYSIZE) {
			Player.y += 1;
		}
	}
	// 지뢰 체크키
	if (GetAsyncKeyState(VK_LCONTROL) && 0x8000) {
		if (Map[Player.y][Player.x / 2] == -5) {
			GameOver(0);
			return;
		}
		MineCntFind(Player.x / 2, Player.y);
	}
	// 깃발 꽂기
	if (GetAsyncKeyState(VK_SPACE) && 0x8000) {
		if (Front_map[Player.y][Player.x / 2].flag) {
			Front_map[Player.y][Player.x / 2].flag = false;
		}
		else {
			Front_map[Player.y][Player.x / 2].flag = true;
		}
	}
}

void MineCntFind(int Playerx, int Playery) {
	int i, j;

	if (Playery < 0 || Playery >= XYSIZE) return;
	if (Playerx < 0 || Playerx >= XYSIZE) return;
	if (Front_map[Playery][Playerx].active) return;
	if (Map[Playery][Playerx] == -5) return;
	if (Map[Playery][Playerx] >= 1) {
		Front_map[Playery][Playerx].active = true; 
		return;
	}

	Front_map[Playery][Playerx].active = true;
	for (i = Playerx - 1; i < Playerx + 2; i++) {
		for (j = Playery - 1; j < Playery + 2; j++) {
			MineCntFind(i,j);	
		}
	}
}

void GameOver(int N) {
	// 전체 맵 보여주기
	for (int i = 0; i < XYSIZE; i++) {
		for (int j = 0; j < XYSIZE; j++) {
			if (Front_map[i][j].active) continue;
			Front_map[i][j].active = true;
		}
	}
	DrawMap();
	Sleep(2000);

	// 종료메뉴
	while (1) {
		Clear();
		SetColor(0, 15);
		
		GotoXY(30, 3);
		if (N == 0) printf("실패!");
		else if (N == 1) printf("승리!");
		
		GotoXY(20, 7);
		printf("시작메뉴로 가기 - 왼쪽ALT키");
		GotoXY(26, 8);
		printf("종료 - ESC키");

		if (GetAsyncKeyState(VK_ESCAPE) && 0x8000) {
			exit(0);
		}
		else if (GetAsyncKeyState(VK_LMENU) && 0x8000) {
			break;
		}
	}
	main();
	return;
}

void StartMenu() {
	while (1) {
		Clear();
		SetColor(0, 15);
		GotoXY(30, 4);
		printf("지뢰 찾기");
		GotoXY(20, 9);
		printf("시작하려면 엔터키를 누르십시오");

		if (GetAsyncKeyState(VK_RETURN) && 0x8000) {
			break;
		}
		Sleep(100);
	}
}
void GameStart() {
	while (1) {
		Clear();

		// GUI
		GUI();

		// 맵 그리기
		DrawMap();

		// 입력키
		InputProcess();

		// 승리체크
		Check_FlagToMine();
		
		// 포인터 그리기
		DrawPlayer();

		Sleep(50);
	}
}

void Check_FlagToMine() {
	int cnt = 0;
	for (int TMPy = 0; TMPy < XYSIZE; TMPy++) {
		for (int TMPx = 0; TMPx < XYSIZE; TMPx++) {
			if (Front_map[TMPy][TMPx].flag) {
				if (Map[TMPy][TMPx] == -5) {
					cnt++;
				}
			}
		}
	}
	if (cnt == XYSIZE) {
		GameOver(1);
	}
}

void GUI() {
	GotoXY(XYSIZE * 2 + 4, 3);
	printf("조작키");
	GotoXY(XYSIZE * 2 + 4, 6);
	printf("오른쪽 : → / 왼쪽 : ←");
	GotoXY(XYSIZE * 2 + 4, 7);
	printf("위 : ↑ / 아래 : ↓");
	GotoXY(XYSIZE * 2 + 4, 9);
	printf("깃발 : SPACE");
	GotoXY(XYSIZE * 2 + 4, 10);
	printf("지뢰확인 : LCTRL");
	GotoXY(XYSIZE * 2 + 17, 1);
	printf("지뢰 개수 : %d", MAX);

	// 흰색 커서 지우기
	cursorInfo.bVisible = 0;
	cursorInfo.dwSize = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
	srand(time(NULL));
}

// 랜덤
int randx()
{
	static int flag = 0;
	int retVal;
	if (flag == 0)
	{
		srand(time(NULL));
		rand(); rand(); rand(); rand();
		srand(rand());
		flag = 1;
	}
	retVal = rand() % XYSIZE;
	return retVal;
}