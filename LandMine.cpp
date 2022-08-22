#define MAX 10
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

int main() {
	
	// 게임 세팅
	Init();

	// 게임 시작
	StartMenu();

	// 맵 생성
 	CreateMap();

	// 맵에 지뢰 생성
	CreateMine();

	// 지뢰 주변 숫자 생성
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

	system("mode con cols=70 lines=13");

	Player.x = 0;
	Player.y = 0;
}

void CreateMap() {
	for (int i = 0; i < XYSIZE; i++) {
		for (int j = 0; j < XYSIZE; j++) {
			Map[i][j] = 0;
			Front_map[i][j].flag = false;
			Front_map[i][j].active = false;
		}
	}
}

void CreateMine() {
	for (int i = 0; i < MAX; i++) {
		while(1) {
			MCheck = false;
			srand(time(NULL));
			Sleep(1000);
			Mine[i].x = rand() % XYSIZE;
			Mine[i].y = rand() % XYSIZE;
			
			// 전에 나온 값인지 체크
			for (int j = 0; j < i; j++) {
				if (Mine[i].x == Mine[j].x && Mine[i].y == Mine[j].y) {
					MCheck = true;
					break;
				}
			}

			// 전에 나온 값과 겹치지 않을 경우
			if (MCheck == false) {
				break;
			}
		}
		Map[Mine[i].y][Mine[i].x] = -5;
	}
}

void CreateMinecnt() {
	// 숫자 만들기
	for (int TMPy = 0; TMPy < XYSIZE; TMPy++) {
		for (int TMPx = 0; TMPx < XYSIZE; TMPx++) {
			if (Map[TMPy][TMPx] == -5) continue;
			for (int i = TMPx - 1; i < TMPx + 2; i++) {
				if (i < 0 || i >= XYSIZE) continue;
				for (int j = TMPy - 1; j < TMPy + 2; j++) {
					if (j < 0 || j >= XYSIZE) continue;
					if (Map[j][i] == -5) {
						MineCnt[TMPy][TMPx]++;
					}
				}
			}
			// 숫자 적용
			if (Map[TMPy][TMPx] != -5) {
				Map[TMPy][TMPx] = MineCnt[TMPy][TMPx];
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
	// 흰색 커서 지우기
	cursorInfo.bVisible = 1;
	cursorInfo.dwSize = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
	srand(time(NULL));

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
	// MineCnt 재귀
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
	for (int i = 0; i < XYSIZE; i++) {
		for (int j = 0; j < XYSIZE; j++) {
			if (Front_map[i][j].active) continue;
			Front_map[i][j].active = true;
		}
	}
	DrawMap();
	Sleep(2000);

	while (1) {
		Clear();
		SetColor(0, 15);
		
		GotoXY(30, 3);
		if (N == 0) printf("실패!");
		else if (N == 1) printf("승리!");
		
		GotoXY(20, 6);
		printf("시작메뉴로 가기 - 왼쪽ALT키");
		GotoXY(26, 7);
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

		if (GetAsyncKeyState(VK_RETURN) && 0x8000); {
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
	GotoXY(XYSIZE * 2 + 17, 1);
	printf("지뢰 개수 : %d", XYSIZE);

	// 흰색 커서 지우기
	cursorInfo.bVisible = 0;
	cursorInfo.dwSize = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
	srand(time(NULL));
}