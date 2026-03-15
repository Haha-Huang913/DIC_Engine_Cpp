#include "DICImage.h"
#include "DICSubset.h"
#include <cmath>
#include <vector>

using namespace std;
using namespace cv;

DICSubset::DICSubset(int cx, int cy, int subsetsize, const DICImage& img) {
	centerX = cx;
	centerY = cy;
	size = subsetsize;

	int startX = cx - size / 2;
	int startY = cy - size / 2;

	if (startX < 0 || startY < 0 || startX + size > img.getWidth() || startY + size > img.getHeight()) {
		std::cout << "计算子区越出图像边界！" << std::endl;
		exit(1);
	}

	subset = img.getImage()(cv::Rect(startX, startY, size, size)).clone();  //由此，subset对象只认识相对坐标，包括最后一个取相对坐标像素也是如此
	
	computeStatistics();

}

void DICSubset::computeStatistics() {
	double totalpixels = 0;
	double pixelCount = 0;
	double pixel_Square = 0;
	double variance = 0;
	for (int j = 0; j < size; j++) {
		for (int i = 0; i < size; i++) {
			totalpixels += subset.at<double>(j, i);
			pixelCount += 1;
			pixel_Square += subset.at<double>(j, i) * subset.at<double>(j, i);
		}
	}

	mean = totalpixels / pixelCount;
	variance = (pixel_Square / pixelCount) - (mean * mean);
	variance = max(0.0, variance);
	stddev = sqrt(variance);
}

int DICSubset::getcenterX() const {
	return centerX;
}

int DICSubset::getcenterY() const {
	return centerY;
}

int DICSubset::getsize() const {
	return size;
}

double DICSubset::getmean() const {
	return mean;
}

double DICSubset::getstddev() const {
	return stddev;
}

double DICSubset::getlocalPixel(int localX, int localY) const {
	return subset.at<double>(localY, localX);
}

