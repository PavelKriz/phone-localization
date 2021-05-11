#include "SProcessParams.h"

string algToStr(const EAlgorithm alg) {
	switch (alg)
	{
	case EAlgorithm::ALG_SIFT:
		return SIFT_STR;
	case EAlgorithm::ALG_ROOTSIFT:
		return ROOTSIFT_STR;
	case EAlgorithm::ALG_PRECISE_ROOTSIFT:
		return PRECISE_ROOTSIFT_STR;
	case EAlgorithm::ALG_ORB:
		return ORB_STR;

#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
	case EAlgorithm::ALG_BEBLID:
		return BEBLID_STR;
#endif

	case EAlgorithm::ALG_BF_MATCHING:
		return BF_MATCHING_STR;
	case EAlgorithm::ALG_FLANN_MATCHING:
		return FLANN_MATCHING_STR;
	default:
		throw invalid_argument("Error algorithm method cannot be converted to string, the string is not known! (probably non recognized method)");
		break;
	}
}

EAlgorithm strToAlg(const string& str)
{
	if (str == SIFT_STR) {
		return EAlgorithm::ALG_SIFT;
	}
	else if (str == ROOTSIFT_STR) {
		return EAlgorithm::ALG_ROOTSIFT;
	}
	else if (str == PRECISE_ROOTSIFT_STR) {
		return EAlgorithm::ALG_PRECISE_ROOTSIFT;
	}
	else if (str == ORB_STR) {
		return EAlgorithm::ALG_ORB;
	}
#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
	else if (str == BEBLID_STR) {
		return EAlgorithm::ALG_BEBLID;
	}
#endif
	else if (str == BF_MATCHING_STR) {
		return EAlgorithm::ALG_BF_MATCHING;
	}
	else if (str == FLANN_MATCHING_STR) {
		return EAlgorithm::ALG_FLANN_MATCHING;
	}
	else {
		throw invalid_argument("Error invalid algorithm method was used! The string has to have one of following form: " +
			SIFT_STR + " , " + ROOTSIFT_STR + " , " + PRECISE_ROOTSIFT_STR + " , " + ORB_STR +
#ifdef COMPILE_EXPERIMENTAL_MODULES_ENABLED
			BEBLID_STR + " , " +
#endif
			BF_MATCHING_STR + " , " + FLANN_MATCHING_STR
		);
	}
}
