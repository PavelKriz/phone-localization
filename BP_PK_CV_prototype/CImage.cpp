#include "CImage.h"

//=================================================================================================

Ptr<Feature2D> CImage::getDetector(const SProcessParams& params)
{
	Ptr<Feature2D> detector;
	switch (params.detectMethod_)
	{
	case EAlgorithm::ALG_SIFT:
		detector = SIFT::create(params.siftParams_.nfeatures_,
								params.siftParams_.nOctaveLayers_,
								params.siftParams_.contrastThreshold_,
								params.siftParams_.edgeTreshold_,
								params.siftParams_.sigma_);
		break;
	case EAlgorithm::ALG_ORB:
		detector = ORB::create(params.orbParams_.nfeatures_,
							   params.orbParams_.scaleFactor_,
							   params.orbParams_.nlevels_,
							   params.orbParams_.edgeTreshold_,
							   params.orbParams_.firstLevel_,
							   params.orbParams_.WTA_K_,
							   params.orbParams_.scoreType_,
							   params.orbParams_.patchSize_,
							   params.orbParams_.fastTreshold_);
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
		extractor = SIFT::create(params.siftParams_.nfeatures_,
								 params.siftParams_.nOctaveLayers_,
								 params.siftParams_.contrastThreshold_,
								 params.siftParams_.edgeTreshold_,
								 params.siftParams_.sigma_);
		break;
	case EAlgorithm::ALG_ORB:
		extractor = ORB::create(params.orbParams_.nfeatures_,
								params.orbParams_.scaleFactor_,
								params.orbParams_.nlevels_,
								params.orbParams_.edgeTreshold_,
								params.orbParams_.firstLevel_,
								params.orbParams_.WTA_K_,
								params.orbParams_.scoreType_,
								params.orbParams_.patchSize_,
								params.orbParams_.fastTreshold_);
		break;
#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
	case EAlgorithm::ALG_BEBLID:
		extractor = xfeatures2d::BEBLID::create(params.beblidParams_.scale_factor_,
												params.beblidParams_.n_bits_);
		break;
#endif
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
		detectorExtractor = SIFT::create(params.siftParams_.nfeatures_,
										 params.siftParams_.nOctaveLayers_,
										 params.siftParams_.contrastThreshold_,
										 params.siftParams_.edgeTreshold_,
										 params.siftParams_.sigma_);
	case EAlgorithm::ALG_ORB:
		detectorExtractor = ORB::create(params.orbParams_.nfeatures_,
										params.orbParams_.scaleFactor_,
										params.orbParams_.nlevels_,
										params.orbParams_.edgeTreshold_,
										params.orbParams_.firstLevel_,
										params.orbParams_.WTA_K_,
										params.orbParams_.scoreType_,
										params.orbParams_.patchSize_,
										params.orbParams_.fastTreshold_);
		break;
	default:
		throw invalid_argument("Error feature detecting method was used! (non recognized method)");
		break;
	}
	return detectorExtractor;
}

void CImage::detectDescribeFeatures(const SProcessParams & params, Ptr<CLogger>& logger)
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

void CImage::processCLAHE(Ptr<CLogger>& logger)
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

CImage::CImage(const string& filePath)
	: filePath_(filePath)
{
	image_ = imread(filePath, CV_8U);
	if (image_.empty())
	{
		throw ios_base::failure("Can't load image with file path: " + filePath);
	}
}

void CImage::process(const SProcessParams& params, Ptr<CLogger>& logger)
{
	logger->log("CImage with filepath: " + filePath_ + " is being processed.").endl();
	processCLAHE(logger);
	detectDescribeFeatures(params, logger);
}

const vector<KeyPoint>& CImage::getKeypoints() const
{
	if (!wasProcessed_) {
		throw logic_error("CImage - to get keypoints first the process function has to be called.");
	}
	return imageKeypoints_;
}

const Mat& CImage::getDescriptors() const
{
	if (!wasProcessed_) {
		throw logic_error("CImage - to get descriptors of the keypoints first the process function has to be called.");
	}
	return keypointsDescriptors_;
}

//=================================================================================================
