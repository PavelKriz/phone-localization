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

const string OUTPUT_ROOT = "output\\testingOutput\\";
const string imagesRoot = "input_images\\";
const string dumRottRoot = imagesRoot + "dum_rott\\";

//if the output is going to be to files or only to the console
const EOutputType OUTPUT_TYPE = EOutputType::console;
//display the result in the run?
const bool PREVIEW_RESULT = true;

//define test numbers
#define TEST1 1
#define TEST2 2 

//Current used test / tests are changed with preprocessor
#define CURRENT_TEST TEST1

//inits for each tests
#if CURRENT_TEST == TEST1
	const string SCENE_FILE_PATH = dumRottRoot + "dumRottScene2.jpg";
	const vector<string> OBJECT_FILE_PATHS = { dumRottRoot + "dumRottRef.png", imagesRoot + "white_house\\" +  "whiteHouse1ref.png" };
	const string RUN_NAME = "fileTest";
#elif CURRENT_TEST == TEST2
	const string SCENE_FILE_PATH = sefcikImagesScenes[49];
	const vector<string> OBJECT_FILE_PATHS = sefcikImagesObjects;
	const string RUN_NAME = "testing_objects";  //better to write spaces with underscore and not to do camelcase
#endif

//========================================timing optimalizations========================================
//timing of the calculation - needed when the timing is the highest priority
const bool TIMING = false;

//========================================chosen algorithms for matching, describing, matching========================================
const EAlgorithm DETECT_METHOD = EAlgorithm::ALG_ORB;
const EAlgorithm DESCRIBE_METHOD = EAlgorithm::ALG_ORB;
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
const bool BEBLID_N_BITS_TEST = false;
const bool BEBLID_N_BITS = xfeatures2d::BEBLID::SIZE_512_BITS;
#endif

//========================================Lowe's ratio test parameter========================================
const float LOWE_RATIO_TEST_ALPHA = 0.7f;