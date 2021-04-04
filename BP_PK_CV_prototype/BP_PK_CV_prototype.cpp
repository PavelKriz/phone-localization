/// main.cpp main file for the project - Pavel Kriz - Recognition and editing of urban scenes(bachelor thesis)

#include <cmath>
#include <iostream>
#include <set>
#include <map>
#include <limits>
#include <utility>
#include <exception>

//project includes
#include "CObjectInSceneFinder.h"
#include "parameters.h"
#include "SProcessParams.h"
#include "COperator.h"

using namespace std;

//=================================================================================================

int main(int argc, char** argv)
{
	int retValue = 0;
	if ((retValue = COperator::run()) != 1) {
		return retValue;
	}
	return 0;
}
