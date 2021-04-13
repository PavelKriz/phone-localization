//----------------------------------------------------------------------------------------
/**
 * \file       CRuntimeLogger.h
 * \author     Pavel Kriz
 * \date       9/4/2021
 * \brief      Contains class for logging output from the app to the console
 *
 *  It is designed for printing result in the runtime to give results immidiately if the timing optimalization isn't set
 *
*/
//----------------------------------------------------------------------------------------

#pragma once

#include <iostream>
#include "COstreamLogger.h"

//reading writing images
#include <opencv2/imgcodecs.hpp>
//showing image functions
#include <opencv2/highgui.hpp>

using namespace std;

/**
 * @brief Class for logging output from the app to the console
 * 
 * It is designed for printing result in the runtime to give results immidiately if the timing optimalization isn't set
 * 
 * The flush function has to be called always at the end or every time when the result is immidiately needed
 * 
*/
class CRuntimeLogger : public COstreamLogger {
protected:
	/**
	 * @brief It returns the stream that the logger is using
	 * @return it returns std::cout
	*/
	inline virtual ostream& out() override { return cout; }
public:
	/**
	 * @brief Empty constructor is deleted.
	*/
	CRuntimeLogger() = delete;
	/**
	 * @brief Constructor of the class
	 * @param timing information whether the timing optimalization will take place and the images wont be saved
	*/
	CRuntimeLogger(bool timing);
	/**
	 * @brief default virtual destructor
	*/
	virtual ~CRuntimeLogger() override { flush(); }
	/**
	 * @brief Flushes every images or text to the output (implementation)
	 * 
	 * It clears the buffered images after displaying
	 * 
	*/
	virtual void flush() override;
};


