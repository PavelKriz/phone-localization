#pragma once

#include <cmath>
#include <iostream>

#include "SProcessParams.h"
#include "CObjectInSceneFinder.h"
#include "parameters.h"

using namespace std;

class COperator
{
    //get the currently set params
    static SProcessParams getParams(Ptr<CLogger> logger);
public:
    //run the tests
    static int run();
};

