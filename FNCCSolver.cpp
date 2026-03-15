#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include "ICGNSolver.h"
#include "DICImage.h"
#include "DICSubset.h"
#include "FNCCSolver.h"

using namespace std;
using namespace cv;

FNCCSolver::FNCCSolver(int subsetSize) {
	subsetsize = subsetSize;
}

void FNCCSolver::setCenterPoint(int x, int y) {
	centerXofsubset = x;
	centerYofsubset = y;
}

vector<int> FNCCSolver::solver(const DICImage& refImg, const DICImage& defImg) {
	DICSubset ref_subset(centerXofsubset, centerYofsubset, subsetsize, refImg);
	
	double maxofzncc = -2.0;

	vector<int> xy = { 0 ,0 };

	for (int y = 0; y < refImg.getHeight() - subsetsize; y++) {
		for (int x = 0; x < refImg.getWidth() - subsetsize; x++) {
			DICSubset def_subset(x + subsetsize / 2, y + subsetsize / 2, subsetsize, defImg);
			double covariance = 0;
			double zncc = 0;
			for (int j = 0; j < subsetsize; j++) {
				for (int i = 0; i < subsetsize; i++) {
					covariance += ((long double(def_subset.getlocalPixel(i,j) - def_subset.getmean())) * (long double(ref_subset.getlocalPixel(i, j) - ref_subset.getmean())));
				}
			}
			zncc = covariance / (def_subset.getstddev() * ref_subset.getstddev());

			if (maxofzncc < zncc) {
				maxofzncc = zncc;
				xy[0] = x + subsetsize / 2;
				xy[1] = y + subsetsize / 2;
			}
		}
	}

	return xy;
}