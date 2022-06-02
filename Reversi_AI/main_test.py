from time import sleep

from env import Board
from brain import DQN


def train():
    step = 0
    for episode in range(1):
        state, current_player, available_actions = env.reset()
        env.render()

        while True:
            # pos = input().strip()
            # pos = int(pos[0]), int(pos[1])

            available_actions = env.find_available_pos(current_player)

            if current_player == ai_type:
                # no learning test
                action = RL.choose_action(state, available_actions)
                state_, reward, switch_player, done = env.step(action, current_player)

                # Store memory
                # RL.store_mem(state, action, reward, state_)

                # if step >= 500 and step % 21 == 0:
                #     RL.learn()
            else:
                # human
                pos = (-1, -1)
                while pos not in available_actions:
                    pos = input('Input action: ').strip()
                    if len(pos) != 2:
                        continue
                    pos = int(pos[0]), int(pos[1])
                state_, reward, switch_player, done = env.step(pos, current_player)
            if switch_player:
                if current_player == env.BLACK:
                    current_player = env.WHITE
                else:
                    current_player = env.BLACK

            state = state_
            step += 1

            env.render()
            if current_player == env.BLACK:
                print(f'turn to: {env.BLACK_PIC}')
            else:
                print(f'turn to: {env.WHITE_PIC}')

            if done:
                break

        # no model save
        # if RL1.epsilon >= RL1.epsilon_max/2 and episode % 5 == 4:
        #     RL1.save_model()
        # if RL2.epsilon >= RL2.epsilon_max/2 and episode % 5 == 4:
        #     RL2.save_model()

    # end of game
    print('game over')


if __name__ == '__main__':
    env = Board()

    _type = input('choose player: [b/w]')
    if not _type in ('b', 'w', 'B', 'W'):
        exit()

    if _type == 'b' or _type == 'B':
        ai_type = env.WHITE
    else:
        ai_type = env.BLACK

    if ai_type == env.BLACK:
        RL = DQN('_black_',  # "black"
                 env.all_actions,
                 learning_rate=0.01,
                 reward_decay=0.9,
                 e_greedy=1,
                 replace_target_step=200,
                 memory_size=2000,
                 batch_size=32,
                 # e_greedy_increment=0.05
                 # output_graph=True
                 )
    else:
        RL = DQN('_white_',  # "white"
                 env.all_actions,
                 learning_rate=0.01,
                 reward_decay=0.9,
                 e_greedy=1,
                 replace_target_step=200,
                 memory_size=2000,
                 batch_size=32,
                 # e_greedy_increment=0.05
                 # output_graph=True
                 )
    train()
