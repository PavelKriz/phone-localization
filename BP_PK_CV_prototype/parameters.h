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

//========================================SIFT parameters========================================
//SIFT TEST PARAMETERS (if set to false default value is choosen, otherwise the value stated here is chosen)
//used only when SIFT is chosen as a used method
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

//========================================JSON INPUT PARAMETERS========================================
const string ROOT_CONFIG_JSON_FILE = "config.json";

const string CONFIG_RUN_NAME_JSON_KEY = "run_name";
const string CONFIG_REFERENCES_JSON_KEY = "references";
const string CONFIG_SCENES_JSON_KEY = "scenes";
const string CONFIG_OUTPUT_ROOT_JSON_KEY = "output_root";
const string CONFIG_PARAMETERS_JSON_KEY = "parameters";

const string REFERENCES_FILEPATHS_JSON_KEY = "filepaths";

const string OUTPUT_TYPE_JSON_KEY = "output_type";
const string PREVIEW_RESULT_JSON_KEY = "preview_result";
const string TIMING_OPTIMALISATION_JSON_KEY = "timing_optimalisation";
const string DETECTION_METHOD_JSON_KEY = "detection_method";
const string DESCRIPTION_METHOD_JSON_KEY = "description_method";
const string FEATURES_LIMIT_JSON_KEY = "features_limit";
const string MATCHING_METHOD_JSON_KEY = "matching_method";
const string RATIO_TEST_ALPHA_JSON_KEY = "ratio_test_alpha";
const string STANDING_PERSON_OPTIMALISATION_JSON_KEY = "standing_person_optimalisation";
const string FIND_PROJECTION_JSON_KEY = "find_projection_from_3d";
const string FIND_GPS_JSON_KEY = "find_GPS";

const string SCENE_INDEX_JSON_KEY = "scene_index";
const string SCENES_ARRAY_JSON_KEY = "scenes";

const string CAMERA_NAME_JSON_KEY = "camera_name";
const string FOCAL_LENGTH_JSON_KEY = "focal_length";
const string SENSOR_SIZE_X_JSON_KEY = "sensor_size_x";
const string SENSOR_SIZE_Y_JSON_KEY = "sensor_size_y";