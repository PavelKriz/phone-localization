#include "CObjectInSceneFinder.h"


CObjectInSceneFinder::CObjectInSceneFinder(const SProcessParams& params, Ptr<CLogger> &logger, const string& runName, const string& sceneFilePath, const vector<string>& objectFilePaths)
	:
	params_(params),
	logger_(logger)
{
	detectorExtractor_ = CImage::createDetectorExtractor(params);
	if (logger_.empty()) {
		throw invalid_argument("CObjectInSceneFinder constructor was called with empty pointer to logger (CLogger) object.");
	}
	logger_->logSection("Run: " + runName, 0);
	sceneImage_ = new CImage(sceneFilePath);
	for (auto& it : objectFilePaths) {
		objectImages_.push_back(new CImage(it));
	}
	logger_->log("images loaded").endl();
}

//=================================================================================================

void CObjectInSceneFinder::run( const string& runName, bool viewResult)
{
	if (logger_.empty()) {
		throw invalid_argument(
			string("CObjectInSceneFinder: method run was called but the logger is empty") +
			"(the given logger in constructor has to stay valid for the whole lifetime of CObjectInSceneFinder),");
	}
	//set begin time
	chrono::steady_clock::time_point begin = chrono::steady_clock::now();

	logger_->logSection("detectig and describing features", 1);
	logger_->logSection("Scene", 2);
	//prepare the scene
	sceneImage_->process(params_, logger_, detectorExtractor_);

	//prepare the object
	logger_->logSection("Objects", 2);
	for (auto& ptr : objectImages_) {
		ptr->process(params_, logger_, detectorExtractor_);
	}

	logger_->logSection("Timing", 2);
	chrono::steady_clock::time_point afterDetectingDescring = chrono::steady_clock::now();
	logger_->log("Detecting and describing all features took: ").
		log(to_string(chrono::duration_cast<chrono::milliseconds>(afterDetectingDescring - begin).count())).
		log("[ms]").endl();

	logger_->logSection("Matching", 1);
	//searching for the scene object matching combination with lowest avarage distance of matches
	double lowestDistance = numeric_limits<double>::max();
	int lowestDistanceIndex = 0;
	//that many matches will be created
	matches_.reserve(objectImages_.size());
	for (int i = 0; i < objectImages_.size(); ++i) {
		//computing the keypoints, descriptors, matches
		//move construction
		logger_->endl().log("Compare index: ").log(to_string(i)).endl();
		logger_->log("Matching scene with object that has filepath: ").log(objectImages_[i]->getFilePath()).endl();
		matches_.emplace_back(CImagesMatch(objectImages_[i], sceneImage_, logger_, params_));

		//checking if the match is possible to be the best until now
		double currentDist = matches_.back().getAvarageMatchesDistance();
		if (currentDist < lowestDistance) {
			lowestDistanceIndex = i;
			lowestDistance = currentDist;
		}
		bestMatchExist_ = true;
	}

	logger_->logSection("Timing", 2);
	chrono::steady_clock::time_point afterMatching = chrono::steady_clock::now();
	logger_->log("Finding the right object took: ").
		log(to_string(chrono::duration_cast<chrono::milliseconds>(afterMatching - afterDetectingDescring).count())).
		log("[ms]").endl();

	logger_->log("Time of the whole process: ").
		log(to_string(chrono::duration_cast<chrono::milliseconds>(afterMatching - begin).count())).
		log("[ms]").endl();

	bestMatchIndex_ = lowestDistanceIndex;
	logger_->logSection("Result", 2);
	logger_->log("Best object match for scene is object with compare index: ").log(to_string(bestMatchIndex_)).endl();
	logger_->log("Best object match for scene is object with filepath: ").log(objectImages_[bestMatchIndex_]->getFilePath()).endl();

	if (viewResult) {

		matches_[bestMatchIndex_].drawPreviewAndResult(runName, logger_);
		logger_->log("The result object stats: ").endl();
		logger_->log("Avarage feature match distance: ").log(to_string(matches_[bestMatchIndex_].getAvarageMatchesDistance())).endl();
		logger_->log("Average first to second ratio is  ").log(to_string(matches_[bestMatchIndex_].getAvarageFirstToSecondRatio())).endl();
		logger_->log("Ratio of filtered matches to number of keypoints of object is: ").log(to_string(matches_[bestMatchIndex_].getMatchedObjectFeaturesRatio())).endl();
		logger_->logSection("Timing", 2);
		chrono::steady_clock::time_point afterRenderingResult = chrono::steady_clock::now();
		logger_->log("Result output took: ").
			log(to_string(chrono::duration_cast<chrono::milliseconds>(afterRenderingResult - afterMatching).count())).
			log("[ms]").endl();
		
		logger_->log("Time of the whole process with view: ").
			log(to_string(chrono::duration_cast<chrono::milliseconds>(afterRenderingResult - begin).count())).
			log("[ms]").endl();
	}

	return;
}

//=================================================================================================

void CObjectInSceneFinder::viewBestResult(const string& runName)
{
	if (logger_.empty()) {
		throw invalid_argument(
			string("CObjectInSceneFinder: method viewBestResult was called but the logger is empty") +
			"(the given logger in constructor has to stay valid for the whole lifetime of CObjectInSceneFinder),");
	}
	if (bestMatchExist_) {
		matches_[bestMatchIndex_].drawPreviewAndResult(runName, logger_);
	}
	else {
		throw logic_error("View of matches was called without computing matches first");
	}

}