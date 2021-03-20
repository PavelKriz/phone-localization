#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>


//reading writing images
#include <opencv2/imgcodecs.hpp>

#include "COstreamLogger.h"
#include "parameters.h"

using namespace std;

//can use flush only once
class CFileLogger : public COstreamLogger {
protected:
	ostringstream out_;
	const string outputRoot_;
	const string runName_;
	bool wasFlushed_ = false;

	inline virtual ostream& out() override { return out_; }
public:
	CFileLogger() = delete;
	//file logger allways produces images
	CFileLogger(const string& outputRoot, const string & runName);
	virtual ~CFileLogger() override {}
	// TODO throwing input output exceptions (checking input)
	virtual void flush() override;
};

