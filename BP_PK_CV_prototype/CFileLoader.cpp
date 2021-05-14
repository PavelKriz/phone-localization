#include "CFileLoader.h"

void CFileLoader::loadCameraInfo()
{
    if (!scenesJsonLoaded) {
        loadScenes();
    }
    float focalLength = 0;
    float sensorSizeX = 0;
    float sensorSizeY = 0;
    try {
        string cameraInfoFilePath = sio::getFilePathWithoutSuffix(scenesFilepaths_[sceneIndex_]) + ".json";
        // Create a root
        pt::ptree root;
        // Load the json file in this ptree
        // can throw exceptions if the json isn't valid
        pt::read_json(cameraInfoFilePath, root);
        focalLength = root.get<float>(FOCAL_LENGTH_JSON_KEY);
        sensorSizeX = root.get<float>(SENSOR_SIZE_X_JSON_KEY);
        sensorSizeY = root.get<float>(SENSOR_SIZE_Y_JSON_KEY);
    }
    catch (exception& exc) {
        throw ios_base::failure(jsonErrorIntroduction_ + exc.what());
    }

    if (!sio::numberInPositiveRange<float>(focalLength)) {
        throw ios_base::failure(jsonErrorIntroduction_ + "Focal length has to be positive number!");
    }
    if (!sio::numberInPositiveRange<float>(sensorSizeX)) {
        throw ios_base::failure(jsonErrorIntroduction_ + "Size of sensor has to be positive number!");
    }
    if (!sio::numberInPositiveRange<float>(sensorSizeY)) {
        throw ios_base::failure(jsonErrorIntroduction_ + "Size of sensor has to be positive number!");
    }

    processParams_.cameraInfo_.focalLength_ = focalLength;
    processParams_.cameraInfo_.chipSizeX_ = sensorSizeX;
    processParams_.cameraInfo_.chipSizeY_ = sensorSizeY;
}

void CFileLoader::loadReferencesFilepaths()
{
    try {
        // Create a root
        pt::ptree root;
        // Load the json file in this ptree
        // can throw exceptions if the json isn't valid
        pt::read_json(referenceImagesJsonFilePath_, root);
        for (auto& item : root.get_child(REFERENCES_FILEPATHS_JSON_KEY)) {
            references_.push_back(item.second.get_value<string>());
        }
    }
    catch (exception& exc) {
        throw ios_base::failure(jsonErrorIntroduction_ + exc.what());
    }
}

void CFileLoader::loadProcessParameters()
{
//===================load values===============
    string outputType;
    EAlgorithm detMethodAlg;
    string detMethod;
    EAlgorithm desMethodAlg;
    string desMethod;
    int featuresLimit = 0;
    EAlgorithm matchingMethodAlg;
    string matchingMethod;
    double ratioTestAlpha = 0;
    bool standingPersonOptimalisation = true;
    bool findProjection = true;
    bool findGPS;
    try {
        // Create a root
        pt::ptree root;
        // Load the json file in this ptree
        // can throw exceptions if the json isn't valid
        pt::read_json(parametersJsonFilePath_, root);
        outputType = root.get<string>(OUTPUT_TYPE_JSON_KEY);
        previewResult_ = root.get<bool>(PREVIEW_RESULT_JSON_KEY);
        timingOptimalisation_ = root.get<bool>(TIMING_OPTIMALISATION_JSON_KEY);
        detMethod = root.get<string>(DETECTION_METHOD_JSON_KEY);
        desMethod = root.get<string>(DESCRIPTION_METHOD_JSON_KEY);
        featuresLimit = root.get<int>(FEATURES_LIMIT_JSON_KEY);
        matchingMethod = root.get<string>(MATCHING_METHOD_JSON_KEY);
        ratioTestAlpha = root.get<double>(RATIO_TEST_ALPHA_JSON_KEY);
        standingPersonOptimalisation = root.get<bool>(STANDING_PERSON_OPTIMALISATION_JSON_KEY);
        findProjection = root.get<bool>(FIND_PROJECTION_JSON_KEY);
        findGPS = root.get<bool>(FIND_GPS_JSON_KEY);
    }
    catch (exception& exc) {
        throw ios_base::failure(jsonErrorIntroduction_ + exc.what());
    }

//===========prepare values==========
    try {
        detMethodAlg = strToAlg(detMethod);
        desMethodAlg = strToAlg(desMethod);
        matchingMethodAlg = strToAlg(matchingMethod);
    }
    catch (invalid_argument& ia) {
        throw ios_base::failure(jsonErrorIntroduction_ + ia.what());
    }
    if (outputType == "FILE") {
        outputType_ = EOutputType::FILE;
    }
    else if (outputType == "CONSOLE") {
        outputType_ = EOutputType::CONSOLE;
    }
    else {
        throw ios_base::failure(jsonErrorIntroduction_ + OUTPUT_TYPE_JSON_KEY + " can have value only \"FILE\" or \"CONSOLE\"");
    }

    //check values
    if (!sio::numberInPositiveRange<int>(featuresLimit)) {
        throw ios_base::failure(jsonErrorIntroduction_ + "Limit of the features has to be positive value!");
    }
    if (!sio::numberInRange<double>(ratioTestAlpha, 0.5, 1.0)) {
        throw ios_base::failure(jsonErrorIntroduction_ + "Lowe's ratio test alpha has to be in range <0.5, 1.0>!");
    }

//=========fill values============
    processParams_.detectMethod_ = detMethodAlg;
    processParams_.describeMethod_ = desMethodAlg;
    processParams_.matchingMethod_ = matchingMethodAlg;
    processParams_.loweRatioTestAlpha_ = ratioTestAlpha;
    processParams_.considerPhoneHoldHeight_ = standingPersonOptimalisation;
    processParams_.calcProjectionFrom3D_ = findProjection;
    processParams_.calcGCSLocation_ = findGPS;
//SIFT
    SSIFTParams siftParams;
    siftParams.nfeatures_ = featuresLimit;
    processParams_.siftParams_ = siftParams;
//ORB
    SORBParams orbParams;
    orbParams.nfeatures_ = featuresLimit;
    processParams_.orbParams_ = orbParams;
#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
//BEBLID
    SBEBLIDParams beblidParams;
    if (detMethodAlg == EAlgorithm::ALG_ORB) {
        beblidParams.scale_factor_ = 1.0f;
    }
    else if (detMethodAlg == EAlgorithm::ALG_SIFT) {
        beblidParams.scale_factor_ = 6.75f;
    }
    processParams_.beblidParams_ = beblidParams;
#endif
}

void CFileLoader::loadScenes()
{
    int sceneIndex;

    //load
    try {
        // Create a root
        pt::ptree root;
        // Load the json file in this ptree
        // can throw exceptions if the json isn't valid
        pt::read_json(scenesJsonFilePath_, root);

        sceneIndex = root.get<int>(SCENE_INDEX_JSON_KEY);
        for (auto& item : root.get_child(SCENES_ARRAY_JSON_KEY)) {
            scenesFilepaths_.push_back(item.second.get_value<string>());
        }
    }
    catch (exception& exc) {
        throw ios_base::failure(jsonErrorIntroduction_ + exc.what());
    }
    
    //check values
    if (!sio::numberInRange<int>(sceneIndex, (int)0, (int) scenesFilepaths_.size())) {
        throw ios_base::failure(jsonErrorIntroduction_ +
            "Scene index has to be a valid index (0 <= index < number of scenes in array )" + SCENES_ARRAY_JSON_KEY);
    }
    sceneIndex_ = (size_t)sceneIndex;
    scenesJsonLoaded = true;
}

void CFileLoader::loadRoot()
{
    try {
        // Create a root
        pt::ptree root;
        // Load the json file in this ptree
        // can throw exceptions if the json isn't valid
        pt::read_json(rootConfigFilepath_, root);
        runName_ = root.get<string>(CONFIG_RUN_NAME_JSON_KEY);
        referenceImagesJsonFilePath_ = root.get<string>(CONFIG_REFERENCES_JSON_KEY);
        scenesJsonFilePath_ = root.get<string>(CONFIG_SCENES_JSON_KEY);
        outputRoot_ = root.get<string>(CONFIG_OUTPUT_ROOT_JSON_KEY);
        parametersJsonFilePath_ = root.get<string>(CONFIG_PARAMETERS_JSON_KEY);
    }
    catch (exception& exc) {
        throw ios_base::failure(jsonErrorIntroduction_ + exc.what());
    }
}

CFileLoader::CFileLoader(const string& rootConfigFilepath)
    :
    rootConfigFilepath_(rootConfigFilepath),
    processParams_(EAlgorithm::ALG_SIFT,
        EAlgorithm::ALG_SIFT,
        SSIFTParams(),
        SORBParams(),
#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
        SBEBLIDParams(),
#endif
        EAlgorithm::ALG_BF_MATCHING,
        0.0,
        SCameraInfo(0, 0.0, 0.0),
        true,
        true,
        true
    )
{
}

void CFileLoader::load()
{
    if (locked_) {
        throw logic_error("Trying to load already locked file loader!");
    }
    //methods has to be called in following order
    loadRoot();
    loadReferencesFilepaths();
    loadScenes();
    loadCameraInfo();
    loadProcessParameters();
    loaded_ = true;
}

const vector<string>& CFileLoader::getReferencesFilepaths() const
{
    if (loaded_) {
        return references_;
    }

    throw logic_error("The configurations have not been loaded yet.");
}

const string& CFileLoader::getSceneFilepath() const
{
    if (loaded_) {
        return scenesFilepaths_[sceneIndex_];
    }

    throw logic_error("The configurations have not been loaded yet.");
}

const SProcessParams& CFileLoader::getProcessParams() const
{
    if (loaded_) {
        return processParams_;
    }

    throw logic_error("The configurations have not been loaded yet.");
}

const string& CFileLoader::getRunName() const
{
    if (loaded_) {
        return runName_;
    }

    throw logic_error("The configurations have not been loaded yet.");
}

EOutputType CFileLoader::getOutputType() const
{
    if (loaded_) {
        return outputType_;
    }

    throw logic_error("The configurations have not been loaded yet.");
}

bool CFileLoader::previewResult() const
{
    if (loaded_) {
        return previewResult_;
    }

    throw logic_error("The configurations have not been loaded yet.");
}

bool CFileLoader::timingOptimalisation() const
{
    if (loaded_) {
        return timingOptimalisation_;
    }

    throw logic_error("The configurations have not been loaded yet.");
}

const string& CFileLoader::outputRoot() const
{
    if (loaded_) {
        return outputRoot_;
    }

    throw logic_error("The configurations have not been loaded yet.");
}

void CFileLoader::setSIFT(const SSIFTParams& siftParams)
{
    if (locked_) {
        throw logic_error("Trying to set already locked file loader!");
    }
    int limitTmp = processParams_.siftParams_.nfeatures_;
    processParams_.siftParams_ = siftParams;
    processParams_.siftParams_.nfeatures_ = limitTmp;
}

void CFileLoader::setORB(const SORBParams& orbParams)
{
    if (locked_) {
        throw logic_error("Trying to set already locked file loader!");
    }
    int limitTmp = processParams_.orbParams_.nfeatures_;
    processParams_.orbParams_ = orbParams;
    processParams_.orbParams_.nfeatures_ = limitTmp;
}

#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
void CFileLoader::setBEBLID(const SBEBLIDParams& beblidParams)
{
    if (locked_) {
        throw logic_error("Trying to set already locked file loader!");
    }
    processParams_.beblidParams_ = beblidParams;
}
#endif
