# -*- coding: utf-8 -*-
"""

Author: Vaasudevan Srinivasan (vaasdevans.github.io)
Created on: April 09, 2019
Last Modified: April 11, 2019

Implemented various SVM classifiers on the Satellite Imagery which is
read using GDAL

"""

#%% Import the modules
import matplotlib.pyplot as plt
from osgeo import gdal, gdal_array
from sklearn.metrics import *
from sklearn import svm
import numpy as np

#%% Inorder to see the accuracy of SVM Classifier, it is assumed that
# MLC classifier performed in ArcGIS is 100% accurate

inRaster = "Dataset.tif"
trainRaster = "trainingSites.tif"
goldStdRaster = "MaxLikelihood.tif"

RGB = (4,3,2) # Specify the RGB bands for visualisation
classes = {'0':"Forest", '1':"Urban", '2':"Water", '3':"Barren Land"}

#%% Load the dataset and access the raster properties
ds = gdal.Open(inRaster)
cols, rows = ds.RasterXSize, ds.RasterYSize
bands = ds.RasterCount
dsType = gdal_array.GDALTypeCodeToNumericTypeCode(ds.GetRasterBand(1).DataType)
dsProj = ds.GetProjection()
dsTransform = ds.GetGeoTransform()
print("Shape: %s | type: %s | Bands: %s" % ((rows, cols), dsType, bands))

# Create a numpy array with all the bands from the input raster
imgArr = np.zeros((cols, rows, bands), dsType)
for band in range(bands):
    imgArr[:,:,band] = ds.GetRasterBand(band+1).ReadAsArray()

# Load the Training class raster
train_ds = gdal.Open(trainRaster)
trainArr = train_ds.GetRasterBand(1).ReadAsArray()
print("Training classes present: ", np.unique(trainArr))

# Load the Gold Standard raster
goldStd_ds = gdal.Open(goldStdRaster)
goldStdArr = goldStd_ds.GetRasterBand(1).ReadAsArray()

#%% Visualize the dataset and training sites using matplotlib
# Converted to 8-bit image using np.interp just for visualization
imgPlt = np.interp(imgArr,
                   (imgArr.min(), imgArr.max()),
                   (0,255)).astype(np.uint8)

plt.subplot(131)
plt.imshow(imgPlt[:,:,RGB])
plt.title(inRaster)

plt.subplot(132)
plt.imshow(trainArr)
plt.title(trainRaster)

plt.subplot(133)
plt.imshow(goldStdArr, cmap='plasma')
plt.title(goldStdRaster)

plt.tight_layout()
plt.show()

#%% Prepare the Dataset for Training
# Step1: Create a mask from the training raster. values > 0
# Step2: Apply mask on the Original arr to get the X parameter for the model
# Step3: Apply mask on the training raster to get Y parameter for the model

# trainX will become scalar vector of `trainY` length with `n` dimensions

mask = trainArr > 0
trainX = imgArr[mask]
trainY = trainArr[mask]
print("Model-X: %s; Model-Y: %s" % (trainX.shape, trainY.shape))

#%% Helper functions and variables for model training and reporting
# Create a new shape for predicting the whole image to match with the trained
# model shape
newShape = (imgArr.shape[0] * imgArr.shape[1], imgArr.shape[2])

# Change the Image array to the new shape
testArr = imgArr.reshape(newShape)
print("%s is changed to %s" % (imgArr.shape, testArr.shape))

# Prepare the GoldStd array for classifier report
goldStd = goldStdArr.reshape(newShape[0])

# Generic function for printing out the classifier's report
def print_report(predicted):
    accuracy = accuracy_score(goldStd, predicted)
    print(f"Accuracy: {accuracy*100:.2f} %\n")
    print(classification_report(goldStd, predicted, target_names=classes))
    print(confusion_matrix(goldStd, predicted_lin))

#%% Train the SVM Linear model with C value of 0.9
clfLinear = svm.SVC(kernel="linear", C=0.9)
clfLinear.fit(trainX, trainY)

# To get the Support Vectors, uncomment the below line and execute
# print(clfLinear.support_vectors_)

# Predicting for the whole image
predicted_lin = clfLinear.predict(testArr)

# Print the classifier reports
print_report(predicted_lin)

#%% Train the SVM polynomial model with gamma value of 1.0 and coef0 as 100
clfPoly = svm.SVC(kernel="poly", gamma=1.0, coef0=100)
clfPoly.fit(trainX, trainY)

# Predicting for the whole image
predicted_poly = clfPoly.predict(testArr)

# Print the classifier reports
print_report(predicted_poly)

#%% Train the SVM RBF (Radial Bias Function) model with gamma value of 1.0
clfRBF = svm.SVC(kernel="rbf", gamma=1.0)
clfRBF.fit(trainX, trainY)

# Predicting for the whole image
predicted_rbf = clfRBF.predict(testArr)

# Print the classifier reports
print_report(predicted_rbf)

#%% Convert the predicted shape to image shape for Visualisation and saving
imgShp = imgArr.shape[:-1]

# Visualise the Image again!
plt.subplot(231)
plt.imshow(imgPlt[:,:,(4,3,2)])
plt.title(inRaster)

# Visualise the GoldStandard again!
plt.subplot(232)
plt.imshow(goldStdArr, cmap='plasma')
plt.title(goldStdRaster)

# Visualise the Predicted classes Rasters
plt.subplot(234)
plt.imshow(predicted_lin.reshape(imgShp), cmap='plasma')
plt.title("Linear- 0.9")

plt.subplot(235)
plt.imshow(predicted_poly.reshape(imgShp), cmap='plasma')
plt.title("Gamma- 1.0")

plt.subplot(236)
plt.imshow(predicted_rbf.reshape(imgShp), cmap='plasma')
plt.title("RBF- 1.0")

plt.tight_layout()
plt.show()

#%% Write the classified rasters using GDAL
def save_classified(data, outfile):

    # Create a Driver for GTiff
    driver = gdal.GetDriverByName('GTiff')
    driver.Register()

    # Create a Dataset
    outDataset = driver.Create(outfile, cols, rows, 1, gdal.GDT_Byte)
    outDataset.SetGeoTransform(dsTransform)
    outDataset.SetProjection(dsProj)

    # Write the raster array to the Dataset
    outBand = outDataset.GetRasterBand(1)
    outBand.WriteArray(data, 0, 0)

# Predicted should be reshaped to match with the image shape
# The following line can be modified for any satisfying predicted raster
save_classified(predicted_lin.reshape(imgShp), "Linear.tif")

#%% Verify the authenticity of the output save_classified function
outRaster = gdal.Open("Linear.tif")
plt.imshow(outRaster.GetRasterBand(1).ReadAsArray())
plt.show()

# EOF