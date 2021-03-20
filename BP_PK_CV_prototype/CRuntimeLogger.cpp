#include "CRuntimeLogger.h"

CLogger& CRuntimeLogger::logSection(const string& name, unsigned int level)  {
	//crop max level
	if (level > 2)
		level = 2;

	//make the name in upper case
	string upperName = name;
	if (level < 2) {
		for (auto& c : upperName)
			c = toupper(c);
	}

	std::cout << std::endl;

	for (int i = -1; i < (int)level; ++i)
		std::cout << "=-=";

	std::cout << upperName;

	for (int i = -1; i < (int)level; ++i)
		std::cout << "=-=";

	std::cout << std::endl;

	return *this;
}
CLogger& CRuntimeLogger::log(const string& toLog)  {
	std::cout << toLog;
	return *this;
}
CLogger& CRuntimeLogger::endl() {
	std::cout << std::endl;
	return *this;
}

void CRuntimeLogger::putImage(const Mat& image, const string& name)
{
	if (putImages_) {
		images_.push_back(make_pair(image, name));
	}
}

void CRuntimeLogger::flush()
{
	if (images_.size() > 0) {
		namedWindow("", WINDOW_NORMAL);
	}
	for (auto& it : images_) {
		imshow(it.second, it.first);
		waitKey(0);
	}
}
