#include "CImage.h"

//=================================================================================================

CImage::CImage(string filePath)
	: filePath_(filePath)
{
	image_ = imread(filePath, CV_8U);
	if (image_.empty())
	{
		throw ios_base::failure("Can't load image with file path: " + filePath);
	}
}

//=================================================================================================

void CImage::detectDescribeFeatures(EProcessMethod method, CLogger* logger)
{
	Ptr<Feature2D> detector;
	switch (method)
	{
	case CImage::SIFT_:
		detector = SIFT::create(500);
		break;
	case CImage::ORB_:
		detector = ORB::create(500);
		break;
	default:
		throw invalid_argument("Error feature detecting method was used! (non recognized method)");
		break;
	}

	detector->detectAndCompute(image_, noArray(), imageKeypoints_, keypointsDescriptors_);
	logger->log("CImage with filepath: " + filePath_ + " keypoints count: ").log(to_string(imageKeypoints_.size())).endl();
	wasProcessed_ = true;
}