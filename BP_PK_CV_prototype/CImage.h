#pragma once

#include <iostream>
#include <vector>

//Matrices
#include <opencv2/core/mat.hpp>
//reading writing
#include <opencv2/imgcodecs.hpp>
//features
#include <opencv2/features2d.hpp>

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
private:
	const string filePath_;
	Mat image_;
	bool wasProcessed_ = false;
	vector<KeyPoint> imageKeypoints_;
	Mat keypointsDescriptors_;
public:
	//throws ios_base::failure exceptions
	CImage(string filePath);
	//TODO move constructor

	//computes both keypoints and theirs descriptors, throws invalid argument
	void detectDescribeFeatures(EProcessMethod method, CLogger* logger);

	const string& getFilePath() { return filePath_; }
	Mat& getImage() { return image_; }
	vector<KeyPoint>& getKeypoints() { return imageKeypoints_; }
	Mat& getDescriptors() { return keypointsDescriptors_; }
};

