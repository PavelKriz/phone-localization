//----------------------------------------------------------------------------------------
/**
 * \file       CLogger.h
 * \author     Pavel Kriz
 * \date       9/4/2021
 * \brief      Contains virtual class for logging output from the app
 *
 *  Basic logging Interface can be used in following manner logger->log("").log(to_string(4).endl();
 *
*/
//----------------------------------------------------------------------------------------

#pragma once

//Matrices
#include <opencv2/core/mat.hpp>

#include <string>

using namespace std;
using namespace cv;

/**
 * @brief  Basic logging interface (virtual class)
*/
class CLogger {
protected:
	bool putImages_; ///< parameter that determines whether the images are saved during the run of the app
public:
	/**
	 * @brief Constructor of the class
	 * @param timing information whether the timing optimalization will take place
	*/
	CLogger(bool timing) { timing ? putImages_ = false : putImages_ = true; }
	/**
	 * @brief empty virtual destructor
	*/
	virtual ~CLogger() {}
	/**
	 * @brief Logs the begening of the section (prints its name). It should be used when starting a section
	 * @param name name of the section
	 * @param level level of the section and the header. There are three options (highest level > 0,1,2 < lowest level), 0,1 are printed with capitals
	 * @return returns reference on this CLogger
	*/
	virtual CLogger& logSection(const string& name, unsigned int level = 0) = 0;
	/**
	 * @brief Logs in the current section with the right indent.
	 * @param toLog string with text to be logged
	 * @return reference on this CLogger
	*/
	virtual CLogger& log(const string& toLog) = 0;
	/**
	 * @brief Method specialised for logging errors
	 * @param toLog string with error message to be logged
	 * @return reference on this CLogger
	*/
	virtual CLogger& logError(const string& toLog) = 0;
	/**
	 * @brief Does the right end of the line in the style of the logger.
	 * @return reference on this CLogger
	*/
	virtual CLogger& endl() = 0;
	/**
	 * @brief Puts the image to the output queue to be logged (printed or saved)
	 * @param image image to be logged (printed or saved)
	 * @param outputPath filepath in regard to the place where the app runs
	*/
	virtual void putImage(const Mat& image, const string& outputPath) = 0;
	/**
	 * @brief Flushes every images or text to the output
	*/
	virtual void flush() = 0;
};