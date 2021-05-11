#pragma once

//loading boost - used for loading json
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <vector>
#include <iostream>

#include "SProcessParams.h"
#include "SpecializedInputOutput.h"
#include "parameters.h"

using namespace std;
using namespace cv;
namespace pt = boost::property_tree;

class CFileLoader
{
    const string jsonErrorIntroduction_ = string("Error occured in reading config json: ");
    const string rootConfigFilepath_;
    //config
    string referenceImagesJsonFilePath_;
    string scenesJsonFilePath_;
    string parametersJsonFilePath_;
    string outputRoot_;
    string runName_;
    //scenes
    size_t sceneIndex_;
    vector<string> scenesFilepaths_;
    //references
    vector<string> references_;
    //params
    SProcessParams processParams_;
    //other params
    EOutputType outputType_;
    bool timingOptimalisation_;
    bool previewResult_;


    //inner values
    bool scenesJsonLoaded = false;
    bool loaded_ = false;
    bool locked_ = false;

    void loadCameraInfo();
    void loadReferencesFilepaths();
    void loadProcessParameters();
    void loadScenes();
    void loadRoot();
public:
    CFileLoader(const string& rootConfigFilepath);
    //throws invalid argument and logic error
    void load();
    //throws logic error
    const vector<string>& getReferencesFilepaths() const;
    //throws logic error
    const string& getSceneFilepath() const;
    //throw logic error
    const SProcessParams& getProcessParams() const;
    //throw logic error
    const string& getRunName() const;
    //throw logic error
    EOutputType getOutputType() const;
    //throw logic error
    bool previewResult() const;
    //throw logic error
    bool timingOptimalisation() const;
    //throw logic error
    const string& outputRoot() const;

    void setSIFT(const SSIFTParams& siftParams);
    void setORB(const SORBParams& orbParams);
#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
    //throw logic error
    void setBEBLID(const SBEBLIDParams& beblidParams);
#endif
    void lock() { locked_ = true; }
};

