#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include <string>
#include "DICSubset.h"
#include "DICImage.h"

class FNCCSolver {
private:
	int centerXofsubset;
	int centerYofsubset;
	int subsetsize;

public:
	FNCCSolver(int subsetSize);		

	void setCenterPoint(int x, int y);

	std::vector<int> solver(const DICImage& refImg, const DICImage& defImg); 
};
