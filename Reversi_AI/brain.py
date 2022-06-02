import os
import numpy as np
from numpy import ndarray
import tensorflow as tf
keras = tf.keras


# 1 -> self
# 2 -> enemy
# 0 -> blank


class DQN:
    def __init__(
            self,
            id,
            all_actions,
            learning_rate=0.01,
            reward_decay=0.9,
            e_greedy=0.9,
            replace_target_step=300,
            memory_size=500,
            batch_size=32,
            e_greedy_increment=None,
            clear_nn=False
    ):
        self.id = id
        self.EVAL_PATH = "./dqn/tmp{}/eval".format(id)
        self.TARGET_PATH = "./dqn/tmp{}/target".format(id)
        self.WEIGHTS_PATH = "./dqn/tmp{}/weights".format(id)

        self.all_actions = all_actions
        self.lr = learning_rate
        self.rd = reward_decay
        self.replace_target_step = replace_target_step  # 更换 target_net 的步数

        # Memory
        self.state_len = 64
        self.action_len = 2
        self.memory_size = memory_size  # 记忆数量上限
        self.batch_size = batch_size    # 每次更新时从 memory 里面取多少记忆出来
        # 初始化全 0 记忆 [s, a, r, s_]
        self.memory = np.zeros((self.memory_size, self.state_len*2+self.action_len+1))  # 和视频中不同, 因为 pandas 运算比较慢, 这里改为直接用 numpy

        # Greedy
        self.epsilon_max = e_greedy     # epsilon 的最大值
        self.epsilon_increment = e_greedy_increment  # epsilon 的增量
        if e_greedy_increment:  # 是否开启探索模式, 并逐步减少探索次数
            self.epsilon = 0
        else:
            self.epsilon = self.epsilon_max

        if clear_nn:
            os.system('rm -rf {}'.format(self.EVAL_PATH))
            os.system('rm -rf {}'.format(self.TARGET_PATH))
            os.system('rm -rf {}'.format(self.WEIGHTS_PATH))
        self._build_model()

    def choose_action(self, state: ndarray, actions):
        if not actions:
            actions = self.all_actions

        actions_cnt = len(actions)

        if np.random.uniform() < self.epsilon:  # Use AI
            actions_list = np.zeros((actions_cnt, self.action_len))
            for i in range(actions_cnt):
                actions_list[i, :] = actions[i]

            # observation shape
            state = tf.constant(state.flatten())
            state = np.full((actions_cnt, self.state_len), state)

            s_a = np.hstack((state, actions_list))

            Q_predict = self.eval.predict(s_a)

            action = actions[np.argmax(Q_predict)]  # choose the max Q
        else:
            i = np.random.choice(actions_cnt)
            action = actions[i]
        return action

    def store_mem(self, s: np.ndarray, a, r, s_: ndarray):
        if not hasattr(self, 'memory_cnt'):
            self.memory_cnt = 0

        # 拉直
        s = s.flatten()
        s_ = s_.flatten()

        # 记录一条 [s, a, r, s_] 记录
        transition = np.hstack((s, a, (r,), s_))  # hstack 是水平拼接

        # 总 memory 大小是固定的, 如果超出总大小, 旧 memory 就被新 memory 替换
        index = self.memory_cnt % self.memory_size
        self.memory[index, :] = transition  # 替换过程

        self.memory_cnt += 1

    def reset_last_mem(self, r):
        index = (self.memory_cnt-1) % self.memory_size
        self.memory[index, self.state_len+self.action_len] = r

    def learn(self):
        # Replace target param
        if not hasattr(self, 'learn_step_cnt'):
            self.learn_step_cnt = 0
        if self.learn_step_cnt % self.replace_target_step == 0:
            self._update_target_param()
            print('\nUpdate target param\n')
        self.learn_step_cnt += 1

        # 随机抽取
        if self.memory_cnt > self.memory_size:
            sample_index = np.random.choice(self.memory_size, size=self.batch_size)
        else:
            sample_index = np.random.choice(self.memory_cnt, size=self.batch_size)
        batch_memory = self.memory[sample_index, :]

        states = batch_memory[:, :self.state_len]
        states_ = batch_memory[:, -self.state_len:]
        actions = batch_memory[:, self.state_len:self.state_len+self.action_len]
        rewards = batch_memory[:, self.state_len+self.action_len]

        max_Q = np.zeros((self.batch_size, 1))
        for action in self.all_actions:
            action = np.full((self.batch_size, 2), action)  # make list
            s__a = np.hstack((states_, action))  # expand
            predict_Q = self.target.predict_on_batch(s__a)  # predict
            max_Q = np.hstack((max_Q, predict_Q))  # add to final list
        max_Q = np.max(max_Q, axis=1)  # find max Q in every row

        target_data = rewards + self.rd * max_Q

        s_a = np.hstack((states, actions))

        self.eval.fit(s_a, target_data, epochs=1, verbose=0)

        if self.epsilon_increment and self.epsilon < self.epsilon_max:
            self.epsilon += self.epsilon_increment

    def save_model(self):
        self.eval.save(self.EVAL_PATH)
        self.target.save(self.TARGET_PATH)

    def _build_model(self):
        # random_init = keras.initializers.HeNormal()

        # --------------- Q net ---------------
        if os.path.exists(self.EVAL_PATH):
            self.eval = keras.models.load_model(self.EVAL_PATH)
        else:
            self.eval = keras.Sequential([
                keras.layers.Dense(128, kernel_initializer=keras.initializers.HeNormal(), input_dim=self.state_len+self.action_len),
                keras.layers.Dense(128, activation='elu', kernel_initializer=keras.initializers.HeNormal()),
                keras.layers.Dense(64, activation='elu', kernel_initializer=keras.initializers.HeNormal()),
                keras.layers.Dense(1, activation='elu', kernel_initializer=keras.initializers.HeNormal())
            ])
            self.eval.compile(
                optimizer=keras.optimizers.Adam(learning_rate=self.lr),
                loss='mean_squared_error',
                metrics=['accuracy']
            )

        # ------------ Target net -------------
        if os.path.exists(self.TARGET_PATH):
            self.target = keras.models.load_model(self.TARGET_PATH)
        else:
            self.target = keras.Sequential([
                keras.layers.Dense(128, kernel_initializer=keras.initializers.HeNormal(), input_dim=self.state_len+self.action_len),
                keras.layers.Dense(128, activation='elu', kernel_initializer=keras.initializers.HeNormal()),
                keras.layers.Dense(64, activation='elu', kernel_initializer=keras.initializers.HeNormal()),
                keras.layers.Dense(1, activation='elu', kernel_initializer=keras.initializers.HeNormal())
            ])
            self.target.compile(
                optimizer=keras.optimizers.Adam(learning_rate=self.lr),
                loss='mean_squared_error',
                metrics=['accuracy']
            )

    def _update_target_param(self):
        self.eval.save_weights(self.WEIGHTS_PATH)
        self.target.load_weights(self.WEIGHTS_PATH)
