#include "COperator.h"

void COperator::setAdvancedParams(CFileLoader& loader)
{
    //====================SIFT==========================
    SSIFTParams siftParams;
    siftParams.nfeatures_ = loader.getProcessParams().siftParams_.nfeatures_;
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
    loader.setSIFT(siftParams);

    //====================ORB==========================
    SORBParams orbParams;
    orbParams.nfeatures_ = loader.getProcessParams().orbParams_.nfeatures_;
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
    loader.setORB(orbParams);

#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
    //====================BEBLID==========================
    SBEBLIDParams beblidParams;
    if (loader.getProcessParams().detectMethod_ == EAlgorithm::ALG_ORB) {
        beblidParams.scale_factor_ = 1.0f;
    }
    else if (loader.getProcessParams().detectMethod_ == EAlgorithm::ALG_SIFT) {
        beblidParams.scale_factor_ = 6.75f;
    }
    if (BEBLID_N_BITS_TEST) {
        beblidParams.n_bits_ = BEBLID_N_BITS;
    }
    loader.setBEBLID(beblidParams);
#endif
}

void COperator::logParams(Ptr<CLogger>& logger, const CFileLoader& loader)
{
    SProcessParams params = loader.getProcessParams();
    if (loader.timingOptimalisation())
        logger->log("TIMING: ON (speed optimalisation)").endl();
    else
        logger->log("TIMING: OFF (speed optimalisation)").endl();
    logger->log("NOTE: When is TIMING ON the images are not saved and also the processing might be slightly faster than without timing optimalisation").endl();

    logger->logSection("Parameters of used algorithms", 1);
    logger->log("Method used for detecting: ").log(algToStr(params.detectMethod_)).endl();
    logger->log("Method used for extracting/describing: ").log(algToStr(params.describeMethod_)).endl();
    logger->log("Method used for matching: ").log(algToStr(params.matchingMethod_)).endl();

    //set SIFT params
    SSIFTParams siftParams;
    if((params.detectMethod_ == EAlgorithm::ALG_SIFT) || (params.describeMethod_ == EAlgorithm::ALG_SIFT)
        || (params.detectMethod_ == EAlgorithm::ALG_ROOTSIFT) || (params.describeMethod_ == EAlgorithm::ALG_ROOTSIFT)
        || (params.detectMethod_ == EAlgorithm::ALG_PRECISE_ROOTSIFT) || (params.describeMethod_ == EAlgorithm::ALG_PRECISE_ROOTSIFT)){

        logger->endl();
        logger->log("SIFT parameters are following:").endl();
        logger->log("note: defaut value means that it wasnt changed, other walua can be also euqal to the default ones").endl();

        logger->log("nFeatures: ").log(to_string(siftParams.nfeatures_)).log(" (OpenCV defaults: SIFT = 0 (means no limit), ORB = 1000)");
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
    if ((params.detectMethod_ == EAlgorithm::ALG_ORB) || (params.describeMethod_ == EAlgorithm::ALG_ORB)) {

        logger->endl();
        logger->log("ORB parameters are following:").endl();
        logger->log("note: defaut value means that it wasnt changed, other walua can be also euqal to the default ones").endl();

        logger->log("nFeatures: ").log(to_string(orbParams.nfeatures_)).log(" (OpenCV defaults: SIFT = 0 (means no limit), ORB = 1000)");
        logger->endl();
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
    if (params.describeMethod_ == EAlgorithm::ALG_BEBLID) {
        //values are set according to documentation: https://docs.opencv.org/master/d7/d99/classcv_1_1xfeatures2d_1_1BEBLID.html
        //for different detecting methods different parameters are set
        logger->endl();
        logger->log("BEBLID parameters are following:").endl();
        logger->log("note: defaut value means that it wasnt changed, other walua can be also euqal to the default ones").endl();

        logger->log("scale_factor_: ").log(to_string(beblidParams.scale_factor_)).log(" (1.0 is used for ORB and 6.75 for SIFT)").endl();

        if (params.beblidParams_.n_bits_ == xfeatures2d::BEBLID::SIZE_512_BITS) {
            logger->log("n_bits_: ").log("512 bits size").log(" (defalut value)").endl();
        }
        else {
            logger->log("n_bits_: ").log("256 bits size").endl();
        }

    }
#endif
    
    logger->endl();
    logger->log("camera focal length: ").log(to_string(params.cameraInfo_.focalLength_)).endl();
    logger->log("camera sensors size x: ").log(to_string(params.cameraInfo_.chipSizeX_)).endl();
    logger->log("camera sensors size y: ").log(to_string(params.cameraInfo_.chipSizeY_)).endl();
}

int COperator::run()
{
    //load values
    CFileLoader fileLoader(ROOT_CONFIG_JSON_FILE);
    try {
        fileLoader.load();
    }
    catch (ios_base::failure& e) {
        std::cout << e.what() << endl;
        system("pause");
        return -1;
    } 
    catch (logic_error& e) {
        std::cout << e.what() << endl;
        system("pause");
        return -1;
    }

    Ptr<CLogger> logger;
    Ptr<CLogger> consoleLogger = new CRuntimeLogger(fileLoader.timingOptimalisation());

    if (fileLoader.getOutputType() == EOutputType::CONSOLE) {
        logger = consoleLogger;
    }
    else {
        logger = new CFileLogger(fileLoader.outputRoot(), fileLoader.getRunName(), fileLoader.timingOptimalisation());
    }

    consoleLogger->logSection("START", 0);
    logger->log("OpenCV version : ").log(CV_VERSION).endl();

    try {
        //set advanced
        setAdvancedParams(fileLoader);
        //lock the file loader so there is not any bad configuration during further run in the program
        fileLoader.lock();
        //log the settings
        logParams(logger, fileLoader);
        //run the algorithms
        CObjectInSceneFinder finder(fileLoader.getProcessParams(), logger, fileLoader.getRunName(), fileLoader.getSceneFilepath(), fileLoader.getReferencesFilepaths());
        finder.run(fileLoader.getRunName(), fileLoader.previewResult());
        finder.report();
    }
    catch (ios_base::failure e) {
        logger->logError(e.what());
        system("pause");
        return -1;
    }
    catch (invalid_argument e) {
        logger->logError(e.what());
        system("pause");
        return -1;
    }
    catch (logic_error e) {
        logger->logError(e.what());
        system("pause");
        return -1;
    }

    consoleLogger->logSection("FINISHED", 0);
    system("pause");
    return 1;
}
