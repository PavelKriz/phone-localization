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
#define TIMINGxx

#ifndef TIMING
#define VIEW_RESULT
#define WRITE_RESULTxx //TODO dont write images until the output is finished
#endif


#define DEBUG

//=================================================================================================
//=================================================================================================

class CLogger{
public:
	virtual ~CLogger() {}
	virtual CLogger& logSection(const string& name, unsigned int level = 0) = 0;
	virtual CLogger& log(const string & toLog) = 0;
	virtual CLogger& endl() = 0;
	virtual ostream& getStream() = 0;
};

//=================================================================================================
//=================================================================================================

class CTermLogger : public CLogger{
public:
	~CTermLogger() override {}
	virtual CLogger& logSection(const string& name, unsigned int level = 0) override {
		//make the name in upper case
		string upperName = name;
		for (auto& c : upperName)
			c = toupper(c);

		if (level > 2)
			level = 2;

		cout << std::endl;

		for (int i = -1; i < (int) level; ++i)
			cout << "==";

		cout << upperName;

		for (int i = -1; i < (int) level; ++i)
			cout << "==";

		std::cout << std::endl;

		return *this;
	}
	virtual CLogger& log(const string& toLog) override {
		cout << toLog;
		return *this;
	}
	virtual CLogger& endl() override {
		std::cout << std::endl;
		return *this;
	}

	virtual ostream& getStream() override { return cout; }
};


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
	void detectDescribeFeatures(EProcessMethod method, CLogger * logger);

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

void CImage::detectDescribeFeatures(EProcessMethod method, CLogger * logger)
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
	logger->getStream() << "CImage with filepath: " + filePath_ + " keypoints count: " << imageKeypoints_.size() << endl;
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
	CImagesMatch(Ptr<CImage> & object, Ptr<CImage> & scene, CLogger* logger, EMatchingMethod method = FLANN_BASED);
	CImagesMatch(CImagesMatch&& right) noexcept;
	void drawPreviewAndResult( string& runName, CLogger* logger);

	size_t getNumberOfMatches() { return matches_.size(); }
	double getAvarageMatchesDistance() { return avarageMatchesDistance_; }
	Ptr<CImage> getObjectImage() { return objectImage_; }
	Ptr<CImage> getSceneImage() { return sceneImage_; }

};

//=================================================================================================

CImagesMatch::CImagesMatch(Ptr<CImage>& object, Ptr<CImage>& scene, CLogger * logger, EMatchingMethod method)
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
		double dist = allMatches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
		avarageDist = ((avarageDist * avarageWeight) + (dist)) / (1 + avarageWeight);
	}

	logger->getStream() << "-- Max dist :" << max_dist << endl;
	logger->getStream() << "-- Min dist :" << min_dist << endl;
	logger->getStream() << "-- Avg dist :" << avarageDist << endl;

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

void CImagesMatch::drawPreviewAndResult( string& runName, CLogger* logger)
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
	string outputNameMatches = "matches_" + runName + ".jpg";
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
	string outputNameImageMatch = "image_match_" + runName + ".jpg";
	imwrite(outputNameImageMatch, imgSceneColor);
#endif

}

//=================================================================================================
//=================================================================================================

//class that searches trough scenes and it is searching where is the object
class CObjectInScenesEngine {
	CLogger* logger_;
	Ptr<CImage> objectImage_;
	vector<Ptr<CImage>> sceneImages_;
	vector<CImagesMatch> matches_;
	size_t bestMatchIndex_;
	bool bestMatchExist_ = false;
public:
	//can throw ios_base::failure exception
	CObjectInScenesEngine(const string & runName, const string& objectFilePath, const vector<string>& sceneFilePaths);
	~CObjectInScenesEngine() { delete logger_; }
	//can throw invalid_argument
	int run(CImage::EProcessMethod method);
	//throws exception logic_error
	void viewBestResult( string& runName);
};

//=================================================================================================

CObjectInScenesEngine::CObjectInScenesEngine(const string& runName, const string& objectFilePath, const vector<string>& sceneFilePaths)
{
	logger_ = new CTermLogger();
	logger_->logSection("Run: " + runName, 0);
	objectImage_ = new CImage(objectFilePath);
	for (auto& it : sceneFilePaths) {
		sceneImages_.push_back(new CImage(it));
	}
	logger_->log("images loaded").endl();
}

//=================================================================================================

int CObjectInScenesEngine::run(CImage::EProcessMethod method)
{
	logger_->logSection("detectig and describing features", 1);
	logger_->logSection("object", 2);
	//prepare the object
	objectImage_->detectDescribeFeatures(method, logger_);

	//prepare the scenes
	logger_->logSection("scenes", 2);
	for (auto & ptr : sceneImages_) {
		ptr->detectDescribeFeatures(method, logger_);
	}

	logger_->logSection("matching", 1);
	//searching for the scene object matching combination with lowest avarage distance of matches
	double lowestDistance = numeric_limits<double>::max();
	int lowestDistanceIndex = 0;
	//that many matches will be created
	matches_.reserve(sceneImages_.size());
	for (int i = 0; i < sceneImages_.size(); ++i) {
		//computing the keypoints, descriptors, matches
		//move construction
		logger_->getStream() << endl << "Matching object with scene with filepath: " << sceneImages_[i]->getFilePath() << endl;
		matches_.emplace_back(CImagesMatch(objectImage_, sceneImages_[i], logger_, CImagesMatch::EMatchingMethod::BRUTE_FORCE));

		//checking if the match is possible to be the best until now
		double currentDist = matches_.back().getAvarageMatchesDistance();
		if (currentDist < lowestDistance) {
			lowestDistanceIndex = i;
			lowestDistance = currentDist;
		}
		bestMatchExist_ = true;
		logger_->getStream() << "Compare index:" << i << " | avarage distance: " << currentDist << endl;
	}

	bestMatchIndex_ = lowestDistanceIndex;
	logger_->logSection("result", 2);
	logger_->getStream() << "Best scene match for object is scene with filepath: " << sceneImages_[bestMatchIndex_]->getFilePath() << endl;

	return lowestDistanceIndex;
}

//=================================================================================================

void CObjectInScenesEngine::viewBestResult(string& runName)
{
	if (bestMatchExist_) {
		matches_[bestMatchIndex_].drawPreviewAndResult( runName, logger_);
	}
	else {
		throw logic_error("View of matches was called without computing matches first");
	}
	
}

//=================================================================================================

//=================================================================================================
//=================================================================================================

//function for finding object in multiple scene, result is the best one match
int comparisonRun(string& objectFilePath, vector<string>& sceneFilePaths, string& runName) {

	try {
		CObjectInScenesEngine engine(runName, objectFilePath, sceneFilePaths);
		engine.run(CImage::EProcessMethod::SIFT_);
		engine.viewBestResult(runName);
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
	vector<string> sceneFilePaths = { "pavelOnPalette.png" };
	string runName = "test1";
#elif CURRENT_TEST == TEST2
	string objectFilePath = dumRottRoot + "dumRottRef.png";
	vector<string> sceneFilePaths = { dumRottRoot + "dumRottScene.jpg" };
	string runName = "test2";
#elif CURRENT_TEST == TEST3
	string objectFilePath = dumRottRoot + "dumRottRef.png";
	vector<string> sceneFilePaths = { dumRottRoot + "dumRottScene2.jpg" };
	string runName = "test3";
#elif CURRENT_TEST == TEST4
	string objectFilePath = dumRottRoot + "dumRottRef.png";
	vector<string> sceneFilePaths = { dumRottRoot + "dumRottScene3.jpg" };
	string runName = "test4";
#elif CURRENT_TEST == TEST5
	string objectFilePath = dumRottRoot + "dumRottRef.png";
	vector<string> sceneFilePaths = { dumRottRoot + "dumRottScen4.jpg" };
	string runName = "test5";
#elif CURRENT_TEST == TEST6
	string objectFilePath = dumRottRoot + "dumRottRef.png";
	vector<string> sceneFilePaths = { dumRottRoot + "dumRottRef.png" };
	string runName = "test6";
#elif CURRENT_TEST == MTEST1
	string objectFilePath = dumRottRoot + "dumRottRef.png";
	vector<string> sceneFilePaths = { dumRottRoot + "dumRottScene2.jpg", imagesRoot + "white_house\\whiteHouse1.jpg", imagesRoot + "pavel\\pavel.png" };
	string runName = "mtest1";
#endif


	//for different type of test is there different function
	int retValue = 0;
#if CURRENT_TEST < MULTIPLE_TEST_BOTTOM
	if ((retValue = comparisonRun(objectFilePath, sceneFilePath, runName)) != 1) {
		return retValue;
	}
#else
	if ((retValue = comparisonRun(objectFilePath, sceneFilePaths, runName)) != 1) {
		return retValue;
	}
#endif

	chrono::steady_clock::time_point end = chrono::steady_clock::now();
	cout << "The whole process took = " << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

	return 0;
}
