//----------------------------------------------------------------------------------------
/**
 * \file       COperator.h
 * \author     Pavel Kriz
 * \date       9/4/2021
 * \brief      Contains class that operates run of the application.
 *
 *  Class holds static methods that execute the program and initiate the parameters that are being passed in the app
 *
*/
//----------------------------------------------------------------------------------------

#pragma once

//enabling and disabling experimental and nonfree modules
#include "experimentalModules.h"

//project includes
#include "CObjectInSceneFinder.h"
#include "SProcessParams.h"
#include "parameters.h"
#include "CFileLoader.h"

using namespace std;

/**
 * @brief Class holds static methods that execute the program and initiate the parameters that are being passed in the app
 * 
 * 1. it calls the file configuration loading 
 * 2. creates loggers
 * 3. runs the calculation objects
 * 
*/
class COperator
{
    static void setAdvancedParams(CFileLoader& loader);
    /**
     * @brief Get the currently set params
     * @param logger it prints what it does into that logger
     * @return the set parameters
    */
    static void logParams(Ptr<CLogger>& logger, const CFileLoader& loader);
public:
    /**
     * @brief static method that executes the program
     * @return the C style termination state
    */
    static int run();
};

