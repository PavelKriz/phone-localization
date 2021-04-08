#pragma once

//needed for some parameters
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>

#include <string>

using namespace std;
using namespace cv;

//all the parameters documentation can be found in the OpenCV documentation
struct SSIFTParams {
	//OpenCV params with default vaules
	int nfeatures_ = 0; //default OpenCV is really 0 - maybe it is determining some automatic algorithm of telling how many features is detected
	int nOctaveLayers_ = 3;
	double contrastThreshold_ = 0.04;
	double edgeTreshold_ = 10;
	double sigma_ = 1.6;
};

//all the parameters documentation can be found in the OpenCV documentation
struct SORBParams {
	int nfeatures_ = 1000;
	float scaleFactor_ = 1.2f;
	int nlevels_ = 8;
	int edgeTreshold_ = 31;
	int firstLevel_ = 0;
	int WTA_K_ = 2;
	ORB::ScoreType scoreType_ = ORB::HARRIS_SCORE;
	int patchSize_ = 31;
	int fastTreshold_ = 20;
};

struct SBEBLIDParams {
	float scale_factor_ = 1.00f; //default value for ORB only!
	int n_bits_ = xfeatures2d::BEBLID::SIZE_512_BITS;
};

enum class EAlgorithm {
	ALG_SIFT,
	ALG_ORB,
	ALG_BEBLID,
	ALG_BF_MATCHING,
	ALG_FLANN_MATCHING
};


string algToStr(const EAlgorithm alg);

struct SProcessParams {
    //keypoints and descriptors
	const EAlgorithm detectMethod_;
	const EAlgorithm describeMethod_;
	const SSIFTParams siftParams_;
	const SORBParams orbParams_;
	const SBEBLIDParams beblidParams_;
	const EAlgorithm matchingMethod_;

	SProcessParams(const EAlgorithm& detectMethod, const EAlgorithm& describeMethod, SSIFTParams siftParams,
		SORBParams orbParams, SBEBLIDParams beblidParams, EAlgorithm matchingMethod)
		:
		detectMethod_(detectMethod),
		describeMethod_(describeMethod),
		siftParams_(siftParams),
		orbParams_(orbParams),
		beblidParams_(beblidParams),
		matchingMethod_(matchingMethod)
	{}
};