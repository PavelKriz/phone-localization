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

//for the constants
#define _USE_MATH_DEFINES
#include<cmath>
//quaternion
#include <opencv2/core/quaternion.hpp>
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
#include "SpaceModule.h"
#include "CImageLocator3D.h"

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
	const Ptr<CImage> objectImage_; ///< image containing the reference object (sort of training object)
	const Ptr<CImage> sceneImage_; ///< image containing the scene (sort of query object)
	vector<DMatch> matches_; ///< all the matches between the two images keypoints
	double avarageMatchesDistance_ = numeric_limits<double>::max(); ///< average distance of all the matches (the smaller the better)
	double matchedObjectFeaturesRatio_ = -1; ///< ratio between amount of detected matches and amount of filtered matches(the bigger the better)
	double avarageFirstToSecondRatio_ = -1; ///< average ratio from the Lowe's ratio test (called here as first to second ratio)(the smaller the better)
	Mat objectSceneHomography_; ///< transformation matrix of the match
	bool transformMatrixComputed_ = false; ///< information whether the transformation matrix was computed
	/**
	 * @brief Creates the right matcher object
	 * @param params the parameters that determine which matcher would be used
	 * @return smart pointer to the matcher (returns interface/virtual class)
	*/
	static Ptr<DescriptorMatcher> createMatcher(const SProcessParams & params);
	/**
	 * @brief prints the inner transformation matrix of the match
	 * @param clogger logger in which the matrix will be printed in
	 * @throw logic_error when the method is called earlier then the transformation matrix is computed
	*/
	void printTransformationMatrix(Ptr<CLogger>& logger) const;
public:
	/**
	 * @brief Constructor of the class
	 * @param object smart pointer of the reference object (sort of training object) - should stay valid through time of using of this clas
	 * @param scene smart pointer of the scene (sort of query object) - should stay valid through time of using of this clas
	 * @param logger logger in which it will print information about the process
	 * @param params params the parameters that determine which matcher would be used
	 * @throw invalid_argument if there is called a not implemented method for matching
	*/
	CImagesMatch(const Ptr<CImage>& object, const Ptr<CImage>& scene, CLogger* logger, const SProcessParams& params);
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
	 * @param params params the parameters that determine which matcher would be used
	*/
	void drawPreviewAndResult(const string& runName, Ptr<CLogger>& logger, const SProcessParams& params);
	/**
	 * @brief Gives number of filtered matches
	 * @return number of filtered matches
	*/
	size_t getNumberOfMatches() const { return matches_.size(); }
	/**
	 * @brief Gives average distance of all the matches (the smaller the better)
	 * @return the distance
	*/
	double getAvarageMatchesDistance() const { return avarageMatchesDistance_; }
	/**
	 * @brief Gives ratio between amount of detected matches and amount of filtered matches(the bigger the better)
	 * @return the ratio
	*/
	double getMatchedObjectFeaturesRatio() const { return matchedObjectFeaturesRatio_; }
	/**
	 * @brief Gives average ratio from the Lowe's ratio test (called here as first to second ratio)(the smaller the better)
	 * @return the average ratio
	*/
	double getAvarageFirstToSecondRatio() const { return avarageFirstToSecondRatio_; }
	/**
	 * @brief Gives the image containing the reference object (sort of training object)
	 * @return smarter pointer to object CImage
	*/
	const Ptr<CImage> getObjectImage() const { return objectImage_; }
	/**
	 * @brief Gives image containing the scene (sort of query object)
	 * @return smart pointer to scene CImage
	*/
	const Ptr<CImage> getSceneImage() const { return sceneImage_; }

};

