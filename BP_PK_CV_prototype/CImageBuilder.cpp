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
            rightLongtitude = root.get<double>("rightBase.longtitude");
            rightLatitude = root.get<double>("rightBase.latitude");
            leftLongtitude = root.get<double>("leftBase.longtitude");
            leftlatitude = root.get<double>("leftBase.latitude");
        } catch (exception& exc){
            throw invalid_argument(string("Error occured while reading json: ") + string(exc.what()));
        }
    }
    else {
        rightLongtitude = rightLatitude = leftLongtitude = leftlatitude = 0.0;
    }

    Ptr<CImage> ret = new CImage(imageFilePath,
        sm::SGcsCoords(rightLongtitude, rightLatitude), sm::SGcsCoords(leftLongtitude, leftlatitude));

    return ret;
}
