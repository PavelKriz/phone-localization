/// main.cpp main file for the project - Pavel Kriz - Recognition and editing of urban scenes(bachelor thesis)

#include <cmath>
#include <iostream>
#include <set>
#include <map>
#include <limits>
#include <utility>
#include <exception>
//timing
#include <chrono>

//project includes
#include "CObjectInScenesEngine.h"
#include "parameters.h"

using namespace std;

//function for finding object in multiple scene, result is the best one match
int comparisonRun(const string& objectFilePath, const vector<string>& sceneFilePaths, const string& runName) {

	try {
		CObjectInScenesEngine engine(runName, objectFilePath, sceneFilePaths);
		bool viewResult = true;
		engine.run(CImage::EProcessMethod::SIFT_, viewResult);
		engine.report();
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
	cout << "OpenCV version : " << CV_VERSION << endl << endl;

	chrono::steady_clock::time_point begin = chrono::steady_clock::now();

	int retValue = 0;
	if ((retValue = comparisonRun(objectFilePath, sceneFilePaths, runName)) != 1) {
		return retValue;
	}

	chrono::steady_clock::time_point end = chrono::steady_clock::now();
	cout << "The whole process took = " << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

	return 0;
}
