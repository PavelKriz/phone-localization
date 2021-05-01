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

	//Length in meters of 1° of latitude = at 45 degrees is 111.141548 km
	//Length in meters of 1° of longitude = 40075017 m * cos(latitude) / 360
	double metersInLatDeg = 111141.548;
	double metersInLongDeg = 40075017 * cos(p2Gcs.latitude_) / 360;
	double longtitudeAdjustFactor = metersInLatDeg / metersInLongDeg;

	//checking the calculation
	cout << "meters in Lat deg = " << metersInLatDeg << " : meters in adjusted Long deg = " << metersInLongDeg * longtitudeAdjustFactor << endl;

	//adjust the points to make same meassures in both dirrections
	p2Gcs.longtitude_ *= longtitudeAdjustFactor;
	gcsP2Vec.at<double>(0) *= longtitudeAdjustFactor;
	p3Gcs.longtitude_ *= longtitudeAdjustFactor;
	gcsP3Vec.at<double>(0) *= longtitudeAdjustFactor;

	//angle of vector twoToThree to the vector pointing to east
	Mat gcsDiff = gcsP2Vec - gcsP3Vec;
	normalize(gcsDiff, gcsDiff);
	Mat eastVector = (Mat_<double>(2, 1) << 1.0, 0.0);
	cout << "gcsDiff: " << gcsDiff << endl << "eastVector" << eastVector << endl;
	double radAngleWithEast = acos(gcsDiff.dot(eastVector));
	double degAngleWithEast = radAngleWithEast * (180.0 / M_PI);
	cout << "angle with the vector to east: " << degAngleWithEast << endl;

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
	double adjustTheSign = gcsDiff.at<double>(1) < 0 ? -1.0 : 1.0; //the angle will be negative in case of negative y
	double radAngleDirVec = adjustTheSign * radAngleWithEast - radAngleAtP3;//angle between east vector and vector from point three and camera - angle of future direction vector of the line between point three and camera
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
	
	//just fill the parametric formula of a line and get the location of camera
	Point2d cameraLoc;
	cameraLoc.x = p3Gcs.longtitude_ + ((distanceP3ToP1 / distScaleFactor) * lineDirVec.x) / (metersInLongDeg * longtitudeAdjustFactor);
	cameraLoc.y = p3Gcs.latitude_ + ((distanceP3ToP1 / distScaleFactor) * lineDirVec.y) / (metersInLatDeg);

	//converting back to correct longtitude
	cameraLoc.x /= longtitudeAdjustFactor;

	cout << setprecision(9) << "camera location: " << cameraLoc << endl;

	return sm::SGcsCoords(cameraLoc.x, cameraLoc.y);
}
