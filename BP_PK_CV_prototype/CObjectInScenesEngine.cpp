#include "CObjectInScenesEngine.h"


CObjectInScenesEngine::CObjectInScenesEngine(const string& runName, const string& objectFilePath, const vector<string>& sceneFilePaths)
{
	logger_ = new CRuntimeLogger(WRITE_OR_SHOW_IMAGES);
	logger_->logSection("Run: " + runName, 0);
	objectImage_ = new CImage(objectFilePath);
	for (auto& it : sceneFilePaths) {
		sceneImages_.push_back(new CImage(it));
	}
	logger_->log("images loaded").endl();
}

//=================================================================================================

int CObjectInScenesEngine::run(CImage::EProcessMethod method, bool viewResult)
{
	//set begin time
	chrono::steady_clock::time_point begin = chrono::steady_clock::now();

	logger_->logSection("detectig and describing features", 1);
	logger_->logSection("object", 2);
	//prepare the object
	objectImage_->detectDescribeFeatures(method, logger_);

	//prepare the scenes
	logger_->logSection("scenes", 2);
	for (auto& ptr : sceneImages_) {
		ptr->detectDescribeFeatures(method, logger_);
	}

	logger_->logSection("timing", 2);
	chrono::steady_clock::time_point afterDetectingDescring = chrono::steady_clock::now();
	logger_->getStream() << "Detecting and describing all features took: " << 
		chrono::duration_cast<chrono::milliseconds>(afterDetectingDescring - begin).count() << "[ms]" << endl;

	logger_->logSection("matching", 1);
	//searching for the scene object matching combination with lowest avarage distance of matches
	double lowestDistance = numeric_limits<double>::max();
	int lowestDistanceIndex = 0;
	//that many matches will be created
	matches_.reserve(sceneImages_.size());
	for (int i = 0; i < sceneImages_.size(); ++i) {
		//computing the keypoints, descriptors, matches
		//move construction
		logger_->getStream() << endl << "Matching object with scene with filepath: " << sceneImages_[i]->getFilePath() << endl;
		matches_.emplace_back(CImagesMatch(objectImage_, sceneImages_[i], logger_, CImagesMatch::EMatchingMethod::BRUTE_FORCE));

		//checking if the match is possible to be the best until now
		double currentDist = matches_.back().getAvarageMatchesDistance();
		if (currentDist < lowestDistance) {
			lowestDistanceIndex = i;
			lowestDistance = currentDist;
		}
		bestMatchExist_ = true;
		logger_->getStream() << "Compare index:" << i << " | avarage distance: " << currentDist << endl;
	}

	logger_->logSection("timing", 2);
	chrono::steady_clock::time_point afterMatching = chrono::steady_clock::now();
	logger_->getStream() << "Matching the right scene took: " << 
		chrono::duration_cast<chrono::milliseconds>(afterMatching - afterDetectingDescring).count() << "[ms]" << endl;

	logger_->getStream() << "Time of the whole process: " <<
		chrono::duration_cast<chrono::milliseconds>(afterMatching - begin).count() << "[ms]" << endl;

	bestMatchIndex_ = lowestDistanceIndex;
	logger_->logSection("result", 2);
	logger_->getStream() << "Best scene match for object is scene with filepath: " << sceneImages_[bestMatchIndex_]->getFilePath() << endl;

	if (viewResult) {

		matches_[bestMatchIndex_].drawPreviewAndResult(runName, logger_);
		logger_->logSection("timing", 2);
		chrono::steady_clock::time_point afterRenderingResult = chrono::steady_clock::now();
		logger_->getStream() << "Result output took: " <<
			chrono::duration_cast<chrono::milliseconds>(afterRenderingResult - afterMatching).count() << "[ms]" << endl;

		logger_->getStream() << "Time of the whole process with view: " <<
			chrono::duration_cast<chrono::milliseconds>(afterRenderingResult - begin).count() << "[ms]" << endl;
	}

	return lowestDistanceIndex;
}

//=================================================================================================

void CObjectInScenesEngine::viewBestResult(const string& runName)
{
	if (bestMatchExist_) {
		matches_[bestMatchIndex_].drawPreviewAndResult(runName, logger_);
	}
	else {
		throw logic_error("View of matches was called without computing matches first");
	}

}