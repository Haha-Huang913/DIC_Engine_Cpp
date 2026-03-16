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
	std::vector<double> stats = img.getStatistics(cx, cy, size);
	mean = stats[0];
	stddev = stats[1];
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

