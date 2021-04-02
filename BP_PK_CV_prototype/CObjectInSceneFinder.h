#pragma once

#include <vector>
#include <chrono>

//wrapper around basic shared pointer
#include <opencv2/core/cvstd_wrapper.hpp>

#include "CRuntimeLogger.h"
#include "CFileLogger.h"
#include "CImagesMatch.h"



//class that searches trough scenes and it is searching where is the object
class CObjectInSceneFinder {
	CLogger* logger_;
	Ptr<CImage> sceneImage_;
	vector<Ptr<CImage>> objectImages_;
	vector<CImagesMatch> matches_;
	size_t bestMatchIndex_;
	bool bestMatchExist_ = false;
public:
	//can throw ios_base::failure exception
	CObjectInSceneFinder(const string& runName, const string& sceneFilePath, const vector<string>& objectFilePaths);
	~CObjectInSceneFinder() { delete logger_; }
	//can throw invalid_argument
	int run(const SProcessParams& params, bool viewResult = true);
	//throws exception logic_error
	void viewBestResult(const string& runName);
	//print all the log, should be used on the end
	void report() { logger_->flush(); }
};
