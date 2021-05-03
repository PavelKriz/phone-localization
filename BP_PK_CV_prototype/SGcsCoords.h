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
        double longtitude_ = 0.0;
        double latitude_ = 0.0;
        SGcsCoords(double longtitude, double latitude) : longtitude_(longtitude), latitude_(latitude) {}
        /**
         * @brief constructs gcs coordinates from opencv 2d point
         * @param position x has to be longtitude and y has to be latitude
        */
        SGcsCoords(Point2d position) : longtitude_(position.x), latitude_(position.y) {}
        Point2d convertToOpenCVFormat() const { return Point2d(longtitude_, latitude_); }
    };
}