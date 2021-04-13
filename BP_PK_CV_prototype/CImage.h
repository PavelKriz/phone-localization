//----------------------------------------------------------------------------------------
/**
 * \file       CImage.h
 * \author     Pavel Kriz
 * \date       9/4/2021
 * \brief      Contains class that manages the image in context to the keypoint detecting and describing (also some preprocessing)
 *
 * It can load image, detect and describe keypoints with given algorithm (it has to be implemented)
 * 
 * Usage is following: construct object -> create detector extractor (static method createDetectorExtractor) ->call method called process
 *
*/
//----------------------------------------------------------------------------------------

#pragma once

//enabling and disabling experimental and nonfree modules
#include "experimentalModules.h"

#include <vector>

//Matrices
#include <opencv2/core/mat.hpp>
//reading writing
#include <opencv2/imgcodecs.hpp>
//features - detecting, describing
#include <opencv2/features2d.hpp>
#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
//experimental feature algorithms - eg. BEBLID
#include <opencv2/xfeatures2d.hpp>
#endif
//clahe and other image processing
#include <opencv2/imgproc.hpp>

#include "CLogger.h"
#include "SProcessParams.h"

#include <iostream>

using namespace std;
using namespace cv;

/**
 * @brief Class that manages the image in context to the keypoint detecting and describing (also some preprocessing)
 * 
 *  It can load image, detect and describe keypoints with given algorithm (it has to be implemented)
 * 
 * Usage is following: construct object -> create detector extractor (static method createDetectorExtractor) -> call method called process
 * 
 * (preferably all classes should be called with the same detectorExtractor object)
 * 
*/
class CImage {
public:
	/**
	 * @brief class that solves the problem of having more OpenCV detector and extractors for many CImage classes
	 * class can be created only by CImage method
	*/
	class CDetectorExtractor {
		friend class CImage;

		Ptr<Feature2D> detector_; ///< pointer to the detector object
		Ptr<Feature2D> extractor_; ///< pointer to the extractor object
		/**
		 * @brief constructor that constructs the class according to the passed parameters
		 * @param params parameters on which base the class is being constructed
		*/
		CDetectorExtractor(const SProcessParams& params);
		/**
		 * @brief returns pointer to correct detector object (correct according to the params that are passed)
		 * @param params params on which base the detector is created (detector or/and descriptor creator)
		 * @return returns the smart pointer to the detector
		 */
		static Ptr<Feature2D> getDetector(const SProcessParams& params);
		/**
		 * @brief  returns pointer to correct descriptor creator object (correct according to the params that are passed)
		 * @param params  params on which base the descriptor creator is made
		 * @return returns the smart pointer to the descriptor creator object
		*/
		static Ptr<Feature2D> getExtractor(const SProcessParams& params);
		/**
		 * @brief returns pointer to correct detector or/and descriptor creator object (correct according to the params that are passed)
		 * @param params params on which base the detector is created (detector or/and descriptor creator)
		 * @return returns the smart pointer to the detector or/and descriptor creator
		*/
		static Ptr<Feature2D> getDetectorExtractor(const SProcessParams& params);
	};

protected:
	const string filePath_; ///< filepath of the image (with the image itself, relative to the place where the app is running)
	Mat image_; ///< image data in the OpenCV matrix
	bool wasProcessed_ = false; ///< information whether the keypoints have been detected and described
	vector<KeyPoint> imageKeypoints_; ///< vector with the detected keypoints (it is valid when wasProcessed is set to true)
	Mat keypointsDescriptors_; ///< descriptors of the keypoints (it is valid when wasProcessed is set to true)

	/**
	 * @brief computes both keypoints and theirs descriptors, throws invalid argument
	 * @param params parameters that determine which algorithms would be used to detect features and which one used to describe them
	 * @param logger logger in which it will print information about the process
	 * @throw invalid_argument if there the detectorExtractor is empty
	*/
	void detectDescribeFeatures(const SProcessParams& params, Ptr<CLogger>& logger, const Ptr<CDetectorExtractor>& detectorExtractor);
	/**
	 * @brief it proceses a CLAHE (Contrast Limited Adaptive Histogram Equalisation) algorithm over the image 
	 * @param logger the logging output is printed in the logger
	*/
	void processCLAHE(Ptr<CLogger>& logger);
	/**
	 * @brief does change the inner descriptor matrix into descriptors based on Helloinger kernel
	 * this method is precise enough
	 * @param logger the logging output is printed in the logger
	*/
	void fastRootSiftDescriptorsAdjust(Ptr<CLogger>& logger);
	/**
	 * @brief does change the inner descriptor matrix into descriptors based on Helloinger kernel
	 * this method is a bit more precise then the casual faster method but there is not a big difference both in accuracy gain and speed loss
	 * @param logger the logging output is printed in the logger
	*/
	void preciseRootSiftDescriptorsAdjust(Ptr<CLogger>& logger);
public:
	/**
	 * @brief Constructor (the image is loaded during the constructor run)
	 * @param filePath filepath of the image (with the image itself, relative to the place where the app is running)
	 * @throw ios_base::failure
	*/
	CImage(const string& filePath);
	/**
	 * @brief method that creates nested class object that is neede for the image to be processed (the object can be used for infinite amount of CImage classes)
	 * @param params parameters that determine which algorithms would be used to detect features and which one used to describe them
	 * @return the detector extractor object
	*/
	static Ptr<CDetectorExtractor> createDetectorExtractor(const SProcessParams& params);
	/**
	 * @brief method that runs all the preprocessing of the image, detection and descripion of the keypoints
	 * @param params parameters that determine which algorithms would be used to detect features and which one used to describe them
	 * @param logger logger in which it will print information about the process
	*/
	void process(const SProcessParams& params, Ptr<CLogger>& logger, const Ptr<CDetectorExtractor>& detectorExtractor);
	/**
	 * @brief Gives relative filepath of the image (with the image name itself)
	 * @return the filepath
	*/
	const string& getFilePath() const { return filePath_; }
	/**
	 * @brief Gives the image data
	 * @return the image data in OpenCV format (matrix)
	*/
	const Mat& getImage() const { return image_; }
	/**
	 * @brief Gives keypoints
	 * @return vector of keypoints
	 * @throw logic_error message: "CImage - to get keypoints first the process function has to be called."
	*/
	const vector<KeyPoint>& getKeypoints() const;
	/**
	 * @brief Gives descriptors of the image keypoints
	 * @return descriptors of the keypoints (matrix)
	 * @throw logic_error message: "CImage - to get descriptors of the keypoints first the process function has to be called."
	*/
	const Mat& getDescriptors() const;
};