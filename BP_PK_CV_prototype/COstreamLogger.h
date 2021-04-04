#pragma once

#include <iostream>
#include <string>

#include "CLogger.h"

class COstreamLogger : public CLogger {
protected:
	unsigned int currentSectionLevel_ = 0;
	vector<pair<Mat, string>> images_;

	inline virtual ostream& out() = 0;
public:
	COstreamLogger() = delete;
	COstreamLogger(bool timing) : CLogger(timing){}
	virtual ~COstreamLogger() override { }
	//lowest level can be 2 and highest 0
	virtual CLogger& logSection(const string& name, unsigned int level = 0) override;
	virtual CLogger& log(const string& toLog) override;
	virtual CLogger& endl() override;
	virtual void putImage(const Mat& image, const string& name) override;
	virtual void flush() override = 0;
};

