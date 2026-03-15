#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>

class DICImage{
private:
    cv::Mat image;  //原始灰度图像矩阵 
    cv::Mat gradX;  //图像X方向梯度矩阵
    cv::Mat gradY;  //图像Y 方向梯度矩阵

    void computeGradients();    //梯度计算函数

public:
    DICImage(const std::string& path);  //构造函数,传路径

    int getWidth() const;
    int getHeight() const;

    double getSubPixel(double x, double y) const;
    double getPixel(int x, int y) const;

    double getGradX(double x, double y) const;
    double getGradY(double x, double y) const;

    const cv::Mat& getImage() const;    //返回常量Mat类型引用的常量成员函数

};
