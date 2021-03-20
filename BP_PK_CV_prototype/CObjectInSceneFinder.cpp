#include "CObjectInSceneFinder.h"


CObjectInSceneFinder::CObjectInSceneFinder(const string& runName, const string& sceneFilePath, const vector<string>& objectFilePaths)
{
	if (OUTPUT_TYPE == EOutputType::console) {
		logger_ = new CRuntimeLogger(WRITE_OR_SHOW_IMAGES);
	}
	else {
		logger_ = new CFileLogger(OUTPUT_ROOT, runName);
	}

	logger_->logSection("Run: " + runName, 0);
	sceneImage_ = new CImage(sceneFilePath);
	for (auto& it : objectFilePaths) {
		objectImages_.push_back(new CImage(it));
	}
	logger_->log("images loaded").endl();
}

//=================================================================================================

int CObjectInSceneFinder::run(CImage::EProcessMethod method, bool viewResult)
{
	//set begin time
	chrono::steady_clock::time_point begin = chrono::steady_clock::now();

	logger_->logSection("detectig and describing features", 1);
	logger_->logSection("object", 2);
	//prepare the scene
	sceneImage_->detectDescribeFeatures(method, logger_);

	//prepare the scenes
	logger_->logSection("scenes", 2);
	for (auto& ptr : objectImages_) {
		ptr->detectDescribeFeatures(method, logger_);
	}

	logger_->logSection("timing", 2);
	chrono::steady_clock::time_point afterDetectingDescring = chrono::steady_clock::now();
	logger_->log("Detecting and describing all features took: ").
		log(to_string(chrono::duration_cast<chrono::milliseconds>(afterDetectingDescring - begin).count())).
		log("[ms]").endl();

	logger_->logSection("matching", 1);
	//searching for the scene object matching combination with lowest avarage distance of matches
	double lowestDistance = numeric_limits<double>::max();
	int lowestDistanceIndex = 0;
	//that many matches will be created
	matches_.reserve(objectImages_.size());
	for (int i = 0; i < objectImages_.size(); ++i) {
		//computing the keypoints, descriptors, matches
		//move construction
		logger_->endl().log("Matching scene with object that has filepath: ").log(objectImages_[i]->getFilePath()).endl();
		matches_.emplace_back(CImagesMatch(objectImages_[i], sceneImage_, logger_, CImagesMatch::EMatchingMethod::BRUTE_FORCE));

		//checking if the match is possible to be the best until now
		double currentDist = matches_.back().getAvarageMatchesDistance();
		if (currentDist < lowestDistance) {
			lowestDistanceIndex = i;
			lowestDistance = currentDist;
		}
		bestMatchExist_ = true;
		logger_->log("Compare index: ").log(to_string(i)).log(" | avarage distance: ").log(to_string(currentDist)).endl();
	}

	logger_->logSection("timing", 2);
	chrono::steady_clock::time_point afterMatching = chrono::steady_clock::now();
	logger_->log("Finding the right object took: ").
		log(to_string(chrono::duration_cast<chrono::milliseconds>(afterMatching - afterDetectingDescring).count())).
		log("[ms]").endl();

	logger_->log("Time of the whole process: ").
		log(to_string(chrono::duration_cast<chrono::milliseconds>(afterMatching - begin).count())).
		log("[ms]").endl();

	bestMatchIndex_ = lowestDistanceIndex;
	logger_->logSection("result", 2);
	logger_->log("Best object match for scene is object with filepath: ").log(objectImages_[bestMatchIndex_]->getFilePath()).endl();

	if (viewResult) {

		matches_[bestMatchIndex_].drawPreviewAndResult(runName, logger_);
		logger_->logSection("timing", 2);
		chrono::steady_clock::time_point afterRenderingResult = chrono::steady_clock::now();
		logger_->log("Result output took: ").
			log(to_string(chrono::duration_cast<chrono::milliseconds>(afterRenderingResult - afterMatching).count())).
			log("[ms]").endl();
		
		logger_->log("Time of the whole process with view: ").
			log(to_string(chrono::duration_cast<chrono::milliseconds>(afterRenderingResult - begin).count())).
			log("[ms]").endl();
	}

	return lowestDistanceIndex;
}

//=================================================================================================

void CObjectInSceneFinder::viewBestResult(const string& runName)
{
	if (bestMatchExist_) {
		matches_[bestMatchIndex_].drawPreviewAndResult(runName, logger_);
	}
	else {
		throw logic_error("View of matches was called without computing matches first");
	}

}