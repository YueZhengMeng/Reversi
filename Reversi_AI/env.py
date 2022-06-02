import os
import numpy as np


class Board:
    def __init__(self):
        self.EMPTY = 0
        self.BLACK = 1
        self.WHITE = -1
        self.BLACK_PIC = '🔹'
        self.WHITE_PIC = '🔸'
        self.EMPTY_PIC = '  '
        self.R_WIN = 8
        self.R_LOSS = -8
        self.R_FLIP = 0.01
        self.ALL_DIR = ((1, 1), (1, 0), (1, -1), (0, 1), (0, -1), (-1, 1), (-1, 0), (-1, -1))
        self.features = 64
        self.all_actions = []
        for x in range(8):
            for y in range(8):
                self.all_actions.append((x, y))
        self.reset()

    def reset(self):
        self.winner = 0
        self.black_cnt = 2
        self.white_cnt = 2
        self.board = np.zeros(shape=(8, 8), dtype=np.int8)
        self.board[3, 4] = self.board[4, 3] = self.BLACK
        self.board[3, 3] = self.board[4, 4] = self.WHITE
        return self.board.copy(), self.BLACK, self.find_available_pos(self.BLACK)

    def step(self, pos, type):
        # update board
        dirs = self._avaliable_dirs(pos, type)
        flip_cnt = self._update_board(pos, dirs, type)

        reward = flip_cnt * self.R_FLIP
        if self._check_done():  # game over
            switch_player = False
            done = True
            reward += self.R_WIN if self._check_win(type) else self.R_LOSS
        else:
            # find available pos for opponent
            if not self.find_available_pos(self.BLACK if type == self.WHITE else self.WHITE):
                switch_player = False
                if not self.find_available_pos(type):
                    reward += self.R_WIN if self._check_win(type) else self.R_LOSS
                    done = True
                else:
                    done = False
            else:
                switch_player = True
                done = False

        return self.board.copy(), reward, switch_player, done

    def find_available_pos(self, this_player_type):
        available_actions = []
        for pos in self.all_actions:
            has_dirs = self._avaliable_dirs(pos, this_player_type)
            if has_dirs:  # avaliable pos
                available_actions.append(pos)
        return available_actions

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

    def get_end_reward(self, type):
        if self._check_win(type):
            return self.R_WIN
        else:
            return self.R_LOSS

    def get_winner(self):
        if self._check_win(self.BLACK):
            return self.BLACK
        else:
            return self.WHITE

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

    def _update_board(self, pos, dirs, type) -> int:
        """Return how many cells you flip"""
        cnt = 0
        # change this pos
        x, y = pos[0], pos[1]
        self.board[x, y] = type
        # traverse all directions
        for direction in dirs:
            x, y = pos[0], pos[1]
            x, y = x+direction[0], y+direction[1]
            while self.board[x, y] != type:
                self.board[x, y] = type
                cnt += 1
                x, y = x+direction[0], y+direction[1]
        if type == self.BLACK:
            self.black_cnt += cnt+1
            self.white_cnt -= cnt
        else:
            self.white_cnt += cnt+1
            self.black_cnt -= cnt
        return cnt+1

    def _check_done(self):
        return self.black_cnt+self.white_cnt == 64 or self.black_cnt == 0 or self.white_cnt == 0

    def _check_win(self, type):
        """Check done first"""
        if type == self.BLACK:
            return self.black_cnt > self.white_cnt
        else:
            return self.white_cnt > self.black_cnt

    def _out_of_bound(self, x):
        return x >= 8 or x < 0


if __name__ == '__main__':
    b = Board()

    observation = b.reset()

    current_player = 1
    while True:
        b.render()
        if current_player == 1:
            print('turn to: {}'.format(b.BLACK_PIC))
        else:
            print('turn to: {}'.format(b.WHITE_PIC))

        pos = input().strip()
        pos = int(pos[0]), int(pos[1])

        observation_, reward, switch_player, done = b.step(pos, current_player)

        if switch_player:
            if current_player == 1:
                current_player = 2
            else:
                current_player = 1

        if done:
            break

    print('Game over')
