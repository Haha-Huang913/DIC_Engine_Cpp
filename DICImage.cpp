#include "DICImage.h"
#include <cmath>

using namespace std;
using namespace cv;

DICImage::DICImage(const std::string& path) {
	Mat tempImage = imread(path, IMREAD_GRAYSCALE);
	if (tempImage.empty()) {
		cout << "找不到图片，请检查路径是否正确！路径: " << path << endl;
		exit(1);
	}

	tempImage.convertTo(image, CV_64F);     //自动转化为双浮点精度

	computeGradients(); //创建完图像后即刻计算梯度
}

void DICImage::computeGradients() {
    gradX = Mat::zeros(image.size(), CV_64F);     //X和Y方向的梯度矩阵与图像矩阵同大小
    gradY = Mat::zeros(image.size(), CV_64F);

    for (int y = 1; y < image.rows - 1; y++)
    {
        for (int x = 1; x < image.cols - 1; x++)
        {
            gradX.at<double>(y, x) = (image.at<double>(y, x + 1) - image.at<double>(y, x - 1)) / 2.0;   // 计算图像梯度（使用中心差分）,f'(x) ≈ [f(x+1) - f(x-1)] / 2
            gradY.at<double>(y, x) = (image.at<double>(y + 1, x) - image.at<double>(y - 1, x)) / 2.0;
        }
    }
}

int DICImage::getWidth() const {
    return image.cols;
}

int DICImage::getHeight() const {
    return image.rows;
}

double DICImage::getSubPixel(double x, double y) const {
    if (x < 0 || x >= image.cols - 1 || y < 0 || y >= image.rows - 1)
    {
        return 0.0;   // 界外给0
    }

    int x1 = floor(x);  //左
    int y1 = floor(y);  //上
    int x2 = x1 + 1;    //右
    int y2 = y1 + 1;    //下

    double dx = x - x1;
    double dy = y - y1;

    double f11 = image.at<double>(y1, x1);    //左上
    double f12 = image.at<double>(y2, x1);    //左下
    double f21 = image.at<double>(y1, x2);    //右上
    double f22 = image.at<double>(y2, x2);    //右下

    double pixel = (1 - dx) * (1 - dy) * f11 + (1 - dx) * dy * f12 + dx * (1 - dy) * f21 + dx * dy * f22;    // 双线性插值公式

    return pixel;
}

double DICImage::getPixel(int x, int y) const {
    if (x < 0 || x >= image.cols || y < 0 || y >= image.rows) {
        return 0.0; //同样界外给0
    }
    return image.at<double>(y, x);
}

double DICImage::getGradX(double x, double y) const {
    if (x < 0 || x >= gradX.cols - 1 || y < 0 || y >= gradX.rows - 1)
    {
        return 0;   // 界外给0
    }

    int x1 = floor(x);  //左
    int y1 = floor(y);  //上
    int x2 = x1 + 1;    //右
    int y2 = y1 + 1;    //下

    double dx = x - x1;
    double dy = y - y1;

    double f11 = gradX.at<double>(y1, x1);    //左上
    double f12 = gradX.at<double>(y2, x1);    //左下
    double f21 = gradX.at<double>(y1, x2);    //右上
    double f22 = gradX.at<double>(y2, x2);    //右下

    return (1 - dx) * (1 - dy) * f11 + (1 - dx) * dy * f12 + dx * (1 - dy) * f21 + dx * dy * f22;
}

double DICImage::getGradY(double x, double y) const {
    if (x < 0 || x >= gradY.cols - 1 || y < 0 || y >= gradY.rows - 1)
    {
        return 0;   // 界外给0
    }

    int x1 = floor(x);  //左
    int y1 = floor(y);  //上
    int x2 = x1 + 1;    //右
    int y2 = y1 + 1;    //下

    double dx = x - x1;
    double dy = y - y1;

    double f11 = gradY.at<double>(y1, x1);    //左上
    double f12 = gradY.at<double>(y2, x1);    //左下
    double f21 = gradY.at<double>(y1, x2);    //右上
    double f22 = gradY.at<double>(y2, x2);    //右下

    return (1 - dx) * (1 - dy) * f11 + (1 - dx) * dy * f12 + dx * (1 - dy) * f21 + dx * dy * f22;
}

const cv::Mat& DICImage::getImage() const {
    return image;
}