#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include "ICGNSolver.h"
#include "DICImage.h"
#include "DICSubset.h"

using namespace std;
using namespace cv;

ICGNSolver::ICGNSolver(int maxIter, double tol) {
	maxIterations = maxIter;
	tolerance = tol;
}

void ICGNSolver::setParameter(int newMaxIter, double newTol) {
	maxIterations = newMaxIter;
	tolerance = newTol;
}

vector <double> ICGNSolver::solver(const DICImage& refImg, const DICImage& defImg, int subsetcx,int subsetcy,int subsetsize, double init_u, double init_v) {
	vector<double> p(6, 0.0);
	p[0] = init_u;
	p[3] = init_v;

	DICSubset ref_subset(subsetcx, subsetcy, subsetsize,refImg);	//先创建参考图像子集
	double mean_ref = ref_subset.getmean();			//获取参考图像子集统计量
	double stddev_ref = ref_subset.getstddev();
	if (stddev_ref < 1e-5) stddev_ref = 1e-5;

	Mat hessian = Mat::zeros(6, 6, CV_64F);		//往下计算参考图像子集的黑塞矩阵以及每个像素的最速下降梯度
	Mat gradient = Mat::zeros(6, 1, CV_64F);

	int numPixels = subsetsize * subsetsize;
	vector<vector<double>> sd_ref_subset(numPixels, vector<double>(6, 0.0));	//二维vector，共numPixels个像素的sd向量
	int pixel_serialNumber = 0;		//服务于上面的二维vector向量，将图像二维坐标压缩为一维序号

	int halfSize = subsetsize / 2;
	int startX = subsetcx - halfSize;
	int startY = subsetcy - halfSize;
	int endX = subsetcx + halfSize;
	int endY = subsetcy + halfSize;

	for (int y = startY; y <= endY; y++) {	//开始遍历参考图像子集计算,这里用绝对坐标最好，所以上面先算起始点以及结束点
		for (int x = startX; x <= endX; x++) {
			int dx = x - subsetcx;
			int dy = y - subsetcy;

			double gx_ref_subset = refImg.getGradX(x, y);	//计算该点的X，Y梯度
			double gy_ref_subset = refImg.getGradY(x, y);

			// J = [jacobian_X] = [1, dx, dy, 0, 0,  0]
		   //      [jacobian_Y]   [0, 0,  0,  1, dx, dy]
			vector<double> jacobian_X(6);
			jacobian_X[0] = 1.0;
			jacobian_X[1] = dx;
			jacobian_X[2] = dy;
			jacobian_X[3] = 0.0;
			jacobian_X[4] = 0.0;
			jacobian_X[5] = 0.0;

			vector<double> jacobian_Y(6);
			jacobian_Y[0] = 0.0;
			jacobian_Y[1] = 0.0;
			jacobian_Y[2] = 0.0;
			jacobian_Y[3] = 1.0;
			jacobian_Y[4] = dx;
			jacobian_Y[5] = dy;

			sd_ref_subset[pixel_serialNumber][0] = -(gx_ref_subset * jacobian_X[0] + gy_ref_subset * jacobian_Y[0]) / stddev_ref;	// sd = ([gx_ref,gy_ref] * J ) / standarddeviation_ref
			sd_ref_subset[pixel_serialNumber][1] = -(gx_ref_subset * jacobian_X[1] + gy_ref_subset * jacobian_Y[1]) / stddev_ref;
			sd_ref_subset[pixel_serialNumber][2] = -(gx_ref_subset * jacobian_X[2] + gy_ref_subset * jacobian_Y[2]) / stddev_ref;
			sd_ref_subset[pixel_serialNumber][3] = -(gx_ref_subset * jacobian_X[3] + gy_ref_subset * jacobian_Y[3]) / stddev_ref;
			sd_ref_subset[pixel_serialNumber][4] = -(gx_ref_subset * jacobian_X[4] + gy_ref_subset * jacobian_Y[4]) / stddev_ref;
			sd_ref_subset[pixel_serialNumber][5] = -(gx_ref_subset * jacobian_X[5] + gy_ref_subset * jacobian_Y[5]) / stddev_ref;
			
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < 6; j++)
				{
					hessian.at<double>(i, j) += sd_ref_subset[pixel_serialNumber][i] * sd_ref_subset[pixel_serialNumber][j];
				}
			}

			pixel_serialNumber++;

		}
	}

	for (int iter = 0; iter < maxIterations; iter++) {
		vector<double> def_pixels_subset(numPixels,0.0);   //开辟局部vector矩阵用以储存pxile_def，防止再算一次

		//先计算该次迭代中参考子集经过形变参数映射后得到的变形子集的统计量，每次迭代完清零
		int pixelcount = 0;
		long double mean_def = 0.0, stddev_def = 0.0, totalpixel = 0.0, totalpixel_squared = 0.0;
		gradient.setTo(0);	
		pixel_serialNumber = 0;

		for (int y = startY; y <= endY; y++)
		{
			for (int x = startX; x <= endX; x++)
			{
				double dx = x - subsetcx;
				double dy = y - subsetcy;

				//对于参考子集中的每个像素，根据当前形函数参数p计算其在变形图像中的对应位置x_def和y_def
				double x_def = x + p[0] + p[1] * dx + p[2] * dy;    //x_def = x + u + ux*dx + uy*dy
				double y_def = y + p[3] + p[4] * dx + p[5] * dy;    //y_def = y + v + vx*dx + vy*dy

				def_pixels_subset[pixel_serialNumber] = defImg.getSubPixel(x_def, y_def);

				totalpixel += def_pixels_subset[pixel_serialNumber];
				totalpixel_squared += def_pixels_subset[pixel_serialNumber] * def_pixels_subset[pixel_serialNumber];
				pixelcount += 1;
				pixel_serialNumber++;
			}
		}

		pixel_serialNumber = 0;	

		mean_def = totalpixel / pixelcount;
		double variance_def = totalpixel_squared  - (mean_def * mean_def) * pixelcount;
		variance_def = max(0.0, variance_def);    //防止负数
		stddev_def = sqrt(variance_def);
		if (stddev_def < 1e-5) stddev_def = 1e-5;

		for (int y = startY; y <= endY; y++)
		{
			for (int x = startX; x <= endX; x++)
			{
				double dx = x - subsetcx;
				double dy = y - subsetcy;

				
				double pxile_def = def_pixels_subset[pixel_serialNumber];
				double pxile_ref = ref_subset.getlocalPixel(x - startX, y - startY);	//局部坐标
				double error = ((pxile_ref - mean_ref) / stddev_ref) - ((pxile_def - mean_def) / stddev_def);

				for (int i = 0; i < 6; i++)
				{
					gradient.at<double>(i, 0) += sd_ref_subset[pixel_serialNumber][i] * error;
				}
				pixel_serialNumber++;
			}
		}

		// 求解线性方程组：H * Δp = gradient
		Mat delta_p;
		solve(hessian, gradient, delta_p, DECOMP_SVD);

		// 构造当前形函数的变换矩阵
		// W(p) = [1+ux,   uy, u]
		//        [  vx, 1+vy, v]
		//        [   0,    0, 1]

		double Wp_data[9] =
		{
			1.0 + p[1], p[2],       p[0],
			p[4],       1.0 + p[5], p[3],
			0.0,        0.0,        1.0
		};
		Mat Wp_mat(3, 3, CV_64F, Wp_data);

		// 构造增量形函数的变换矩阵 W(Δp)
		double Wdp_data[9] =
		{
			1.0 + delta_p.at<double>(1, 0), delta_p.at<double>(2, 0),       delta_p.at<double>(0, 0),
			delta_p.at<double>(4, 0),       1.0 + delta_p.at<double>(5, 0), delta_p.at<double>(3, 0),
			0.0,                            0.0,                            1.0
		};
		Mat Wdp_mat(3, 3, CV_64F, Wdp_data);

		// 求 W(Δp) 的逆矩阵 Wdp_inv
		Mat Wdp_inv;
		invert(Wdp_mat, Wdp_inv, DECOMP_LU);

		// 反向组合求出新的变换矩阵：W_new = Wp * Wdp_inv
		Mat Wnew_mat = Wp_mat * Wdp_inv;

		//变换矩阵反求，更新形函数参数
		p[0] = Wnew_mat.at<double>(0, 2);
		p[1] = Wnew_mat.at<double>(0, 0) - 1.0;
		p[2] = Wnew_mat.at<double>(0, 1);
		p[3] = Wnew_mat.at<double>(1, 2);
		p[4] = Wnew_mat.at<double>(1, 0);
		p[5] = Wnew_mat.at<double>(1, 1) - 1.0;

		// 检查收敛条件
		double norm_delta = norm(delta_p);
		if (norm_delta < tolerance)
		{
			cout << "收敛于第 " << iter + 1 << " 次迭代" << endl;
			break;
		}

		if (iter == maxIterations - 1)
		{
			cout << "达到最大迭代次数 " << maxIterations << endl;
		}
	}

	return p;
}

