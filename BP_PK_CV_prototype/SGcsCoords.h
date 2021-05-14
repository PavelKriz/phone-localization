//----------------------------------------------------------------------------------------
/**
 * \file       SpaceModule.h
 * \author     Pavel Kriz
 * \date       2/5/2021
 * \brief      Contains structure representing coordinates in global coordinate system coordinates
*/
//----------------------------------------------------------------------------------------
#pragma once

//3d, 2d point structures and others
#include <opencv2/core/types.hpp>

using namespace cv;

namespace sm {
    /**
     * @brief Global coordinate system coordinates
     */
    struct SGcsCoords {
        double longitude = 0.0; ///< global coordinate longitude
        double latitude_ = 0.0; ///< global coordinate latitude
        /**
         * @brief Constructor is here for obvious(in code)/safe struct initialisation
         * @param longitude global coordinate longitude
         * @param latitude global coordinate latitude
        */
        SGcsCoords(double longitude, double latitude) : longitude(longitude), latitude_(latitude) {}
        /**
         * @brief constructs gcs coordinates from opencv 2d point
         * @param position x has to be longitude and y has to be latitude
        */
        SGcsCoords(Point2d position) : longitude(position.x), latitude_(position.y) {}
        /**
         * @brief Returns the coordinates in the OpenCV format
         * @return the coordinates in Point2D (longitude = x, latitude = y)
        */
        Point2d convertToOpenCVFormat() const { return Point2d(longitude, latitude_); }
    };
}