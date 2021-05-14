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

//========================================method names and keys (these are optians in in config files)========================================
const string SIFT_STR = "SIFT"; 
const string ROOTSIFT_STR = "RootSIFT";
const string PRECISE_ROOTSIFT_STR = "Precise_RootSIFT";
const string ORB_STR =  "ORB";

#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
const string BEBLID_STR = "BEBLID";
#endif

const string BF_MATCHING_STR = "BF_matching";
const string FLANN_MATCHING_STR = "FLANN_matching";

//========================================console or file system========================================
/**
 * @brief Enum for specifying the output type
*/
enum class EOutputType {
	CONSOLE,
	FILE
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

/**
 * @brief Basic information about the camera parameters with which was the scene taken
 * 
 * to fill the chip size - basicaly the x and y has to be different vales and rest should be done automaticaly
*/
struct SCameraInfo {
	double focalLength_; ///< in mm (original, NOT recomputed for the full fram size)
	double chipSizeX_; ///< chip size in x axis
	double chipSizeY_; ///< chip size in y axis
	SCameraInfo( double focalLength, double chipSizeX, double chipSizeY)
		:
		focalLength_(focalLength),
		chipSizeX_(chipSizeX),
		chipSizeY_(chipSizeY)
	{}
};


#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
///BEBLID paramaters (default values are default values from OpenCV documentation, besides scale factor)
/**
  All the parameters documentation can be found in the OpenCV documentation
*/
struct SBEBLIDParams {
	float scale_factor_ = 1.00f; //default value for ORB only!
	xfeatures2d::BEBLID::BeblidSize n_bits_ = xfeatures2d::BEBLID::SIZE_512_BITS;
};
#endif


/**
 * @brief Enum with all implemented algorithms that can be vary
 * 
 * It includes feature detection,extraction, and matching algorithms
 * 
 * It itself does not have any implementation, the enum is used to determine which method will be used in the place of implementation.
 * 
*/
enum class EAlgorithm {
	ALG_SIFT,
	ALG_ROOTSIFT,
	ALG_PRECISE_ROOTSIFT,
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
	EAlgorithm detectMethod_; ///< feature detection method
	EAlgorithm describeMethod_; ///< feature description method
	SSIFTParams siftParams_; ///< parameters of the SIFT method
	SORBParams orbParams_; ///< parameters of the ORB method
#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
	SBEBLIDParams beblidParams_; ///< parameters of the BEBLID method
#endif

	EAlgorithm matchingMethod_; ///< matching method that is used
	double loweRatioTestAlpha_; ///< the alpha of the Lowe's ratio test
	SCameraInfo cameraInfo_; ///< camera intrinsics parameters
	bool considerPhoneHoldHeight_; ///< turns on/off the accuracy optimalistion in calculation of global location (GPS). It is recommended to be enabled for scenes with flat ground, which is everytime for the default image database.
	bool calcProjectionFrom3D_; ///< determines if part of the output will be the volume recognision image (projected guessed bounding box of the building)
	bool calcGCSLocation_; ///<determines if the global location (GPS) calculation will take place


	///basic constructor
	SProcessParams(const EAlgorithm& detectMethod,
		const EAlgorithm& describeMethod,
		SSIFTParams siftParams,
		SORBParams orbParams,
#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
		SBEBLIDParams beblidParams,
#endif
		EAlgorithm matchingMethod,
		double loweRatioTestAlpha,
		const SCameraInfo& cameraInfo,
		bool considerPhoneHoldHeight,
		bool calcProjectionFrom3D,
		bool calcGCSLocation)
		:
		detectMethod_(detectMethod),
		describeMethod_(describeMethod),
		siftParams_(siftParams),
		orbParams_(orbParams),
#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
		beblidParams_(beblidParams),
#endif
		matchingMethod_(matchingMethod),
		loweRatioTestAlpha_(loweRatioTestAlpha),
		cameraInfo_(cameraInfo),
		considerPhoneHoldHeight_(considerPhoneHoldHeight),
		calcProjectionFrom3D_(calcProjectionFrom3D),
		calcGCSLocation_(calcGCSLocation)
	{}
};

//========================================functions to work with algorithm names========================================

/**
 * @brief Converts the algorithm enum into string form
 * 
 * The strings of the algorithms are defined by constants (see function implementation)
 * 
 * @param alg Algorithm to be converted
 * @return algorithm in form of string
*/
string algToStr(const EAlgorithm alg);
/**
 * @brief Converts the string into the algorithm enum
 * 
 * The strings of the algorithms are defined by constants (see function implementation)
 * 
 * @param alg Algorithm to be converted
 * @return algorithm in form of string
*/
EAlgorithm strToAlg(const string& str);
