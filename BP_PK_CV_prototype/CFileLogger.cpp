#include "CFileLogger.h"


CFileLogger::CFileLogger(const string& outputRoot, const string& runName, bool timing)
	: COstreamLogger(timing),
		out_(outputRoot + runName + ".txt", ofstream::trunc),
	outputRoot_(outputRoot),
	runName_(runName)
{

}

void CFileLogger::flush()
{
	//write text to the file
	ofstream txtOut;
	if (wasFlushed_) {
		txtOut.open(outputRoot_ + runName_ + ".txt", ofstream::app);
	}
	else {
		txtOut.open(outputRoot_ + runName_ + ".txt", ofstream::trunc);
	}

	txtOut << out_.str();
	txtOut.close();
	out_.str(std::string());

	//write images to the files (OpenCV functions)
	for(size_t i = 0; i < images_.size(); ++i){
		imwrite(outputRoot_ + runName_ + to_string(i) + ".jpg", images_[i].first);
	}
	
	images_.clear();
	wasFlushed_ = true;
}