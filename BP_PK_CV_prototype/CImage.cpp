#include "CImage.h"

//=================================================================================================

void CImage::detectDescribeFeatures(const SProcessParams & params, CLogger* logger)
{
	Ptr<Feature2D> detector;
	switch (params.detectExtractMethod_)
	{
	case SProcessParams::EDetectExtractMethod::SIFT:
		detector = SIFT::create(params.siftParams_.nfeatures_);
		break;
	case SProcessParams::EDetectExtractMethod::ORB:
		detector = ORB::create(params.orbParams_.nfeatures_);
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

void CImage::process(const SProcessParams& params, CLogger* logger)
{
	logger->log("CImage with filepath: " + filePath_ + " is being processed.").endl();
	processCLAHE(logger);
	detectDescribeFeatures(params, logger);
}

//=================================================================================================
