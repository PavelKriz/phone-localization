//----------------------------------------------------------------------------------------
/**
 * \file       SpaceModule.h
 * \author     Pavel Kriz
 * \date       1/5/2021
 * \brief      Contains structures and functions computing different problems in 3D, 2D and geographical space.
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
     * @brief Global coordinate system coordinates
    */
    struct SGcsCoords {
        double longtitude_ = 0.0;
        double latitude_ = 0.0;
        SGcsCoords(double longtitude, double latitude) : longtitude_(longtitude), latitude_(latitude) {}
        /**
         * @brief constructs gcs coordinates from opencv 2d point
         * @param position x has to be longtitude and y has to be latitude
        */
        SGcsCoords(Point2d position) : longtitude_(position.x), latitude_(position.y){}
        Point2d convertToOpenCVFormat() { return Point2d(longtitude_, latitude_); }
    };
    

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
    SGcsCoords solve3Kto2Kand1U(const Point2d& p1, const Point2d& p2, const Point2d& p3, SGcsCoords p2Gcs, SGcsCoords p3Gcs);
}