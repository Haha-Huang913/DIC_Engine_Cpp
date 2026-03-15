#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

struct DICPointResult {     //常见平行数组解法，新定义数据结构体，以储存全局搜索后得到的单点结果
    cv::Point2f coord;      //存入该点坐标(x,y)
    std::vector<double> p;       //该点的形函数参数
    bool is_valid;      //注：留标记以进行坏点修复
};

struct DICMeshField {
    std::vector<DICPointResult> points;     //储存全局搜索后所有点的结果（表格）,一个point是一个DICPointResult结构变量
    int rows;                               //表格行数
    int cols;                               //表格列数
};
