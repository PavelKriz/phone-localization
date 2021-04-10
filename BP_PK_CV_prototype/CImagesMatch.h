//----------------------------------------------------------------------------------------
/**
 * \file       CImagesMatch.h
 * \author     Pavel Kriz
 * \date       9/4/2021
 * \brief      Contains class that handles matches between two object, mainly it represent the match itself
 *
 * It is designed to be used with one object (training object) and one scene (query image scene)
 * It can run with any images but it is optimised for previously mentioned usage
 * 
 * Usage is following: construct -> <probably: found which match is the best by constructing all the CImageMatch objects>
 *  -> display the match or just get information about the images in the best match
 *
*/
//----------------------------------------------------------------------------------------

#pragma once

//wrapper around basic shared pointer
#include <opencv2/core/cvstd_wrapper.hpp>
//find homography
#include <opencv2/calib3d.hpp>
//image processing - drawing line
#include <opencv2/imgproc.hpp>
//matcher is in the same header file
#include <opencv2/features2d.hpp>
#include <opencv2/flann.hpp>

#include "CImage.h"
#include "SProcessParams.h"

/**
 * @brief Class that handles matches between two object, mainly it represent the match itself
 * 
 * It is designed to be used with one object (training object) and one scene (query image scene)
 * It can run with any images but it is optimised for previously mentioned usage
 * 
 * Usage is following: construct -> <probably: found which match is the best by constructing all the CImageMatch objects>
 *  -> display the match or just get information about the images in the best match
 * 
*/
class CImagesMatch {
	Ptr<CImage> objectImage_; ///< image containing the reference object (sort of training object)
	Ptr<CImage> sceneImage_; ///< image containing the scene (sort of query object)
	vector<DMatch> matches_; ///< all the matches between the two images keypoints
	double avarageMatchesDistance_ = numeric_limits<double>::max(); ///< average distance of all the matches (the smaller the better)
	double matchedObjectFeaturesRatio_ = -1; ///< ratio between amount of detected matches and amount of filtered matches(the bigger the better)
	double avarageFirstToSecondRatio_ = -1; ///< average ratio from the Lowe's ratio test (called here as first to second ratio)(the smaller the better)
	
	/**
	 * @brief Creates the right matcher object
	 * @param params the parameters that determine which matcher would be used
	 * @return smart pointer to the matcher (returns interface/virtual class)
	*/
	Ptr<DescriptorMatcher> createMatcher(const SProcessParams & params);
public:
	/**
	 * @brief Constructor of the class
	 * @param object smart pointer of the reference object (sort of training object) - should stay valid through time of using of this clas
	 * @param scene smart pointer of the scene (sort of query object) - should stay valid through time of using of this clas
	 * @param logger logger in which it will print information about the process
	 * @param params params the parameters that determine which matcher would be used
	 * @throw invalid_argument if there is called a not implemented method for matching
	*/
	CImagesMatch(Ptr<CImage>& object, Ptr<CImage>& scene, CLogger* logger, const SProcessParams& params);
	/**
	 * @brief Move constructor
	 * @param right object to be moved
	*/
	CImagesMatch(CImagesMatch&& right) noexcept;
	/**
	 * @brief Previews the result (it includes computing the homography and smoothing out the matches)
	 * 
	 * It shows the needed transforamtion of the reference image (this transforamtion determines the location of the object in the scene)
	 * It also shows all the filtered matches between the keypoints (and the keypoints to which the matches belong)
	 * 
	 * @param runName name of the current test 
	 * @param logger logger in which it will print information about the process
	*/
	void drawPreviewAndResult(const string& runName, CLogger* logger);
	/**
	 * @brief Gives number of filtered matches
	 * @return number of filtered matches
	*/
	size_t getNumberOfMatches() { return matches_.size(); }
	/**
	 * @brief Gives average distance of all the matches (the smaller the better)
	 * @return the distance
	*/
	double getAvarageMatchesDistance() { return avarageMatchesDistance_; }
	/**
	 * @brief Gives ratio between amount of detected matches and amount of filtered matches(the bigger the better)
	 * @return the ratio
	*/
	double getMatchedObjectFeaturesRatio() { return matchedObjectFeaturesRatio_; }
	/**
	 * @brief Gives average ratio from the Lowe's ratio test (called here as first to second ratio)(the smaller the better)
	 * @return the average ratio
	*/
	double getAvarageFirstToSecondRatio() { return avarageFirstToSecondRatio_; }
	/**
	 * @brief Gives the image containing the reference object (sort of training object)
	 * @return smarter pointer to object CImage
	*/
	Ptr<CImage> getObjectImage() { return objectImage_; }
	/**
	 * @brief Gives image containing the scene (sort of query object)
	 * @return smart pointer to scene CImage
	*/
	Ptr<CImage> getSceneImage() { return sceneImage_; }

};

