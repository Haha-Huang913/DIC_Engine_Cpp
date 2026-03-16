#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include "DICImage.h"

class DICSubset {
private:
	int centerX;
	int centerY;
	int size;	//奇数

	cv::Mat subset;
	cv::Mat image;

	double mean;
	double stddev;

public:
	DICSubset(int cx, int cy, int subsetsize,const DICImage& img );

	int getcenterX() const;
	int getcenterY() const;
	int getsize() const;

	double getmean() const;
	double getstddev() const;

	double getlocalPixel(int localX, int localY) const;	//	局部（计算子区）坐标像素查询	

};

