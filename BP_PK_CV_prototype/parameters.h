//----------------------------------------------------------------------------------------
/**
 * \file       parameters.h
 * \author     Pavel Kriz
 * \date       9/4/2021
 * \brief      All parameters that can be set in the program.
 *
 *  Parameters that can be set: test directories, test names, image input files, output, algorithms, algorithms parameters, feature filters parameters.
 *
*/
//----------------------------------------------------------------------------------------

#pragma once

//enabling and disabling experimental and nonfree modules
#include "experimentalModules.h"
//needed for some params
#include <opencv2/features2d.hpp>
//include for the implemented enums of methods
#include "SProcessParams.h"
//image database
#include "images.h"

using namespace cv;

//========================================tests and files parameters========================================

const string OUTPUT_ROOT = "output\\featureDetectingExtractingMethodsSpeedTest\\";
const string imagesRoot = "input_images\\";
const string dumRottRoot = imagesRoot + "dum_rott\\";

//if the output is going to be to files or only to the console
const EOutputType OUTPUT_TYPE = EOutputType::CONSOLE;
//display the result in the run?
const bool PREVIEW_RESULT = true;

//define test numbers
#define TEST1 1
#define TEST2 2 

//Current used test / tests are changed with preprocessor
#define CURRENT_TEST TEST1

//inits for each tests
#if CURRENT_TEST == TEST1
	//scene was taken with XIAOMI REDMI 5 PLUS (smartphone back camera)
	const bool CALC_PROJECTION_FROM_3D = true;
	const bool CALC_GCS_LOCATION = true;
	const double CAMERA_FOCAL_LENGTH = 4;
	const double CAMERA_CHIP_SIZE_X = 4.96;
	const double CAMERA_CHIP_SIZE_Y = 3.79;
	const string SCENE_FILE_PATH = dumRottRoot + "dumRottScene2.jpg";
	const vector<string> OBJECT_FILE_PATHS = { dumRottRoot + "dumRottRef.png", imagesRoot + "white_house\\" +  "whiteHouse1ref.png" };
	const string RUN_NAME = "fileTest";
#elif CURRENT_TEST == TEST2
	const string SCENE_FILE_PATH = sefcikImagesScenes[13];
	const vector<string> OBJECT_FILE_PATHS = sefcikImagesObjects;
	const string RUN_NAME = "SIFT_ROOTSIFT_BF_1";  //better to write spaces with underscore and not to do camelcase
#endif

//========================================timing optimalizations========================================
//used for logger optimalisation in case of loggers
//timing of the calculation - needed when the timing is the highest priority
//timing information whether the timing optimalization will take placeand the images wont be saved
//in general is usually not needed even for the testing since the loggers are done in a way that the image saving is done on the application end
//reccomended value is false for almost all cases
const bool TIMING = false;

//========================================chosen algorithms for matching, describing, matching========================================
const EAlgorithm DETECT_METHOD = EAlgorithm::ALG_SIFT;
const EAlgorithm DESCRIBE_METHOD = EAlgorithm::ALG_SIFT;
const EAlgorithm MATCHING_METHOD = EAlgorithm::ALG_BF_MATCHING;

//========================================SIFT parameters========================================
//SIFT TEST PARAMETERS (if set to false default value is choosen, otherwise the value stated here is chosen)
//used only when SIFT is chosen as a used method
const bool SIFT_N_FEATURES_TEST = true;
const int SIFT_N_FEATURES = 1000;
const bool SIFT_N_OCTAVE_LAYERS_TEST = false;
const int SIFT_N_OCTAVE_LAYERS = 3;
const bool SIFT_CONTRAST_THRESHOLD_TEST = false;
const double SIFT_CONTRAST_THRESHOLD = 0.04;
const bool SIFT_EDGE_THRESHOLD_TEST = false;
const double SIFT_EDGE_THRESHOLD = 10;
const bool SIFT_SIGMA_TEST = false;
const double SIFT_SIGMA = 1.6;

//========================================ORB parameters========================================
//interesting website on how to set the values
// https://stackoverflow.com/questions/28024048/how-to-get-efficient-result-in-orb-using-opencv-2-4-9
//ORB TEST PARAMETERS (if set to false default value is choosen, otherwise the value stated here is chosen)
//used only when ORB is chosen as a used method
const bool ORB_N_FEATURES_TEST = true;
const int ORB_N_FEATURES = 1000;
const bool ORB_SCALE_FACTOR_TEST = false;
const float ORB_SCALE_FACTOR = 1.2f;
const bool ORB_N_LEVELS_TEST = false;
const int ORB_N_LEVELS = 8;
const bool ORB_EDGE_THRESHOLD_TEST = false;
const int ORB_EDGE_THRESHOLD = 31;
const bool ORB_FIRST_LEVEL_TEST = false;
const int ORB_FIRST_LEVEL = 0;
const bool ORB_WTA_K_TEST = false;
const int ORB_WTA_K = 2;
const bool ORB_SCORE_TYPE_TEST = false;
const ORB::ScoreType ORB_SCORE_TYPE = ORB::HARRIS_SCORE;
const bool ORB_PATCH_SIZE_TEST = false;
const int ORB_PATCH_SIZE = 31;
const bool ORB_FAST_THRESHOLD_TEST = false;
const int ORB_FAST_THRESHOLD = 20;

//========================================BEBLID parameters========================================
#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
//BEBLID descriptor https://docs.opencv.org/master/d7/d99/classcv_1_1xfeatures2d_1_1BEBLID.html
const bool BEBLID_N_BITS_TEST = true;
const xfeatures2d::BEBLID::BeblidSize BEBLID_N_BITS = xfeatures2d::BEBLID::SIZE_512_BITS;
#endif

//========================================Lowe's ratio test parameter========================================
const float LOWE_RATIO_TEST_ALPHA = 0.7f;

//========================================3D locating and positioning parameters========================================
/* *
 * @brief if enabled it will increase the precsion of algorithmn on plane surface
 * (if there would be hills it would be broken anyway so it should be true in most cases)
 * good mainly if is in the scene a building object (building that is not like angled tower in Pisa, but straight building)
 * it enables straightaning the geometry, but in some places where there is a risc that the object geometry is not perpendicular to the ground it might be disabled
 */
const bool CONSIDER_PHONE_HOLD_HEIGHT = true; ///< (if there would be hills it would be broken anyway so it should be true always)