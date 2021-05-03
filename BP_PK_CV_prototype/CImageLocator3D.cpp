#include "CImageLocator3D.h"

void CImageLocator3D::createCameraIntrinsicsMatrix(const SCameraInfo& cameraInfo)
{
	double width = sceneImage_->getImage().cols;		// image size
	double height = sceneImage_->getImage().rows;        // image size
	double sx, sy; //sizes of chip in both axes  = 3.79, sy = 4.96;             // sensor size  - 4.96 x 3.72 mm) 	
	//check if the rotation isnt broken and assign the right side size of chip to right dimension of image
	if (width < height) {
		sx = min(cameraInfo.chipSizeX_, cameraInfo.chipSizeY_);
		sy = max(cameraInfo.chipSizeX_, cameraInfo.chipSizeY_);
	}
	else {
		sx = max(cameraInfo.chipSizeX_, cameraInfo.chipSizeY_);
		sy = min(cameraInfo.chipSizeX_, cameraInfo.chipSizeY_);
	}

	double fx = width * cameraInfo.focalLength_ / sx;   // fx
	double fy = height * cameraInfo.focalLength_ / sy;  // fy
	double cx = width / 2;  // cx
	double cy = height / 2; // cy


	cameraIntrinsicsMatrixA_ = Mat::zeros(3, 3, CV_64FC1); // intrinsic camera parameters

	cameraIntrinsicsMatrixA_.at<double>(0, 0) = fx;       //      [ fx   0  cx ]
	cameraIntrinsicsMatrixA_.at<double>(1, 1) = fy;       //      [  0  fy  cy ]
	cameraIntrinsicsMatrixA_.at<double>(0, 2) = cx;       //      [  0   0   1 ]
	cameraIntrinsicsMatrixA_.at<double>(1, 2) = cy;
	cameraIntrinsicsMatrixA_.at<double>(2, 2) = 1;
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

Mat CImageLocator3D::getCorrectionMatrixForTheCameraLocalSpace(const Mat& p1HomVec, const Mat& p2HomVec, const Mat& p3HomVec,
 const sm::SGcsCoords& gcsP2, const sm::SGcsCoords& gcsP3, Ptr<CLogger>& logger)
{
	//converting points from (x,y,z,1) to (x,y,z)
	Mat p1Vec = p1HomVec(Range(0, 3), Range(0, 1));
	Mat p2Vec = p2HomVec(Range(0, 3), Range(0, 1));
	Mat p3Vec = p3HomVec(Range(0, 3), Range(0, 1));

	Mat standingPoint = Mat::zeros(3, 1, CV_64FC1);

	//scene geometry straithening to be perpendicular with the ground (good on flat grounds and buidlings where the geomtry is clear)
	//following description is a description of params_.considerPhoneHoldHeight_ in process params that comes from CONSIDER_PHONE_HOLD_HEIGHT from parameters.h
/* *
 * if enabled it will increase the precsion of algorithmn on plane surface
 * (if there would be hills it would be broken anyway so it should be true in most cases)
 * good mainly if is in the scene a building object (building that is not like angled tower in Pisa, but straight building)
 * it enables straightaning the geometry, but in some places where there is a risc that the object geometry is not perpendicular to the ground it might be disabled
 */
	if (params_.considerPhoneHoldHeight_) {

		double gcsDistance = sm::gcsDistance(gcsP2, gcsP3);
		//calculate distance in our space
		double distance = sm::distance(p2Vec.at<double>(0), p2Vec.at<double>(1), p2Vec.at<double>(2),
			p3Vec.at<double>(0), p3Vec.at<double>(1), p3Vec.at<double>(2));
		double distScaleFactor = distance / gcsDistance; //how much units is one meter in our 
		logger->log("gcsDistance").log(to_string(gcsDistance)).endl();
		logger->log("our distance").log(to_string(distance)).endl();
		double cameraOffsetFromGround = sm::HUMAN_HEIGHT - sm::CAMERA_HOLDING_OFFSET;
		double lenghtInCameraSpaceUnits = cameraOffsetFromGround * distScaleFactor;

		//get down vector from the building information
		//the down vector is then scaled according to the length
		Mat downVector = p2Vec - p1Vec;
		normalize(downVector, downVector);
		standingPoint.at<double>(0) = /*origin + */ downVector.at<double>(0) * lenghtInCameraSpaceUnits;
		standingPoint.at<double>(1) = /*origin + */ downVector.at<double>(1) * lenghtInCameraSpaceUnits;
		standingPoint.at<double>(2) = /*origin + */ downVector.at<double>(2) * lenghtInCameraSpaceUnits;
	}

	//find real place where the guy has his legs
	Mat vecStandingPointTo3 = p3Vec - standingPoint;//vecStandingPointTo3 is updated z coordinate
	normalize(vecStandingPointTo3, vecStandingPointTo3);
	Mat vecStandingPointTo2 = p2Vec - standingPoint;
	normalize(vecStandingPointTo2, vecStandingPointTo2);
	Mat vecUp = vecStandingPointTo3.cross(vecStandingPointTo2); //vecUp is updated y coordinate
	normalize(vecUp, vecUp);
	Mat right = vecUp.cross(vecStandingPointTo3); //right is updated x coordinate
	normalize(right, right);

	//create change basis matrix
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
	changeBasis.at<double>(0, 2) = vecStandingPointTo3.at<double>(0);
	changeBasis.at<double>(1, 2) = vecStandingPointTo3.at<double>(1);
	changeBasis.at<double>(2, 2) = vecStandingPointTo3.at<double>(2);
	//right bottom one
	changeBasis.at<double>(3, 3) = 1;

	return changeBasis.inv();
}

void CImageLocator3D::computeFlatRotation(const sm::SGcsCoords& gcsP1, const sm::SGcsCoords& gcsP2, const sm::SGcsCoords& gcsP3)
{
	Point2d midPoint = sm::getMidPoint(gcsP2.longtitude_, gcsP2.latitude_, gcsP3.longtitude_, gcsP3.longtitude_);

}

void CImageLocator3D::calcLocation(vector<Point2d>& obj_corners, vector<Point2d>& sceneCorners, Ptr<CLogger>& logger)
{
	//inspiration for this code was taken from this OpenCV tutorial https://docs.opencv.org/master/dc/d2c/tutorial_real_time_pose.html

	
	logger->logSection("debug info start", 2);

	sm::SGcsCoords gcsPoint2 = sm::SGcsCoords(14.4193081, 50.0867628);
	sm::SGcsCoords gcsPoint3 = sm::SGcsCoords(14.4192706, 50.0865958);

	//initiating the matrices by zero values and correct size
	RMatrix_ = Mat::zeros(3, 3, CV_64FC1); // rotation matrix
	RTMatrix_ = Mat::zeros(4, 4, CV_64FC1); // rotation-translation matrix
	TVec_ = cv::Mat::zeros(3, 1, CV_64FC1);          // output translation vector
	RVec_ = cv::Mat::zeros(3, 1, CV_64FC1);          // output rotation vector
	distCoeffs_ = cv::Mat::zeros(4, 1, CV_64FC1);    // vector of distortion coefficients - setting to zero distortion

	bool useExtrinsicGuess = false;   // if true the function uses the provided RVec_ and TVec_ values as
								  // initial approximations of the rotation and translation vectors

	//creating a plane in the 3D space - image visualising a facade of a building (probably building)
	//the plane is then placed in the positive Z direction -> (x, y, 1.0) <- not a homogenous coordinate
	//do the 3d corners - 3d points to PnP have to have 3x1 format
	std::vector<Point3d> objCorners3D(4);
	//the order is - left upper -> right uppper -> right bottom -> left bottom
	for (size_t i = 0; i < 4; ++i) {
		objCorners3D[i] = Point3d(obj_corners[i].x, obj_corners[i].y, 1.0);
	}

	logger->log("Input 3D Coordinates").endl();
	for (size_t i = 0; i < 4; ++i) {
		logger->log(objCorners3D[i]).endl();
	}

	//solve our PnP problem
	//3d points to PnP have to have 3x1 format
	solvePnP(objCorners3D, sceneCorners, cameraIntrinsicsMatrixA_, distCoeffs_, RVec_, TVec_, useExtrinsicGuess, SOLVEPNP_ITERATIVE); //because the method wasnt specified the iterative method will take place

	//converting rotation vector to the rotation matrix
	Rodrigues(RVec_, RMatrix_);

	//Filling the RTMatrix_
	//copy rotation from RMatrix_ to RTMatrix_
	RTMatrix_.at<double>(0, 0) = RMatrix_.at<double>(0, 0);
	RTMatrix_.at<double>(0, 1) = RMatrix_.at<double>(0, 1);
	RTMatrix_.at<double>(0, 2) = RMatrix_.at<double>(0, 2);
	RTMatrix_.at<double>(1, 0) = RMatrix_.at<double>(1, 0);
	RTMatrix_.at<double>(1, 1) = RMatrix_.at<double>(1, 1);
	RTMatrix_.at<double>(1, 2) = RMatrix_.at<double>(1, 2);
	RTMatrix_.at<double>(2, 0) = RMatrix_.at<double>(2, 0);
	RTMatrix_.at<double>(2, 1) = RMatrix_.at<double>(2, 1);
	RTMatrix_.at<double>(2, 2) = RMatrix_.at<double>(2, 2);

	//copy translation
	RTMatrix_.at<double>(0, 3) = TVec_.at<double>(0);
	RTMatrix_.at<double>(1, 3) = TVec_.at<double>(1);
	RTMatrix_.at<double>(2, 3) = TVec_.at<double>(2);

	//fill bottom row of RTMatrix_
	RTMatrix_.at<double>(3, 3) = 1.0;

	logger->log("RVec_: ").log(RVec_).endl();
	logger->log("TVec_: ").log(TVec_).endl();
	logger->log("RTMatrix_: ").endl().log(RTMatrix_).endl();

	//converting points to homogenous coordinates so they can be transformed
	vector<Mat> objCornersHomVec3D(4);
	for (size_t i = 0; i < 4; ++i) {
		objCornersHomVec3D[i] = sm::getHomogenousVector3D(objCorners3D[i].x, objCorners3D[i].y, objCorners3D[i].z);
	}

	//get the corners in camera space
	std::vector<Mat> objCornersCameraSpaceHomVec(4);
	for (size_t i = 0; i < 4; ++i) {
		objCornersCameraSpaceHomVec[i] = RTMatrix_ * objCornersHomVec3D[i];
	}

	//PRINT THE RESULT I GOT
	logger->log("Points coordinates in the local camera space:").endl();
	for (size_t i = 0; i < objCornersCameraSpaceHomVec.size(); ++i) {
		logger->log(objCornersCameraSpaceHomVec[i]).endl();
	}

	logger->log("Check space corners").endl();
	std::vector<Mat> checkImageSpaceCorners(4);
	for (size_t i = 0; i < 4; ++i) {
		logger->log(projectWorldSpacetoImage(objCornersCameraSpaceHomVec[i])).endl();
	}

	logger->log("Scene corners").endl();
	for (size_t i = 0; i < 4; ++i) {
		logger->log(sceneCorners[i]).endl();
	}

	//correcting the points with calculating the right rotation (rotating the world to match the flat ground in one axis - the ground is flat then) 
	Mat correctionMatrix = getCorrectionMatrixForTheCameraLocalSpace(
		objCornersCameraSpaceHomVec[1], objCornersCameraSpaceHomVec[2], objCornersCameraSpaceHomVec[3], gcsPoint2, gcsPoint3, logger);

	//ignoring the y axis to gain only the 2d coordinates
	Mat objCornerOnPlaneVec2 = correctionMatrix * objCornersCameraSpaceHomVec[2];
	Mat objCornerOnPlaneVec3 = correctionMatrix * objCornersCameraSpaceHomVec[3];

	// (x,y,z, w) -> (x, z) and renaming to (x, y)
	Point2d pointTwo(objCornerOnPlaneVec2.at<double>(0), objCornerOnPlaneVec2.at<double>(2));
	Point2d pointThree(objCornerOnPlaneVec3.at<double>(0), objCornerOnPlaneVec3.at<double>(2));

	logger->log("changeBasis").endl().log(correctionMatrix).endl();
	logger->log("New basis point two").endl().log(objCornerOnPlaneVec2).endl();
	logger->log("New basis point three").endl().log(objCornerOnPlaneVec3).endl();

	//find the global coordinates for the camera
	sm::SGcsCoords cameraGcsLoc = sm::solve3Kto2Kand1U(Point2d(0.0, 0.0), pointTwo, pointThree, gcsPoint2, gcsPoint3, logger);

	//display the dummy prism into the scene to visualise the understood perspective and volume
	projectBuildingDraftIntoScene(objCorners3D, logger);

	logger->logSection("debug info end", 2);

	logger->log("camera location: ").log(cameraGcsLoc).endl();
}
