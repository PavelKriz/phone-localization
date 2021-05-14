//----------------------------------------------------------------------------------------
/**
 * \file       CImageBuilder.h
 * \author     Pavel Kriz
 * \date       10/5/2021
 * \brief      Contains class that correctly builds CImage class
 *
 *  It loads geolocation informations (image data are specificaly loaded by the CImage class)
 *
*/
//----------------------------------------------------------------------------------------

#pragma once

//loading boost - used for loading json
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
namespace pt = boost::property_tree;
//Matrices
#include <opencv2/core/mat.hpp>
//reading writing
#include <opencv2/imgcodecs.hpp>

#include "CImage.h"
#include "SGcsCoords.h"
#include "SProcessParams.h"
#include "SpecializedInputOutput.h"
#include "parameters.h"

/**
 * @brief Class that correctly builds the CImage objects
 * 
 *  It loads geolocation informations (image data are specificaly loaded by the CImage class)
 * 
*/
class CImageBuilder
{
public:
    /**
     * @brief Method that builds the CImage
     * @param imageFilePath filepath to the image relative to the place where it runs 
     *
     * Note to the images: not only image but also JSON has to be in the filepath location, because there has to be JSOn of the same name but the suffix
     * 
     * @param params parameters that determine which algorithms would be passed to the CImage. See the notes of the CImage constructor on that topic!
     * @param sceneImage information whether the image to be constructed is going to be a scene image or not
     * @param logger logger in which it will print information about the process
     * @return smart OpenCV pointer to the newly constructed CImage
     * @throw ios_base::failure in case of any io failure
    */
    Ptr<CImage> build(const string& imageFilePath, const SProcessParams& params, bool sceneImage, Ptr<CLogger>& logger);
};

