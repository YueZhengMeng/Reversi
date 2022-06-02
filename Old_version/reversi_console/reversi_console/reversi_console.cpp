// reversi_console.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#pragma comment(lib, "Reversi_interop.lib")
extern "C" int add(int a, int b);
extern "C" int new_game(int side, char* message, size_t message_limit);
extern "C" int terminate_game(char* message, size_t message_limit);
extern "C" int place_chess(int index, char* message, size_t message_limit);
extern "C" int get_board(int* board, int* next_player, bool* over, int* winner, char* message, size_t message_limit);

int main()
{
    char msg[512];
    
    int ret;
    ret = new_game(-1, msg, 512);
    std::cout << ret << ' ' << msg << std::endl;

    int chess[64]; memset(chess, -1, sizeof(int) * 64);
    int next_player;
    bool over = false;
    int winner;
    while (!over) {
        ret = get_board(chess, &next_player, &over, &winner, msg, 512);
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                std::cout << chess[i * 8 + j] << ' ';
            }
            std::cout << std::endl;
        }
        std::cout << "next_player: " << next_player << std::endl;
        std::cout << "over: " << over << std::endl;
        std::cout << "winner: " << winner << std::endl;

        int x, y;
        std::cin >> x >> y;
        int pos = x * 8 + y;
        std::cout << place_chess(pos, msg, 512) << " " << msg << std::endl;
    }

    std::cout << ret << " " << msg;
}