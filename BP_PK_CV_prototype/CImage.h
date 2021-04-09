#pragma once

#include <iostream>
#include <vector>

//enabling and disabling experimental and nonfree modules
#include "experimentalModules.h"

//Matrices
#include <opencv2/core/mat.hpp>
//reading writing
#include <opencv2/imgcodecs.hpp>
//features
#include <opencv2/features2d.hpp>
//experimental feature algorithms - eg. BEBLID
#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
#include <opencv2/xfeatures2d.hpp>
#endif

//clahe and other image processing
#include <opencv2/imgproc.hpp>

#include "CLogger.h"
//process parameters needed to know how to process things
#include "SProcessParams.h"

using namespace std;
using namespace cv;

class CImage {
public:
	//methods for detection and description
protected:
	const string filePath_;
	Mat image_;
	bool wasProcessed_ = false;
	vector<KeyPoint> imageKeypoints_;
	Mat keypointsDescriptors_;

	Ptr<Feature2D> getDetector(const SProcessParams& params);
	Ptr<Feature2D> getExtractor(const SProcessParams& params);
	Ptr<Feature2D> getDetectorExtractor(const SProcessParams& params);
	//computes both keypoints and theirs descriptors, throws invalid argument
	void detectDescribeFeatures(const SProcessParams & params, CLogger* logger);
	void processCLAHE( CLogger* logger);
public:
	//throws ios_base::failure exceptions
	CImage(string filePath);
	//TODO move constructor
	//method determining all the processes with detection and preparation
	void process(const SProcessParams& params, CLogger * logger);
	const string& getFilePath() { return filePath_; }
	Mat& getImage() { return image_; }
	vector<KeyPoint>& getKeypoints() { return imageKeypoints_; }
	Mat& getDescriptors() { return keypointsDescriptors_; }
};

