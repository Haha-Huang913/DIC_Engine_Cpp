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

vector<int> FNCCSolver::solver(const DICImage& refImg, const DICImage& defImg) {	//第一版算法使用DICSubset的对象计算，可读性更高，但底层需要“切割”image并申请内存储存子集，在循环中极度影响计算时间，故这里采用裸指针以及绝对坐标直接平推。
	vector<double> ref_stats = refImg.getStatistics(centerXofsubset, centerYofsubset, subsetsize);
	double ref_mean = ref_stats[0];
	double ref_stddev = ref_stats[1];
	if (ref_stddev < 1e-5) ref_stddev = 1e-5;

	double maxofzncc = -2.0; // ZNCC 的理论下限是 -1
	vector<int> xy = { centerXofsubset, centerYofsubset };

	const Mat& ref_mat = refImg.getImage();
	const Mat& def_mat = defImg.getImage();

	int ref_startX = centerXofsubset - subsetsize / 2;		//参考图计算子集遍历起点,供指针计算协方差用
	int ref_startY = centerYofsubset - subsetsize / 2;

	int startX = subsetsize / 2;	//变形图遍历截取计算子集起、终点（子集的中点）
	int endX = refImg.getWidth() - subsetsize / 2 - 1;
	int startY = subsetsize / 2;
	int endY = refImg.getHeight() - subsetsize / 2 - 1;

    for (int y = startY; y <= endY; y++) {
        for (int x = startX; x <= endX; x++) {

            vector<double> def_stats = defImg.getStatistics(x, y, subsetsize);
            double def_mean = def_stats[0];
            double def_stddev = def_stats[1];

            if (def_stddev < 1e-5) def_stddev = 1e-5;

            double covariance = 0.0;
            int def_startX = x - subsetsize / 2;    //变形图计算子集遍历起点,供指针计算协方差用
            int def_startY = y - subsetsize / 2;

            for (int j = 0; j < subsetsize; j++) {
                const double* ref_row = ref_mat.ptr<double>(ref_startY + j);    //opencv中ptr后面带行号
                const double* def_row = def_mat.ptr<double>(def_startY + j);

                for (int i = 0; i < subsetsize; i++) {
                    double ref_val = ref_row[ref_startX + i] - ref_mean;
                    double def_val = def_row[def_startX + i] - def_mean;
                    covariance += ref_val * def_val;
                }
            }

            double zncc = covariance / (ref_stddev * def_stddev);

            if (zncc > maxofzncc) {
                maxofzncc = zncc;
                xy[0] = x; 
                xy[1] = y; 
            }
        }
    }

    return xy;
}
