//----------------------------------------------------------------------------------------
/**
 * \file       CFileLoader.h
 * \author     Pavel Kriz
 * \date       10/5/2021
 * \brief      Contains class loading configuration from files
 *
 *  CFileLoader is a class that is loading configuration from JSON files and then provides all the loaded data.
 *
*/
//----------------------------------------------------------------------------------------

#pragma once

//loading boost - used for loading json
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <vector>
#include <iostream>

//our includes
#include "SProcessParams.h"
#include "SpecializedInputOutput.h"
#include "parameters.h"

using namespace std;
using namespace cv;
namespace pt = boost::property_tree;

/**
 * @brief CFileLoader is a class that is loading configuration from JSON files and then provides all the loaded data.
*/
class CFileLoader
{
    const string jsonErrorIntroduction_ = string("Error occured in reading config json: "); ///< basic message thrown when some error occur in the beggining
    const string rootConfigFilepath_; ///<  determines the location of the root JSON file (file loaded by loadRoot())
    //config
    string referenceImagesJsonFilePath_; ///< filepath of the reference images JSON file (relative to the directory where the app is running)
    string scenesJsonFilePath_; ///< filepath of the scene images JSON file (relative to the directory where the app is running)
    string parametersJsonFilePath_; ///< filepath of the parameters JSON file (relative to the directory where the app is running)
    string outputRoot_; ///< filepath of the directory/directories where the output would be stored in case of file output (relative to the directory where the app is running)
    string runName_; ///< Name of the application run that is used in the logs and in the names of saved files
    //scenes
    size_t sceneIndex_; ///< index of the used scene image (index in the scenesFilepaths_) 
    vector<string> scenesFilepaths_; ///< array which contains all prepared scene images (their filepaths, relative to the directory where the app is running)
    //references
    vector<string> references_; ///< array, which contains all prepared reference images (their filepaths, relative to the directory where the app is running)
    //params
    SProcessParams processParams_; ///< variable holding all the information about the processing configuration
    //other params
    EOutputType outputType_; ///< output type (where would be the results shown)
    bool timingOptimalisation_; ///< determines if timing optimalisations (such that lower the possibility of time changes across multiple tests, it also lowers the output richness)
    bool previewResult_; ///< determines whether images will be created


    //inner values
    bool scenesJsonLoaded = false; ///< if the scenes JSON was already loaded
    bool loaded_ = false; ///< if all the JSON files have been already loaded
    bool locked_ = false; ///< if locked (method lock()) the variables cannot be further changed

    /**
     * @brief Method that loads from determined JSON file information about camera.
     * 
     * The JSON file has same name as scene image but the suffix is json instead of scene image suffix
     * 
     * Has to be called after the loadRoot and loadScenes
     * 
    */
    void loadCameraInfo();
    /**
     * @brief Method that loads from determined JSON file all the filepaths to the reference images.
     * 
     * Has to be called after the loadRoot
     * 
    */
    void loadReferencesFilepaths();
    /**
     * @brief Method that loads from determined JSON file all the parameters that configure the processing pipeline
     * 
     * Has to be called after the loadRoot
     * 
    */
    void loadProcessParameters();
    /**
     * @brief Method that loads from determined JSON file a filepath where the scene image is stored
     * 
     * Has to be called after the loadRoot
     * 
    */
    void loadScenes();
    /**
     * @brief Method that loads from determined (by the filepath handled over in the constructor) JSON file the basic information
     * 
     * most of the information are the filepaths to the other configuration JSON files
     * 
    */
    void loadRoot();
public:
    /**
     * @brief Constructs empty class that is ready to load some files
     * @param rootConfigFilepath determines the location of the root JSON file (file loaded by loadRoot())
    */
    CFileLoader(const string& rootConfigFilepath);
    /**
     * @brief Method that calles all the loading methods
     * 
     * The loading methods descriptions and their order of calling:
     * 
     * loadRoot()
     *      Method that loads from determined (by the filepath handled over in the constructor) JSON file the basic information
     * loadReferencesFilepaths()
     *      Method that loads from determined JSON file all the parameters that configure the processing pipeline
     * loadScenes()
     *      Method that loads from determined JSON file a filepath where the scene image is stored
     * loadCameraInfo()
     *      Method that loads from determined JSON file information about camera.
     * loadProcessParameters()
     *      Method that loads from determined JSON file all the parameters that configure the processing pipeline
     * 
     * @throw ios_base::failure in case of some io failure
     * @throw logic_error is thrown when the load method is called after the lock() method
    */
    void load();

    /**
     * @brief Returns references filepaths
     * @return vector with strings of all filepaths to reference images
     * @throw logic_error when is the function called earliar than load()
    */
    const vector<string>& getReferencesFilepaths() const;
    /**
     * @brief Returns scene filepath
     * @return string with filepath to scene image
     * @throw logic_error when is the function called earliar than load()
    */
    const string& getSceneFilepath() const;
    /**
     * @brief Returns the processing pipeline configuration (set of parameters)
     * @return the paramaters
     * @throw logic_error when is the function called earliar than load()
    */
    const SProcessParams& getProcessParams() const;
    /**
     * @brief Returns run name
     * @return string with the run name
     * @throw logic_error when is the function called earliar than load()
    */
    const string& getRunName() const;
    /**
     * @brief Returns the output type (where are the results going to be put)
     * @return output type
     * @throw logic_error when is the function called earliar than load()
    */
    EOutputType getOutputType() const;
    /**
     * @brief Returns the information (boolean) whether the result would be previewed
     * @return the boolean value
     * @throw logic_error when is the function called earliar than load()
    */
    bool previewResult() const;
    /**
     * @brief Returns the timing optimalisation on/off boolean
     * @return string with filepath to scene image
     * @throw logic_error when is the function called earliar than load()
    */
    bool timingOptimalisation() const;
    /**
     * @brief Returns the filepath where the files are supposed to be stored in case of any file saving
     * @return string with the filepath
     * @throw logic_error when is the function called earliar than load()
    */
    const string& outputRoot() const;

    /**
     * @brief Method allowing to set the inner SIFT paramaters, that are than part of the SProcessParams returned by getProcessParams()
     * 
     * Because the CFileLoader is not setting the parameters itself, this function is used so they can be set from outside.
     * 
     * @param siftParams parameters that will overwrite the inner SIFT parameters except the feature limit value (nFeatures)
     * @throw logic_error when the method is called after lock() method
    */
    void setSIFT(const SSIFTParams& siftParams);
    /**
     * @brief Method allowing to set the inner ORB paramaters, that are than part of the SProcessParams returned by getProcessParams()
     *
     * Because the CFileLoader is not setting the parameters itself, this function is used so they can be set from outside.
     *
     * @param orbParams parameters that will overwrite the inner ORB parameters except the feature limit value (nFeatures)
     * @throw logic_error when the method is called after lock() method
    */
    void setORB(const SORBParams& orbParams);
#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
    /**
     * @brief Method allowing to set the inner BEBLID paramaters, that are than part of the SProcessParams returned by getProcessParams()
     *
     * Because the CFileLoader is not setting the parameters itself, this function is used so they can be set from outside.
     *
     * @param beblidParams parameters that will overwrite the inner BEBLID parameters except the feature limit value (nFeatures)
     * @throw logic_error when the method is called after lock() method
    */
    void setBEBLID(const SBEBLIDParams& beblidParams);
#endif
    /**
     * @brief Method that allows to lock any further setting. So in other words all the values will become const effectively.
     * 
    */
    void lock() { locked_ = true; }
};

