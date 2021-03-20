#include "CImagesMatch.h"

CImagesMatch::CImagesMatch(Ptr<CImage>& object, Ptr<CImage>& scene, CLogger* logger, EMatchingMethod method)
	: methodUsed_(method), objectImage_(object), sceneImage_(scene)
{
	if (object.empty()) {
		throw invalid_argument("Error in matching. Object image pointer is empty!");
	}
	else if (scene.empty()) {
		throw invalid_argument("Error in matching. Scene image pointer is empty!");
	}

	Ptr<DescriptorMatcher> matcher;
	switch (method)
	{
	case EMatchingMethod::BRUTE_FORCE:
		matcher = DescriptorMatcher::create("BruteForce");
		break;
	case EMatchingMethod::FLANN_BASED:
		matcher = DescriptorMatcher::create("FlannBased");
		break;
	default:
		throw invalid_argument("Error feature detecting method was used! (non recognized method)");
		break;
	}

	//match the keypoints
	vector<DMatch> allMatches;
	matcher->match(object->getDescriptors(), scene->getDescriptors(), allMatches);


	//-- Quick calculation of max, min, avf distances between keypoints

	double maxDistance = 0; double minDistance = numeric_limits<double>::max();
	double avarageDistance = 0;
	double avarageWeight = 0;
	for (int i = 0; i < object->getDescriptors().rows; i++)
	{
		double dist = allMatches[i].distance;
		if (dist < minDistance) minDistance = dist;
		if (dist > maxDistance) maxDistance = dist;
		avarageDistance = ((avarageDistance * avarageWeight) + (dist)) / (1 + avarageWeight);
	}

	logger->log("Max distance :").log(to_string(maxDistance)).endl();
	logger->log("Min distance :").log(to_string(minDistance)).endl();
	logger->log("Average distance :").log(to_string(avarageDistance)).endl();

	avarageMatchesDistance_ = avarageDistance;


	//-- Get only "good" matches (i.e. whose distance is less than something like (2-3)*minDistance )
	// just filtering some "badly" matched matches
	for (auto& it : allMatches)
	{
		if (it.distance <= (good_matches_min_distance_alpha * minDistance))
		{
			matches_.push_back(it);
		}
	}
}

//=================================================================================================

CImagesMatch::CImagesMatch(CImagesMatch&& right) noexcept
	:
	methodUsed_(right.methodUsed_),
	objectImage_(right.objectImage_),
	sceneImage_(right.sceneImage_)
{
	this->matches_ = move(right.matches_);
	avarageMatchesDistance_ = right.avarageMatchesDistance_;
}

//=================================================================================================

void CImagesMatch::drawPreviewAndResult(const string& runName, CLogger* logger)
{
	// drawing the results
	Mat imageMatches;
	drawMatches(
		objectImage_->getImage(), objectImage_->getKeypoints(),
		sceneImage_->getImage(), sceneImage_->getKeypoints(),
		matches_, imageMatches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//-- Localize the object
	std::vector<Point2f> objectKeypointsCoordinates;
	std::vector<Point2f> sceneKeypointsCoordinates;

	for (int i = 0; i < matches_.size(); i++)
	{
		//-- Get the keypoints from the good matches
		objectKeypointsCoordinates.push_back(objectImage_->getKeypoints()[matches_[i].queryIdx].pt);
		sceneKeypointsCoordinates.push_back(sceneImage_->getKeypoints()[matches_[i].trainIdx].pt);
	}

	//TODO more methods then only ransac - more at https://docs.opencv.org/4.5.1/d9/d0c/group__calib3d.html#ga4abc2ece9fab9398f2e560d53c8c9780

	Mat transformationMatrix = findHomography(objectKeypointsCoordinates, sceneKeypointsCoordinates, RANSAC);

	//-- Get the corners from the image_1 ( the object to be "detected" )
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = Point2f(0, 0); // left upper
	obj_corners[1] = Point2f(objectImage_->getImage().cols, 0); // right uppper
	obj_corners[2] = Point2f(objectImage_->getImage().cols, objectImage_->getImage().rows); // right bottom 
	obj_corners[3] = Point2f(0, objectImage_->getImage().rows); // left bottom

	//future cornes coordinates
	std::vector<Point2f> scene_corners(4);
	//transformating the cornes
	perspectiveTransform(obj_corners, scene_corners, transformationMatrix);

	//-- Draw lines between the corners (the mapped object in the scene - image_2 )
	line(imageMatches, scene_corners[0] + Point2f(objectImage_->getImage().cols, 0),
		scene_corners[1] + Point2f(objectImage_->getImage().cols, 0), Scalar(0, 255, 255), 6);
	line(imageMatches, scene_corners[1] + Point2f(objectImage_->getImage().cols, 0),
		scene_corners[2] + Point2f(objectImage_->getImage().cols, 0), Scalar(0, 255, 255), 6);
	line(imageMatches, scene_corners[2] + Point2f(objectImage_->getImage().cols, 0),
		scene_corners[3] + Point2f(objectImage_->getImage().cols, 0), Scalar(0, 255, 255), 6);
	line(imageMatches, scene_corners[3] + Point2f(objectImage_->getImage().cols, 0),
		scene_corners[0] + Point2f(objectImage_->getImage().cols, 0), Scalar(0, 255, 255), 6);

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
	warpPerspective(mask, transformedMask, transformationMatrix, Size(transformedMask.cols, transformedMask.rows));

	//transform object into scene size image plane
	Mat transformedObject(Size(sceneImage_->getImage().cols, sceneImage_->getImage().rows), CV_8U);
	warpPerspective(objectImage_->getImage(), transformedObject, transformationMatrix, Size(transformedObject.cols, transformedObject.rows));

	//redraw the object in the red color and according to mask
	for (int i = 0; i < transformedObject.rows; ++i) {
		for (int j = 0; j < transformedObject.cols; ++j) {
			if (transformedMask.at<unsigned char>(i, j) > 0) {
				sceneImageRenderBuffer.at<Vec3b>(i, j) = Vec3b(60, 60, transformedObject.at<unsigned char>(i, j));
			}
		}
	}

	// draw the object to the scene in the right place
	logger->putImage(sceneImageRenderBuffer, "Matches");
}