#include "COperator.h"

SProcessParams COperator::getParams(Ptr<CLogger>& logger)
{
    if (TIMING)
        logger->log("TIMING: ON (speed optimalisation)").endl();
    else
        logger->log("TIMING: OFF (speed optimalisation)").endl();
    logger->log("NOTE: When is TIMING ON the images are not saved and also the processing might be slightly faster than without timing optimalisation").endl();

    logger->logSection("Parameters of used algorithms", 1);
    logger->log("Method used for detecting: ").log(algToStr(DETECT_METHOD)).endl();
    logger->log("Method used for extracting/describing: ").log(algToStr(DESCRIBE_METHOD)).endl();
    logger->log("Method used for matching: ").log(algToStr(MATCHING_METHOD)).endl();

    //set SIFT params
    SSIFTParams siftParams;
    if((DETECT_METHOD == EAlgorithm::ALG_SIFT) || (DESCRIBE_METHOD == EAlgorithm::ALG_SIFT) 
        || (DETECT_METHOD == EAlgorithm::ALG_ROOTSIFT) || (DESCRIBE_METHOD == EAlgorithm::ALG_ROOTSIFT)
        || (DETECT_METHOD == EAlgorithm::ALG_PRECISE_ROOTSIFT) || (DESCRIBE_METHOD == EAlgorithm::ALG_PRECISE_ROOTSIFT)){
        if (SIFT_N_FEATURES_TEST) {
            siftParams.nfeatures_ = SIFT_N_FEATURES;
        }
        if (SIFT_N_OCTAVE_LAYERS_TEST) {
            siftParams.nOctaveLayers_ = SIFT_N_OCTAVE_LAYERS;
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

        logger->endl();
        logger->log("SIFT parameters are following:").endl();
        logger->log("note: defaut value means that it wasnt changed, other walua can be also euqal to the default ones").endl();

        logger->log("nFeatures: ").log(to_string(siftParams.nfeatures_));
        if (! SIFT_N_FEATURES_TEST) {
            logger->log(" (default value)");
        }
        logger->endl();
        logger->log("nOctaveLayers_: ").log(to_string(siftParams.nOctaveLayers_));
        if (! SIFT_N_OCTAVE_LAYERS_TEST) {
            logger->log(" (default value)");
        }
        logger->endl();
        logger->log("contrastThreshold_: ").log(to_string(siftParams.contrastThreshold_));
        if (! SIFT_CONTRAST_THRESHOLD_TEST) {
            logger->log(" (default value)");
        }
        logger->endl();
        logger->log("edgeTreshold_: ").log(to_string(siftParams.edgeTreshold_));
        if (! SIFT_EDGE_THRESHOLD_TEST) {
            logger->log(" (default value)");
        }
        logger->endl();
        logger->log("sigma_: ").log(to_string(siftParams.sigma_));
        if (! SIFT_SIGMA_TEST) {
            logger->log(" (default value)");
        }
        logger->endl();
    }

    SORBParams orbParams;
    //set ORB params
    if ((DETECT_METHOD == EAlgorithm::ALG_ORB) || (DESCRIBE_METHOD == EAlgorithm::ALG_ORB)) {
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

        logger->endl();
        logger->log("ORB parameters are following:").endl();
        logger->log("note: defaut value means that it wasnt changed, other walua can be also euqal to the default ones").endl();

        logger->log("nFeatures: ").log(to_string(orbParams.nfeatures_));
        if (! ORB_N_FEATURES_TEST) {
            orbParams.nfeatures_ = ORB_N_FEATURES;
        }
        logger->endl();
        logger->log("scaleFactor_: ").log(to_string(orbParams.scaleFactor_));
        if (! ORB_SCALE_FACTOR_TEST) {
            logger->log(" (default value)");
        }
        logger->endl();
        logger->log("nlevels_: ").log(to_string(orbParams.nlevels_));
        if (! ORB_N_LEVELS_TEST) {
            logger->log(" (default value)");
        }
        logger->endl();
        logger->log("edgeTreshold_: ").log(to_string(orbParams.edgeTreshold_));
        if (! ORB_EDGE_THRESHOLD_TEST) {
            logger->log(" (default value)");
        }
        logger->endl();
        logger->log("firstLevel_: ").log(to_string(orbParams.firstLevel_));
        if (! ORB_FIRST_LEVEL_TEST) {
            logger->log(" (default value)");
        }
        logger->endl();
        logger->log("WTA_K_: ").log(to_string(orbParams.WTA_K_));
        if (! ORB_WTA_K_TEST) {
            logger->log(" (default value)");
        }
        logger->endl();
        logger->log("scoreType_: ").log(to_string(orbParams.scoreType_));
        if (! ORB_SCORE_TYPE_TEST) {
            logger->log(" (default value)");
        }
        logger->endl();
        logger->log("patchSize_: ").log(to_string(orbParams.patchSize_));
        if (! ORB_PATCH_SIZE_TEST) {
            logger->log(" (default value)");
        }
        logger->endl();
        logger->log("fastTreshold_: ").log(to_string(orbParams.fastTreshold_));
        if (! ORB_FAST_THRESHOLD_TEST) {
            logger->log(" (default value)");
        }
        logger->endl();

    }

#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
    //set BEBLID params (only descriptor)
    SBEBLIDParams beblidParams;
    if (DESCRIBE_METHOD == EAlgorithm::ALG_BEBLID) {
        //values are set according to documentation: https://docs.opencv.org/master/d7/d99/classcv_1_1xfeatures2d_1_1BEBLID.html
        //for different detecting methods different parameters are set
        logger->endl();
        logger->log("BEBLID parameters are following:").endl();
        logger->log("note: defaut value means that it wasnt changed, other walua can be also euqal to the default ones").endl();

        if (DETECT_METHOD == EAlgorithm::ALG_ORB) {
            beblidParams.scale_factor_ = 1.0f;
            logger->log("scale_factor_: ").log(to_string(beblidParams.scale_factor_)).log(" (value to describe ORB features)").endl();
        }
        else if (DETECT_METHOD == EAlgorithm::ALG_SIFT) {
            beblidParams.scale_factor_ = 6.75f;
            logger->log("scale_factor_: ").log(to_string(beblidParams.scale_factor_)).log(" (value to describe SIFT features)").endl();
        }
        if (BEBLID_N_BITS_TEST) {
            beblidParams.n_bits_ = BEBLID_N_BITS;
        }

        if (BEBLID_N_BITS == xfeatures2d::BEBLID::SIZE_512_BITS) {
            logger->log("n_bits_: ").log("512 bits size").log(" (defalut value)").endl();
        }
        else {
            logger->log("n_bits_: ").log("256 bits size").endl();
        }

    }
#endif
    
    return SProcessParams(
        DETECT_METHOD,
        DESCRIBE_METHOD,
        siftParams,
        orbParams,
#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
        beblidParams,
#endif
        MATCHING_METHOD,
        LOWE_RATIO_TEST_ALPHA
    );
}

int COperator::run()
{
    Ptr<CLogger> logger;
    Ptr<CLogger> consoleLogger = new CRuntimeLogger(TIMING);

    if (OUTPUT_TYPE == EOutputType::console) {
        logger = consoleLogger;
    }
    else {
        logger = new CFileLogger(OUTPUT_ROOT, RUN_NAME, TIMING);
    }

    consoleLogger->logSection("START", 0);
    logger->log("OpenCV version : ").log(CV_VERSION).endl();

    try {
        CObjectInSceneFinder finder(getParams(logger), logger, RUN_NAME, SCENE_FILE_PATH, OBJECT_FILE_PATHS);
        finder.run( RUN_NAME, PREVIEW_RESULT);
        finder.report();
    }
    catch (ios_base::failure e) {
        logger->logError(e.what());
        return -1;
    }
    catch (invalid_argument e) {
        logger->logError(e.what());
        return -1;
    }
    catch (logic_error e) {
        logger->logError(e.what());
        return -1;
    }

    consoleLogger->logSection("FINISHED", 0);
    return 1;
}
