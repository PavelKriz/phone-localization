#pragma once

#include "images.h"

const float good_matches_min_distance_alpha = 2;


//normal single tests
#define TEST1 1
#define TEST2 2 
#define TEST3 3 
#define TEST4 4
#define TEST5 5
#define TEST6 6


//Current used test / tests are changed with preprocessor
#define CURRENT_TEST TEST1

const string OUTPUT_ROOT = "output\\testingOutput\\";

enum class EOutputType{
	console,
	file
};

const EOutputType OUTPUT_TYPE = EOutputType::console;

//timing of the calculation
#define TIMINGxx

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
	const string sceneFilePath = dumRottRoot + "dumRottScene2.jpg";
	const vector<string> objectFilePaths = { dumRottRoot + "dumRottRef.png", imagesRoot + "white_house\\" +  "whiteHouse1ref.png" };
	const string runName = "testingFindingObjectsInScene";
#endif

