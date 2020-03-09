from vowpalwabbit import pyvw
import bandits as bd
import time

n_arms = 6
n_trials = 500
n_experiments = 1 #Note, we can only run one experiment now.

#These were the ones for the tests with estimators:
#"Naive500DM", "Complex500DM","Naive500IPS", "Complex500IPS", "Naive500DR", "Complex2000DR", "Naive2000DM", "Complex2000DM", "Naive2000IPS", "Complex2000IPS", "Naive2000DR"

#These were the ones for the tests with bag/cover numbers for complex cases:
# "Complex500DRT2", "Complex500DMT2", "Complex500IPST2", "Complex2000DRT2", "Complex2000DMT2", "Complex2000IPST2",
#                         "Complex500DRT4", "Complex500DMT4", "Complex500IPST4", "Complex2000DRT4", "Complex2000DMT4","Complex2000IPST4",
#                         "Complex500DRT6", "Complex500DMT6",

test_names={}
test_names[0]=["Complex500IPST6", "Complex500DRT6", "Complex500DMT6"]

test_names[1]=["Complex2000DRT6", "Complex2000DMT6","Complex2000IPST6"]

test_names[2]= ["Complex500DRT8", "Complex500DMT8", "Complex500IPST8"]

test_names[3]=["Complex2000DRT8", "Complex2000DMT6","Complex2000IPST8"]

test_names[4]=["Complex500DRT10", "Complex500DMT10","Complex500IPST10"]

test_names[5]=["Complex2000DRT10", "Complex2000DMT10","Complex2000IPST10"]

test_names[6]=["Complex500DRT14", "Complex500DMT14", "Complex500IPST14"]

test_names[7]=["Complex2000DRT14", "Complex2000DMT14", "Complex2000IPST14"]

test_names[8]=["Complex500DRT16", "Complex500DMT16", "Complex500IPST16"]

test_names[9]=["Complex2000DRT16","Complex2000DMT16", "Complex2000IPST16"]

test_names[10]=["Complex500DRT20", "Complex500DMT20", "Complex500IPST20"]

test_names[11]=["Complex2000DRT20","Complex2000DMT20", "Complex2000IPST20"]



for num in range(0,12):
    contextual_bandits = []
    for tn in test_names[num]:
        if num in [0,2,4,6,8,10]:
            n_trials = 500
        else:
            n_trials = 10000
        test_name = tn
        if (tn.find("Complex")>=0):
            if (tn.find("T2") >= 0):
                labels = ["Default",
                      "Bag 2",
                      "Cover 2"]
                instructions = ["--cb 6", "--cb_explore 6 --bag 2 ", "--cb_explore 6 --cover 2 "]
            elif (tn.find("T4") >= 0):
                labels = ["Default",
                      "Bag 4",
                      "Cover 4"]
                instructions = ["--cb 6", "--cb_explore 6 --bag 4 ", "--cb_explore 6 --cover 4 "]
            elif (tn.find("T6") >= 0):
                labels = ["Default",
                      "Bag 6",
                      "Cover 6"]
                instructions = ["--cb 6", "--cb_explore 6 --bag 6 ", "--cb_explore 6 --cover 6 "]
            elif (tn.find("T8") >= 0):
                labels = ["Default",
                      "Bag 8",
                      "Cover 8"]
                instructions = ["--cb 6", "--cb_explore 6 --bag 8 ", "--cb_explore 6 --cover 8 "]
            elif (tn.find("T10") >= 0):
                labels = ["Default",
                      "Bag 10",
                      "Cover 10"]
                instructions = ["--cb 6", "--cb_explore 6 --bag 10 ", "--cb_explore 6 --cover 10 "]
            elif (tn.find("T14") >= 0):
                labels = ["Default",
                      "Bag 14",
                      "Cover 14"]
                instructions = ["--cb 6", "--cb_explore 6 --bag 14 ", "--cb_explore 6 --cover 14 "]
            elif (tn.find("T16") >= 0):
                labels = ["Default",
                      "Bag 16",
                      "Cover 16"]
                instructions = ["--cb 6", "--cb_explore 6 --bag 16 ", "--cb_explore 6 --cover 16 "]
            elif (tn.find("T20") >= 0):
                labels = ["Default",
                      "Bag 20",
                      "Cover 20"]
                instructions = ["--cb 6", "--cb_explore 6 --bag 20 ", "--cb_explore 6 --cover 20 "]
            else:
                labels = ["Default",
                      "Bag 12",
                      "Cover 12"]
                instructions = ["--cb 6", "--cb_explore 6 --bag 12 ", "--cb_explore 6 --cover 12 "]
            if (tn.find("DR")>=0):
                contextual_bandits = [
                pyvw.vw(instructions[0]),
                pyvw.vw(instructions[1]+"--cb_type dr"),
                pyvw.vw(instructions[2]+"--cb_type dr")
                ]
            elif (tn.find("DM")>=0):
                contextual_bandits = [
                pyvw.vw(instructions[0]),
                pyvw.vw(instructions[1]+"--cb_type dm"),
                pyvw.vw(instructions[2]+"--cb_type dm")
                ]
            else: #IPS
                contextual_bandits = [
                    pyvw.vw(instructions[0]),
                    pyvw.vw(instructions[1] + "--cb_type ips"),
                    pyvw.vw(instructions[2] + "--cb_type ips")
                ]
        elif(tn.find("Naive")>=0):#Naive
            labels = ["Default",
                      "First 12",
                      "EGreedy 0.1",
                      "EGreedy 0.01",
                      ]
            instructions = ["--cb 6", "--cb_explore 6 --first 12 ", "--cb_explore 6 --epsilon 0.1 ", "--cb_explore 6 --epsilon 0.01 "]
            if (tn.find("DR")>=0):
                contextual_bandits = [
                    pyvw.vw(instructions[0]),
                    pyvw.vw(instructions[1] + "--cb_type dr"),
                    pyvw.vw(instructions[2] + "--cb_type dr"),
                    pyvw.vw(instructions[3] + "--cb_type dr")
                ]
            elif (tn.find("DM")>=0):
                contextual_bandits = [
                    pyvw.vw(instructions[0]),
                    pyvw.vw(instructions[1] + "--cb_type dm"),
                    pyvw.vw(instructions[2] + "--cb_type dm"),
                    pyvw.vw(instructions[3] + "--cb_type dm")
                ]
            else: #IPS
                contextual_bandits = [
                    pyvw.vw(instructions[0]),
                    pyvw.vw(instructions[1] + "--cb_type ips"),
                    pyvw.vw(instructions[2] + "--cb_type ips"),
                    pyvw.vw(instructions[3] + "--cb_type ips")
                ]

        if tn.find("2000")>=0:
            n_trials=10000
        else:
            n_trials=500

        print(test_name)
        print(labels)
        print(instructions)
        env = bd.ContextualEnvironment(contextual_bandits, labels, 6, 'Testing Several Contextual Agents')
        start_time = time.time()
        scores, optimal = env.run(n_trials, n_experiments, test_name)
        elapsed_time = time.time() - start_time
        env.plot_results(scores, optimal, test_name)
        print("Execution time", elapsed_time)


'''
train_data = [{'action': 1, 'cost': 2, 'probability': 0.4, 'feature1': 'a', 'feature2': 'c', 'feature3': ''},
              {'action': 3, 'cost': 0, 'probability': 0.2, 'feature1': 'b', 'feature2': 'd', 'feature3': ''},
              {'action': 4, 'cost': 1, 'probability': 0.5, 'feature1': 'a', 'feature2': 'b', 'feature3': ''},
              {'action': 2, 'cost': 1, 'probability': 0.3, 'feature1': 'a', 'feature2': 'b', 'feature3': 'c'},
              {'action': 3, 'cost': 1, 'probability': 0.7, 'feature1': 'a', 'feature2': 'd', 'feature3': ''}]

train_df = pd.DataFrame(train_data)

## add index to df
train_df['index'] = range(1, len(train_df) + 1)
train_df = train_df.set_index("index")

# generate some test data that you want the CB to make decisions for, e.g. features describing new users, for the CB to exploit
test_data = [{'feature1': 'b', 'feature2': 'c', 'feature3': ''},
            {'feature1': 'a', 'feature2': '', 'feature3': 'b'},
            {'feature1': 'b', 'feature2': 'b', 'feature3': ''},
            {'feature1': 'a', 'feature2': '', 'feature3': 'b'}]

test_df = pd.DataFrame(test_data)

## add index to df
test_df['index'] = range(1, len(test_df) + 1)
test_df = test_df.set_index("index")

print(train_df)
print(test_df)

#Here we create a model with 4 actions, explore first 2
vwdefault = pyvw.vw("--cb 4")
vwfirst = pyvw.vw("--cb_explore 4 --first 2 --cb_type dr") #we could also use as types the dm, dr or ips
vwegreedy = pyvw.vw("--cb_explore 4 --epsilon 0.1 --cb_type dr") #we could also use as types the dm, dr or ips
vwbag = pyvw.vw("--cb_explore 4 --bag 5 --cb_type dr") #we could also use as types the dm, dr or ips
vwcover = pyvw.vw("--cb_explore 4 --cover 4 --cb_type dr") #we could also use as types the dm, dr or ips

# use the learn method to train the vw model, train model row by row using a loop
#i==1
for i in train_df.index:
  ## provide data to cb in requested format
  action = train_df.loc[i, "action"]
  cost = train_df.loc[i, "cost"]
  probability = train_df.loc[i, "probability"]
  feature1 = train_df.loc[i, "feature1"]
  feature2 = train_df.loc[i, "feature2"]
  feature3 = train_df.loc[i, "feature3"]
  ## do the actual learning
  
  vwdefault.learn(
      str(action) + ":" + str(cost) + ":" + str(probability) + " | " + str(feature1) + " " + str(feature2) + " " + str(
          feature3))
  vwfirst.learn(str(action)+":"+str(cost)+":"+str(probability)+" | "+str(feature1)+" "+str(feature2)+" "+str(feature3))
  vwegreedy.learn(
      str(action) + ":" + str(cost) + ":" + str(probability) + " | " + str(feature1) + " " + str(feature2) + " " + str(
          feature3))
  vwbag.learn(
      str(action) + ":" + str(cost) + ":" + str(probability) + " | " + str(feature1) + " " + str(feature2) + " " + str(
          feature3))
  vwcover.learn(
      str(action) + ":" + str(cost) + ":" + str(probability) + " | " + str(feature1) + " " + str(feature2) + " " + str(
          feature3))
  

# use the same model object that was trained to perform predictions

# predict row by row and output results
#j==1
for j in test_df.index:
  feature1 = test_df.loc[j, "feature1"]
  feature2 = test_df.loc[j, "feature2"]
  feature3 = test_df.loc[j, "feature3"]

  choice = vwdefault.predict("| " + str(feature1) + " " + str(feature2) + " " + str(feature3))
  print("Default", j, choice)

  choice = vwfirst.predict("| " + str(feature1) + " " + str(feature2) + " " + str(feature3))
  print("First", j, choice)

  choice = vwegreedy.predict("| "+str(feature1)+" "+str(feature2)+" "+str(feature3))
  print("Greedy",  j, choice)

  choice = vwbag.predict("| "+str(feature1)+" "+str(feature2)+" "+str(feature3))
  print("Bag",  j, choice)

  choice = vwcover.predict("| "+str(feature1)+" "+str(feature2)+" "+str(feature3))
  print("Cover",  j, choice)

# the CB assigns every instance to action 3 as it should per the cost structure of the train data; you can play with the cost structure to see that the CB updates its predictions accordingly
'''

'''
# BONUS: save and load the CB model
# save model
vwdefault.save('cbdefault.model')
del vwdefault
# load from saved file
vwdefault = pyvw.vw("--cb 4 -i cbdefault.model")
print(vwdefault.predict('| a b'))

vwfirst.save('cbfirst.model')
del vwfirst
# load from saved file
vwfirst = pyvw.vw("--cb 4 -i cbfirst.model")
print(vwfirst.predict('| a b'))

vwegreedy.save('cbegreedy.model')
del vwegreedy
# load from saved file
vwegreedy = pyvw.vw("--cb 4 -i cbegreedy.model")
print(vwegreedy.predict('| a b'))

vwbag.save('cbbag.model')
del vwbag
# load from saved file
vwbag = pyvw.vw("--cb 4 -i cbbag.model")
print(vwbag.predict('| a b'))

vwcover.save('cbcover.model')
del vwcover
# load from saved file
vwcover = pyvw.vw("--cb 4 -i cbcover.model")
print(vwcover.predict('| a b'))
'''