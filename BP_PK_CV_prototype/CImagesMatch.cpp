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
	for (size_t i = 0; i < transformationMatrix_.rows; ++i) {
		logger->log("(");
		for (size_t j = 0; j < transformationMatrix_.cols; ++j) {
			if (j < transformationMatrix_.cols - 1) {
				logger->log(to_string(transformationMatrix_.at<double>(i, j))).log(", ");
			}
			else {
				logger->log(to_string(transformationMatrix_.at<double>(i, j))).log(")");
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

void CImagesMatch::calcLocation(vector<Point2f>& obj_corners, std::vector<Point3f>& obj_corners3D, vector<Point2f>& sceneCorners, Ptr<CLogger>& logger)
{
	//major parts of FOLLOWING CODE was taken from OpenCV tutorial about the OpenCV Calib3D module https://docs.opencv.org/master/dc/d2c/tutorial_real_time_pose.html
	//TUTORIAL INSPIRED/OVERTAKEN CODE PART BEGIN=====================================================================
	std::cout << "INPUT 3D Coordinates" << std::endl;
	for (size_t i = 0; i < 4; ++i) {
		std::cout << obj_corners3D[i] << std::endl;
	}

	//future cornes coordinates
	std::vector<Point2f> scene_corners(4);
	//transformating the cornes
	perspectiveTransform(obj_corners, scene_corners, transformationMatrix_);

	//SOME RANDOM WEBCAM - hope for the best - here just for the testing
	double f = 4;                           // focal length in mm
	double sx = 4.96, sy = 3.79;             // sensor size  - 4.96 x 3.72 mm) 	
	double width = sceneImage_->getImage().cols;		// image size
	double height = sceneImage_->getImage().rows;        // image size
	double fx = width * f / sx;   // fx
	double fy = height * f / sy;  // fy
	double cx = width / 2;      // cx
	double cy = height / 2;    // cy


	Mat A_matrix_ = Mat::zeros(3, 3, CV_64FC1); // intrinsic camera parameters
	Mat R_matrix_ = Mat::zeros(3, 3, CV_64FC1); // rotation matrix
	Mat t_matrix_ = Mat::zeros(3, 1, CV_64FC1); // translation matrix
	Mat P_matrix_ = Mat::zeros(4, 4, CV_64FC1); // rotation-translation matrix

	A_matrix_.at<double>(0, 0) = fx;       //      [ fx   0  cx ]
	A_matrix_.at<double>(1, 1) = fy;       //      [  0  fy  cy ]
	A_matrix_.at<double>(0, 2) = cx;       //      [  0   0   1 ]
	A_matrix_.at<double>(1, 2) = cy;
	A_matrix_.at<double>(2, 2) = 1;

	cv::Mat distCoeffs = cv::Mat::zeros(4, 1, CV_64FC1);    // vector of distortion coefficients - setting to zero distortion
	cv::Mat rvec = cv::Mat::zeros(3, 1, CV_64FC1);          // output rotation vector
	cv::Mat tvec = cv::Mat::zeros(3, 1, CV_64FC1);          // output translation vector

	bool useExtrinsicGuess = false;   // if true the function uses the provided rvec and tvec values as
								  // initial approximations of the rotation and translation vectors

	solvePnP(obj_corners3D, scene_corners, A_matrix_, distCoeffs, rvec, tvec, useExtrinsicGuess); //because the method wasnt specified the iterative method will take place

	//converting rotation vector to the rotation matrix
	Rodrigues(rvec, R_matrix_);

	Quatd rotQuat = Quatd::createFromRotMat(R_matrix_);

	//TODO P_matrix
	//copy rotation
	P_matrix_.at<double>(0, 0) = R_matrix_.at<double>(0, 0);
	P_matrix_.at<double>(0, 1) = R_matrix_.at<double>(0, 1);
	P_matrix_.at<double>(0, 2) = R_matrix_.at<double>(0, 2);
	P_matrix_.at<double>(1, 0) = R_matrix_.at<double>(1, 0);
	P_matrix_.at<double>(1, 1) = R_matrix_.at<double>(1, 1);
	P_matrix_.at<double>(1, 2) = R_matrix_.at<double>(1, 2);
	P_matrix_.at<double>(2, 0) = R_matrix_.at<double>(2, 0);
	P_matrix_.at<double>(2, 1) = R_matrix_.at<double>(2, 1);
	P_matrix_.at<double>(2, 2) = R_matrix_.at<double>(2, 2);

	//copy translation
	P_matrix_.at<double>(0, 3) = tvec.at<double>(0);
	P_matrix_.at<double>(1, 3) = tvec.at<double>(1);
	P_matrix_.at<double>(2, 3) = tvec.at<double>(2);

	//bottom row
	P_matrix_.at<double>(3, 3) = 1.0;

	std::cout << "RVEC: " << rvec << std::endl;
	std::cout << "TVEC: " << tvec << std::endl;
	std::cout << "P_Matrix: " << P_matrix_ << std::endl;

	Mat objCorner3D_4_0 = Mat::zeros(4, 1, CV_64FC1);
	objCorner3D_4_0.at<double>(0) = obj_corners3D[0].x;
	objCorner3D_4_0.at<double>(1) = obj_corners3D[0].y;
	objCorner3D_4_0.at<double>(2) = obj_corners3D[0].z;
	objCorner3D_4_0.at<double>(3) = 1;
	Mat objCorner3D_4_1 = Mat::zeros(4, 1, CV_64FC1);
	objCorner3D_4_1.at<double>(0) = obj_corners3D[1].x;
	objCorner3D_4_1.at<double>(1) = obj_corners3D[1].y;
	objCorner3D_4_1.at<double>(2) = obj_corners3D[1].z;
	objCorner3D_4_1.at<double>(3) = 1;
	Mat objCorner3D_4_2 = Mat::zeros(4, 1, CV_64FC1);
	objCorner3D_4_2.at<double>(0) = obj_corners3D[2].x;
	objCorner3D_4_2.at<double>(1) = obj_corners3D[2].y;
	objCorner3D_4_2.at<double>(2) = obj_corners3D[2].z;
	objCorner3D_4_2.at<double>(3) = 1;
	Mat objCorner3D_4_3 = Mat::zeros(4, 1, CV_64FC1);
	objCorner3D_4_3.at<double>(0) = obj_corners3D[3].x;
	objCorner3D_4_3.at<double>(1) = obj_corners3D[3].y;
	objCorner3D_4_3.at<double>(2) = obj_corners3D[3].z;
	objCorner3D_4_3.at<double>(3) = 1;

	//get the corners in camera space
	std::vector<Mat> objCameraSPaceCorners(4);
	objCameraSPaceCorners[0] = P_matrix_ * objCorner3D_4_0;
	objCameraSPaceCorners[1] = P_matrix_ * objCorner3D_4_1;
	objCameraSPaceCorners[2] = P_matrix_ * objCorner3D_4_2;
	objCameraSPaceCorners[3] = P_matrix_ * objCorner3D_4_3;

	//PRINT THE RESULT I GOT

	for (size_t i = 0; i < objCameraSPaceCorners.size(); ++i) {
		logger->log("Point ").log(to_string(i)).log(" := (");
		logger->log(to_string(objCameraSPaceCorners[i].at<double>(0))).log(" ,");
		logger->log(to_string(objCameraSPaceCorners[i].at<double>(1))).log(" ,");
		logger->log(to_string(objCameraSPaceCorners[i].at<double>(2))).log(" ,");
		logger->log(to_string(objCameraSPaceCorners[i].at<double>(3))).log(" )").endl();
	}



	Mat takeFirstThree = Mat::zeros(3, 4, CV_64FC1);
	takeFirstThree.at<double>(0, 0) = 1.0;
	takeFirstThree.at<double>(1, 1) = 1.0;
	takeFirstThree.at<double>(2, 2) = 1.0;

	std::vector<Mat> checkImageSpaceCorners(4);
	for (size_t i = 0; i < 4; ++i) {
		checkImageSpaceCorners[i] = A_matrix_ * takeFirstThree * objCameraSPaceCorners[i];
	}
	std::vector<Point2f> checkImageSpaceCornersNorm(4);
	for (size_t i = 0; i < 4; ++i) {
		checkImageSpaceCornersNorm[i].x = checkImageSpaceCorners[i].at<double>(0) / checkImageSpaceCorners[i].at<double>(2);
		checkImageSpaceCornersNorm[i].y = checkImageSpaceCorners[i].at<double>(1) / checkImageSpaceCorners[i].at<double>(2);
	}

	std::cout << "Check space corners" << std::endl;
	for (size_t i = 0; i < 4; ++i) {
		std::cout << checkImageSpaceCornersNorm[i] << std::endl;
	}
	std::cout << "Scene corners" << std::endl;
	for (size_t i = 0; i < 4; ++i) {
		std::cout << scene_corners[i] << std::endl;
	}

	//TODO consider the height of the camera (it might change the location a tiny bit) - camera is at point (0,0,0,1) - origin
	Mat vecTwo = objCameraSPaceCorners[2](Range(0, 3), Range(0, 1));
	Mat normVecTwo;
	normalize(vecTwo, normVecTwo);

	Mat vecThree = objCameraSPaceCorners[3](Range(0, 3), Range(0, 1));
	Mat normVecThree;
	normalize(vecThree, normVecThree);

	cout << "normVecTwo:" << endl << normVecTwo << endl;
	cout << "normVecThree:" << endl << normVecThree << endl;

	//normVecThree stays as a updated z coordinate
	Mat vecUp = normVecThree.cross(normVecTwo); //vecUp is updated y coordinate
	normalize(vecUp, vecUp);
	Mat right = vecUp.cross(normVecThree); //right is updated x coordinate
	normalize(right, right);

	Mat changeBasis = Mat::zeros(4, 4, CV_64FC1);
	//x basis vector
	changeBasis.at<double>(0, 0) = right.at<double>(0);
	changeBasis.at<double>(1, 0) = right.at<double>(1);
	changeBasis.at<double>(2, 0) = right.at<double>(2);
	//y basis vector
	changeBasis.at<double>(0, 1) = vecUp.at<double>(0);
	changeBasis.at<double>(1, 1) = vecUp.at<double>(1);
	changeBasis.at<double>(2, 1) = vecUp.at<double>(2);
	//z basis vector
	changeBasis.at<double>(0, 2) = normVecThree.at<double>(0);
	changeBasis.at<double>(1, 2) = normVecThree.at<double>(1);
	changeBasis.at<double>(2, 2) = normVecThree.at<double>(2);
	//right bottom one
	changeBasis.at<double>(3, 3) = 1;

	//our coordinate system
	Mat pointTwoOnPlane = changeBasis.inv() * objCameraSPaceCorners[2];
	Mat pointThreeOnPlane = changeBasis.inv() * objCameraSPaceCorners[3];
	Point2d pointTwo(pointTwoOnPlane.at<double>(0), pointTwoOnPlane.at<double>(2)); // (x,y,z, w) -> (x, z) and renaming to (x, y)
	Mat pointTwoVec(pointTwo);
	Point2d pointThree(pointThreeOnPlane.at<double>(0), pointThreeOnPlane.at<double>(2));
	Mat pointThreeVec(pointThree);

	cout << "changeBasis" << endl << changeBasis << endl;
	cout << "New basis point two" << endl << pointTwoOnPlane << endl;
	cout << "New basis point three" << endl << pointThreeOnPlane << endl;

	//gcs (Geographic coordinate system) coordinate system
	//first longtitude (x) and then lattitude(y)
	//50.0867628N, 14.4193081E
	Point2d gcsTwoPoint(14.4193081, 50.0867628);
	Mat gcsTwoVec(gcsTwoPoint);
	//third point 50.0865958N, 14.4192706E 
	Point2d gcsThreePoint(14.4192706, 50.0865958);
	Mat gcsThreeVec(gcsThreePoint);

	//Length in meters of 1° of latitude = at 45 degrees is 111.141548 km
	//Length in meters of 1° of longitude = 40075017 m * cos(latitude) / 360
	double metersInLatDeg = 111141.548;
	double metersInLongDeg = 40075017 * cos(gcsTwoPoint.y) / 360;
	double longtitudeAdjustFactor = metersInLatDeg / metersInLongDeg;

	//checking the calculation
	cout << "meters in Lat deg = " << metersInLatDeg << " : meters in adjusted Long deg = " << metersInLongDeg * longtitudeAdjustFactor << endl;

	//adjust the points to make same meassures in both dirrections
	gcsTwoPoint.x *= longtitudeAdjustFactor;
	gcsTwoVec.at<double>(1) *= longtitudeAdjustFactor;
	gcsThreePoint.x *= longtitudeAdjustFactor;
	gcsThreeVec.at<double>(1) *= longtitudeAdjustFactor;

	//angle of vector twoToThree to the vector pointing to east
	Mat gcsDiff = gcsTwoVec - gcsThreeVec;
	normalize(gcsDiff, gcsDiff);
	Mat eastVector = (Mat_<double>(2, 1) << 1.0, 0.0);
	cout << "gcsDiff: " << gcsDiff << endl << "eastVector" << eastVector << endl;
	double radAngleWithEast = acos(gcsDiff.dot(eastVector));
	double degAngleWithEast = radAngleWithEast * (180.0 / M_PI);
	cout << "angle with the vector to east: " << degAngleWithEast << endl;

	//Point two to three vec
	Mat threeToTwo = pointTwoVec - pointThreeVec;
	normalize(threeToTwo, threeToTwo);
	Mat threeToZero = /* origin/camera */ -pointThreeVec;
	normalize(threeToZero, threeToZero);
	cout << "threeToZero: " << threeToZero << endl << "threeToTwo" << threeToTwo << endl;
	//angle between vectors twoToThree and twoToZero
	double radAngleAtTwo = acos(threeToTwo.dot(threeToZero));
	double degAngleAtTwo = radAngleAtTwo * (180.0 / M_PI);
	cout << "angle that is by the point two: " << degAngleAtTwo << endl;

	//create vector from point three to the camera in the gcs
	double adjustTheSign = gcsDiff.at<double>(1) < 0 ? -1.0 : 1.0; //the angle will be negative in case of negative y
	double radAngleDirVec = adjustTheSign * radAngleWithEast - radAngleAtTwo;//angle between east vector and vector from point three and camera - angle of future direction vector of the line between point three and camera
	Point2d lineDirVec(cos(radAngleDirVec), sin(radAngleDirVec));

	//second point 50.0867628N, 14.4193081E
	//third point 50.0865958N, 14.4192706E 
	//Meassure distance
	double radLong2 = 14.4193081 * (M_PI / 180.0);
	double radLat2 = 50.0867628 * (M_PI / 180.0);
	double radLong3 = 14.4192706 * (M_PI / 180.0);
	double radLat3 = 50.0865958 * (M_PI / 180.0);

	double diffLong = (radLong3 - radLong2);
	double diffLat = (radLat3 - radLat2);

	// Haversine formula to calculate gcs distance:
	double earthRadius = 6371000; //(in meters)globaly considered average
	double insideSqrt = pow(sin(diffLat / 2), 2) +
		cos(radLat2) * cos(radLat3) * pow(sin(diffLong / 2), 2);
	double gcsDistance = earthRadius * 2 * asin (sqrt(insideSqrt)); //in meters
	
	//calculate distance in our space
	Mat dthreeToTwo = pointTwoVec - pointThreeVec;
	double distance = sqrt(pow(dthreeToTwo.at<double>(0),2) + pow(dthreeToTwo.at<double>(1), 2));

	std::cout << "gcsDistance: " << gcsDistance << endl;
	std::cout << "our distance: " << distance << endl;
	double distScaleFactor = distance / gcsDistance;
	std::cout << "scale factor of the distances: " << distance / gcsDistance << endl;
	
	Mat dthreeToCamera = pointThreeVec /* - camera (0,0)*/;
	double distanceThreeToCamera = sqrt(pow(dthreeToCamera.at<double>(0),2) + pow(dthreeToCamera.at<double>(1), 2));
	Point2d cameraLoc;
	cameraLoc.x = gcsThreePoint.x + ((distanceThreeToCamera / distScaleFactor) * lineDirVec.x) / (metersInLongDeg * longtitudeAdjustFactor);
	cameraLoc.y = gcsThreePoint.y + ((distanceThreeToCamera / distScaleFactor) * lineDirVec.y) / (metersInLatDeg);

	//converting back to correct longtitude
	cameraLoc.x /= longtitudeAdjustFactor;

	cout << setprecision(9) << "camera location: " << cameraLoc << endl;
//TUTORIAL INSPIRED/OVERTAKEN CODE PART END=====================================================================
}




//=================================================================================================

void CImagesMatch::drawPreviewAndResult(const string& runName, Ptr<CLogger>& logger)
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
	std::vector<Point2f> objectKeypointsCoordinates;
	std::vector<Point2f> sceneKeypointsCoordinates;

	for (int i = 0; i < matches_.size(); i++)
	{
		//-- Get the keypoints from the good matches
		objectKeypointsCoordinates.push_back(objectImage_->getKeypoints()[matches_[i].queryIdx].pt);
		sceneKeypointsCoordinates.push_back(sceneImage_->getKeypoints()[matches_[i].trainIdx].pt);
	}

	transformationMatrix_ = findHomography(objectKeypointsCoordinates, sceneKeypointsCoordinates, RANSAC);
	transformMatrixComputed_ = true;

	// Get the corners from the image_1 ( the object to be "detected" )
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = Point2f(0, 0); // left upper
	obj_corners[1] = Point2f(objectImage_->getImage().cols, 0); // right uppper
	obj_corners[2] = Point2f(objectImage_->getImage().cols, objectImage_->getImage().rows); // right bottom 
	obj_corners[3] = Point2f(0, objectImage_->getImage().rows); // left bottom

	//do the 3d corners
	std::vector<Point3f> obj_corners3D(4);
	obj_corners3D[0] = Point3f(0, 0, 0); // left upper
	obj_corners3D[1] = Point3f(objectImage_->getImage().cols, 0, 0); // right uppper
	obj_corners3D[2] = Point3f(objectImage_->getImage().cols, objectImage_->getImage().rows, 0); // right bottom 
	obj_corners3D[3] = Point3f(0, objectImage_->getImage().rows, 0); // left bottom

		//future cornes coordinates
	std::vector<Point2f> scene_corners(4);
	//transformating the cornes
	perspectiveTransform(obj_corners, scene_corners, transformationMatrix_);

	//calculate the real location
	calcLocation(obj_corners, obj_corners3D, scene_corners, logger);

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
	warpPerspective(mask, transformedMask, transformationMatrix_, Size(transformedMask.cols, transformedMask.rows));

	//transform object into scene size image plane
	Mat transformedObject(Size(sceneImage_->getImage().cols, sceneImage_->getImage().rows), CV_8U);
	warpPerspective(objectImage_->getImage(), transformedObject, transformationMatrix_, Size(transformedObject.cols, transformedObject.rows));

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
	logger->putImage(sceneImageRenderBuffer, "Matches");
}