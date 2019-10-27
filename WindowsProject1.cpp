// WindowsProject1.cpp : Определяет точку входа для приложения.
//

#include "stdafx.h"
#include "WindowsProject1.h"
#include<vector>
#include<queue>
#include<ctime>
#include<algorithm>
#include<iostream>
#include<stack>

#define MAX_LOADSTRING 100

const int n = 21;
std::vector<std::vector<int>> graph;
std::vector<std::vector<char>> matrix;
std::vector<bool> used;
std::vector<bool> vampires;
std::vector<bool> vampires_new;
std::vector<int> prev_node;
std::vector<int> nodes;
std::stack<char> to_do;
int Hero = (n / 2) * (n + 1);
int cnt = 1;
bool game_over = false;
int speed = 1000;

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void GenerateMatrixAndGraph();
void AddVapmires();
void CalculateNextStep();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	srand(time(0));
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void MoveVampire(int u) {
	while (prev_node[prev_node[u]] != -1) {
		u = prev_node[u];
	}
	matrix[prev_node[u] / n + 1][prev_node[u] % n + 1] = '.';
	matrix[u / n + 1][u % n + 1] = 'V';
	vampires_new[prev_node[u]] = false;
	vampires_new[u] = true;
	if (u == Hero) {
		game_over = true;
	}
}

void MoveHero() {
	if (to_do.empty()) return;
	char act = to_do.top();
	int i = Hero / n, j = Hero % n;
	switch (act) {
	case 'u':
		if (matrix[i][j + 1] != '#') {
			matrix[i + 1][j + 1] = '.';
			matrix[i][j + 1] = 'H';
			Hero = Hero - n;
		}
		break;
	case 'd':
		if (matrix[i + 2][j + 1] != '#') {
			matrix[i + 1][j + 1] = '.';
			matrix[i + 2][j + 1] = 'H';
			Hero = Hero + n;
		}
		break;
	case 'l':
		if (matrix[i + 1][j] != '#') {
			matrix[i + 1][j + 1] = '.';
			matrix[i + 1][j] = 'H';
			Hero = Hero - 1;
		}
		break;
	case 'r':
		if (matrix[i + 1][j + 2] != '#') {
			matrix[i + 1][j + 1] = '.';
			matrix[i + 1][j + 2] = 'H';
			Hero = Hero + 1;
		}
		break;
	}
	if (vampires_new[Hero] == true) {
		game_over = true;
	}
	to_do = std::stack<char>();
}

bool bfs() {
	int size_ = nodes.size();
	if (size_ > 1) std::random_shuffle(nodes.begin(), nodes.end());
	for (int i = 0; i < size_; ++i) {
		int v = nodes[0];
		nodes.erase(nodes.begin());
		used[v] = true;
		for (int u : graph[v]) {
			if (u == Hero) {
				used[u] = true;
				prev_node[u] = v;
				MoveVampire(u);
				return true;
			}
			if (used[u] == false) {
				used[u] = true;
				prev_node[u] = v;
				nodes.push_back(u);
			}
		}
	}
	if (nodes.empty()) return false;
	if (bfs()) return true;
	else return false;
}

void ChangeCorner(int v) {
	switch (v) {
	case 0:
		vampires_new[n - 1] = true;
		matrix[1][n] = 'V';
		break;
	case n - 1:
		vampires_new[n*n - n] = true;
		matrix[n][1] = 'V';
		break;
	case n*n - n:
		vampires_new[n*n - 1] = true;
		matrix[n][n] = 'V';
		break;
	case n*n - 1:
		vampires_new[0] = true;
		matrix[1][1] = 'V';
		break;
	default:
		return;
	}
	vampires_new[v] = false;
	matrix[v / n + 1][v % n + 1] = '.';
}

void CalculateNextStep(HWND hWnd) {
	used = std::vector<bool>(n*n, false);
	nodes = std::vector<int>();
	prev_node = std::vector<int>(n*n, -1);
	for (int i = 0; i < vampires.size(); ++i) {
		if (vampires[i] == true) {
			used[i] = true;
		}
	}
	std::vector<int> to_add;
	for (int i = 0; i < vampires.size(); ++i) {
		if (vampires[i] == true) {
			to_add.push_back(i);
		}
	}
	std::random_shuffle(to_add.begin(), to_add.end());
	for (int i = 0; i < to_add.size(); ++i) {
		nodes.push_back(to_add[i]);
		if (!bfs()) {
			ChangeCorner(to_add[i]);
		}
		vampires = vampires_new;
		if (game_over == true) {
			game_over = false;
			GenerateMatrixAndGraph();
			AddVapmires();
			KillTimer(hWnd, 5);
			InvalidateRect(hWnd, NULL, true);
			MessageBox(hWnd, L"YOU DIED", L"Game over!", MB_OK);
			break;
		}
		used = std::vector<bool>(n*n, false);
		nodes = std::vector<int>();
		prev_node = std::vector<int>(n*n, -1);
		for (int i = 0; i < vampires.size(); ++i) {
			if (vampires[i] == true) {
				used[i] = true;
			}
		}
	}
}

void AddVapmires() {
	static int corner = rand() % 4;
	switch (corner) {
	case 0:
		vampires[0] = true;
		vampires_new[0] = true;
		matrix[1][1] = 'V';
		break;
	case 1:
		vampires[n - 1] = true;
		vampires_new[n - 1] = true;
		matrix[1][n] = 'V';
		break;
	case 2:
		vampires[n*n - n] = true;
		vampires_new[n*n - n] = true;
		matrix[n][1] = 'V';
		break;
	case 3:
		vampires[n*n - 1] = true;
		vampires_new[n*n - 1] = true;
		matrix[n][n] = 'V';
		break;
	}
	++corner;
	corner %= 4;
}

char randomChar() {
	int nmb = rand() % 4;
	if (nmb == 0) return '#';
	else return '.';
}

void GenerateMatrixAndGraph() {
	cnt = 1;
	Hero = (n / 2) * (n + 1);
	matrix = std::vector<std::vector<char>>(n + 2, std::vector<char>(n + 2));
	graph = std::vector<std::vector<int>>(n*n);
	vampires = std::vector<bool>(n*n, false);
	vampires_new = std::vector<bool>(n*n, false);
	for (int i = 0; i < n + 2; ++i) {
		matrix[0][i] = '#';
		matrix[n + 1][i] = '#';
		matrix[i][0] = '#';
		matrix[i][n + 1] = '#';
	}
	for (int i = 1; i < n + 1; ++i) {
		for (int j = 1; j < n + 1; ++j) {
			char temp = randomChar();
			if (i == 1 && j == 1) temp = '.';
			if (i == 1 && j == n) temp = '.';
			if (i == n && j == 1) temp = '.';
			if (i == n && j == n) temp = '.';
			if (i == n / 2 + 1 && j == i) temp = 'H';
			if (temp != '#') {
				int nmb = (i - 1) * n + j - 1;
				if (i > 0 && matrix[i - 1][j] != '#') {
					graph[nmb].push_back((i - 2) * n + j - 1);
					graph[(i - 2) * n + j - 1].push_back(nmb);
				}
				if (j > 0 && matrix[i][j - 1] != '#') {
					graph[nmb].push_back((i - 1) * n + j - 2);
					graph[(i - 1) * n + j - 2].push_back(nmb);
				}
			}
			matrix[i][j] = temp;
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND play_button, pause_button;
	static HWND radio1, radio2, radio3;
    switch (message)
    {
	case WM_CREATE:
	{
		GenerateMatrixAndGraph();
		AddVapmires();
		pause_button = CreateWindow(L"button", L"pause", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 550, 50, 60, 20, hWnd, (HMENU)10000, hInst, NULL);
		play_button = CreateWindow(L"button", L"play", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 550, 100, 60, 20, hWnd, (HMENU)10001, hInst, NULL);
		radio1 = CreateWindow(L"button", L"", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 540, 130, 20, 20, hWnd, (HMENU)10010, hInst, NULL);
		radio2 = CreateWindow(L"button", L"", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 580, 130, 20, 20, hWnd, (HMENU)10011, hInst, NULL);
		radio3 = CreateWindow(L"button", L"", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 620, 130, 20, 20, hWnd, (HMENU)10012, hInst, NULL);
		SendMessage(radio1, BM_SETCHECK, 1, 0);
	}
	break;
	case WM_TIMER:
	{
		MoveHero();
		if (game_over == true) {
			game_over = false;
			GenerateMatrixAndGraph();
			AddVapmires();
			InvalidateRect(hWnd, NULL, false);
			KillTimer(hWnd, 5);
			MessageBox(hWnd, L"YOU DIED", L"Game over!", MB_OK);
			break;
		}
		CalculateNextStep(hWnd);
		if (cnt % 20 == 0) AddVapmires();
		++cnt;
		InvalidateRect(hWnd, NULL, false);
	}
	break;
	case WM_KEYDOWN:
		switch (LOWORD(wParam)) {
		case 0x57:
			to_do.push('u');
			break;
		case VK_UP:
			to_do.push('u');
			break;
		case VK_NUMPAD8:
			to_do.push('u');
			break;
		case 0x53:
			to_do.push('d');
			break;
		case VK_DOWN:
			to_do.push('d');
			break;
		case VK_NUMPAD2:
			to_do.push('d');
			break;
		case 0x41:
			to_do.push('l');
			break;
		case VK_LEFT:
			to_do.push('l');
			break;
		case VK_NUMPAD4:
			to_do.push('l');
			break;
		case 0x44:
			to_do.push('r');
			break;
		case VK_RIGHT:
			to_do.push('r');
			break;
		case VK_NUMPAD6:
			to_do.push('r');
			break;
		case VK_RETURN:
			SendMessage(play_button, BM_CLICK, 0, 0);
			break;
		case VK_SPACE:
			SendMessage(play_button, BM_CLICK, 0, 0);
			break;
		}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
			case 10010:
				switch (HIWORD(wParam)) {
				case BN_CLICKED:
					if (SendMessage(radio1, BM_GETCHECK, 0, 0) == false) {
						SendMessage(radio1, BM_SETCHECK, 1, 0);
						SendMessage(radio2, BM_SETCHECK, 0, 0);
						SendMessage(radio3, BM_SETCHECK, 0, 0);
						speed = 1000;
					}
					break;
				}
				break;
			case 10011:
				switch (HIWORD(wParam)) {
				case BN_CLICKED:
					if (SendMessage(radio2, BM_GETCHECK, 0, 0) == false) {
						SendMessage(radio1, BM_SETCHECK, 0, 0);
						SendMessage(radio2, BM_SETCHECK, 1, 0);
						SendMessage(radio3, BM_SETCHECK, 0, 0);
						speed = 800;
					}
					break;
				}
				break;
			case 10012:
				switch (HIWORD(wParam)) {
				case BN_CLICKED:
					if (SendMessage(radio3, BM_GETCHECK, 0, 0) == false) {
						SendMessage(radio1, BM_SETCHECK, 0, 0);
						SendMessage(radio2, BM_SETCHECK, 0, 0);
						SendMessage(radio3, BM_SETCHECK, 1, 0);
						speed = 600;
					}
					break;
				}
				break;
			case 10000:
				KillTimer(hWnd, 5);
				break;
			case 10001:
				SetTimer(hWnd, 5, speed, NULL);
				break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
			SetFocus(hWnd);
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			HBRUSH hBrush = CreateSolidBrush(RGB(100, 120, 120));
			HBRUSH hTransperent = CreateSolidBrush(RGB(255, 255, 255));
			HPEN hPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
			SelectObject(hdc, hPen);
			int size = 20;
			SetBkMode(hdc, TRANSPARENT);
			TextOut(hdc, 530, 160, L"Low", 4);
			TextOut(hdc, 560, 160, L"Medium", 7);
			TextOut(hdc, 615, 160, L"Fast", 5);
			for (int i = 0; i < n + 2; ++i) {
				for (int j = 0; j < n + 2; ++j) {
					if (matrix[i][j] == '#') {
						SelectObject(hdc, hBrush);
						Rectangle(hdc, 50 + j * size, 50 + i * size, 50 + (j + 1) * size, 50 + (i + 1) * size);
					}
					else {
						SelectObject(hdc, hTransperent);
						Rectangle(hdc, 50 + j * size, 50 + i * size, 50 + (j + 1) * size, 50 + (i + 1) * size);
						if (matrix[i][j] == 'V') {
							TextOut(hdc, 50 + j * size + 6, 50 + i * size + 2, L"V", 2);
						}
						if (matrix[i][j] == 'H') {
							TextOut(hdc, 50 + j * size + 6, 50 + i * size + 2, L"X", 2);
						}
					}
				}
			}
			DeleteObject(hBrush);
			DeleteObject(hTransperent);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
