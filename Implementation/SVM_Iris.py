# -*- coding: utf-8 -*-
"""

Author: Vaasudevan Srinivasan (vaasdevans.github.io)
Created on: April 09, 2019
Last Modified: April 11, 2019

About: Implemented various SVM classifiers on the Iris dataset

"""

# Import the modules
from mlxtend.plotting import plot_decision_regions
from sklearn import svm, model_selection as ms
from sklearn.metrics import *
import matplotlib.pyplot as plt
import pandas as pd

#%% Load the Dataset
cols = ["SLength", "SWidth", "PLength", "PWidth", "Class"]
types = ["Setosa", "Versicolor", "Virginica"]
iris = pd.read_csv("iris-data.txt", sep="\t", names=cols)

#%% Plot the values (plotting is done with pandas)
colors = [{1:'red', 2:'blue', 3:'green'}[i] for i in iris.Class]
iris.plot.scatter(x='SLength', y='PLength', c=colors)

#%% Split the Dataset into Training (70%) and Testing(30%)
# Model just uses the Sepal and Petal Length
iris_len = pd.DataFrame([iris.SLength, iris.PLength, iris.Class]).transpose()
train, test = ms.train_test_split(iris_len, test_size=0.3, random_state=1)
cTrain, cTest = train.pop('Class'), test.pop('Class')

# Test the trained model with various SVM models and visualize them
for params,p in [({'kernel': 'linear', 'C': 1.0}, 221),
                 ({'kernel': 'poly', 'gamma': 0.1}, 222),
                 ({'kernel': 'rbf', 'gamma': 1}, 223),
                 ({'kernel': 'rbf', 'gamma': 20}, 224)
                ]:

    # Classifier
    clf = svm.SVC(**params)
    clf.fit(train, cTrain)

    # Predict
    accuracy = accuracy_score(cTest, clf.predict(test)) *100

    # Prepare for visualising the Hyper-plane
    X, Y = train.values, cTrain.values.astype(pd.np.integer)

    # Plot the hyperplane
    ttl = "%s kernel: %.2f%%" % (params['kernel'], accuracy)
    plt.subplot(p)
    plt.title(ttl)
    plot_decision_regions(X=X, y=Y, clf=clf, legend=2)

plt.tight_layout()
plt.show()

# EOF