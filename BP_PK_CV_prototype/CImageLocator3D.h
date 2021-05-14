//----------------------------------------------------------------------------------------
/**
 * \file       CImageLocator3D.h
 * \author     Pavel Kriz
 * \date       1/5/2021
 * \brief      File containing class used to locate the camera of the image in the space (relative towards the building and global position)
 * 
 * usage: construct and then call CalcLocation
 * 
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

/**
 * @brief Class used to locate the camera of the image in the space (relative towards the building and global position)
 * 
 * usage: construct and then call CalcLocation
 * 
*/
class CImageLocator3D
{
    Ptr<CImage> sceneImage_; ///< scene image (image data)
    Ptr<CImage> objectImage_; ///< reference image (image data)
    Mat RTMatrix_; ///< rotation-translation matrix - from world space to local camera space
    Mat RMatrix_; ///< rotation part of the RTMatrix_
    Mat TVec_; ///< translation part of the RTMatrix_
    Mat RVec_; ///< rotation (RMatrix_) in a vector form (can be transformed from matrix by using OpenCV function Rodrigues and vice versa)
    Mat distCoeffs_; ///< distortion coefficients of the scene image
    Mat cameraIntrinsicsMatrixA_; ///< camera intrinsinc parameters matrix created by createCameraIntrinsicsMatrix
    Mat worldToImageProjectionMat_; ///< matrix that projects given point in world space to the image 
    const SProcessParams params_; ///< used processing parameters
    sm::SGcsCoords cameraGcsLoc_; ///< "GPS" location of the camera
    Quatd flatGroundObjRotationFromEast_; ///< rotation around y axis (quaternion)
    bool projectionProcessed_ = false; ///< information whether all the important calculations regarding projection locating have been done
    bool gcsProcessed_ = false; ///< information whether all the important calculations regarding gcs locating have been done

    /**
     * @brief Creates the camera intrinsics matrix from the camera information
     * 
     * constructs (fills in) the cameraIntrinsicsMatrixA_ inner matrix
     * 
     * @param cameraInfo the camera intrinsic parameters
    */
    void createCameraIntrinsicsMatrix(const SCameraInfo& cameraInfo);
    /**
     * @brief Creates all needed matrices from the rotation, translation
     *
     * creates (fills in) RTMatrix_ and worldToImageProjectionMat_
     * 
    */
    void createTransformationMatrices();
    /**
     * @brief Method that projects given point in camera local space to the image
     *
     * the needed matrix cameraIntrinsicsMatrixA_ is calculated in the class constructing process in method createCameraIntrinsicsMatrix
     *
     * @param toProject point to be projected in homogenous coordinates (size 4x1 - column vector)
     * @return the projected point (2x1 row matrix)
    */
    Mat projectCameraSpacetoImage(const Mat& toProject) const;
    /**
     * @brief Method that projects given point in world space to the image 
     * 
     * worldToImageProjectionMat_ has to be computed by the time it is called (createTransformationMatrices method creates it)
     * 
     * @param toProject point to be projected in homogenous coordinates (size 4x1 - column vector)
     * @return the projected point (2x1 row matrix)
    */
    Mat projectWorldSpacetoImage(const Mat& toProject) const;
    /**
     * @brief It projects the building volume (guessed, because we dont know the depth) into the screan
     * 
     * The projected volume is more a bounding volume (bounding box)
     * 
     * @param objCorners3D 3D reference image corners (0, 0, 1), (w, 0, 1), (w, h, 1), (0, h, 1) transforemed in the camera local space
     * @param logger into which is the optional processing info logged
    */
    void projectBuildingDraftIntoScene(const vector<Point3d>& objCorners3D, Ptr<CLogger>& logger) const;
    /**
     * @brief Method that returns correction matrix that corrects the local camera space into centered space around the point directly underneath the camera
     * 
     * it is the inner parameter in params_ set so the standing person optimalisation solution is included in the correction matrix
     * 
     * 3d points have to be in 4x1 format (homogenous coordinates)
     * 
     * @param p1Vec 3D reference image corner (w, 0, 1, 1) transforemed in the camera local space 
     * @param p2Vec 3D reference image corner (w, h, 1, 1) transforemed in the camera local space
     * @param p3Vec 3D reference image corner (0, h, 1, 1) transforemed in the camera local space
     * @param gcsP2 gcs (GPS) location of the p2Vec
     * @param gcsP3 gcs (GPS) location of the p3Vec
     * @param logger into which is the optional processing info logged
     * @return the correction matrix
    */
    Mat getCorrectionMatrixForTheCameraLocalSpace(const Mat& p1HomVec, const Mat& p2HomVec, const Mat& p3HomVec,
        const sm::SGcsCoords& gcsP2, const sm::SGcsCoords& gcsP3, Ptr<CLogger>& logger);
    /**
     * @brief Computes flat rotation from the east vector (1, 0) to the direction vector pointing to the midpoint between gcsP2 and gcsP3
     * @param gcsCamera camera GCS/GPS location
     * @param gcsP2 gcs (GPS) location of the 3D reference image corner (w, h, 1, 1) transforemed in the camera local space
     * @param gcsP3 gcs (GPS) location of the 3D reference image corner (0, h, 1, 1) transforemed in the camera local space
     * @return the rotation in radians
    */
    double computeFlatRotation(const sm::SGcsCoords& gcsCamera,const sm::SGcsCoords& gcsP2, const sm::SGcsCoords& gcsP3);
    /**
     * @brief It transforms the localisational problem from 3D to 2D (projected on ground) including possible optimalisations
     * 
     * it uses the method getCorrectionMatrixForTheCameraLocalSpace()
     * 
     * @param objCorners3D 3D reference image corners (0, 0, 1), (w, 0, 1), (w, h, 1), (0, h, 1) transforemed in the camera local space
     * @param sceneCorners 2D projections of the obj_corners in the scene image ( object_corners = corners of the reference image (w,h = reference image width and height) - (0, 0)^T, (w, 0)^T, (w, h)^T, (0, h)^T)
     * @param gcsPoint2 objCorners3D[2] point's corresponding real world cordinates (GCS/GPS)
     * @param gcsPoint3 objCorners3D[3] point's corresponding real world cordinates (GCS/GPS)
     * @param p2Out point objCorners3D[2] projected on the flat ground (with optimalisations or simple without)
     * @param p3Out point objCorners3D[3] projected on the flat ground (with optimalisations or simple without)
     * @param logger into which is the optional processing info logged
    */
    void gcsLocatingProblemFrom3Dto2D(vector<Point3d> objCorners3D, vector<Point2d>& sceneCorners,
        const sm::SGcsCoords& gcsPoint2, const sm::SGcsCoords& gcsPoint3, Point2d& p2Out, Point2d& p3Out, Ptr<CLogger>& logger);
public:
    /**
     * @brief Constructor of the CImageLocator
     * @param sceneImage the scene image that was queried to be recognised and there is the building to be located somewhere (already located)
     * @param objectImage reference image that was already located in the scene image
     * @param params parameters determining the processing configuration
    */
    CImageLocator3D(const Ptr<CImage>& sceneImage, const Ptr<CImage>& objectImage, const SProcessParams& params)
        :
        sceneImage_(sceneImage),
        objectImage_(objectImage),
        params_(params),
        cameraGcsLoc_(0.0, 0.0)
    {
        createCameraIntrinsicsMatrix(params.cameraInfo_);
    }
    /**
     * @brief Method called to calculate the location (relative 3D transformation and GCS/GPS location)
     * @param obj_corners corners of the reference image (w,h = reference image width and height) - (0, 0)^T, (w, 0)^T, (w, h)^T, (0, h)^T
     * @param sceneCorners 2D projections of the obj_corners in the scene image
     * @param logger logger to which the processing and results are outputed
    */
    void calcLocation(vector<Point2d>& obj_corners, vector<Point2d>& sceneCorners, Ptr<CLogger>& logger);
};

