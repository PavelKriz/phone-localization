#pragma once

#include <iostream>
#include <vector>

//Matrices
#include <opencv2/core/mat.hpp>
//reading writing
#include <opencv2/imgcodecs.hpp>
//features
#include <opencv2/features2d.hpp>
//clahe and other image processing
#include <opencv2/imgproc.hpp>

#include "CLogger.h"

using namespace std;
using namespace cv;

class CImage {
public:
	//methods for detection and description
	enum EProcessMethod {
		SIFT_,
		ORB_
	};
protected:
	const string filePath_;
	Mat image_;
	bool wasProcessed_ = false;
	vector<KeyPoint> imageKeypoints_;
	Mat keypointsDescriptors_;

	//computes both keypoints and theirs descriptors, throws invalid argument
	void detectDescribeFeatures(EProcessMethod method, CLogger* logger);
	void processCLAHE( CLogger* logger);
public:
	//throws ios_base::failure exceptions
	CImage(string filePath);
	//TODO move constructor
	//method determining all the processes with detection and preparation
	void process(EProcessMethod method, CLogger * logger);
	const string& getFilePath() { return filePath_; }
	Mat& getImage() { return image_; }
	vector<KeyPoint>& getKeypoints() { return imageKeypoints_; }
	Mat& getDescriptors() { return keypointsDescriptors_; }
};

