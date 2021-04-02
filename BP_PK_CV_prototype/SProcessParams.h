#pragma once

struct SProcessParams {
    //keypoints and descriptors
	enum class EDetectExtractMethod {
		SIFT,
		ORB
	};
	const EDetectExtractMethod detectExtractMethod_;

	//matching
	enum class EMatchingMethod {
		BRUTE_FORCE,
		FLANN_BASED
	};
	const EMatchingMethod matchingMethod_;

	SProcessParams(EDetectExtractMethod detectExtractMethod, EMatchingMethod matchingMethod)
		:
		detectExtractMethod_(detectExtractMethod),
		matchingMethod_(matchingMethod)
	{}
};