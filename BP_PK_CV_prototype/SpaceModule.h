//----------------------------------------------------------------------------------------
/**
 * \file       SpaceModule.h
 * \author     Pavel Kriz
 * \date       1/5/2021
 * \brief      Contains functions computing different problems in 3D, 2D and geographical space.
*/
//----------------------------------------------------------------------------------------


#pragma once

//for the constants
#define _USE_MATH_DEFINES
#include<cmath>
//output
#include <iostream>
//manipulating the stream
#include <iomanip>
//3d, 2d point structures and others
#include <opencv2/core/types.hpp>
//matrix
#include <opencv2/core/mat.hpp>
//core functions
#include <opencv2/core.hpp>

#include "SGcsCoords.h"
#include "CLogger.h"

using namespace cv;
using namespace std;

namespace sm {
    /**
     * @brief globaly considered average of earth radius
    */
    const double EARTH_RADIUS_IN_METERS = 6371000;
    /**
     * @brief average height of human body in Czech Republic across genders and ages
    */
    const double AVERAGE_CZECH_HUMAN_HEIGHT = 1.75; // in meters
    /**
     * @brief average human height that has to be considered in calculations
    */
    const double HUMAN_HEIGHT = AVERAGE_CZECH_HUMAN_HEIGHT;
    /**
     * @brief offset of camera in the height of the top of head
    */
    const double CAMERA_HOLDING_OFFSET = 0.2; // in meters
    
    /**
     * @brief returns distance in meters between two points in global coordinate system
     * @param first point with coordinates in degrees
     * @param second point with coordinates in degrees
     * @return distance in meters between first and second
    */
    double gcsDistance(SGcsCoords first, SGcsCoords second);
    /**
     * @brief Calculates Euclidean distance between points a and b in 2D
     * @param ax x coordinate of point a
     * @param ay y coordinate of point a
     * @param bx x coordinate of point b
     * @param by y coordinate of point b 
     * @return distance between points a and b
    */
    double distance(double ax, double ay, double bx, double by);
    /**
     * @brief Calculates Euclidean distance between points a and b in 3D
     * @param ax x coordinate of point a
     * @param ay y coordinate of point a
     * @param az z coordinate of point a
     * @param bx x coordinate of point b
     * @param by y coordinate of point b
     * @param bz z coordinate of point b
     * @return distance between points a and b
    */
    double distance(double ax, double ay, double az, double bx, double by, double bz);
    
    /**
     * @brief Calculate point between A and B (halfway on the line from A to B)
     * method averages these two points to get the result
     * @param ax x coordinate of point a
     * @param ay y coordinate of point a
     * @param bx x coordinate of point b
     * @param by y coordinate of point b
     * @return the midpoint
    */
    Point2d getMidPoint2D(double ax, double ay, double bx, double by);
    /**
     * @brief From (x,y,z) creates (x,y,z,1)^T matrix
     * @param x coordinate
     * @param y coordinate
     * @param z coordinate
     * @return the homogeneus coordinates
    */
    Mat getHomogenousVector3D(double x, double y, double z);
    /**
     * @brief Get how many meters are in one latitude degree for given latitude
     * @param latitude latitude of the space that is being evaluated
     * @return meters in one latitude degree
    */
    double metersInLatDeg(double latitude);
    /**
     * @brief Get how many meters are in one longitude degree for given latitude
     * @param latitude latitude of the space that is being evaluated
     * @return meters in one longitude degree
    */
    double metersInLongDeg(double latitude);
    /**
     * @brief Factor ALPHA that does this at given latitude: ALPHA * longitude = latitude
     * @param latitude given latitude
     * @return the factor ALPHA
    */
    double longtitudeAdjustingFactor(double latitude);

    /**
     * @brief Returns the inverse of the factor from longtitudeAdjustingFactor method call (for the same latitude)
     * @param latitude given latitude
     * @return the factor ALPHA^-1
    */
    double longtitudeCorrectionFactor(double latitude);
    /**
     * @brief Convertion from radians to degrees
     * @param radAngle angle in radians
     * @return angle in degrees
    */
    double radToDeg(double radAngle);
    /**
     * @brief Convertion from degrees to radians
     * @param degAngle angle in degrees
     * @return angle in radians
    */
    double degToRad(double degAngle);
    /**
     * @brief Angle between two vectors defined by points ABC, first vector: A - B, second vector C - B
     * @param ax x point A coordinate
     * @param ay y point A coordinate
     * @param bx x point B coordinate
     * @param by y point B coordinate
     * @param cx x point C coordinate
     * @param cy y point C coordinate
     * @return the angle between the two vectors
    */
    double vectorAngle2D(double ax, double ay, double bx, double by, double cx, double cy);
    /**
     * @brief return the angle between given vector and vector pointing to east (1.0, 0.0)
     * 
     * for negative Y is the angle negative
     * 
     * @param ax x coordinate of give vector
     * @param ay y coordinate of give vector
     * @return the angle in radians
    */
    double getVecRotFromEast(double ax, double ay);

    /**
     * @brief computes global coordinates (global coordinates system) for third point
     * 
     * the local space geometry is known for all points (p1, p2, p3) and global coordniates are known for p2 and p3
     * the points are in such order that if they would create triangle they would be named in counterclockwise manner
     * 
     * @param p1 the local space geometry point
     * @param p2 the local space geometry point
     * @param p3 the local space geometry point
     * @param p2Gcs global location of p2
     * @param p3Gcs global location of p3
     * @return global coordinates of p1
    */
    SGcsCoords solve3Kto2Kand1U(const Point2d& p1, const Point2d& p2, const Point2d& p3,
        const SGcsCoords& p2Gcs, const SGcsCoords& p3Gcs, Ptr<CLogger>& logger);
}