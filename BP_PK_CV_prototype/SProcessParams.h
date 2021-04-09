//----------------------------------------------------------------------------------------
/**
 * \file       SProcessParams.h
 * \author     Pavel Kriz
 * \date       9/4/2021
 * \brief      All parameters that are being passed in the program and its enums, structures and functions
 *
 *  All the parameters that are depending on the algorithms and they are not intended to be changed are here.
 *
*/
//----------------------------------------------------------------------------------------

#pragma once

//enabling and disabling experimental and nonfree modules
#include "experimentalModules.h"

//needed for some parameters
#include <opencv2/features2d.hpp>
//needed for experimental parameters
#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
#include <opencv2/xfeatures2d.hpp>
#endif
#include <string>

using namespace std;
using namespace cv;

//========================================console========================================
///Enum for specifying the output type
enum class EOutputType {
	console,
	file
};

//========================================algorithm parameters structs========================================
///SIFT paramaters (default values are default values from OpenCV documentation)
/**
  All the parameters documentation can be found in the OpenCV documentation
*/
struct SSIFTParams {
	//OpenCV params with default vaules
	int nfeatures_ = 0; //default OpenCV is really 0 - maybe it is determining some automatic algorithm of telling how many features is detected
	int nOctaveLayers_ = 3;
	double contrastThreshold_ = 0.04;
	double edgeTreshold_ = 10;
	double sigma_ = 1.6;
};

///ORB paramaters (default values are default values from OpenCV documentation)
/**
  All the parameters documentation can be found in the OpenCV documentation
*/
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


#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
///BEBLID paramaters (default values are default values from OpenCV documentation, besides scale factor)
/**
  All the parameters documentation can be found in the OpenCV documentation
*/
struct SBEBLIDParams {
	float scale_factor_ = 1.00f; //default value for ORB only!
	int n_bits_ = xfeatures2d::BEBLID::SIZE_512_BITS;
};
#endif

///Enum with all implemented algorithms that can be vary
enum class EAlgorithm {
	ALG_SIFT,
	ALG_ORB,
#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
	ALG_BEBLID,
#endif
	ALG_BF_MATCHING,
	ALG_FLANN_MATCHING
};

///All parameters that are being passed in the program
/**
  Just a struct to pass all the parameters for the algorithms in the program
*/
struct SProcessParams {
    //keypoints and descriptors
	const EAlgorithm detectMethod_;
	const EAlgorithm describeMethod_;
	const SSIFTParams siftParams_;
	const SORBParams orbParams_;
	const double loweRatioTestAlpha_;

#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
	const SBEBLIDParams beblidParams_;
#endif

	const EAlgorithm matchingMethod_;

#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
	///basic constructor
	SProcessParams(const EAlgorithm& detectMethod, const EAlgorithm& describeMethod, SSIFTParams siftParams,
		SORBParams orbParams, SBEBLIDParams beblidParams, EAlgorithm matchingMethod, double loweRatioTestAlpha)
		:
		detectMethod_(detectMethod),
		describeMethod_(describeMethod),
		siftParams_(siftParams),
		orbParams_(orbParams),
		beblidParams_(beblidParams),
		matchingMethod_(matchingMethod),
		loweRatioTestAlpha_(loweRatioTestAlpha)
	{}
#else
	///basic constructor without experimental methods
	SProcessParams(const EAlgorithm& detectMethod, const EAlgorithm& describeMethod, SSIFTParams siftParams,
		SORBParams orbParams, EAlgorithm matchingMethod, double loweRatioTestAlpha)
		:
	detectMethod_(detectMethod),
		describeMethod_(describeMethod),
		siftParams_(siftParams),
		orbParams_(orbParams),
		matchingMethod_(matchingMethod),
		loweRatioTestAlpha_(loweRatioTestAlpha)
	{}
#endif
};

///Function that gives strings (with algorithm names) to all members of EAlgorithm enum
string algToStr(const EAlgorithm alg);
