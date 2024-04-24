
export module Statistics;

import :InternalUtilities;

export import :Mean;
export import :Median;
export import :Midrange;
export import :MinMax;
export import :Mode;
export import :Skewness;
export import :Ranks;
export import :Variance;

export import :StandardScore;
export import :PearsonCorrelationCoefficient;



/*
	Almost all these functions take range r-value references are input,
	but despite being r-value refs that imply a move has occured
	and that the original value from the calling location is no longer valid,
	this doesn't have that deficiency. Somehow, it seems the Range (concept) r-value ref
	becomes a vector (or whatever container you send in) reference.

	Would prefer if it was const, but it already took like 40 minutes of reading range docs and stackoverflow to get this far.
*/
