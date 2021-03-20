#pragma once

#include <iostream>
#include <vector>
#include <utility>
#include "CLogger.h"
#include "COstreamLogger.h"

//reading writing images
#include <opencv2/imgcodecs.hpp>
//showing image functions
#include <opencv2/highgui.hpp>

using namespace std;

class CRuntimeLogger : public COstreamLogger {
protected:
	inline virtual ostream& out() override { return cout; }
public:
	CRuntimeLogger() = delete;
	CRuntimeLogger(bool putImages);
	virtual ~CRuntimeLogger() override {}
	virtual void flush() override;
};


