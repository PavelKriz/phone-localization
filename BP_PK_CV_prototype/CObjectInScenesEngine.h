#pragma once

#include <vector>
#include <chrono>

//wrapper around basic shared pointer
#include <opencv2/core/cvstd_wrapper.hpp>

#include "CRuntimeLogger.h"
#include "CImagesMatch.h"



//class that searches trough scenes and it is searching where is the object
class CObjectInScenesEngine {
	CLogger* logger_;
	Ptr<CImage> objectImage_;
	vector<Ptr<CImage>> sceneImages_;
	vector<CImagesMatch> matches_;
	size_t bestMatchIndex_;
	bool bestMatchExist_ = false;
public:
	//can throw ios_base::failure exception
	CObjectInScenesEngine(const string& runName, const string& objectFilePath, const vector<string>& sceneFilePaths);
	~CObjectInScenesEngine() { delete logger_; }
	//can throw invalid_argument
	int run(CImage::EProcessMethod method, bool viewResult = true);
	//throws exception logic_error
	void viewBestResult(const string& runName);
	//print all the log, should be used on the end
	void report() { logger_->flush(); }
};
