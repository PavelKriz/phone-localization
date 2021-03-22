#include "CImage.h"

//=================================================================================================

void CImage::detectDescribeFeatures(EProcessMethod method, CLogger* logger)
{
	Ptr<Feature2D> detector;
	switch (method)
	{
	case CImage::SIFT_:
		detector = SIFT::create(1000);
		break;
	case CImage::ORB_:
		detector = ORB::create(1000);
		break;
	default:
		throw invalid_argument("Error feature detecting method was used! (non recognized method)");
		break;
	}

	detector->detectAndCompute(image_, noArray(), imageKeypoints_, keypointsDescriptors_);
	logger->log("Detection and description done, keypoints count: ").log(to_string(imageKeypoints_.size())).endl();
	wasProcessed_ = true;
}

//=================================================================================================

void CImage::processCLAHE(CLogger* logger)
{
	//TODO think about the parameters for clahe
	Ptr<CLAHE> clahePtr = createCLAHE();
	//wikipedia:  Common values limit the resulting amplification to between 3 and 4. 
	//from a test I got a better result with 3
	clahePtr->setClipLimit(3);
	clahePtr->apply(image_, image_);

	logger->log("CLAHE was applied on the image.");
}

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

void CImage::process(EProcessMethod method, CLogger* logger)
{
	logger->log("CImage with filepath: " + filePath_ + " is being processed.").endl();
	processCLAHE(logger);
	detectDescribeFeatures(method, logger);
}

//=================================================================================================
