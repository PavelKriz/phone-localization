#pragma once

//wrapper around basic shared pointer
#include <opencv2/core/cvstd_wrapper.hpp>
//find homography
#include <opencv2/calib3d.hpp>
//image processing - drawing line
#include <opencv2/imgproc.hpp>
//matcher is in the same hearder file
#include <opencv2/features2d.hpp>
#include <opencv2/flann.hpp>

#include "CImage.h"
#include "parameters.h"
#include "SProcessParams.h"

class CImagesMatch {
	Ptr<CImage> objectImage_;
	Ptr<CImage> sceneImage_;
	vector<DMatch> matches_;
	double avarageMatchesDistance_ = numeric_limits<double>::max(); // the smaller the better
	double matchedObjectFeaturesRatio_ = -1; // the bigger the better
	double avarageFirstToSecondRatio_ = -1; // the smaller the better
	
	Ptr<DescriptorMatcher> createMatcher(const SProcessParams & params);
public:
	//throws invalid argument
	CImagesMatch(Ptr<CImage>& object, Ptr<CImage>& scene, CLogger* logger, const SProcessParams& params);
	CImagesMatch(CImagesMatch&& right) noexcept;
	void drawPreviewAndResult(const string& runName, CLogger* logger);

	size_t getNumberOfMatches() { return matches_.size(); }
	double getAvarageMatchesDistance() { return avarageMatchesDistance_; }
	double getMatchedObjectFeaturesRatio() { return matchedObjectFeaturesRatio_; }
	double getAvarageFirstToSecondRatio() { return avarageFirstToSecondRatio_; }
	Ptr<CImage> getObjectImage() { return objectImage_; }
	Ptr<CImage> getSceneImage() { return sceneImage_; }

};

