# 与基于强化学习的AI对战的黑白棋（Reversi）小游戏

/Old_vision       不能悔棋单机版源代码1，能悔棋的单机版源代码2，控制台测试网络模块的源代码

/Release          可以直接玩的单机版和对战版，其中单机版有悔棋功能

/Reversi          对战版游戏源代码，需要导入编译完成的网络模块，注意导入依赖的路径

/Reversi_AI       AI源代码，需要tensorflow与flask与flask_cors

/Reversi_interop  网络模块，需要下载.gitmodules里的libhv依赖库并导入。

头文件路径：/libhv/win64/include/hv   库文件路径：/libhv/win64/lib/Debug


运行单机版：
1. 运行/Release/Reversi_offline.exe
2. 点击开始游戏

运行对战版：
1. 运行/Reversi_AI/backend.py
2. 运行/Release/Reversi_online.exe
3. 点击开始游戏

“提示”按钮：用小红点显示当前执棋方可以落子的位置

“悔棋”按钮：还原上一次落子时的棋盘状态与执棋方，仅限一步
