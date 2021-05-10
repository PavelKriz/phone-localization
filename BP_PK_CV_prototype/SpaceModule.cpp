#include "SpaceModule.h"

double sm::gcsDistance(SGcsCoords first, SGcsCoords second)
{
	//get all the values to radians
	first.longtitude_ = degToRad(first.longtitude_);
	first.latitude_ = degToRad(first.latitude_);
	second.longtitude_ = degToRad(second.longtitude_);
	second.latitude_ = degToRad(second.latitude_);

	double diffLong = (second.longtitude_ - first.longtitude_);
	double diffLat = (second.latitude_ - first.latitude_);

	// Haversine formula to calculate gcs distance:
	double insideSqrt = pow(sin(diffLat / 2), 2) +
		cos(first.latitude_) * cos(second.latitude_) * pow(sin(diffLong / 2), 2);
	return EARTH_RADIUS_IN_METERS * 2 * asin(sqrt(insideSqrt)); //in meters
}

double sm::distance(double ax, double ay, double bx, double by)
{
	double diffX = ax - bx;
	double diffY = ay - by;
	return sqrt(pow(diffX, 2) + pow(diffY, 2));
}

double sm::distance(double ax, double ay, double az, double bx, double by, double bz)
{
	double diffX = ax - bx;
	double diffY = ay - by;
	double diffZ = az - bz;
	return sqrt(pow(diffX, 2) + pow(diffY, 2) + pow(diffZ, 2));
}

Point2d sm::getMidPoint2D(double ax, double ay, double bx, double by)
{
	return Point2d((ax + bx)/2, (ay + by)/2);
}

Mat sm::getHomogenousVector3D(double x, double y, double z)
{
	return (Mat_<double>(4,1) << x, y, z, 1.0);
}

double sm::metersInLatDeg(double latitude)
{
	//Length in meters of 1° of latitude = at 45 degrees is 111.141548 km - optimised only for latitudes around 45°
	return 111141.548;
}

double sm::metersInLongDeg(double latitude)
{
	//Length in meters of 1° of longitude = 40075017 m * cos(latitude) / 360
	//2 * M_PI * (6371000 * cos()
	return 40075017 * cos(degToRad(latitude)) / 360.0;
}

double sm::longtitudeAdjustingFactor(double latitude)
{
	double metersInLatDegreeRetVal = metersInLatDeg(latitude);
	double metersInLongDegRetVal = metersInLongDeg(latitude);
	double longtitudeAdjustFactor = metersInLatDegreeRetVal / metersInLongDegRetVal;

	//checking the calculation
	//cout << "meters in Lat deg = " << metersInLatDegreeRetVal << " : meters in adjusted Long deg = " << metersInLongDegRetVal * longtitudeAdjustFactor << endl;
	return longtitudeAdjustFactor;
}

double sm::longtitudeCorrectionFactor(double latitude)
{
	return 1 / longtitudeAdjustingFactor(latitude);
}


double sm::radToDeg(double radAngle)
{
	return radAngle * (180.0 / M_PI);
}

double sm::degToRad(double degAngle)
{
	return degAngle * (M_PI / 180.0);
}

double sm::vectorAngle2D(double ax, double ay, double bx, double by, double cx, double cy)
{
	Mat vec1 = (Mat_<double>(2, 1) << (ax - bx), (ay - by));
	Mat vec2 = (Mat_<double>(2, 1) << (cx - bx), (cy - by));
	normalize(vec1, vec1);
	normalize(vec2, vec2);
	return acos(vec1.dot(vec2));
}

double sm::getVecRotFromEast(double ax, double ay)
{
	Mat vec = (Mat_<double>(2, 1) << ax, ay);
	normalize(vec, vec);
	Mat eastVector = (Mat_<double>(2, 1) << 1.0, 0.0);

	double adjustTheSign = vec.at<double>(1) < 0 ? -1.0 : 1.0; //the angle will be negative in case of negative y
	double radAngleWithEast = adjustTheSign * acos(vec.dot(eastVector));
	//cout << "angle with the vector to east: " << radToDeg(radAngleWithEast) << endl;

	return radAngleWithEast;
}

sm::SGcsCoords sm::solve3Kto2Kand1U(const Point2d& p1, const Point2d& p2, const Point2d& p3,
	const SGcsCoords& p2Gcs, const SGcsCoords& p3Gcs, Ptr<CLogger>& logger)
{
	//convert points to vectors
	Mat p1Vec(p1);
	Mat p2Vec(p2);
	Mat p3Vec(p3);
	
	//gcs (Geographic coordinate system) coordinate system
	//first longtitude (x) and then lattitude(y)
	//converting coordinates to 
	Mat gcsP2Vec(p2Gcs.convertToOpenCVFormat());
	Mat gcsP3Vec(p3Gcs.convertToOpenCVFormat());

	//the longtitude and latitude are not in the same scale is it is needed to scale them to same scale for the computations
	//keep in mind that for meassuring distances using global methods like haversine formula it is needed to convert the longtitude back for the meassuring
	double longtitudeAdjustFactor = longtitudeAdjustingFactor(p3Gcs.latitude_);

	//adjust the points to make same meassures in both dirrections
	//gcsP2Vec.at<double>(0) *= longtitudeAdjustFactor;
	//gcsP3Vec.at<double>(0) *= longtitudeAdjustFactor;

	//angle of vector twoToThree to the vector pointing to east
	Mat gcsDiff = gcsP2Vec - gcsP3Vec;
	double radAngleWithEast = getVecRotFromEast(gcsDiff.at<double>(0), gcsDiff.at<double>(1));
	logger->log("gcs Diff x: ").log(to_string(gcsDiff.at<double>(0))).log(" gcs Diff y: ").log(to_string(gcsDiff.at<double>(1))).endl();
	logger->log("angle from east: ").log(to_string(radToDeg(radAngleWithEast))).endl();

	//p3 to p2
	double radAngleAtP3 = sm::vectorAngle2D(p2.x, p2.y, p3.x, p3.y, p1.x, p1.y);
	logger->log("angle that is by the point three: ").log(to_string(radToDeg(radAngleAtP3))).endl();

	//create vector from point three to the camera in the gcs
	double radAngleDirVec = radAngleWithEast - radAngleAtP3;//angle between east vector and vector from point three and camera - angle of future direction vector of the line between point three and camera
	Point2d lineDirVec(cos(radAngleDirVec), sin(radAngleDirVec));
	logger->log("angle that is taken from the east to the right point: ").log(to_string(radToDeg(radAngleDirVec))).endl();
	logger->log("cos: ").log(to_string(cos(radAngleDirVec))).log(" sin:").log(to_string(sin(radAngleDirVec))).endl();

	//Computing the scale between two scales (between the camera space scale and the gcs scale)
	//for meassuring distance we have to use correct coordinates (so the unchanged coordinates have to be used)
	double gcsDistance = sm::gcsDistance(p2Gcs, p3Gcs);
	//calculate distance in our space
	double distance = sm::distance(p2.x, p2.y, p3.x, p3.y);
	double distScaleFactor = distance / gcsDistance;
	logger->log("gcsDistance: ").log(to_string(gcsDistance)).endl();
	logger->log("our distance: ").log(to_string(distance)).endl();
	logger->log("scale factor of the distances: ").log(to_string(distScaleFactor)).endl();


	//Calculating the p1 gcs location
	double distanceP3ToP1 = sm::distance(p1.x, p1.y, p3.x, p3.y);
	//just fill the parametric formula of a line and get the location of p1 (camera in the project)
	Point2d p1GcsLoc;
	logger->log("distance from camera to p3: ").log(to_string((distanceP3ToP1 / distScaleFactor))).endl();
	p1GcsLoc.x = p3Gcs.longtitude_ + ((distanceP3ToP1 / distScaleFactor) * lineDirVec.x) / (metersInLongDeg(p3Gcs.latitude_));
	p1GcsLoc.y = p3Gcs.latitude_ + ((distanceP3ToP1 / distScaleFactor) * lineDirVec.y) / (metersInLatDeg(p3Gcs.latitude_));
	logger->log("what is being added: ").log(to_string(metersInLongDeg(p3Gcs.latitude_))).endl();

	//converting back to correct longtitude
	//p1GcsLoc.x *= longtitudeCorrectionFactor(p3Gcs.latitude_);

	return sm::SGcsCoords(p1GcsLoc.x, p1GcsLoc.y);
}
