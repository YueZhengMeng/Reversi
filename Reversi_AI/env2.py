import os
from time import sleep
import numpy as np


class Board:
    def __init__(self):
        # const init
        self.BLACK = 1
        self.WHITE = -1
        self.EMPTY = 0
        self.BLACK_PIC = '🔹'
        self.WHITE_PIC = '🔸'
        self.EMPTY_PIC = '  '
        self.R_WIN = 8
        self.R_LOSE = -8
        self.R_FLIP = 0.01
        self.R_CORNER = 0.02
        self.ALL_DIR = ((1, 1), (1, 0), (1, -1), (0, 1), (0, -1), (-1, 1), (-1, 0), (-1, -1))
        self.ALL_CORNER = ((0, 0), (0, 7), (7, 0), (7, 7))
        self.ALL_ACTIONS = []
        for x in range(8):
            for y in range(8):
                self.ALL_ACTIONS.append((x, y))
        # variable
        self.reset()

    def reset(self):
        """Returns: (board, available positions)"""
        # variable
        self.gameover = False
        self.winner = self.EMPTY
        self.black_cnt = 2
        self.white_cnt = 2
        self.current_player = self.BLACK
        self.board = np.zeros((8, 8), dtype=np.int8)
        self.board[3, 4] = self.board[4, 3] = self.BLACK
        self.board[3, 3] = self.board[4, 4] = self.WHITE
        return self.board, self.get_avaliable_pos()

    def step(self, pos):
        """Returns: (board, reward, avaliable positions, gameover)"""
        dirs = self._avaliable_dirs(pos, self.current_player)
        if not dirs:
            raise Exception('Not avaliable position')

        # update board
        flip_cnt = 0
        self.board[pos] = self.current_player
        for dir in dirs:
            x, y = pos
            x, y = x+dir[0], y+dir[1]
            while self.board[x, y] != self.current_player:
                self.board[x, y] = self.current_player
                flip_cnt += 1
                x, y = x+dir[0], y+dir[1]

        # flip reward
        reward = self.R_CORNER if pos in self.ALL_CORNER else self.R_FLIP
        reward *= flip_cnt

        # check state
        if not self.get_avaliable_pos(self.annother_player()):
            # 对方无子可下
            if not self.get_avaliable_pos():
                # 自己也无子可下
                # 游戏结束
                self.gameover = True
                winner = self.get_winner()
                if winner == self.current_player:
                    reward += self.R_WIN
                elif winner == self.annother_player():
                    reward += self.R_LOSE
                else:
                    pass  # keep reward
        else:
            self.current_player = self.annother_player()

        return self.board, reward, self.get_avaliable_pos(), self.gameover

    def get_avaliable_pos(self, type=None):
        if not type:
            type = self.current_player
        avaliable_pos = []
        for p in self.ALL_ACTIONS:
            if self._avaliable_dirs(p, type):
                avaliable_pos.append(p)
        return avaliable_pos

    def get_winner(self):
        if self.black_cnt > self.white_cnt:
            return self.BLACK
        elif self.black_cnt < self.white_cnt:
            return self.WHITE
        else:
            return self.EMPTY

    def annother_player(self, type=None):
        if not type:
            type = self.current_player
        return self.BLACK if type == self.WHITE else self.WHITE

    def render(self):
        os.system('cls')
        # head index
        print('  ', end='')
        for x in range(8):
            print(f' {x}', end='')
        print('')
        # body
        for x in range(8):
            # index
            print(x, end=' ')
            # board
            for y in range(8):
                if self.board[x, y] == self.BLACK:
                    print(self.BLACK_PIC, end='')
                elif self.board[x, y] == self.WHITE:
                    print(self.WHITE_PIC, end='')
                else:
                    print(self.EMPTY_PIC, end='')
            print('')
        print(f'black: {self.black_cnt}, white: {self.white_cnt}')

    def _avaliable_dirs(self, pos, type) -> list:
        """Return all available directions"""
        if self.board[pos[0], pos[1]] != 0:
            return None
        ret = []
        for direction in self.ALL_DIR:
            x, y = pos[0], pos[1]
            state = 0
            while True:
                x, y = x+direction[0], y+direction[1]
                if self._out_of_bound(x) or self._out_of_bound(y):
                    break
                type_ = self.board[x, y]
                if state == 0:  # find different
                    if type_ == self.EMPTY or type_ == type:
                        break
                    else:
                        state = 1
                elif state == 1:  # find close
                    if type_ == self.EMPTY:  # dead end
                        break
                    elif type_ == type:  # close
                        ret.append(direction)
        return ret

    def _out_of_bound(self, x):
        return x >= 8 or x < 0


if __name__ == '__main__':
    env = Board()

    state, available_actions = env.reset()
    env.render()

    while True:
        # player input
        pos = (-1, -1)
        while pos not in available_actions:
            pos = input('input pos: ')
            if len(pos) != 2:
                continue
            pos = int(pos[0]), int(pos[1])

        state_, reward, available_actions, gameover = env.step(pos)

        env.render()

        if gameover:
            print('winner is: '+env.get_winner())
            break

    print('gameover')
