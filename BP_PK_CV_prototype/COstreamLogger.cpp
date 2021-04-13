#include "COstreamLogger.h"

CLogger& COstreamLogger::logSection(const string& name, unsigned int level) {
	//crop max level
	if (level > 2)
		level = 2;
	currentSectionLevel_ = level;

	//make the name in upper case
	string upperName = name;
	if (level < 2) {
		for (auto& c : upperName)
			c = toupper(c);
	}

	out() << std::endl;

	for (int i = -1; i < (int)level; ++i)
		out() << "===";

	out() << upperName;

	for (int i = -1; i < (int)level; ++i)
		out() << "===";

	endl();

	return *this;
}

CLogger& COstreamLogger::log(const string& toLog) {
	out() << toLog;
	return *this;
}

CLogger& COstreamLogger::logError(const string& toLog)
{
	emergencyStart();
	int tmpLvl = currentSectionLevel_;
	logSection("Error", 0);
	log(toLog);
	out() << std::endl;
	log("===========================");
	
	currentSectionLevel_ = tmpLvl;
	endl();

	emergencyEnd();
	return *this;
}

CLogger& COstreamLogger::endl() {
	out() << std::endl;
	for (int i = 0; i < currentSectionLevel_; ++i) out() << "    ";
	return *this;
}

void COstreamLogger::putImage(const Mat& image, const string& name)
{
	if (putImages_) {
		images_.push_back(make_pair(image, name));
	}
}

