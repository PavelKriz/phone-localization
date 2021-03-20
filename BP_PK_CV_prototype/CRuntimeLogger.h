#pragma once

#include <iostream>
#include <vector>
#include <utility>
#include "CLogger.h"

//reading writing images
#include <opencv2/imgcodecs.hpp>
//showing image functions
#include <opencv2/highgui.hpp>

using namespace std;

class CRuntimeLogger : public CLogger {
	vector<pair<Mat, string>> images_;
public:
	CRuntimeLogger() = delete;
	CRuntimeLogger(bool putImages) : CLogger(putImages){}
	~CRuntimeLogger() override {}
	virtual CLogger& logSection(const string& name, unsigned int level = 0) override;
	virtual CLogger& log(const string& toLog) override;
	virtual CLogger& endl() override;
	inline virtual ostream& getStream() override { return std::cout; }
	virtual void putImage(const Mat& image, const string & name) override;
	virtual void flush() override;
};


