#include "SpaceModule.h"

double sm::gcsDistance(SGcsCoords first, SGcsCoords second)
{
	//get all the values to radians
	first.longtitude_ *= (M_PI / 180.0);
	first.latitude_ *= (M_PI / 180.0);
	second.longtitude_ *= (M_PI / 180.0);
	second.latitude_ *= (M_PI / 180.0);

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

Point2d sm::getMidPoint(double ax, double ay, double bx, double by)
{
	return Point2d((ax + bx)/2, (ay + by)/2);
}

double sm::metersInLatDeg(double latitude)
{
	//Length in meters of 1° of latitude = at 45 degrees is 111.141548 km - optimised only for latitudes around 45°
	return 111141.548;
}

double sm::metersInLongDeg(double latitude)
{
	//Length in meters of 1° of longitude = 40075017 m * cos(latitude) / 360
	return 40075017 * cos(latitude) / 360.0;
}

double sm::longtitudeAdjustingFactor(double latitude)
{
	double metersInLatDegreeRetVal = metersInLatDeg(latitude);
	double metersInLongDegRetVal = metersInLongDeg(latitude);
	double longtitudeAdjustFactor = metersInLatDegreeRetVal / metersInLongDegRetVal;

	//checking the calculation
	cout << "meters in Lat deg = " << metersInLatDegreeRetVal << " : meters in adjusted Long deg = " << metersInLongDegRetVal * longtitudeAdjustFactor << endl;
	return longtitudeAdjustFactor;
}

double sm::longtitudeCorrectionFactor(double latitude)
{
	return 1 / longtitudeAdjustingFactor(latitude);
}

double sm::getVecRotFromEast(double ax, double ay)
{
	Mat vec = (Mat_<double>(2, 1) << ax, ay);
	normalize(vec, vec);
	Mat eastVector = (Mat_<double>(2, 1) << 1.0, 0.0);

	double adjustTheSign = vec.at<double>(1) < 0 ? -1.0 : 1.0; //the angle will be negative in case of negative y
	double radAngleWithEast = adjustTheSign * acos(vec.dot(eastVector));
	double degAngleWithEast = radAngleWithEast * (180.0 / M_PI);
	cout << "angle with the vector to east: " << degAngleWithEast << endl;

	return radAngleWithEast;
}

sm::SGcsCoords sm::solve3Kto2Kand1U(const Point2d& p1, const Point2d& p2, const Point2d& p3, SGcsCoords p2Gcs, SGcsCoords p3Gcs)
{
	Mat p1Vec(p1);
	Mat p2Vec(p2);
	Mat p3Vec(p3);
	
	//gcs (Geographic coordinate system) coordinate system
	//first longtitude (x) and then lattitude(y)
	//50.0867628N, 14.4193081E
	Mat gcsP2Vec(p2Gcs.convertToOpenCVFormat());
	//third point 50.0865958N, 14.4192706E 
	Mat gcsP3Vec(p3Gcs.convertToOpenCVFormat());

	//
	double longtitudeAdjustFactor = longtitudeAdjustingFactor(p2Gcs.latitude_);

	//adjust the points to make same meassures in both dirrections
	p2Gcs.longtitude_ *= longtitudeAdjustFactor;
	gcsP2Vec.at<double>(0) *= longtitudeAdjustFactor;
	p3Gcs.longtitude_ *= longtitudeAdjustFactor;
	gcsP3Vec.at<double>(0) *= longtitudeAdjustFactor;

	//angle of vector twoToThree to the vector pointing to east
	Mat gcsDiff = gcsP2Vec - gcsP3Vec;
	double radAngleWithEast = getVecRotFromEast(gcsDiff.at<double>(0), gcsDiff.at<double>(1));

	//p3 to p2
	Mat p3ToP2Vec = p2Vec - p3Vec;
	normalize(p3ToP2Vec, p3ToP2Vec);
	//p6 to p2
	Mat p3ToP1Vec = p1Vec - p3Vec;
	normalize(p3ToP1Vec, p3ToP1Vec);
	cout << "p3ToP1Vec: " << p3ToP1Vec << endl << "p3ToP2Vec" << p3ToP2Vec << endl;
	//angle between vectors twoToThree and twoToZero
	double radAngleAtP3 = acos(p3ToP2Vec.dot(p3ToP1Vec));
	double degAngleAtP3 = radAngleAtP3 * (180.0 / M_PI);
	cout << "angle that is by the point two: " << degAngleAtP3 << endl;

	//create vector from point three to the camera in the gcs
	double radAngleDirVec = radAngleWithEast - radAngleAtP3;//angle between east vector and vector from point three and camera - angle of future direction vector of the line between point three and camera
	Point2d lineDirVec(cos(radAngleDirVec), sin(radAngleDirVec));

	//second point 50.0867628N, 14.4193081E
	//third point 50.0865958N, 14.4192706E 
	//global distance
	double gcsDistance = sm::gcsDistance(sm::SGcsCoords(14.4193081, 50.0867628), sm::SGcsCoords(14.4192706, 50.0865958));
	//calculate distance in our space
	double distance = sm::distance(p2.x, p2.y, p3.x, p3.y);
	double distScaleFactor = distance / gcsDistance;

	std::cout << "gcsDistance: " << gcsDistance << endl;
	std::cout << "our distance: " << distance << endl;
	std::cout << "scale factor of the distances: " << distance / gcsDistance << endl;

	double distanceP3ToP1 = sm::distance(p1.x, p1.y, p3.x, p3.y);
	
	//just fill the parametric formula of a line and get the location of p1 (camera in the project)
	Point2d p1GcsLoc;
	p1GcsLoc.x = p3Gcs.longtitude_ + ((distanceP3ToP1 / distScaleFactor) * lineDirVec.x) / (metersInLongDeg(p3Gcs.latitude_) * longtitudeAdjustFactor);
	p1GcsLoc.y = p3Gcs.latitude_ + ((distanceP3ToP1 / distScaleFactor) * lineDirVec.y) / (metersInLatDeg(p3Gcs.latitude_));

	//converting back to correct longtitude
	p1GcsLoc.x *= longtitudeCorrectionFactor(p3Gcs.latitude_);

	return sm::SGcsCoords(p1GcsLoc.x, p1GcsLoc.y);
}
