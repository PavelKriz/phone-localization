#include "SProcessParams.h"

string algToStr(const EAlgorithm alg) {
	switch (alg)
	{
	case EAlgorithm::ALG_SIFT:
		return "SIFT";
	case EAlgorithm::ALG_ORB:
		return "ORB";

#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
	case EAlgorithm::ALG_BEBLID:
		return "BEBLID";
#endif

	case EAlgorithm::ALG_BF_MATCHING:
		return "brute force matching";
	case EAlgorithm::ALG_FLANN_MATCHING:
		return "FLANN matching";
	default:
		throw invalid_argument("Error algorithm method cannot be converted to string, the string is not known! (probably non recognized method)");
		break;
	}
}