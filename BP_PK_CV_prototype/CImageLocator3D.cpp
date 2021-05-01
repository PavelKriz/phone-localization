#include "CImageLocator3D.h"

void CImageLocator3D::printTransformationMatrix(Ptr<CLogger>& logger) const
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

Mat CImageLocator3D::projectCameraSpacetoImage(const Mat& toProject) const
{
	Mat takeFirstThree = Mat::zeros(3, 4, CV_64FC1);
	takeFirstThree.at<double>(0, 0) = 1.0;
	takeFirstThree.at<double>(1, 1) = 1.0;
	takeFirstThree.at<double>(2, 2) = 1.0;

	Mat point2Dtmp = cameraIntrinsicsMatrixA_ * takeFirstThree * toProject;
	
	//normalize
	Mat point2DNorm = Mat::zeros(2, 1, CV_64FC1);
	point2DNorm.at<double>(0) = point2Dtmp.at<double>(0) / point2Dtmp.at<double>(2);
	point2DNorm.at<double>(1) = point2Dtmp.at<double>(1) / point2Dtmp.at<double>(2);
	
	return point2DNorm;
}

Mat CImageLocator3D::projectWorldSpacetoImage(const Mat& toProject) const
{
	return projectCameraSpacetoImage(RTMatrix_ * toProject);
}

void CImageLocator3D::projectBuildingDraftIntoScene(const vector<Point3d>& objCorners3D, Ptr<CLogger>& logger) const
{
	//just creating some depth in same scale as the object image
	double depthGuess = sm::distance(objCorners3D[0].x, objCorners3D[0].y, objCorners3D[1].x, objCorners3D[1].y);

	vector<Mat> objectPrismDummy3D(8);
	objectPrismDummy3D[0] = (Mat_<double>(4, 1) << objCorners3D[0].x, objCorners3D[0].y, objCorners3D[0].z, 1.0);
	objectPrismDummy3D[1] = (Mat_<double>(4, 1) << objCorners3D[1].x, objCorners3D[1].y, objCorners3D[1].z, 1.0);
	objectPrismDummy3D[2] = (Mat_<double>(4, 1) << objCorners3D[2].x, objCorners3D[2].y, objCorners3D[2].z, 1.0);
	objectPrismDummy3D[3] = (Mat_<double>(4, 1) << objCorners3D[3].x, objCorners3D[3].y, objCorners3D[3].z, 1.0);

	objectPrismDummy3D[4] = (Mat_<double>(4, 1) << objCorners3D[0].x, objCorners3D[0].y, objCorners3D[0].z + depthGuess, 1.0);
	objectPrismDummy3D[5] = (Mat_<double>(4, 1) << objCorners3D[1].x, objCorners3D[1].y, objCorners3D[1].z + depthGuess, 1.0);
	objectPrismDummy3D[6] = (Mat_<double>(4, 1) << objCorners3D[2].x, objCorners3D[2].y, objCorners3D[2].z + depthGuess, 1.0);
	objectPrismDummy3D[7] = (Mat_<double>(4, 1) << objCorners3D[3].x, objCorners3D[3].y, objCorners3D[3].z + depthGuess, 1.0);

	vector<Point2d> objectDummyProjected(8);
	for (int i = 0; i < objectPrismDummy3D.size(); ++i) {
		Mat projected = projectWorldSpacetoImage(objectPrismDummy3D[i]);
		objectDummyProjected[i] = Point2d(projected.at<double>(0), projected.at<double>(1));
	}

	//-- Draw the prism dummy wireframe
	Mat dummyInScene;// = sceneImage_->getImage().clone();
	cvtColor(sceneImage_->getImage(), dummyInScene, COLOR_GRAY2RGB);

	line(dummyInScene, objectDummyProjected[0],
		objectDummyProjected[1], Scalar(0, 255, 255), 8);
	line(dummyInScene, objectDummyProjected[1],
		objectDummyProjected[2], Scalar(0, 255, 255), 8);
	line(dummyInScene, objectDummyProjected[2],
		objectDummyProjected[3], Scalar(0, 255, 255), 8);
	line(dummyInScene, objectDummyProjected[3],
		objectDummyProjected[0], Scalar(0, 255, 255), 8);

	line(dummyInScene, objectDummyProjected[4],
		objectDummyProjected[5], Scalar(0, 255, 255), 4);
	line(dummyInScene, objectDummyProjected[5],
		objectDummyProjected[6], Scalar(0, 255, 255), 4);
	line(dummyInScene, objectDummyProjected[6],
		objectDummyProjected[7], Scalar(0, 255, 255), 4);
	line(dummyInScene, objectDummyProjected[7],
		objectDummyProjected[4], Scalar(0, 255, 255), 4);

	line(dummyInScene, objectDummyProjected[0],
		objectDummyProjected[4], Scalar(0, 255, 255), 6);
	line(dummyInScene, objectDummyProjected[1],
		objectDummyProjected[5], Scalar(0, 255, 255), 6);
	line(dummyInScene, objectDummyProjected[2],
		objectDummyProjected[6], Scalar(0, 255, 255), 6);
	line(dummyInScene, objectDummyProjected[3],
		objectDummyProjected[7], Scalar(0, 255, 255), 6);

	//draw the matches and places where the object should be placed in the scene
	logger->putImage(dummyInScene, "dummyProjected");

}

void CImageLocator3D::calcLocation(vector<Point2f>& obj_corners, vector<Point2f>& sceneCorners, Ptr<CLogger>& logger)
{
	//major parts of FOLLOWING CODE was taken from OpenCV tutorial about the OpenCV Calib3D module https://docs.opencv.org/master/dc/d2c/tutorial_real_time_pose.html
	//TUTORIAL INSPIRED/OVERTAKEN CODE PART BEGIN=====================================================================

	//future cornes coordinates
	std::vector<Point2f> scene_corners(4);
	//transformating the cornes
	perspectiveTransform(obj_corners, scene_corners, transformationMatrix_);

	//XIAOMI REDMI 5 PLUS - hope for the best - here just for the testing
	double f = 4;                           // focal length in mm
	double sx = 3.79, sy = 4.96;             // sensor size  - 4.96 x 3.72 mm) 	
	double width = sceneImage_->getImage().cols;		// image size
	double height = sceneImage_->getImage().rows;        // image size
	double fx = width * f / sx;   // fx
	double fy = height * f / sy;  // fy
	double cx = width / 2;      // cx
	double cy = height / 2;    // cy


	cameraIntrinsicsMatrixA_ = Mat::zeros(3, 3, CV_64FC1); // intrinsic camera parameters
	Mat R_matrix_ = Mat::zeros(3, 3, CV_64FC1); // rotation matrix
	RTMatrix_ = Mat::zeros(4, 4, CV_64FC1); // rotation-translation matrix
	TVec_ = cv::Mat::zeros(3, 1, CV_64FC1);          // output translation vector
	RVec_ = cv::Mat::zeros(3, 1, CV_64FC1);          // output rotation vector
	distCoeffs_ = cv::Mat::zeros(4, 1, CV_64FC1);    // vector of distortion coefficients - setting to zero distortion

	cameraIntrinsicsMatrixA_.at<double>(0, 0) = fx;       //      [ fx   0  cx ]
	cameraIntrinsicsMatrixA_.at<double>(1, 1) = fy;       //      [  0  fy  cy ]
	cameraIntrinsicsMatrixA_.at<double>(0, 2) = cx;       //      [  0   0   1 ]
	cameraIntrinsicsMatrixA_.at<double>(1, 2) = cy;
	cameraIntrinsicsMatrixA_.at<double>(2, 2) = 1;


	bool useExtrinsicGuess = false;   // if true the function uses the provided RVec_ and TVec_ values as
								  // initial approximations of the rotation and translation vectors

	//do the 3d corners
	std::vector<Point3d> objCorners3D(4);
	objCorners3D[0] = Point3d(obj_corners[0].x, obj_corners[0].y, 1.0); // left upper
	objCorners3D[1] = Point3d(obj_corners[1].x, obj_corners[1].y, 1.0); // right uppper
	objCorners3D[2] = Point3d(obj_corners[2].x, obj_corners[2].y, 1.0); // right bottom 
	objCorners3D[3] = Point3d(obj_corners[3].x, obj_corners[3].y, 1.0); // left bottom

	std::cout << "INPUT 3D Coordinates" << std::endl;
	for (size_t i = 0; i < 4; ++i) {
		std::cout << objCorners3D[i] << std::endl;
	}

	//mirror coordinates
	std::vector<Point2f> sceneCornersMirrored(4);
	cout << "scene corners, scene image columns: " << sceneImage_->getImage().rows << endl;
	for (int i = 0; i < 4; ++i) {
		sceneCornersMirrored[i].x = scene_corners[i].x;
		sceneCornersMirrored[i].y = sceneImage_->getImage().rows - scene_corners[i].y;
		cout << "mirrored: " << sceneCornersMirrored[i] << endl;
		cout << "orginal: " << scene_corners[i] << endl;
	}

	solvePnP(objCorners3D, scene_corners, cameraIntrinsicsMatrixA_, distCoeffs_, RVec_, TVec_, useExtrinsicGuess, SOLVEPNP_ITERATIVE); //because the method wasnt specified the iterative method will take place

	//converting rotation vector to the rotation matrix
	Rodrigues(RVec_, R_matrix_);

	Quatd rotQuat = Quatd::createFromRotMat(R_matrix_);

	//TODO P_matrix
	//copy rotation
	RTMatrix_.at<double>(0, 0) = R_matrix_.at<double>(0, 0);
	RTMatrix_.at<double>(0, 1) = R_matrix_.at<double>(0, 1);
	RTMatrix_.at<double>(0, 2) = R_matrix_.at<double>(0, 2);
	RTMatrix_.at<double>(1, 0) = R_matrix_.at<double>(1, 0);
	RTMatrix_.at<double>(1, 1) = R_matrix_.at<double>(1, 1);
	RTMatrix_.at<double>(1, 2) = R_matrix_.at<double>(1, 2);
	RTMatrix_.at<double>(2, 0) = R_matrix_.at<double>(2, 0);
	RTMatrix_.at<double>(2, 1) = R_matrix_.at<double>(2, 1);
	RTMatrix_.at<double>(2, 2) = R_matrix_.at<double>(2, 2);

	//copy translation
	RTMatrix_.at<double>(0, 3) = TVec_.at<double>(0);
	RTMatrix_.at<double>(1, 3) = TVec_.at<double>(1);
	RTMatrix_.at<double>(2, 3) = TVec_.at<double>(2);

	//bottom row
	RTMatrix_.at<double>(3, 3) = 1.0;

	std::cout << "RVec_: " << RVec_ << std::endl;
	std::cout << "TVec_: " << TVec_ << std::endl;
	std::cout << "RTMatrix_: " << RTMatrix_ << std::endl;

	Mat objCorner3D_4_0 = Mat::zeros(4, 1, CV_64FC1);
	objCorner3D_4_0.at<double>(0) = objCorners3D[0].x;
	objCorner3D_4_0.at<double>(1) = objCorners3D[0].y;
	objCorner3D_4_0.at<double>(2) = objCorners3D[0].z;
	objCorner3D_4_0.at<double>(3) = 1;
	Mat objCorner3D_4_1 = Mat::zeros(4, 1, CV_64FC1);
	objCorner3D_4_1.at<double>(0) = objCorners3D[1].x;
	objCorner3D_4_1.at<double>(1) = objCorners3D[1].y;
	objCorner3D_4_1.at<double>(2) = objCorners3D[1].z;
	objCorner3D_4_1.at<double>(3) = 1;
	Mat objCorner3D_4_2 = Mat::zeros(4, 1, CV_64FC1);
	objCorner3D_4_2.at<double>(0) = objCorners3D[2].x;
	objCorner3D_4_2.at<double>(1) = objCorners3D[2].y;
	objCorner3D_4_2.at<double>(2) = objCorners3D[2].z;
	objCorner3D_4_2.at<double>(3) = 1;
	Mat objCorner3D_4_3 = Mat::zeros(4, 1, CV_64FC1);
	objCorner3D_4_3.at<double>(0) = objCorners3D[3].x;
	objCorner3D_4_3.at<double>(1) = objCorners3D[3].y;
	objCorner3D_4_3.at<double>(2) = objCorners3D[3].z;
	objCorner3D_4_3.at<double>(3) = 1;

	//get the corners in camera space
	std::vector<Mat> objCameraSPaceCorners(4);
	objCameraSPaceCorners[0] = RTMatrix_ * objCorner3D_4_0;
	objCameraSPaceCorners[1] = RTMatrix_ * objCorner3D_4_1;
	objCameraSPaceCorners[2] = RTMatrix_ * objCorner3D_4_2;
	objCameraSPaceCorners[3] = RTMatrix_ * objCorner3D_4_3;

	//PRINT THE RESULT I GOT

	for (size_t i = 0; i < objCameraSPaceCorners.size(); ++i) {
		logger->log("Point ").log(to_string(i)).log(" := (");
		logger->log(to_string(objCameraSPaceCorners[i].at<double>(0))).log(" ,");
		logger->log(to_string(objCameraSPaceCorners[i].at<double>(1))).log(" ,");
		logger->log(to_string(objCameraSPaceCorners[i].at<double>(2))).log(" ,");
		logger->log(to_string(objCameraSPaceCorners[i].at<double>(3))).log(" )").endl();
	}

	//Mat takeFirstThree = Mat::zeros(3, 4, CV_64FC1);
	//takeFirstThree.at<double>(0, 0) = 1.0;
	//takeFirstThree.at<double>(1, 1) = 1.0;
	//takeFirstThree.at<double>(2, 2) = 1.0;

	std::cout << "Check space corners" << std::endl;
	std::vector<Mat> checkImageSpaceCorners(4);
	for (size_t i = 0; i < 4; ++i) {
		std::cout << projectWorldSpacetoImage(objCameraSPaceCorners[i]) << endl;
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

	sm::SGcsCoords coords = sm::solve3Kto2Kand1U(Point2d(0.0, 0.0), pointTwo, pointThree, sm::SGcsCoords(14.4193081, 50.0867628), sm::SGcsCoords(14.4192706, 50.0865958));
	cout << "new method camera location: " << coords.longtitude_ << ", " << coords.latitude_ << endl;

	//TUTORIAL INSPIRED/OVERTAKEN CODE PART END=====================================================================

	projectBuildingDraftIntoScene(objCorners3D, logger);
}
