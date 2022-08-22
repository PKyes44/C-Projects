#define MAX 10
#define XYSIZE 13

#include <stdio.h>
#include <windows.h>
#include "Console.h"
#include <time.h>
#include <conio.h>
#include <process.h>

// �����ǿ� ������ ������ ���ڰ� ��
// ���� Ž���ϸ� ������ ���ڰ� �� �� �ִ��� �˷��ֱ�

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
	
	// ���� ����
	Init();

	// ���� ����
	StartMenu();

	// �� ����
 	CreateMap();

	// �ʿ� ���� ����
	CreateMine();

	// ���� �ֺ� ���� ����
	CreateMinecnt();

	// ���� ��ŸƮ
	GameStart();
}

void Init() {
	// ��� Ŀ�� �����
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
			
			// ���� ���� ������ üũ
			for (int j = 0; j < i; j++) {
				if (Mine[i].x == Mine[j].x && Mine[i].y == Mine[j].y) {
					MCheck = true;
					break;
				}
			}

			// ���� ���� ���� ��ġ�� ���� ���
			if (MCheck == false) {
				break;
			}
		}
		Map[Mine[i].y][Mine[i].x] = -5;
	}
}

void CreateMinecnt() {
	// ���� �����
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
			// ���� ����
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
					case 1: printf("��"); break;
					case 2: printf("��"); break;
					case 3: printf("��"); break;
					case 4: printf("��"); break;
					case 5: printf("��"); break;
					case 6: printf("��"); break;
					case 7: printf("��"); break;
					case 8: printf("��"); break;
					}
				}
				else if (Map[i][j] == 0) {
					GotoXY(j * 2, i);
					SetColor(0, 15);
					printf("��");
				}
				else if (Map[i][j] == -5) {
					GotoXY(j * 2, i);
					SetColor(0, 12);
					printf("��");
				}
			}
			else {
				if (Front_map[i][j].flag) {
					GotoXY(j * 2, i);
					SetColor(0, 9);
					printf("��");
				}
				else {
					GotoXY(j * 2, i);
					SetColor(0, 8);
					printf("��");
				}
			}
		}
	}
}

void DrawPlayer() {
	// ��� Ŀ�� �����
	cursorInfo.bVisible = 1;
	cursorInfo.dwSize = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
	srand(time(NULL));

	SetColor(0, 15);
	GotoXY(Player.x, Player.y);
}

void InputProcess() {
	// �������� �̵�
	if (GetAsyncKeyState(VK_LEFT) && 0x8000) {
		if (Player.x - 2 >= 0) {
			Player.x -= 2;
		}
	}
	// ���������� �̵�
	if (GetAsyncKeyState(VK_RIGHT) && 0x8000) {
		if (Player.x + 2 < XYSIZE * 2) {
			Player.x += 2;
		}
	}
	// ���� �̵�
	if (GetAsyncKeyState(VK_UP) && 0x8000) {
		if (Player.y - 1 >= 0) {
			Player.y -= 1;
		}
	}
	// �Ʒ��� �̵�
	if (GetAsyncKeyState(VK_DOWN) && 0x8000) {
		if (Player.y + 1 < XYSIZE) {
			Player.y += 1;
		}
	}
	// ���� üũŰ
	if (GetAsyncKeyState(VK_LCONTROL) && 0x8000) {
		if (Map[Player.y][Player.x / 2] == -5) {
			GameOver(0);
			return;
		}
		MineCntFind(Player.x / 2, Player.y);
	}
	// ��� �ȱ�
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
	// MineCnt ���
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
		if (N == 0) printf("����!");
		else if (N == 1) printf("�¸�!");
		
		GotoXY(20, 6);
		printf("���۸޴��� ���� - ����ALTŰ");
		GotoXY(26, 7);
		printf("���� - ESCŰ");

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
		printf("���� ã��");
		GotoXY(20, 9);
		printf("�����Ϸ��� ����Ű�� �����ʽÿ�");

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

		// �� �׸���
		DrawMap();

		// �Է�Ű
		InputProcess();

		// �¸�üũ
		Check_FlagToMine();
		
		// ������ �׸���
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
	printf("����Ű");
	GotoXY(XYSIZE * 2 + 4, 6);
	printf("������ : �� / ���� : ��");
	GotoXY(XYSIZE * 2 + 4, 7);
	printf("�� : �� / �Ʒ� : ��");
	GotoXY(XYSIZE * 2 + 4, 9);
	printf("��� : SPACE");
	GotoXY(XYSIZE * 2 + 17, 1);
	printf("���� ���� : %d", XYSIZE);

	// ��� Ŀ�� �����
	cursorInfo.bVisible = 0;
	cursorInfo.dwSize = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
	srand(time(NULL));
}