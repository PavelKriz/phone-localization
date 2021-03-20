#pragma once

//wrapper around basic shared pointer
#include <opencv2/core/cvstd_wrapper.hpp>
//find homography
#include <opencv2/calib3d.hpp>
//image processing - drawing line
#include <opencv2/imgproc.hpp>
//matcher is in the same hearder file
#include <opencv2/features2d.hpp>

#include "CImage.h"
#include "parameters.h"

class CImagesMatch {
public:
	// methods for matching
	enum EMatchingMethod {
		BRUTE_FORCE,
		FLANN_BASED
	};
private:
	EMatchingMethod methodUsed_;
	Ptr<CImage> objectImage_;
	Ptr<CImage> sceneImage_;
	vector<DMatch> matches_;
	double avarageMatchesDistance_ = numeric_limits<double>::max();


public:
	//throws invalid argument
	CImagesMatch(Ptr<CImage>& object, Ptr<CImage>& scene, CLogger* logger, EMatchingMethod method = FLANN_BASED);
	CImagesMatch(CImagesMatch&& right) noexcept;
	void drawPreviewAndResult(const string& runName, CLogger* logger);

	size_t getNumberOfMatches() { return matches_.size(); }
	double getAvarageMatchesDistance() { return avarageMatchesDistance_; }
	Ptr<CImage> getObjectImage() { return objectImage_; }
	Ptr<CImage> getSceneImage() { return sceneImage_; }

};

