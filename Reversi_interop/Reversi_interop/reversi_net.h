#pragma once

extern "C" __declspec(dllexport) int add(int a, int b);
extern "C" __declspec(dllexport) int new_game(int side, char* message, size_t message_limit);
extern "C" __declspec(dllexport) int terminate_game(char* message, size_t message_limit);
extern "C" __declspec(dllexport) int place_chess(int index, char* message, size_t message_limit);
extern "C" __declspec(dllexport) int get_board(int* board, int* next_player, bool* over, int* winner, char* message, size_t message_limit);