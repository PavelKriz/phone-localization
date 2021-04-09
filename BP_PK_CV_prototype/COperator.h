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

using namespace std;

/**
 * @brief Class holds static methods that execute the program and initiate the parameters that are being passed in the app
*/
class COperator
{
    /**
     * @brief Get the currently set params
     * @param logger it prints what it does into that logger
     * @return the set parameters
    */
    static SProcessParams getParams(Ptr<CLogger> logger);
public:
    /**
     * @brief static method that executes the program
     * @return the C style termination state
    */
    static int run();
};

