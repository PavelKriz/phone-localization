#include "CImagesMatch.h"

Ptr<DescriptorMatcher> CImagesMatch::createMatcher(const SProcessParams & params)
{
	Ptr<DescriptorMatcher> matcher;
	switch (params.matchingMethod_)
	{
	case EAlgorithm::ALG_BF_MATCHING:
		if (params.describeMethod_ == EAlgorithm::ALG_SIFT ||
			params.describeMethod_ == EAlgorithm::ALG_ROOTSIFT ||
			params.describeMethod_ == EAlgorithm::ALG_PRECISE_ROOTSIFT) {
			//matcher = DescriptorMatcher::create("BruteForce");
			matcher = BFMatcher::create(NORM_L2);
		}
		//ORB, BEBLID or other hamming distance base descriptor extracting methods
		else {
			matcher = BFMatcher::create(NORM_HAMMING);
		}
		break;
	case EAlgorithm::ALG_FLANN_MATCHING:
		if (params.describeMethod_ == EAlgorithm::ALG_SIFT) {
			//matcher = DescriptorMatcher::create("FlannBased");
			matcher = FlannBasedMatcher::create();
		}
		else {
			//explaining why to use lsh or hiearchical clustering
			//https://stackoverflow.com/questions/23634730/opencv-flann-matcher-crashes/23639463?noredirect=1#comment36322682_23639463
			//cv::makePtr<flann::LshIndexParams>(12, 20, 2) is a way how to make the matcher work with binary descriptors
			//there is being build a tree in the flann based method and the tree uses indexes based on the distance used by the descriptors
			//see: https://stackoverflow.com/questions/43830849/opencv-use-flann-with-orb-descriptors-to-match-features
			//the values are explained here in the older documentation (still the same implementation):
			// https://docs.opencv.org/2.4/modules/flann/doc/flann_fast_approximate_nearest_neighbor_search.html
			// in the previous link - also very handy explanation of other distances used in combination with flann

			//there is a table in one comment saying which descriptors are using hamming (binary descriptors) and which euclidean distance (floating point descriptors)
			//https://stackoverflow.com/questions/11565255/opencv-flann-with-orb-descriptors

			//documentation in opencv 4.5.1 https://docs.opencv.org/4.5.1/db/d18/classcv_1_1flann_1_1GenericIndex.html
			//tips to set up the values https://answers.opencv.org/question/924/lsh-matching-very-slow/

			//LSH is slow for small datasets of descriptors it is better to use hiearchical clustering instead
			//https://stackoverflow.com/questions/23635921/lsh-slower-than-bruteforce-matching

			//it is showing that flannbassedmatcher supports only LshIndexParams and not HierarchicalClusteringIndexParams
			//guy in the comment here thinks the same: https://answers.opencv.org/question/229870/flann-with-hierarchical-clustering-index/
			//he says that it might work only for indexing

			//I have also tested that these values make the best results - more then 1 matches per feature
			matcher = new FlannBasedMatcher(makePtr<flann::LshIndexParams>(12, 20, 2));
		}
		break;
	default:
		throw invalid_argument("Error feature detecting method was used! (non recognized method)");
		break;
	}

	return matcher;
}

void CImagesMatch::printTransformationMatrix(Ptr<CLogger>& logger) const
{
	//transformation matrix returned from findHomography contains doubles
	for (size_t i = 0; i < objectSceneHomography_.rows; ++i) {
		logger->log("(");
		for (size_t j = 0; j < objectSceneHomography_.cols; ++j) {
			if (j < objectSceneHomography_.cols - 1) {
				logger->log(to_string(objectSceneHomography_.at<double>(i, j))).log(", ");
			}
			else {
				logger->log(to_string(objectSceneHomography_.at<double>(i, j))).log(")");
			}
		}
		logger->endl();
	}
}

//=================================================================================================

CImagesMatch::CImagesMatch(const Ptr<CImage>& object, const Ptr<CImage>& scene, CLogger* logger, const SProcessParams & params)
	: objectImage_(object), sceneImage_(scene)
{
	//checking for valid input
	if (object.empty()) {
		throw invalid_argument("Error in matching. Object image pointer is empty!");
	}
	else if (scene.empty()) {
		throw invalid_argument("Error in matching. Scene image pointer is empty!");
	}

	
	FlannBasedMatcher matcher2(new flann::LshIndexParams(20, 10, 2));

	Ptr<DescriptorMatcher> matcher = createMatcher(params);

	//knn matches
	vector<vector<DMatch>> knnMatches;
	matcher->knnMatch(object->getDescriptors(), scene->getDescriptors(), knnMatches, 2);

	//Looping over all the matches and doing some usefull stuff (filtering and others)
	double maxDistance = 0; double minDistance = numeric_limits<double>::max();
	double avarageDistance = 0;
	double avarageWeight = 0;
	double avarageFirstToSecondRatio = 0;
	for (int i = 0; i < knnMatches.size(); i++)
	{
		//the matcher might not find anything
		if (knnMatches[i].size() == 0) {
			continue;
		}

		//-- Quick calculation of max, min, avg distances between keypoints
		++avarageWeight;
		double dist = knnMatches[i][0].distance;
		if (dist < minDistance) minDistance = dist;
		if (dist > maxDistance) maxDistance = dist;
		avarageDistance = ((avarageDistance * avarageWeight) + (dist)) / (1 + avarageWeight);

		//check if there is at least two of found pairs and if not do something (push the match for example)
		//less than two matches might happen for flann based matchers (maybe for others, but I havent experienced it)
		if (knnMatches[i].size() < 2) {
			matches_.push_back(knnMatches[i][0]);
			continue;
		}

		// Lowe's ratio test
		// just filtering some "badly" matched matches, getting only good matches
		//TODO might be a slow solution
		double currentFirstToSecondRatio = knnMatches[i][0].distance / knnMatches[i][1].distance;
		avarageFirstToSecondRatio = ((avarageFirstToSecondRatio * avarageWeight) + (currentFirstToSecondRatio)) / (1 + avarageWeight);
		if (knnMatches[i][0].distance < params.loweRatioTestAlpha_ * knnMatches[i][1].distance) {
			matches_.push_back(knnMatches[i][0]);
		}
	}

	avarageMatchesDistance_ = avarageDistance;
	avarageFirstToSecondRatio_ = avarageFirstToSecondRatio;
	matchedObjectFeaturesRatio_ = (double) matches_.size() / (double) knnMatches.size();

	logger->log("Min distance: ").log(to_string(minDistance)).log(" | Max distance:").log(to_string(maxDistance)).endl();
	logger->log("Average distance:").log(to_string(avarageDistance)).endl();
	logger->log("Average first to second ratio is: ").log(to_string(avarageFirstToSecondRatio_)).endl();
	logger->log("Ratio of filtered matches to number of keypoints of object is: ").log(to_string(matchedObjectFeaturesRatio_)).endl();

}

//=================================================================================================

CImagesMatch::CImagesMatch(CImagesMatch&& right) noexcept
	:
	objectImage_(right.objectImage_),
	sceneImage_(right.sceneImage_)
{
	this->matches_ = move(right.matches_);
	avarageMatchesDistance_ = right.avarageMatchesDistance_;
	matchedObjectFeaturesRatio_ = right.matchedObjectFeaturesRatio_;
	avarageFirstToSecondRatio_ = right.avarageFirstToSecondRatio_;
}

//=================================================================================================

void CImagesMatch::drawPreviewAndResult(const string& runName, Ptr<CLogger>& logger, const SProcessParams& params)
{
	// drawing the results
	Mat imageMatches;
	drawMatches(
		objectImage_->getImage(), objectImage_->getKeypoints(),
		sceneImage_->getImage(), sceneImage_->getKeypoints(),
		matches_, imageMatches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//TODO some speed optimalization can be done here by pushing back these points already in the lowe's ratio test
	//-- Localize the object
	std::vector<Point2d> objectKeypointsCoordinates;
	std::vector<Point2d> sceneKeypointsCoordinates;

	for (int i = 0; i < matches_.size(); i++)
	{
		//-- Get the keypoints from the good matches
		objectKeypointsCoordinates.push_back(objectImage_->getKeypoints()[matches_[i].queryIdx].pt);
		sceneKeypointsCoordinates.push_back(sceneImage_->getKeypoints()[matches_[i].trainIdx].pt);
	}

	objectSceneHomography_ = findHomography(objectKeypointsCoordinates, sceneKeypointsCoordinates, RANSAC);
	transformMatrixComputed_ = true;

	// Get the corners from the image_1 ( the object to be "detected" )
	std::vector<Point2d> obj_corners(4);
	obj_corners[0] = Point2d(0, 0); // left upper
	obj_corners[1] = Point2d(objectImage_->getImage().cols, 0); // right uppper
	obj_corners[2] = Point2d(objectImage_->getImage().cols, objectImage_->getImage().rows); // right bottom 
	obj_corners[3] = Point2d(0, objectImage_->getImage().rows); // left bottom

		//future cornes coordinates
	std::vector<Point2d> scene_corners(4);
	//transformating the cornes
	perspectiveTransform(obj_corners, scene_corners, objectSceneHomography_);

	//-- Draw lines between the corners (the mapped object in the scene - image_2 )
	line(imageMatches, scene_corners[0] + Point2d(objectImage_->getImage().cols, 0),
		scene_corners[1] + Point2d(objectImage_->getImage().cols, 0), Scalar(0, 255, 255), 6);
	line(imageMatches, scene_corners[1] + Point2d(objectImage_->getImage().cols, 0),
		scene_corners[2] + Point2d(objectImage_->getImage().cols, 0), Scalar(0, 255, 255), 6);
	line(imageMatches, scene_corners[2] + Point2d(objectImage_->getImage().cols, 0),
		scene_corners[3] + Point2d(objectImage_->getImage().cols, 0), Scalar(0, 255, 255), 6);
	line(imageMatches, scene_corners[3] + Point2d(objectImage_->getImage().cols, 0),
		scene_corners[0] + Point2d(objectImage_->getImage().cols, 0), Scalar(0, 255, 255), 6);

	//draw the matches and places where the object should be placed in the scene
	logger->putImage(imageMatches, "Matches");

	//TODO zmenit tuhle matici na barevnou
	//Mat sceneImageRenderBuffer(sceneImage_->getImage().rows, sceneImage_->getImage().cols, CV_8UC3, Scalar(0, 0, 0));

	//TODO possible to wirte the warped object already in the scene
	//transform the image by the matrix
	Mat sceneImageRenderBuffer(Size(sceneImage_->getImage().cols, sceneImage_->getImage().rows), CV_8UC3);
	cvtColor(sceneImage_->getImage(), sceneImageRenderBuffer, COLOR_GRAY2RGB);

	//create mask
	Mat mask(objectImage_->getImage().rows, objectImage_->getImage().cols, CV_8U, Scalar(255));
	Mat transformedMask(sceneImage_->getImage().rows, sceneImage_->getImage().cols, CV_8U, Scalar(0));
	warpPerspective(mask, transformedMask, objectSceneHomography_, Size(transformedMask.cols, transformedMask.rows));

	//transform object into scene size image plane
	Mat transformedObject(Size(sceneImage_->getImage().cols, sceneImage_->getImage().rows), CV_8U);
	warpPerspective(objectImage_->getImage(), transformedObject, objectSceneHomography_, Size(transformedObject.cols, transformedObject.rows));

	//redraw the object in the red color and according to mask
	for (int i = 0; i < transformedObject.rows; ++i) {
		for (int j = 0; j < transformedObject.cols; ++j) {
			if (transformedMask.at<unsigned char>(i, j) > 0) {
				sceneImageRenderBuffer.at<Vec3b>(i, j) = Vec3b(60, 60, transformedObject.at<unsigned char>(i, j));
			}
		}
	}
	logger->log("The result transformation matrix is:").endl();
	printTransformationMatrix(logger);
	// draw the object to the scene in the right place
	logger->putImage(sceneImageRenderBuffer, "Image match");

	
	//calculate the real location
	CImageLocator3D imageLocator3D(sceneImage_, params);
	imageLocator3D.calcLocation(obj_corners, scene_corners, logger);
}