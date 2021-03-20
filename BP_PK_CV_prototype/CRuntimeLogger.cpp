#include "CRuntimeLogger.h"

CRuntimeLogger::CRuntimeLogger(bool putImages) : COstreamLogger(putImages){}

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
