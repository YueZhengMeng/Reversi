from time import sleep

from env import Board
from brain import DQN


def train():
    step_black = 0
    step_white = 0
    black_win, white_win = 0, 0
    for episode in range(5000):
        print(episode, end=' ')
        first_step = True
        state, current_player, available_actions = env.reset()

        while True:
            available_actions = env.find_available_pos(current_player)

            if current_player == env.BLACK:
                action_b = agent_black.choose_action(state, available_actions)
                state_, reward_black, switch_player, done = env.step(action_b, current_player)

                if not first_step:
                    # delay store memory
                    agent_white.store_mem(_state, action_w, reward_white-reward_black, state)
                else:
                    first_step = False

                step_black += 1
                if step_black >= 500 and step_black % 20 == 0:
                    agent_black.learn()
            else:
                action_w = agent_white.choose_action(state, available_actions)
                state_, reward_white, switch_player, done = env.step(action_w, current_player)

                # delay store memory
                agent_black.store_mem(_state, action_b, reward_black-reward_white, state)

                step_white += 1
                if step_white >= 500 and step_white % 20 == 0:
                    agent_white.learn()

            if switch_player:
                current_player = env.BLACK if current_player == env.WHITE else env.WHITE

            if done:
                if current_player == env.BLACK:  # last step is BLACK
                    agent_black.store_mem(state, action_b, reward_black, state_)
                    # agent_white.reset_last_mem(env.get_end_reward(env.WHITE))
                else:
                    agent_white.store_mem(state, action_w, reward_white, state_)
                    # agent_black.reset_last_mem(env.get_end_reward(env.BLACK))
                if env.get_winner() == env.BLACK:
                    black_win += 1
                else:
                    white_win += 1
                break

            # pass state
            _state = state.copy()
            state = state_
            # print('ai: {}'.format(action))

            # env.render()
            # print(f'turn to: {env.BLACK_PIC if current_player == env.BLACK else env.WHITE_PIC}')

            # key to stop
            # input('press any')

        # model save
        if agent_black.epsilon >= agent_black.epsilon_max/2 and episode % 50 == 49:
            agent_black.save_model()
        if agent_white.epsilon >= agent_white.epsilon_max/2 and episode % 50 == 49:
            agent_white.save_model()

        # key to stop
        # input('press any')

    # end of training
    print('training over')
    print(step_black+step_white)
    print(f'black: {black_win}, white: {white_win}')


if __name__ == '__main__':
    env = Board()
    # 超参
    learning_rate = 0.01
    replace_target_step = 200
    greedy = 0.8
    reward_decay = 0.95
    memory_size = 2000
    batch_size = 64
    e_greedy_increment = 0.01  # None for no increments

    agent_black = DQN('_black_',  # "black"
                      env.all_actions,
                      learning_rate=learning_rate,
                      reward_decay=reward_decay,
                      e_greedy=greedy,
                      replace_target_step=replace_target_step,
                      memory_size=memory_size,
                      batch_size=batch_size,
                      e_greedy_increment=e_greedy_increment
                      )
    agent_white = DQN('_white_',  # "white"
                      env.all_actions,
                      learning_rate=learning_rate,
                      reward_decay=reward_decay,
                      e_greedy=greedy,
                      replace_target_step=replace_target_step,
                      memory_size=memory_size,
                      batch_size=batch_size,
                      e_greedy_increment=e_greedy_increment
                      )
    train()
