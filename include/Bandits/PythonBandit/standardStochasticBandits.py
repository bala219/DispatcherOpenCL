import os
import sys
module_path = os.path.abspath(os.path.join('..'))
if module_path not in sys.path:
    sys.path.append(module_path)

import bandits as bd
import time

n_arms = 6
n_trials = 10
n_experiments = 1
bandit1 = bd.BernoulliBandit(n_arms, t=3*n_trials) #.GaussianBandit(n_arms)

bandit2 = bd.GaussianBandit(n_arms)

policy = bd.SoftmaxPolicy()
gaussian_agents = [
    #bd.Agent(bandit2, bd.GreedyPolicy()),
    bd.Agent(bandit2, bd.GreedyPolicy(), prior=5)
    #bd.Agent(bandit2, bd.EpsilonGreedyPolicy(0.01)),
    #bd.Agent(bandit2, bd.EpsilonGreedyPolicy(0.1)),
    #bd.Agent(bandit2, bd.UCBPolicy(2)),
    #bd.Agent(bandit2, bd.UCBPolicy(1)),
    #bd.GradientAgent(bandit2, policy, alpha=0.1),
    #bd.GradientAgent(bandit2, policy, alpha=0.4),
    #bd.GradientAgent(bandit2, policy, alpha=0.1, baseline=False),
    #bd.GradientAgent(bandit2, policy, alpha=0.4, baseline=False)
]


bernoulli_agents = [
    #bd.BetaAgent(bandit1, bd.GreedyPolicy()), #Thompson Sampling
    #bd.Agent(bandit1, bd.GreedyPolicy()),
    #bd.Agent(bandit1, bd.GreedyPolicy(), prior=5),
    bd.Agent(bandit1, bd.EpsilonGreedyPolicy(0.01)),
    #bd.Agent(bandit1, bd.EpsilonGreedyPolicy(0.1))
  #  bd.Agent(bandit1, bd.UCBPolicy(2)),
   # bd.Agent(bandit1, bd.UCBPolicy(1)),


]

env = bd.Environment(bandit2, gaussian_agents, 'Testing Several Gaussian Agents')
counter = 0
for agent in gaussian_agents:
    print(counter)
    print("Value estimates", agent.value_estimates)
    print("Action attempts", agent.action_attempts)
    counter+=1

start_time = time.time()
scores, optimal = env.run(n_trials, n_experiments)
elapsed_time = time.time() - start_time

print(" First test (Gaussian) Done in (seconds), ", elapsed_time)
env.plot_results(scores, optimal)
env.plot_beliefs()
counter = 0
for agent in gaussian_agents:
    print(counter)
    print("Value estimates", agent.value_estimates)
    print("Action attempts", agent.action_attempts)
    counter+=1

'''
print("Starting 2nd test: Bernoulli")

env = bd.Environment(bandit1, bernoulli_agents, 'Testing Several Bernoulli Agents')
counter = 0
for agent in bernoulli_agents:
    print(counter)
    print("Value estimates", agent.value_estimates)
    print("Action attempts", agent.action_attempts)
    counter+=1

start_time = time.time()
scores, optimal = env.run(n_trials, n_experiments)
elapsed_time = time.time() - start_time

print(" Second test (Bernoulli) done in (seconds), ", elapsed_time)
env.plot_results(scores, optimal)
env.plot_beliefs()
counter = 0
for agent in bernoulli_agents:
    print(counter)
    print("Value estimates", agent.value_estimates)
    print("Action attempts", agent.action_attempts)
    counter+=1
'''


