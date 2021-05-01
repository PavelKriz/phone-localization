//----------------------------------------------------------------------------------------
/**
 * \file       CImageLocator3D.h
 * \author     Pavel Kriz
 * \date       1/5/2021
 * \brief      
*/
//----------------------------------------------------------------------------------------

#pragma once

#include <vector>
#include <iostream>
//max/min function
#include <algorithm>

//3d, 2d point structures and others
#include <opencv2/core/types.hpp>
//matrix
#include <opencv2/core/mat.hpp>
//core functions
#include <opencv2/core.hpp>
//opencv 3d manipulation tools
#include <opencv2/calib3d.hpp>
//quaternion
#include <opencv2/core/quaternion.hpp>

#include "CLogger.h"
#include "CImage.h"
#include "SpaceModule.h"
#include "SProcessParams.h"

using namespace std;
using namespace cv;

class CImageLocator3D
{
    Ptr<CImage> sceneImage_;
    Mat RTMatrix_; // rotation-translation matrix - from world space to local camera space
    Mat TVec_;
    Mat RVec_;
    Mat distCoeffs_;
    Mat cameraIntrinsicsMatrixA_;
    const SProcessParams params_;

    void createCameraIntrinsicsMatrix(const SCameraInfo& cameraInfo);
    Mat projectCameraSpacetoImage(const Mat& toProject) const;
    Mat projectWorldSpacetoImage(const Mat& toProject) const;
    void projectBuildingDraftIntoScene(const vector<Point3d>& objCorners3D, Ptr<CLogger>& logger) const;
    Mat getCorrectionMatrixForTheCameraLocalSpace(const Mat& p1Vec, const Mat& p2Vec, const Mat& p3Vec,
        const sm::SGcsCoords& gcsP2Vec, const sm::SGcsCoords& gcsP3Vec);
public:
    CImageLocator3D(const Ptr<CImage>& sceneImage, const SProcessParams& params)
        :
        sceneImage_(sceneImage),
        params_(params)
    {
        createCameraIntrinsicsMatrix(params.cameraInfo_);
    }
    void calcLocation(vector<Point2f>& obj_corners, vector<Point2f>& sceneCorners, Ptr<CLogger>& logger);
};

