#include "CFileLogger.h"


CFileLogger::CFileLogger(const string& outputRoot, const string& runName) 
	: COstreamLogger(true),
		out_(outputRoot + runName + ".txt", ofstream::trunc),
	outputRoot_(outputRoot),
	runName_(runName)
{

}

void CFileLogger::flush()
{
	ofstream txtOut;
	if (wasFlushed_) {
		txtOut.open(outputRoot_ + runName_ + ".txt", ofstream::app);
	}
	else {
		txtOut.open(outputRoot_ + runName_ + ".txt", ofstream::trunc);
	}

	txtOut << out_.str();
	txtOut.close();

	for(size_t i = 0; i < images_.size(); ++i){
		imwrite(outputRoot_ + runName + to_string(i) + ".jpg", images_[i].first);
	}
	
	wasFlushed_ = true;
}