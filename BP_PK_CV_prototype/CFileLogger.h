//----------------------------------------------------------------------------------------
/**
 * \file       CFileLogger.h
 * \author     Pavel Kriz
 * \date       9/4/2021
 * \brief      Contains class for logging output from the app to the file
 *
 *  It is designed for printing result after the run of the app (untill then it stores everything that is passed to be printed)
 *
*/
//----------------------------------------------------------------------------------------

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>


//reading writing images
#include <opencv2/imgcodecs.hpp>

#include "COstreamLogger.h"
#include "parameters.h"

using namespace std;

/**
 * @brief Class for logging output from the app to the file
 * 
 * It is designed for printing result after the run of the app (untill then it stores everything that is passed to be printed)
 * 
 * The real output is done with function flush
 * The flush function has to be called always at the end or every time when the result is immidiately needed
 * 
*/
class CFileLogger : public COstreamLogger {
protected:
	ostringstream out_; ///< stream to it the method prints until
	const string outputRoot_; ///< test root filepath - in relation to the run of the app
	const string runName_; ///< name of the current test
	bool emergencyMode_ = false; ///< if true the logger logs into console
	bool wasFlushed_ = false; ///< information whether the flush method was called at least once
	/**
	 * @brief It returns the stream that the logger is using
	 * the logger is normaly using file stream but in emergency mode (file output doesn´t work) it uses console
	 * @return It returns internal ostream (buffer)
	*/
	inline virtual ostream& out() override { return emergencyMode_ ? cout : out_; }
	/**
	 * @brief the logger will start to log into console
	*/
	virtual void emergencyStart() override { emergencyMode_ = true; }
	/**
	 * @brief the logger will start behaving as normally
	*/
	virtual void emergencyEnd() override { emergencyMode_ = false; }
public:
	/**
	 * @brief empty constructor is deleted
	*/
	CFileLogger() = delete;
	//file logger allways produces images
	/**
	 * @brief Constructor of the logger
	 * @param outputRoot test root filepath - in relation to the run of the app
	 * @param runName name of the current test
	 * @param timing information whether the timing optimalization will take place and the images wont be saved
	*/
	CFileLogger(const string& outputRoot, const string & runName, bool timing);
	/**
	 * @brief default destructor
	*/
	virtual ~CFileLogger() override { flush(); }
	/**
	 * @brief Flushes (saves) every images or text to the output (implementation)
	 * 
	 * The flush function has to be called always at the end or every time when the result is immidiately needed
	 * 
	 * It clears the buffered images and text after saving
	 * 
	 * On first flush call it truncate the text output file it saves to, on other calles it appends to the file
	*/
	virtual void flush() override;
};