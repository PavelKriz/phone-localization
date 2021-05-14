//used for logger optimalisation in case of loggers
//timing of the calculation - needed when the timing is the highest priority
//timing information whether the timing optimalization will take placeand the images wont be saved
//in general is usually not needed even for the testing since the loggers are done in a way that the image saving is done on the application end
//reccomended value is false for almost all cases
//const bool TIMING = false;

//if the output is going to be to files or only to the console
//const EOutputType OUTPUT_TYPE = EOutputType::FILE;
//display the result in the run?
//const bool PREVIEW_RESULT = true;



//========================================Lowe's ratio test parameter========================================
//const float LOWE_RATIO_TEST_ALPHA = 0.75f;

//========================================3D locating and positioning parameters========================================
/* *
 * @brief if enabled it will increase the precsion of algorithmn on plane surface
 * (if there would be hills it would be broken anyway so it should be true in most cases)
 * good mainly if is in the scene a building object (building that is not like angled tower in Pisa, but straight building)
 * it enables straightaning the geometry, but in some places where there is a risc that the object geometry is not perpendicular to the ground it might be disabled
 */
//const bool CONSIDER_PHONE_HOLD_HEIGHT = true; ///< (if there would be hills it would be broken anyway so it should be true always)
