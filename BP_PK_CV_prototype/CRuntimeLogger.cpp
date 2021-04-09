#include "CRuntimeLogger.h"

CRuntimeLogger::CRuntimeLogger(bool timing) : COstreamLogger(timing){}

void CRuntimeLogger::flush()
{
	//following block of code can be added for different windows behavior
	//if (images_.size() > 0) {
	//	namedWindow("", WINDOW_NORMAL);
	//}
	for (auto& it : images_) {
		//if namedWindow is not used the autosize mode is used
		namedWindow(it.second, WINDOW_NORMAL);
		imshow(it.second, it.first);
		waitKey(0);
	}

	images_.clear();
}
