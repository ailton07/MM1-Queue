//extern "C" {
//	void Log(const char * str);
//	double ExecMM1(int exponent, float interArrivalMean, float serviceRateMean, int servers, bool listQueue);
//}

#pragma once

#ifdef MM1_EXPORTS
#define MM1_API __declspec(dllexport) 
#else
#define MM1_API __declspec(dllimport) 
#endif
namespace MM1
{
	// This class is exported from the MathLibrary.dll
	class Functions
	{
	public:
		static void Log(const char * str);
		static double ExecMM1(int exponent, float interArrivalMean, float serviceRateMean, int servers, bool listQueue);
	};

}