#include "CImageBuilder.h"


Ptr<CImage> CImageBuilder::build(const string& imageFilePath, const SProcessParams& params, bool sceneImage, Ptr<CLogger>& logger)
{
    double rightLongtitude, rightLatitude, leftLongtitude, leftlatitude;
    if (params.calcGCSLocation_ && !sceneImage) {
        string filepathWithoutSuffix = sio::getFilePathWithoutSuffix(imageFilePath);
        try {
            // Create a root
            pt::ptree root;
            // Load the json file in this ptree
            // can throw exceptions if the json isn't valid
            pt::read_json(filepathWithoutSuffix + ".json", root);
            rightLongtitude = root.get<double>(IMAGE_RIGHT_BASE_LONGITUDE_JSON_KEY);
            rightLatitude = root.get<double>(IMAGE_RIGHT_BASE_LATITUDE_JSON_KEY);
            leftLongtitude = root.get<double>(IMAGE_LEFT_BASE_LONGITUDE_JSON_KEY);
            leftlatitude = root.get<double>(IMAGE_LEFT_BASE_LATITUDE_JSON_KEY);
        } catch (exception& exc){
            throw ios_base::failure(string("Error occured while reading json: ") + string(exc.what()));
        }
    }
    else {
        rightLongtitude = rightLatitude = leftLongtitude = leftlatitude = 0.0;
    }

    Ptr<CImage> ret = new CImage(imageFilePath,
        sm::SGcsCoords(rightLongtitude, rightLatitude), sm::SGcsCoords(leftLongtitude, leftlatitude));

    return ret;
}
