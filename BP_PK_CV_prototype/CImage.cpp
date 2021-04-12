#include "CImage.h"

//=================================================================================================

Ptr<Feature2D> CImage::getDetector(const SProcessParams& params)
{
	Ptr<Feature2D> detector;
	switch (params.detectMethod_)
	{
	case EAlgorithm::ALG_ROOTSIFT:
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
	case EAlgorithm::ALG_ROOTSIFT:
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
	case EAlgorithm::ALG_ROOTSIFT:
	case EAlgorithm::ALG_SIFT:
		detectorExtractor = SIFT::create(params.siftParams_.nfeatures_,
										 params.siftParams_.nOctaveLayers_,
										 params.siftParams_.contrastThreshold_,
										 params.siftParams_.edgeTreshold_,
										 params.siftParams_.sigma_);
		break;
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



void CImage::fastRootSiftDescriptorsAdjust(Ptr<CLogger>& logger)
{
	cout << "TYPE" << keypointsDescriptors_.type() << endl;
	double eps = 1e-7;
	Mat result(keypointsDescriptors_.rows, keypointsDescriptors_.cols, CV_8U);
	Mat floatDescriptors;
	keypointsDescriptors_.convertTo(floatDescriptors, CV_64F);

	for (size_t i = 0; i < floatDescriptors.rows; ++i) {
		//descriptor field

		double norm1Sum = 0;
		for (size_t j = 0; j < floatDescriptors.cols; ++j) {
			//the values in the descriptor vector should be in range [0,1]
			norm1Sum += (floatDescriptors.at<double>(i, j) = (floatDescriptors.at<double>(i, j) / 255));
		}

		double norm2Sum = 0;

		for (size_t j = 0; j < floatDescriptors.cols; ++j) {
			//no problem here with the sqrt since the descriptopr will be always positive
			(floatDescriptors.at<double>(i, j) = sqrt((floatDescriptors.at<double>(i, j) + eps) / norm1Sum));
			norm2Sum += (floatDescriptors.at<double>(i, j) * floatDescriptors.at<double>(i, j));
		}

		norm2Sum += eps;

		for (size_t j = 0; j < floatDescriptors.cols; ++j) {
			//the values in the descriptor vector should be in range [0,1]
			floatDescriptors.at<double>(i, j) = floatDescriptors.at<double>(i, j) / norm2Sum;
			//512 is used, because wast majority of values is encoded in range [0,1/2], so it is not cared about the rest
			result.at<uint8_t>(i, j) = (uint8_t)(floatDescriptors.at<double>(i, j) * 512);
			keypointsDescriptors_.at<float>(i, j) = (float)(floatDescriptors.at<double>(i, j) * 255 + eps);
		}
		Mat x;
	}

	for (size_t i = 0; i < keypointsDescriptors_.rows; ++i) {
		for (size_t j = 0; j < keypointsDescriptors_.cols; ++j) {
			//the values in the descriptor vector should be in range [0,1]
			if (i < 2)
				cout << keypointsDescriptors_.at<float>(i, j) << "  ";
		}
	}

	Mat x;
	/*
	for (size_t i = 0; i < floatDescriptors.rows; ++i) {
		for (size_t j = 0; j < floatDescriptors.cols; ++j) {
			//the values in the descriptor vector should be in range [0,1]
			if (i < 2)
				cout << (double)floatDescriptors.at<double>(i, j) << "  ";
		}
	}

	normalize(floatDescriptors, normDescriptors, 1.0, 0.0, NORM_L1);
	double norm1Sum = 0;
	for (size_t i = 0; i < floatDescriptors.rows; ++i) {
		for (size_t j = 0; j < floatDescriptors.cols; ++j) {
			//the values in the descriptor vector should be in range [0,1]
			norm1Sum += (floatDescriptors.at<double>(i, j) = floatDescriptors.at<double>(i, j) / 255);

		}
	}


	cout << "original" << endl;
	cout << keypointsDescriptors_.row(0).col(0) << endl;
	cout << "float val: float descriptors" << endl;
	cout << floatDescriptors.row(0).col(0) << endl;
	cout << "normalized (opencv)" << endl;
	cout << normDescriptors.row(0).col(0) << endl;
	cout << "normalized (me)" << endl;
	cout << floatDescriptors.row(0).col(0) / norm1Sum << endl;

	double norm2Sum = 0;
	for (size_t i = 0; i < normDescriptors.rows; ++i) {
		for (size_t j = 0; j < normDescriptors.cols; ++j) {
			//no problem here since the descriptopr will be always positive
			(normDescriptors.at<double>(i, j) = sqrt((normDescriptors.at<double>(i, j) + eps) / norm1Sum));
			norm2Sum += (normDescriptors.at<double>(i, j) * normDescriptors.at<double>(i, j));
		}
	}
	cout << "hellinger" << endl;
	cout << normDescriptors.row(0).col(0) << endl;

	Mat result(keypointsDescriptors_.rows, keypointsDescriptors_.cols, CV_8U);

	//normalize(floatDescriptors, normDescriptors, 1.0, 0.0, NORM_L2);
	for (size_t i = 0; i < normDescriptors.rows; ++i) {
		for (size_t j = 0; j < normDescriptors.cols; ++j) {
			//the values in the descriptor vector should be in range [0,1]
			normDescriptors.at<double>(i, j) = normDescriptors.at<double>(i, j) / norm2Sum;
			result.at<uint8_t>(i, j) = (uint8_t) (normDescriptors.at<double>(i, j) * 255);
		}
	}

	cout << "converted back (mine)" << endl;
	cout << (uint) result.at<uint8_t>(0, 0) << endl;
	cout << "norm l2 back (mine)" << endl;
	cout << normDescriptors.row(0).col(0)  << endl;
	cout << "norm l2 back (opencv)" << endl;
	cout << normDescriptors.row(0).col(0) << endl;

	for (size_t i = 0; i < result.rows; ++i) {
		for (size_t j = 0; j < result.cols; ++j) {
			//the values in the descriptor vector should be in range [0,1]
			if(i < 2)
				cout << (uint) result.at<uint8_t>(i, j) << "  ";
		}
	}

	cout << endl << endl;
	normalize(floatDescriptors, floatDescriptors, 255.0, 0.0, NORM_MINMAX);

	for (size_t i = 0; i < floatDescriptors.rows; ++i) {
		for (size_t j = 0; j < floatDescriptors.cols; ++j) {
			//the values in the descriptor vector should be in range [0,1]
			if (i < 2)
				cout << floatDescriptors.at<double>(i, j) << "  ";
		}
	}

	Mat x;
	*/
	//normalize(l1NormDescriptors, l1NormDescriptors, 1.0, 0.0, NORM_L2);
	//cout << l1NormDescriptors.row(0).col(0) << endl;
	/*
	//iterate over each descriptor
	for (size_t i = 0; i < floatDescriptors.rows; ++i) {
		double norm1Sum = 0;
		for (size_t j = 0; j < floatDescriptors.cols; ++j) {
			//the values in the descriptor vector should be in range [0,1]
			norm1Sum += floatDescriptors.at<float>(i, j);
			cout << floatDescriptors.at<float>(i, j) << "  " << endl;
		}

	}*/
	
}

void CImage::preciseRootSiftDescriptorsAdjust(Ptr<CLogger>& logger)
{
	cout << "TYPE" << keypointsDescriptors_.type() << endl;
	double eps = 1e-7;
	Mat result(keypointsDescriptors_.rows, keypointsDescriptors_.cols, CV_8U);
	Mat floatDescriptors;
	keypointsDescriptors_.convertTo(floatDescriptors, CV_64F);

	for (size_t i = 0; i < floatDescriptors.rows; ++i) {
		//descriptor field

		double norm1Sum = 0;
		for (size_t j = 0; j < floatDescriptors.cols; ++j) {
			//the values in the descriptor vector should be in range [0,1]
			norm1Sum += (floatDescriptors.at<double>(i, j) = (floatDescriptors.at<double>(i, j) / 255));
		}

		double norm2Sum = 0;

		for (size_t j = 0; j < floatDescriptors.cols; ++j) {
			//no problem here with the sqrt since the descriptopr will be always positive
			(floatDescriptors.at<double>(i, j) = sqrt((floatDescriptors.at<double>(i, j) + eps) / norm1Sum));
			norm2Sum += (floatDescriptors.at<double>(i, j) * floatDescriptors.at<double>(i, j));
		}

		norm2Sum += eps;

		for (size_t j = 0; j < floatDescriptors.cols; ++j) {
			//the values in the descriptor vector should be in range [0,1]
			floatDescriptors.at<double>(i, j) = floatDescriptors.at<double>(i, j) / norm2Sum;
			//512 is used, because wast majority of values is encoded in range [0,1/2], so it is not cared about the rest
			result.at<uint8_t>(i, j) = (uint8_t)(floatDescriptors.at<double>(i, j) * 512);
			keypointsDescriptors_.at<float>(i, j) = (float)(floatDescriptors.at<double>(i, j) * 255 + eps);
		}
		Mat x;
	}

	for (size_t i = 0; i < keypointsDescriptors_.rows; ++i) {
		for (size_t j = 0; j < keypointsDescriptors_.cols; ++j) {
			//the values in the descriptor vector should be in range [0,1]
			if (i < 2)
				cout << keypointsDescriptors_.at<float>(i, j) << "  ";
		}
	}

	Mat x;
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
	if (params.describeMethod_ == EAlgorithm::ALG_ROOTSIFT) {
		fastRootSiftDescriptorsAdjust(logger);
	}
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
