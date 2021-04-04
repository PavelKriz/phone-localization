#pragma once

//needed for some parameters
#include <opencv2/features2d.hpp>

//all the parameters documentation can be found in the OpenCV documentation
struct SSIFTParams {
	//OpenCV params with default vaules
	int nfeatures_ = 3000; //default OpenCV is really 0 - maybe it is determining some automatic algorithm of telling how many features is detected
	int nOctaveLayers = 3;
	double contrastThreshold_ = 0.04;
	double edgeTreshold_ = 10;
	double sigma_ = 1.6;
};

//all the parameters documentation can be found in the OpenCV documentation
struct SORBParams {
	int nfeatures_ = 3000;
	float scaleFactor_ = 1.2f;
	int nlevels = 8;
	int edgeTreshold = 31;
	int firstLevel = 0;
	int WTA_K = 2;
	ORB::ScoreType scoreType_ = ORB::HARRIS_SCORE;
	int patchSize_ = 31;
	int fastTreshold_ = 20;
};

struct SProcessParams {
    //keypoints and descriptors
	enum class EDetectExtractMethod {
		SIFT,
		ORB
	};
	const EDetectExtractMethod detectExtractMethod_;
	const SSIFTParams siftParams_;
	const SORBParams orbParams_;


	//matching
	enum class EMatchingMethod {
		BRUTE_FORCE,
		FLANN_BASED
	};
	const EMatchingMethod matchingMethod_;

	SProcessParams(EDetectExtractMethod detectExtractMethod, SSIFTParams siftParams, SORBParams orbParams, EMatchingMethod matchingMethod)
		:
		detectExtractMethod_(detectExtractMethod),
		siftParams_(siftParams),
		orbParams_(orbParams),
		matchingMethod_(matchingMethod)
	{}
};