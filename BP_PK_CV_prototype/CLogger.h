#pragma once

#include <iostream>

//Matrices
#include <opencv2/core/mat.hpp>

using namespace std;
using namespace cv;

class CLogger {
protected:
	bool putImages_;
public:
	CLogger(bool putImages) : putImages_(putImages){}
	virtual ~CLogger() {}
	virtual CLogger& logSection(const string& name, unsigned int level = 0) = 0;
	virtual CLogger& log(const string& toLog) = 0;
	virtual CLogger& endl() = 0;
	virtual ostream& getStream() = 0;
	virtual void putImage(const Mat& image, const string& outputPath) = 0;
	virtual void flush() = 0;
};