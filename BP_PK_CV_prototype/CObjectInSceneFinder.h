//----------------------------------------------------------------------------------------
/**
 * \file       CObjectInSceneFinder.h
 * \author     Pavel Kriz
 * \date       9/4/2021
 * \brief      Contains class that manages the other classes to find the best reference image for the scene
 *
 *  The usage should be as following:
 construct -> calling run method -> viewBestResult (optional, but needed to see result) -> report (to print/save result).
 *
*/
//----------------------------------------------------------------------------------------

#pragma once

#include <vector>
#include <chrono>

//wrapper around basic shared pointer
#include <opencv2/core/cvstd_wrapper.hpp>

#include "CRuntimeLogger.h"
#include "CFileLogger.h"
#include "CImagesMatch.h"
#include "CImage.h"


/**
 * @brief Class that manages the other classes to find the best reference image for the scene
 * 
 *  The usage should be as following:
 construct -> calling run method -> viewBestResult (optional, but needed to see result) -> report (to print/save result).
 * 
*/
class CObjectInSceneFinder {
	Ptr<CImage::CDetectorExtractor> detectorExtractor_; ///< detector extractor with which all the CImage processing is being called
	const SProcessParams params_; ///<parameters used for the processing
	Ptr<CLogger> logger_; ///< smart pointer to logger to which is being logged the results and all the process
	Ptr<CImage> sceneImage_; ///< smart pointer to a scene in which the object is being searched
	vector<Ptr<CImage>> objectImages_; ///< vector of smart pointers pointing to images of the objects that are being found in the image
	vector<CImagesMatch> matches_; ///< vector in which all the matches are stored (matches between a scane and some reference object)
	size_t bestMatchIndex_; ///< Index pointing to the best result, in other words the object that was "found" (doesn't has to be found) in the scene. (index in the objectImages_ vector)
	bool bestMatchExist_ = false; ///< information whether bestMatchIndex_ is valid
public:
	/**
	 * @brief Constructor
	 * @param params parameters of the algorithms that would be used
	 * @param logger smart pointer to logger to which will be logged the results and all the processes information (for correct working the logger has to stay valid for the using time of this class)
	 * @param runName name of the current test
	 * @param sceneFilePath  filepath of the scene image (relative to the place of run of the app)
	 * @param objectFilePaths  vector with filepaths of images of the reference objects (relative to the place of run of the app)
	 * @throw ios_base::failure (because of image loading)
	 * @throw invalid_argument (if the pointer to logger is empty)
	*/
	CObjectInSceneFinder(const SProcessParams& params, Ptr<CLogger>& logger, const string& runName, const string& sceneFilePath, const vector<string>& objectFilePaths);
	/**
	 * @brief the main body of the process (detecting and describing features, matching and keypoints matches filtering)
	 * @param runName name of the current test
	 * @param viewResult information whether the result should be viewed (basically if also the viewBestResult should be called, but here some extra timing information will be printed)	 * @throw invalid_argument (if the pointer to logger is empty)
	 * @throw all CImage and CImage Match exceptions, because they aren´t catched in this class)
	 * @throw invalid_argument (if the pointer to logger is empty)
	*/
	void run(const string& runName, bool viewResult = true);
	/**
	 * @brief displayes the result - match between scene and best suiting reference object (all matches are displayed and the transformation borders, the object is then being put into the scene)
	 * @param runName name of the current test
	 * @throw logic_error is thrown if the method run was not called first
	 * @throw invalid_argument (if the pointer to logger is empty)
	*/
	void viewBestResult(const string& runName);
	//print all the log, should be used on the end

	/**
	 * @brief (if it wasn´t done before) log all the rest information and the images, should be used on the end
	*/
	void report() { logger_->flush(); }
};
