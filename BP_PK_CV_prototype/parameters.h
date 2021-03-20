#pragma once

const float good_matches_min_distance_alpha = 2;


//normal single tests
#define TEST1 1
#define TEST2 2 
#define TEST3 3 
#define TEST4 4
#define TEST5 5
#define TEST6 6


//Current used test / tests are changed with preprocessor
#define CURRENT_TEST TEST7

//timing of the calculation
#define TIMING

#ifndef TIMING
const bool WRITE_OR_SHOW_IMAGES = true;
#else
const bool WRITE_OR_SHOW_IMAGES = false;
#endif


#define DEBUG

const string imagesRoot = "input_images\\";
const string dumRottRoot = imagesRoot + "dum_rott\\";

//inits for each tests
#if CURRENT_TEST == TEST1
	const string objectFilePath = "pavel.png";
	const vector<string> sceneFilePaths = { "pavelOnPalette.png" };
	const string runName = "test1";
#elif CURRENT_TEST == TEST2
	const string objectFilePath = dumRottRoot + "dumRottRef.png";
	const vector<string> sceneFilePaths = { dumRottRoot + "dumRottScene.jpg" };
	const string runName = "test2";
#elif CURRENT_TEST == TEST3
	const string objectFilePath = dumRottRoot + "dumRottRef.png";
	const vector<string> sceneFilePaths = { dumRottRoot + "dumRottScene2.jpg" };
	const string runName = "test3";
#elif CURRENT_TEST == TEST4
	const string objectFilePath = dumRottRoot + "dumRottRef.png";
	const vector<string> sceneFilePaths = { dumRottRoot + "dumRottScene3.jpg" };
	const string runName = "test4";
#elif CURRENT_TEST == TEST5
	const string objectFilePath = dumRottRoot + "dumRottRef.png";
	const vector<string> sceneFilePaths = { dumRottRoot + "dumRottScen4.jpg" };
	const string runName = "test5";
#elif CURRENT_TEST == TEST6
	const string objectFilePath = dumRottRoot + "dumRottRef.png";
	const vector<string> sceneFilePaths = { dumRottRoot + "dumRottRef.png" };
	const string runName = "test6";
#elif CURRENT_TEST == TEST7
	const string objectFilePath = dumRottRoot + "dumRottRef.png";
	const vector<string> sceneFilePaths = { dumRottRoot + "dumRottScene2.jpg", imagesRoot + "white_house\\whiteHouse1.jpg", imagesRoot + "pavel\\pavel.png" };
	const string runName = "mtest1";
#endif

