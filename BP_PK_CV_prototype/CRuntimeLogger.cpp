#include "CRuntimeLogger.h"

CLogger& CRuntimeLogger::logSection(const string& name, unsigned int level)  {
	//make the name in upper case
	string upperName = name;
	for (auto& c : upperName)
		c = toupper(c);

	if (level > 2)
		level = 2;

	std::cout << std::endl;

	for (int i = -1; i < (int)level; ++i)
		std::cout << "==";

	std::cout << upperName;

	for (int i = -1; i < (int)level; ++i)
		std::cout << "==";

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

void CRuntimeLogger::putImage(const Mat& image, const string& outputPath)
{
	if (putImages_) {
		namedWindow("", WINDOW_NORMAL);
		imshow(outputPath, image);
		waitKey(0);
	}
}
