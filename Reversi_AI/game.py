from turtle import st
from env2 import Board
from brain import DQN
import tensorflow as tf

class Game:
    def __init__(self) -> None:
        devices = tf.config.list_physical_devices('GPU')
        tf.config.experimental.set_memory_growth(devices[0], True)
        self.env = Board()

    def reset(self, side):
        if side != self.env.BLACK and side != self.env.WHITE:
            return False
        
        state, available_actions = self.env.reset()
        self.ai_side = -side
        if self.ai_side == self.env.BLACK:
            self.RL = DQN('_black_',  # "black"
                    self.env.ALL_ACTIONS,
                    learning_rate=0.01,
                    reward_decay=0.9,
                    e_greedy=1,
                    replace_target_step=200,
                    memory_size=2000,
                    batch_size=32,
                    # e_greedy_increment=0.05
                    # output_graph=True
                    )
            action = self.RL.choose_action(state, available_actions)
            self.env.step(action)
        else:
            self.RL = DQN('_white_',  # "white"
                    self.env.ALL_ACTIONS,
                    learning_rate=0.01,
                    reward_decay=0.9,
                    e_greedy=1,
                    replace_target_step=200,
                    memory_size=2000,
                    batch_size=32,
                    # e_greedy_increment=0.05
                    # output_graph=True
                    )
        return True

    def _2d_to_1d(self, x, y):
        return x * 8 + y
    
    def _1d_to_2d(self, index):
        x = index // 8
        y = index % 8
        return x, y

    def next_player(self):
        return self.env.current_player

    def place(self, index):
        pos = self._1d_to_2d(index)
        board, reward, avaliable_pos, gameover = self.env.step(pos)
        while not gameover and self.env.current_player == self.ai_side:
            action = self.RL.choose_action(board, avaliable_pos)
            board, reward, avaliable_pos, gameover = self.env.step(action)

    def winner(self):
        return self.env.get_winner()

