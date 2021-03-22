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

using namespace std;

//function for finding object in multiple scene, result is the best one match
int comparisonRun(const string& sceneFilePath, const vector<string>& objectFilePaths, const string& runName) {

	try {
		CObjectInSceneFinder finder(runName, sceneFilePath, objectFilePaths);
		bool viewResult = true;
		finder.run(CImage::EProcessMethod::SIFT_, viewResult);
		finder.report();
	}
	catch(ios_base::failure e){
		cout << e.what() << endl;
		return -1;
	}
	catch(invalid_argument e) {
		cout << e.what() << endl;
		return -1;
	}
	catch (logic_error e) {
		cout << e.what() << endl;
		return -1;
	}

	return 1;
}

//=================================================================================================

int main(int argc, char** argv)
{
	cout << "start" << endl;
	cout << "OpenCV version : " << CV_VERSION << endl << endl;

	int retValue = 0;
	if ((retValue = comparisonRun(sceneFilePath, objectFilePaths, runName)) != 1) {
		return retValue;
	}

	cout << "end" << endl;
	return 0;
}
