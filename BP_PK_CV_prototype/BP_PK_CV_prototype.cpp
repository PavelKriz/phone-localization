/// main.cpp main file for the project - Pavel Kriz - Recognition and editing of urban scenes(bachelor thesis)

#include <cmath>
#include <iostream>
#include <set>
#include <map>
#include <limits>
#include <utility>
#include <exception>
//timing
#include <chrono>

//opencv includes
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d.hpp>

# include "opencv2/features2d/features2d.hpp"

#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc.hpp"


using namespace cv;
using namespace std;

const float good_matches_min_distance_alpha = 2;

//normal single tests
#define TEST1 1
#define TEST2 2 
#define TEST3 3 
#define TEST4 4
#define TEST5 5
#define TEST6 6

//Border between normal tests and multiple picture tests
#define MULTIPLE_TEST_BOTTOM 1000
//Tests with more images, the alghorithm needst to find the right one
#define MTEST1 1001

//Current used test / tests are changed with preprocessor
#define CURRENT_TEST MTEST1

//timing of the calculation
#define TIMINGx

#ifndef TIMING
#define VIEW_RESULT
#define WRITE_RESULTxx //TODO dont write images until the output is finished
#endif


#define DEBUG

//=================================================================================================
//=================================================================================================


class CImage {
public:
	//methods for detection and description
	enum EProcessMethod {
		SIFT_,
		ORB_
	};
private:
	const string filePath_;
	Mat image_;
	bool wasProcessed_ = false;
	vector<KeyPoint> imageKeypoints_;
	Mat keypointsDescriptors_;
public:
	//throws ios_base::failure exceptions
	CImage(string filePath);
	//TODO move constructor
	
	//computes both keypoints and theirs descriptors, throws invalid argument
	void detectDescribeFeatures(EProcessMethod method);

	const string& getFilePath() { return filePath_; }
	Mat& getImage() { return image_; }
	vector<KeyPoint>& getKeypoints() { return imageKeypoints_; }
	Mat& getDescriptors() { return keypointsDescriptors_; }
};

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

void CImage::detectDescribeFeatures(EProcessMethod method)
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
	cout << "CImage with filepath: " + filePath_ +" keypoints count: " << imageKeypoints_.size() << endl;
	wasProcessed_ = true;
}

//=================================================================================================
//=================================================================================================

class CImagesMatch{
public:
	// methods for matching
	enum EMatchingMethod {
		BRUTE_FORCE,
		FLANN_BASED
	};
private:
	EMatchingMethod methodUsed_;
	Ptr<CImage> objectImage_;
	Ptr<CImage> sceneImage_;
	vector<DMatch> matches_;
	double avarageMatchesDistance_ = numeric_limits<double>::max();


public:
	//throws invalid argument
	CImagesMatch(Ptr<CImage> & object, Ptr<CImage> & scene, EMatchingMethod method = FLANN_BASED);
	CImagesMatch(CImagesMatch&& right);
	void drawPreviewAndResult( string& testName);

	size_t getNumberOfMatches() { return matches_.size(); }
	double getAvarageMatchesDistance() { return avarageMatchesDistance_; }
	Ptr<CImage> getObjectImage() { return objectImage_; }
	Ptr<CImage> getSceneImage() { return sceneImage_; }

};

//=================================================================================================

CImagesMatch::CImagesMatch(Ptr<CImage>& object, Ptr<CImage>& scene, EMatchingMethod method)
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

	double max_dist = 0; double min_dist = numeric_limits<double>::max();
	double avarageDist = 0;
	double avarageWeight = 0;
	for (int i = 0; i < object->getDescriptors().rows; i++)
	{
		//cout << allMatches[i].distance << endl;
		double dist = allMatches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
		avarageDist = ((avarageDist * avarageWeight) + (dist)) / (1 + avarageWeight);
	}

	cout << "-- Max dist :" << max_dist << endl;
	cout << "-- Min dist :" << min_dist << endl;
	cout << "-- Avg dist :" << avarageDist << endl;

	avarageMatchesDistance_ = avarageDist;


	//-- Get only "good" matches (i.e. whose distance is less than something like (2-3)*min_dist )
	// just filtering some "badly" matched matches
	for (auto & it : allMatches)
	{
		if (it.distance <= (good_matches_min_distance_alpha * min_dist))
		{
			matches_.push_back(it);
		}
	}
}

//=================================================================================================

CImagesMatch::CImagesMatch(CImagesMatch&& right)
	:
	methodUsed_(right.methodUsed_),
	objectImage_(right.objectImage_),
	sceneImage_(right.sceneImage_)
{
	this->matches_ = move(right.matches_);
	avarageMatchesDistance_ = right.avarageMatchesDistance_;
}

//=================================================================================================

void CImagesMatch::drawPreviewAndResult( string& testName)
{
	// drawing the results
#ifdef VIEW_RESULT
	namedWindow("matches", 1);
#endif
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
#ifdef VIEW_RESULT
	imshow("matches", imageMatches);
	waitKey(0);
#endif
#ifdef WRITE_RESULT
	string outputNameMatches = "matches_" + testName + ".jpg";
	imwrite(outputNameMatches, imageMatches);
#endif

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
#ifdef VIEW_RESULT
	imshow("result", sceneImageRenderBuffer);
	waitKey(0);
#endif
#ifdef WRITE_RESULT
	string outputNameImageMatch = "image_match_" + testName + ".jpg";
	imwrite(outputNameImageMatch, imgSceneColor);
#endif

}

//=================================================================================================
//=================================================================================================
/*
class CObjectIntoSceneMatcher {
public:
	//methods for detection
	enum EDetectionMethod {
		SIFT_DETECTION_,
		ORB_DETECTION_
	};
	// methods for descriptors extraction 
	enum EDescriptorsExtractionMethod {
		SIFT_DESCRIPTORS_EXTRACTING_,
		ORB_DESCRIPTORS_EXTRACTING_
	};
	// methods for matching
	enum EMatcherType {
		BRUTE_FORCE,
		FLANN_BASED
	};

private:
	const bool debug_;
	vector<KeyPoint> objectKeypoints_;
	vector<KeyPoint> sceneKeypoints_;
	Mat objectDescriptors_;
	Mat sceneDescriptors_;
	vector<DMatch> matches_;
	double avarageMatchesDistance_ = numeric_limits<double>::max();
	void detectDescribeKeypoints(Mat& image, vector<KeyPoint>& keypoints, Mat& descriptors,
		EDetectionMethod detMethod = SIFT_DETECTION_, EDescriptorsExtractionMethod desMethod = SIFT_DESCRIPTORS_EXTRACTING_);


public:
	CObjectIntoSceneMatcher(bool debug = false);
	size_t getNumberOfMatches() { return matches_.size(); }
	double getAvarageMatchesDistance() { return avarageMatchesDistance_; }
	CObjectIntoSceneMatcher& operator =(const CObjectIntoSceneMatcher& second);

	void detectDescribeObjectKeypoints(Mat& image, EDetectionMethod detMethod = SIFT_DETECTION_, EDescriptorsExtractionMethod desMethod = SIFT_DESCRIPTORS_EXTRACTING_);
	void detectDescribeSceneKeypoints(Mat& image, EDetectionMethod detMethod = SIFT_DETECTION_, EDescriptorsExtractionMethod desMethod = SIFT_DESCRIPTORS_EXTRACTING_);
	void match(EMatcherType method);
	void drawPreviewAndResult(Mat& objectImg, Mat& sceneImg, Mat& imgSceneColor, string& testName);
};

//=================================================================================================

CObjectIntoSceneMatcher::CObjectIntoSceneMatcher(bool debug) : debug_(debug)
{
}

//=================================================================================================

CObjectIntoSceneMatcher& CObjectIntoSceneMatcher::operator=(const CObjectIntoSceneMatcher& second)
{
	objectKeypoints_ = second.objectKeypoints_;
	sceneKeypoints_ = second.sceneKeypoints_;
	objectDescriptors_ = second.objectDescriptors_;
	sceneDescriptors_ = second.sceneDescriptors_;
	matches_ = second.matches_;

	return *this;
}

//=================================================================================================

void CObjectIntoSceneMatcher::detectDescribeObjectKeypoints(Mat& image, EDetectionMethod detMethod, EDescriptorsExtractionMethod desMethod)
{
	detectDescribeKeypoints(image, objectKeypoints_, objectDescriptors_, detMethod, desMethod);
#ifdef DEBUG
	cout << "object keypoints count: " << objectKeypoints_.size() << endl;
#endif
}

//=================================================================================================

void CObjectIntoSceneMatcher::detectDescribeSceneKeypoints(Mat& image, EDetectionMethod detMethod, EDescriptorsExtractionMethod desMethod)
{
	detectDescribeKeypoints(image, sceneKeypoints_, sceneDescriptors_, detMethod, desMethod);
#ifdef DEBUG
	cout << "scene keypoints count: " << sceneKeypoints_.size() << endl;
#endif
}

//=================================================================================================

void CObjectIntoSceneMatcher::detectDescribeKeypoints(Mat& image, vector<KeyPoint>& keypoints, Mat& descriptors, EDetectionMethod detMethod, EDescriptorsExtractionMethod desMethod)
{
	Ptr<Feature2D> detector;
	if (detMethod == EDetectionMethod::SIFT_DETECTION_) {
		detector = SIFT::create(500);
	}
	else if (detMethod == EDetectionMethod::ORB_DETECTION_) {
		detector = ORB::create(500);
	}
	else {
		cout << "Error feature detecting method was used!!! ORB is used instead." << endl;
		detector = ORB::create(500);
	}

	detector->detectAndCompute(image, noArray(), keypoints, descriptors);
}


//=================================================================================================

void CObjectIntoSceneMatcher::match(EMatcherType method)
{
	//get the right matcher
	Ptr<DescriptorMatcher> matcher;
	switch (method)
	{
	case CObjectIntoSceneMatcher::BRUTE_FORCE:
		matcher = DescriptorMatcher::create("BruteForce");
		break;
	case CObjectIntoSceneMatcher::FLANN_BASED:
		matcher = DescriptorMatcher::create("FlannBased");
		break;
	default:
		matcher = DescriptorMatcher::create("BruteForce");
		break;
	}

	//match the keypoints
	vector<DMatch> allMatches;
	matcher->match(objectDescriptors_, sceneDescriptors_, allMatches);


	//-- Quick calculation of max, min, avf distances between keypoints

	double max_dist = 0; double min_dist = numeric_limits<double>::max();
	double avarageDist = 0;
	double avarageWeight = 0;
	for (int i = 0; i < objectDescriptors_.rows; i++)
	{
		//cout << allMatches[i].distance << endl;
		double dist = allMatches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
		avarageDist = ((avarageDist * avarageWeight) + (dist)) / (1 + avarageWeight);
	}

	cout << "-- Max dist :" << max_dist << endl;
	cout << "-- Min dist :" << min_dist << endl;
	cout << "-- Avg dist :" << avarageDist << endl;

	avarageMatchesDistance_ = avarageDist;


	//-- Get only "good" matches (i.e. whose distance is less than something like (2-3)*min_dist )
	// just filtering some "badly" matched matches
	for (int i = 0; i < objectDescriptors_.rows; i++)
	{
		if (allMatches[i].distance <= (good_matches_min_distance_alpha * min_dist) + numeric_limits<double>::min())
		{
			matches_.push_back(allMatches[i]);
		}
	}
}

//=================================================================================================

void CObjectIntoSceneMatcher::drawPreviewAndResult(Mat& objectImg, Mat& sceneImg, Mat& imgSceneColor, string& testName)
{
	// drawing the results
#ifdef VIEW_RESULT
	namedWindow("matches", 1);
#endif
	Mat imageMatches;
	//drawMatches(objectImg, objectKeypoints, sceneImg, sceneKeypoints, matches_, imageMatches);
	drawMatches(objectImg, objectKeypoints_, sceneImg, sceneKeypoints_, matches_, imageMatches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//-- Localize the object
	std::vector<Point2f> objectKeypointsCoordinates;
	std::vector<Point2f> sceneKeypointsCoordinates;

	for (int i = 0; i < matches_.size(); i++)
	{
		//-- Get the keypoints from the good matches
		objectKeypointsCoordinates.push_back(objectKeypoints_[matches_[i].queryIdx].pt);
		sceneKeypointsCoordinates.push_back(sceneKeypoints_[matches_[i].trainIdx].pt);
	}

	//TODO more methods then only ransac - more at https://docs.opencv.org/4.5.1/d9/d0c/group__calib3d.html#ga4abc2ece9fab9398f2e560d53c8c9780

	Mat transformationMatrix = findHomography(objectKeypointsCoordinates, sceneKeypointsCoordinates, RANSAC);

	//-- Get the corners from the image_1 ( the object to be "detected" )
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = Point2f(0, 0); // left upper
	obj_corners[1] = Point2f(objectImg.cols, 0); // right uppper
	obj_corners[2] = Point2f(objectImg.cols, objectImg.rows); // right bottom 
	obj_corners[3] = Point2f(0, objectImg.rows); // left bottom

	//future cornes coordinates
	std::vector<Point2f> scene_corners(4);
	//transformating the cornes
	perspectiveTransform(obj_corners, scene_corners, transformationMatrix);

	//-- Draw lines between the corners (the mapped object in the scene - image_2 )
	line(imageMatches, scene_corners[0] + Point2f(objectImg.cols, 0), scene_corners[1] + Point2f(objectImg.cols, 0), Scalar(0, 255, 255), 6);
	line(imageMatches, scene_corners[1] + Point2f(objectImg.cols, 0), scene_corners[2] + Point2f(objectImg.cols, 0), Scalar(0, 255, 255), 6);
	line(imageMatches, scene_corners[2] + Point2f(objectImg.cols, 0), scene_corners[3] + Point2f(objectImg.cols, 0), Scalar(0, 255, 255), 6);
	line(imageMatches, scene_corners[3] + Point2f(objectImg.cols, 0), scene_corners[0] + Point2f(objectImg.cols, 0), Scalar(0, 255, 255), 6);

	//draw the matches and places where the object should be placed in the scene
#ifdef VIEW_RESULT
	imshow("matches", imageMatches);
	waitKey(0);
#endif
#ifdef WRITE_RESULT
	string outputNameMatches = "matches_" + testName + ".jpg";
	imwrite(outputNameMatches, imageMatches);
#endif


	//transform the image by the matrix
	Mat transformedObject(Size(sceneImg.cols, sceneImg.rows), CV_8U);
	warpPerspective(objectImg, transformedObject, transformationMatrix, Size(transformedObject.cols, transformedObject.rows));

	// draw the object in the scene in red color
	for (int i = 0; i < transformedObject.rows; ++i) {
		for (int j = 0; j < transformedObject.cols; ++j) {
			if (transformedObject.at<unsigned char>(i, j) != 0) {
				uchar pixel = transformedObject.at<unsigned char>(i, j);
				imgSceneColor.at<Vec3b>(i, j) = Vec3b(60, 60, pixel);
			}
		}
	}

	// draw the object to the scene in the right place
#ifdef VIEW_RESULT
	imshow("matches", imgSceneColor);
	waitKey(0);
#endif
#ifdef WRITE_RESULT
	string outputNameImageMatch = "image_match_" + testName + ".jpg";
	imwrite(outputNameImageMatch, imgSceneColor);
#endif

}
*/

//=================================================================================================
//=================================================================================================

//class that searches trough scenes and it is searching where is the object
class CObjectInScenesEngine {
	Ptr<CImage> objectImage_;
	vector<Ptr<CImage>> sceneImages_;
	vector<CImagesMatch> matches_;
	size_t bestMatchIndex_;
	bool bestMatchExist_ = false;
public:
	//can throw ios_base::failure exception
	CObjectInScenesEngine(string& objectFilePath, vector<string>& sceneFilePaths);
	//can throw invalid_argument
	int run(CImage::EProcessMethod method);
	//throws exception logic_error
	void viewBestResult( string& testName);
};

//=================================================================================================

CObjectInScenesEngine::CObjectInScenesEngine(string& objectFilePath, vector<string>& sceneFilePaths)
{
		objectImage_ = new CImage(objectFilePath);
		for (auto& it : sceneFilePaths) {
			sceneImages_.push_back(new CImage(it));
		}
}

//=================================================================================================

int CObjectInScenesEngine::run(CImage::EProcessMethod method)
{
	//prepare the object
	objectImage_->detectDescribeFeatures(method);

	//prepare the scenes
	for (auto & ptr : sceneImages_) {
		ptr->detectDescribeFeatures(method);
	}

	//searching for the scene object matching combination with lowest avarage distance of matches
	double lowestDistance = numeric_limits<double>::max();
	int lowestDistanceIndex = 0;
	for (int i = 0; i < sceneImages_.size(); ++i) {
		//computing the keypoints, descriptors, matches
		//move construction
		matches_.emplace_back( CImagesMatch(objectImage_, sceneImages_[i], CImagesMatch::EMatchingMethod::BRUTE_FORCE));

		//checking if the match is possible to be the best until now
		double currentDist = matches_.back().getAvarageMatchesDistance();
		if (currentDist < lowestDistance) {
			lowestDistanceIndex = i;
			lowestDistance = currentDist;
		}
		bestMatchExist_ = true;
		cout << "Compare index:" << i << " | avarage distance: " << currentDist << endl;
	}

	bestMatchIndex_ = lowestDistanceIndex;

	return lowestDistanceIndex;
}

//=================================================================================================

void CObjectInScenesEngine::viewBestResult(string& testName)
{
	if (bestMatchExist_) {
		matches_[bestMatchIndex_].drawPreviewAndResult( testName);
	}
	else {
		throw logic_error("View of matches was called without computing matches first");
	}
	
}

//=================================================================================================


//=================================================================================================
//=================================================================================================

/*
//class that searches trough scenes and it is searching where is the object
class CObjectInSceneSearcher {
	Mat objectImage_;
	CObjectIntoSceneMatcher bestMatch_;
	bool bestMatchExist_ = false;
public:
	CObjectInSceneSearcher(Mat& objectImages);
	int compare(vector<Mat>& sceneImages);
	void viewBestResult(Mat& objectImg, Mat& sceneImg, Mat& imgSceneColor, string& testName);
};

//=================================================================================================

CObjectInSceneSearcher::CObjectInSceneSearcher(Mat& objectImage) : objectImage_(objectImage)
{
}

//=================================================================================================

int CObjectInSceneSearcher::compare(vector<Mat>& sceneImages)
{
	//future vector of the matchers
	vector<CObjectIntoSceneMatcher> objectIntoSceneMatchers;

	//prepare the object with the matcher
	CObjectIntoSceneMatcher objectIntoSceneMatcher(true);
	objectIntoSceneMatcher.detectDescribeObjectKeypoints(objectImage_, CObjectIntoSceneMatcher::ORB_DETECTION_, CObjectIntoSceneMatcher::ORB_DESCRIPTORS_EXTRACTING_);

	//searching for the scene object matching combination with lowest avarage distance of matches
	double lowestDistance = numeric_limits<double>::max();
	int lowestDistanceIndex = 0;
	for (int i = 0; i < sceneImages.size(); ++i) {
		//computing the keypoints, descriptors, matches
		objectIntoSceneMatchers.push_back(objectIntoSceneMatcher);
		objectIntoSceneMatchers.back().detectDescribeSceneKeypoints(sceneImages[i], CObjectIntoSceneMatcher::ORB_DETECTION_, CObjectIntoSceneMatcher::ORB_DESCRIPTORS_EXTRACTING_);
		objectIntoSceneMatchers.back().match(CObjectIntoSceneMatcher::BRUTE_FORCE);

		//checking if the match is possible to be the best until now
		int tmpDist = objectIntoSceneMatchers.back().getAvarageMatchesDistance();
		if (tmpDist < lowestDistance) {
			lowestDistanceIndex = i;
			lowestDistance = tmpDist;
		}
		bestMatchExist_ = true;
		cout << "Compare index:" << i << " | avarage distance: " << tmpDist << endl;
	}

	bestMatch_ = objectIntoSceneMatchers[lowestDistanceIndex];
	return lowestDistanceIndex;
}


//=================================================================================================

void CObjectInSceneSearcher::viewBestResult(Mat& objectImg, Mat& sceneImg, Mat& imgSceneColor, string& testName)
{
	bestMatch_.drawPreviewAndResult(objectImg, sceneImg, imgSceneColor, testName);
}
*/
//=================================================================================================

//method to find object in scene
/*
int run(string& objectFilePath, string& sceneFilePath, string& testName) {

	//init matcher
	CObjectIntoSceneMatcher objectIntoSceneMatcher(true);

	//loading images
	Mat sceneImg = imread(sceneFilePath, CV_8U);
	Mat objectImg = imread(objectFilePath, CV_8U);

	if (sceneImg.empty() || objectImg.empty())
	{
		cout << "Can't read one of the images\n";
		return -1;
	}

	//detecting and describing keypoints
	objectIntoSceneMatcher.detectDescribeObjectKeypoints(objectImg, CObjectIntoSceneMatcher::ORB_DETECTION_, CObjectIntoSceneMatcher::ORB_DESCRIPTORS_EXTRACTING_);
	objectIntoSceneMatcher.detectDescribeSceneKeypoints(sceneImg, CObjectIntoSceneMatcher::ORB_DETECTION_, CObjectIntoSceneMatcher::ORB_DESCRIPTORS_EXTRACTING_);

	// matching descriptors
	objectIntoSceneMatcher.match(CObjectIntoSceneMatcher::BRUTE_FORCE);

	//loading scene in colors to display the object in it
	Mat imgSceneColor = imread(sceneFilePath, IMREAD_COLOR);

	if (imgSceneColor.empty())
	{
		cout << "Can't read dumRottScene2\n";
		return -1;
	}

	objectIntoSceneMatcher.drawPreviewAndResult(objectImg, sceneImg, imgSceneColor, testName);

	return 1;
}
*/
//=================================================================================================

/*
//function for finding object in multiple scene, result is the best one match
int comparisonRun(string& objectFilePath, vector<string>& sceneFilePaths, string& testName) {

	//loading image of the object
	Mat objectImg = imread(objectFilePath, CV_8U);
	if (objectImg.empty())
	{
		cout << ("Can't read one of the images with filepath: " + objectFilePath) << endl;
		return -1;
	}

	//loading images of the scenes
	vector<Mat> sceneImages;
	for (int i = 0; i < sceneFilePaths.size(); ++i) {
		sceneImages.push_back(imread(sceneFilePaths[i], CV_8U));
		if (sceneImages.back().empty()) {
			cout << ("Can't read one of the images with filepath: " + sceneFilePaths[i]) << endl;
			return -1;
		}
	}



	CObjectInSceneSearcher searcher(objectImg);
	int index = searcher.compare(sceneImages);

	//loading scene in colors to display the object in it
	Mat imgSelectedSceneColor = imread(sceneFilePaths[index], IMREAD_COLOR);
	if (imgSelectedSceneColor.empty())
	{
		cout << "Can't read dumRottScene2\n";
		return -1;
	}

	searcher.viewBestResult(objectImg, sceneImages[index], imgSelectedSceneColor, testName);

	return 1;
}
*/


//function for finding object in multiple scene, result is the best one match
int comparisonRun(string& objectFilePath, vector<string>& sceneFilePaths, string& testName) {

	try {
		CObjectInScenesEngine engine(objectFilePath, sceneFilePaths);
		engine.run(CImage::EProcessMethod::SIFT_);
		engine.viewBestResult(testName);
	}
	catch(ios_base::failure e){
		cout << e.what() << endl;
		return -1;
	}
	catch(invalid_argument e) {
		cout << e.what() << endl;
		return -1;
	}
	catch (logic_error e) {
		cout << e.what() << endl;
		return -1;
	}

	return 1;
}

//=================================================================================================

int main(int argc, char** argv)
{
	cout << "OpenCV version : " << CV_VERSION << endl << endl;

	chrono::steady_clock::time_point begin = chrono::steady_clock::now();

	const string imagesRoot = "input_images\\";
	const string dumRottRoot = imagesRoot + "dum_rott\\";

	//inits for each tests
#if CURRENT_TEST == TEST1
	string objectFilePath = "pavel.png";
	vector<string> sceneFilePaths = "pavelOnPalette.png";
	string testName = "test1";
#elif CURRENT_TEST == TEST2
	string objectFilePath = dumRottRoot + "dumRottRef.png";
	vector<string> sceneFilePaths = dumRottRoot + "dumRottScene.jpg";
	string testName = "test2";
#elif CURRENT_TEST == TEST3
	string objectFilePath = dumRottRoot + "dumRottRef.png";
	vector<string> sceneFilePaths = dumRottRoot + "dumRottScene2.jpg";
	string testName = "test3";
#elif CURRENT_TEST == TEST4
	string objectFilePath = dumRottRoot + "dumRottRef.png";
	vector<string> sceneFilePaths = dumRottRoot + "dumRottScene3.jpg";
	string testName = "test4";
#elif CURRENT_TEST == TEST5
	string objectFilePath = dumRottRoot + "dumRottRef.png";
	vector<string> sceneFilePaths = dumRottRoot + "dumRottScen4.jpg";
	string testName = "test5";
#elif CURRENT_TEST == TEST6
	string objectFilePath = dumRottRoot + "dumRottRef.png";
	vector<string> sceneFilePaths = { dumRottRoot + "dumRottRef.png" };
	string testName = "test6";
#elif CURRENT_TEST == MTEST1
	string objectFilePath = dumRottRoot + "dumRottRef.png";
	vector<string> sceneFilePaths = { dumRottRoot + "dumRottScene2.jpg", imagesRoot + "white_house\\whiteHouse1.jpg", imagesRoot + "pavel\\pavel.png" };
	string testName = "mtest1";
#endif


	//for different type of test is there different function
	int retValue = 0;
#if CURRENT_TEST < MULTIPLE_TEST_BOTTOM
	if ((retValue = comparisonRun(objectFilePath, sceneFilePath, testName)) != 1) {
		return retValue;
	}
#else
	if ((retValue = comparisonRun(objectFilePath, sceneFilePaths, testName)) != 1) {
		return retValue;
	}
#endif

	chrono::steady_clock::time_point end = chrono::steady_clock::now();
	cout << "The whole process took = " << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

	return 0;
}
