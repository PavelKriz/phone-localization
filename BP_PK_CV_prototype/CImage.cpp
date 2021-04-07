#include "CImage.h"

//=================================================================================================

Ptr<Feature2D> CImage::getDetector(const SProcessParams& params)
{
	Ptr<Feature2D> detector;
	switch (params.detectMethod_)
	{
	case EAlgorithm::ALG_SIFT:
		detector = SIFT::create(params.siftParams_.nfeatures_);
		break;
	case EAlgorithm::ALG_ORB:
		detector = ORB::create(params.orbParams_.nfeatures_);
		break;
	default:
		throw invalid_argument("Error feature detecting method was used! (non recognized method)");
		break;
	}
	return detector;
}

Ptr<Feature2D> CImage::getExtractor(const SProcessParams& params)
{
	Ptr<Feature2D> extractor;
	switch (params.describeMethod_)
	{
	case EAlgorithm::ALG_SIFT:
		extractor = SIFT::create(params.siftParams_.nfeatures_);
		break;
	case EAlgorithm::ALG_ORB:
		extractor = ORB::create(params.orbParams_.nfeatures_);
		break;
	case EAlgorithm::ALG_BEBLID:
		extractor = xfeatures2d::BEBLID::create(params.beblidParams_.scale_factor_, params.beblidParams_.n_bits_);
		break;
	default:
		throw invalid_argument("Error feature detecting method was used! (non recognized method)");
		break;
	}
	return extractor;
}

Ptr<Feature2D> CImage::getDetectorExtractor(const SProcessParams& params)
{
	Ptr<Feature2D> detectorExtractor;
	switch (params.detectMethod_)
	{
	case EAlgorithm::ALG_SIFT:
		detectorExtractor = SIFT::create(params.siftParams_.nfeatures_);
		break;
	case EAlgorithm::ALG_ORB:
		detectorExtractor = ORB::create(params.orbParams_.nfeatures_);
		break;
	default:
		throw invalid_argument("Error feature detecting method was used! (non recognized method)");
		break;
	}
	return detectorExtractor;
}

void CImage::detectDescribeFeatures(const SProcessParams & params, CLogger* logger)
{
	if (params.detectMethod_ == params.describeMethod_) {
		Ptr<Feature2D> detector = getDetectorExtractor(params);

		detector->detectAndCompute(image_, noArray(), imageKeypoints_, keypointsDescriptors_);
		logger->log("Detection and description done, keypoints count: ").log(to_string(imageKeypoints_.size())).endl();
		wasProcessed_ = true;
	}
	else {
		Ptr<Feature2D> detector = getDetector(params);
		Ptr<Feature2D> extractor = getExtractor(params);

		detector->detect(image_, imageKeypoints_);
		extractor->compute(image_, imageKeypoints_, keypointsDescriptors_);
		logger->log("Detection and description done, keypoints count: ").log(to_string(imageKeypoints_.size())).endl();
		wasProcessed_ = true;
	}
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
