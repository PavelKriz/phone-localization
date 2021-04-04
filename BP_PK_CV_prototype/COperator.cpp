#include "COperator.h"


SProcessParams COperator::getParams()
{
    //set SIFT params
    SSIFTParams siftParams;
    if (SIFT_N_FEATURES_TEST) {
        siftParams.nfeatures_ = SIFT_N_FEATURES;
    }
    if (SIFT_N_OCTAVE_LAYERS_TEST) {
        siftParams.nOctaveLayers = SIFT_N_OCTAVE_LAYERS;
    }
    if (SIFT_CONTRAST_THRESHOLD_TEST) {
        siftParams.contrastThreshold_ = SIFT_CONTRAST_THRESHOLD;
    }
    if (SIFT_EDGE_THRESHOLD_TEST) {
        siftParams.edgeTreshold_ = SIFT_EDGE_THRESHOLD;
    }
    if (SIFT_SIGMA_TEST) {
        siftParams.sigma_ = SIFT_SIGMA;
    }

    SORBParams orbParams;
    //set ORB params
    if (ORB_N_FEATURES_TEST) {
        orbParams.nfeatures_ = ORB_N_FEATURES;
    }
    if (ORB_SCALE_FACTOR_TEST) {
        orbParams.scaleFactor_ = ORB_SCALE_FACTOR;
    }
    if (ORB_N_LEVELS_TEST) {
        orbParams.nlevels_ = ORB_N_LEVELS;
    }
    if (ORB_EDGE_THRESHOLD_TEST) {
        orbParams.edgeTreshold_ = ORB_EDGE_THRESHOLD;
    }
    if (ORB_FIRST_LEVEL_TEST) {
        orbParams.firstLevel_ = ORB_FIRST_LEVEL;
    }
    if (ORB_WTA_K_TEST) {
        orbParams.WTA_K_ = ORB_WTA_K;
    }
    if (ORB_SCORE_TYPE_TEST) {
        orbParams.scoreType_ = ORB_SCORE_TYPE;
    }
    if (ORB_PATCH_SIZE_TEST) {
        orbParams.patchSize_ = ORB_PATCH_SIZE;
    }
    if (ORB_FAST_THRESHOLD_TEST) {
        orbParams.fastTreshold_ = ORB_FAST_THRESHOLD;
    }

    return SProcessParams(
        DETECT_EXTRACT_METHOD,
        siftParams,
        orbParams,
        MATCHING_METHOD
    );
}

int COperator::run()
{
    Ptr<CLogger> logger;
    if (OUTPUT_TYPE == EOutputType::console) {
        logger = new CRuntimeLogger(TIMING);
    }
    else {
        logger = new CFileLogger(OUTPUT_ROOT, RUN_NAME);
    }

    cout << "START" << endl;
    logger->log("OpenCV version : ").log(CV_VERSION).endl().endl();

    try {
        CObjectInSceneFinder finder(logger, RUN_NAME, SCENE_FILE_PATH, OBJECT_FILE_PATHS);
        finder.run(getParams(), RUN_NAME, PREVIEW_RESULT);
        finder.report();
    }
    catch (ios_base::failure e) {
        cout << e.what() << endl;
        return -1;
    }
    catch (invalid_argument e) {
        cout << e.what() << endl;
        return -1;
    }
    catch (logic_error e) {
        cout << e.what() << endl;
        return -1;
    }

    cout << endl << "FINISHED" << endl;

    return 1;
}
