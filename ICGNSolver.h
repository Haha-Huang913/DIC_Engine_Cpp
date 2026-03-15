#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include <string>
#include "DICSubset.h"
#include "DICImage.h"

class ICGNSolver {
private:
	int maxIterations;
	double tolerance;

public:
	ICGNSolver(int maxIter, double tol);

	void setParameter(int newMaxIter, double newTol);

	std::vector<double> solver(const DICImage& refImg, const DICImage& defImg, int subsetcx, int subsetcy, int subsetsize, double init_u = 0.0 , double init_v = 0.0);

};
