
// Reversi.h: PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"		// 主符号


// CReversiApp:
// 有关此类的实现，请参阅 Reversi.cpp
//
#pragma comment(lib, "Reversi_interop.lib")
extern "C" int new_game(int side, char* message, size_t message_limit);
extern "C" int terminate_game(char* message, size_t message_limit);
extern "C" int place_chess(int index, char* message, size_t message_limit);
extern "C" int get_board(int* board, int* next_player, bool* over, int* winner, char* message, size_t message_limit);

class CReversiApp : public CWinApp
{
public:
	CReversiApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CReversiApp theApp;
