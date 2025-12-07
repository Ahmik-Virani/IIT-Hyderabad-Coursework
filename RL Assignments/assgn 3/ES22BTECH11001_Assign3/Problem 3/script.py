import gymnasium as gym
import ale_py
import cv2
import numpy as np
import pygame
import random
from collections import deque
import matplotlib.pyplot as plt

import torch
import torch.nn as nn
import torch.nn.functional as F
from torchsummary import summary
import torch.optim as optim

# We will use a simple single hidden layer neural network
class PolicyNetwork(nn.Module):
    def __init__(self, state_dim, action_dim, hidden_dim=128):
        super(PolicyNetwork, self).__init__()

        self.fc1 = nn.Linear(state_dim, hidden_dim)
        self.fc2 = nn.Linear(hidden_dim, action_dim)
    
    def forward(self, x):
        x = torch.relu(self.fc1(x))
        output = torch.softmax(self.fc2(x), dim=-1)
        return output
    
class REINFORCE():
    def __init__(self, state_dim, action_dim, reward_to_go, advantage_normalization, learning_rate, hidden_dim=128):
        # We will use the above neural network for the policy
        self.policy = PolicyNetwork(state_dim, action_dim, hidden_dim)

        # We use adam optimizer
        self.optimizer = optim.Adam(self.policy.parameters(), lr=learning_rate)

        # We need to do some book keeping for rewards and log probabilities
        self.episode_rewards = []
        self.episode_log_probs = []

        # Boolean values, if we need reward to go and advantage normalization
        self.reward_to_go = reward_to_go
        self.advantage_normalization = advantage_normalization

    # Finds the probability for action given the state
    def select_action(self, state, testing = False):
        state = torch.tensor(np.asarray(state, dtype=np.float32).flatten()).unsqueeze(0)
        # We get the probability distribution of actions over the states
        probs = self.policy(state)
        # We sample actions acording to the given probabilities
        distributions = torch.distributions.Categorical(probs)
        action = distributions.sample()
        # Need to store the log probability for policy gradient step
        if not testing:
            log_prob = distributions.log_prob(action)
            self.episode_log_probs.append(log_prob)
        return action.item()

    # Function to get the policy gradient value
    def get_policy_grad(self, gamma):
        # We need the rewards G_t for each time step of the episode
        discounted_rewards = []
        cumulative_reward = 0

        # If reward to go, then at each time step, we have to compute the reward as r_t + gamma * r_{t-1}
        if(self.reward_to_go):
            for reward in reversed(self.episode_rewards):
                cumulative_reward = reward + gamma * cumulative_reward
                discounted_rewards.insert(0, cumulative_reward)
        # If not reward to go, each time step gets the same G_{0:infinity}
        else:
            for reward in reversed(self.episode_rewards):
                cumulative_reward = reward + gamma * cumulative_reward
            discounted_rewards = [cumulative_reward] * len(self.episode_rewards)
        
        discounted_rewards = torch.tensor(discounted_rewards, dtype=torch.float32)

        # If we do advantage normalization, I use mean baseline and then normalize
        if(self.advantage_normalization):
            discounted_rewards = (discounted_rewards - discounted_rewards.mean()) / (discounted_rewards.std() + 1e-9)

        # Compute the policy loss
        loss=0
        for log_prob, reward in zip(self.episode_log_probs, discounted_rewards):
            loss += -log_prob * reward

        # Reset the episode rewards before going to the next episode
        self.episode_rewards = []
        self.episode_log_probs = []
        
        return loss

    # Perform the gradient descent step
    def update_policy(self, average_loss):
        self.optimizer.zero_grad()
        average_loss.backward()
        self.optimizer.step()

# Training curves
def plot_rewards(rewards, window=50):
    plt.figure(figsize=(10, 5))
    
    # Plot raw rewards
    plt.plot(rewards, label='Episode Reward', color='lightgray', alpha=0.7)
    
    # Compute moving average
    if len(rewards) >= window:
        moving_avg = np.convolve(rewards, np.ones(window)/window, mode='valid')
        plt.plot(range(window - 1, len(rewards)), moving_avg,
                 label=f'Moving Average (window={window})', color='blue', linewidth=2)
    
    plt.xlabel('Episode')
    plt.ylabel('Total Reward')
    plt.title('Training Progress at each episode')
    plt.legend()
    plt.grid(True, linestyle=':', alpha=0.7)
    plt.show()

def plot_test_rewards(rewards):
    plt.figure(figsize=(10,5))
    plt.plot(rewards, label='Average Episode Reward')
    plt.xlabel('Number of Iterations')
    plt.ylabel('Average reward on 100 tests after each iteration')
    plt.title('Training Progress at each iteration')
    plt.legend()
    plt.show()

def run(env_name, reward_to_go, advantage_normalization, number_of_iterations, batch_size, gamma, lr, hidden_dim_size):
    rewards_with_different_settings = []

    # This is the training loop as per the lecture slides (pg.pdf)
    env = gym.make(env_name)
    agent = REINFORCE(env.observation_space.shape[0], env.action_space.n, reward_to_go, advantage_normalization, lr, hidden_dim_size)
    episode_rewards = []

    test_episode_rewards = []

    N = number_of_iterations
    K = batch_size

    for n in range(N):
        total_loss = 0

        # Sample K trajectories by executing the policy
        for episode in range(K):

            state, _ = env.reset()
            total_reward = 0
            done = False
            # All the sampling is done as per the current policy
            while not done:
                action = agent.select_action(state)
                next_state, reward, terminated, truncated, info = env.step(action)
                done = (terminated or truncated)
                agent.episode_rewards.append(reward)
                total_reward += reward
                state = next_state
            episode_rewards.append(total_reward)
            total_loss += agent.get_policy_grad(gamma)
        average_loss = total_loss / K
        # We update the policy depending on the sample loss
        agent.update_policy(average_loss)

        # Let us test for some episodes
        total_test_reward = 0
        for test_episode in range(100):
            state, _ = env.reset()
            done = False
            while not done:
                action = agent.select_action(state, True)
                next_state, reward, terminated, truncated, info = env.step(action)
                done = (terminated or truncated)
                total_test_reward += reward
                state = next_state
        # Append average reward per 100 episodes
        test_episode_rewards.append(total_test_reward/100)

    plot_rewards(episode_rewards)
    plot_test_rewards(test_episode_rewards)
    rewards_with_different_settings.append(test_episode_rewards)
    env.close()


def main():
    env_name = input("Enter the environment to be run on - LunarLander-v3 or CartPole-v0: ")
    reward_to_go = input("Enable reward to go? (Enter True or False): ")
    advantage_normalization = input("Enable advantage normalization? (Enter True or False): ")
    number_of_iterations = int(input("Enter number of iterations: "))
    batch_size = int(input("Enter the batch size: "))
    gamma = float(input("Enter discount rate: "))
    lr = float(input("Enter learning rate: "))
    hidden_dim = int(input("Enter the hidden layer size of the Neural Network: "))

    if reward_to_go == 'True':
        reward_to_go = True
    else:
        reward_to_go = False

    if advantage_normalization == 'True':
        advantage_normalization = True
    else:
        advantage_normalization = False

    run(env_name, reward_to_go, advantage_normalization, number_of_iterations, batch_size, gamma, lr, hidden_dim)

if __name__ == "__main__":
    main()