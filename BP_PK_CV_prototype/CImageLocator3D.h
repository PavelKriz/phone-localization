//----------------------------------------------------------------------------------------
/**
 * \file       CImageLocator3D.h
 * \author     Pavel Kriz
 * \date       1/5/2021
 * \brief      Class used to locate the camera of the image in the space (relative towards the building and global position)
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
#include "parameters.h"

using namespace std;
using namespace cv;

class CImageLocator3D
{
    Ptr<CImage> sceneImage_;
    Ptr<CImage> objectImage_;
    Mat RTMatrix_; // rotation-translation matrix - from world space to local camera space
    Mat RMatrix_;
    Mat TVec_;
    Mat RVec_;
    Mat distCoeffs_;
    Mat cameraIntrinsicsMatrixA_;
    Mat worldToImageProjectionMat_;
    const SProcessParams params_;
    sm::SGcsCoords cameraGcsLoc_;
    Quatd flatGroundObjRotationFromEast_; ///<rotation around y axis
    bool projectionProcessed_ = false; //information whether all the important calculations regarding projection locating have been done
    bool gcsProcessed_ = false; //information whether all the important calculations regarding gcs locating have been done

    void createCameraIntrinsicsMatrix(const SCameraInfo& cameraInfo);
    void createTransformationMatrices();
    Mat projectCameraSpacetoImage(const Mat& toProject) const;
    Mat projectWorldSpacetoImage(const Mat& toProject) const;
    void projectBuildingDraftIntoScene(const vector<Point3d>& objCorners3D, Ptr<CLogger>& logger) const;
    //3d points have to be in 4x1 format (homogenous coordinates)
    Mat getCorrectionMatrixForTheCameraLocalSpace(const Mat& p1Vec, const Mat& p2Vec, const Mat& p3Vec,
        const sm::SGcsCoords& gcsP2, const sm::SGcsCoords& gcsP3, Ptr<CLogger>& logger);
    double computeFlatRotation(const sm::SGcsCoords& gcsCamera,const sm::SGcsCoords& gcsP2, const sm::SGcsCoords& gcsP3);
    void gcsLocatingProblemFrom3Dto2D(vector<Point3d> objCorners3D, vector<Point2d>& sceneCorners,
        const sm::SGcsCoords& gcsPoint2, const sm::SGcsCoords& gcsPoint3, Point2d& p2Out, Point2d& p3Out, Ptr<CLogger>& logger);
public:
    CImageLocator3D(const Ptr<CImage>& sceneImage, const Ptr<CImage>& objectImage, const SProcessParams& params)
        :
        sceneImage_(sceneImage),
        objectImage_(objectImage),
        params_(params),
        cameraGcsLoc_(0.0, 0.0)
    {
        createCameraIntrinsicsMatrix(params.cameraInfo_);
    }
    void calcLocation(vector<Point2d>& obj_corners, vector<Point2d>& sceneCorners, Ptr<CLogger>& logger);
};

