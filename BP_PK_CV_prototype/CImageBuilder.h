//----------------------------------------------------------------------------------------
/**
 * \file       CImageBuilder.h
 * \author     Pavel Kriz
 * \date       10/5/2021
 * \brief      Contains class that correctly constructs CImage class
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


class CImageBuilder
{
public:
    Ptr<CImage> build(const string& imageFilePath, const SProcessParams& params, bool sceneImage, Ptr<CLogger>& logger);
};

