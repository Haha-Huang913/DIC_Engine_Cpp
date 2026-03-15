#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <fstream>
#include "ICGNSolver.h"
#include "DICImage.h"
#include "DICSubset.h"
#include "FNCCSolver.h"
#include "DICPostProcessor.h"

using namespace std;
using namespace cv;

int main() {
    system("chcp 65001 > nul");

    DICImage refImg("D:/DIC_App/TEMP1.tif");
    DICImage defImg("D:/DIC_App/TEMP2.tif");

    {
    Mat refShow, defShow;
    refImg.getImage().convertTo(refShow, CV_8U);
    defImg.getImage().convertTo(defShow, CV_8U);
    namedWindow("Reference Image", WINDOW_NORMAL);
    namedWindow("Deformed Image", WINDOW_NORMAL);
    imshow("Reference Image", refShow);
    imshow("Deformed Image", defShow);
    waitKey(0);
    destroyAllWindows();
    }

    DICMeshField fullFieldResults;
    DICPostProcessor fixpoints;
    int pointcount_rows = 0;    //行列数基于步长和边界保护值改变，不宜封装入类，需单独记录
    int pointcount_cols = 0;

    int subsetSize = 31;
    int step = 5;

    ICGNSolver fineSearch(50, 1e-3);

    int cxstartpoint = subsetSize / 2;
    int cxendpoint = refImg.getWidth() - subsetSize / 2;
    int cystartpoint = subsetSize / 2;
    int cyendpoint = refImg.getHeight() - subsetSize / 2;

    FNCCSolver coarseSearch(subsetSize);
    for (int y = cystartpoint + 15; y < cyendpoint - 15; y += step) {   //15是边界保护值
        pointcount_cols = 0;
        for (int x = cxstartpoint + 15; x < cxendpoint - 15; x += step) {
            coarseSearch.setCenterPoint(x, y);
            vector<int> coarseSearchxy = coarseSearch.solver(refImg, defImg);
            int init_u = coarseSearchxy[0] - x;
            int init_v = coarseSearchxy[1] - y;
            vector<double> p = fineSearch.solver(refImg, defImg, x, y, subsetSize, init_u, init_v);

            DICPointResult singlePointResult;
            singlePointResult.coord = Point2f(x, y);

            if (!p.empty()) {
                singlePointResult.p = p;
                singlePointResult.is_valid = true;
            }
            else {
                singlePointResult.p = { 0,0,0,0,0,0 };
                singlePointResult.is_valid = false;
            }
            fullFieldResults.points.push_back(singlePointResult);
            pointcount_cols++;
        }
        fullFieldResults.cols = pointcount_cols;
        pointcount_rows++;
    }
    fullFieldResults.rows = pointcount_rows;

    fixpoints.fixInvalidHoles( fullFieldResults);

    string output_filename = "D:/DIC_App/FNCC+ICGN.csv";
    ofstream outFile(output_filename);

    outFile << "X,Y,U,V,UX,UY,VX,VY,IsValid" << endl;

    int valid_count = 0;
    for (int i = 0; i < fullFieldResults.points.size(); i++) {
        DICPointResult pt = fullFieldResults.points[i];

        outFile << pt.coord.x << ","
                << pt.coord.y << ","
                << pt.p[0] << "," // U
                << pt.p[3] << "," // V
                << pt.p[1] << "," // UX
                << pt.p[2] << "," // UY
                << pt.p[4] << "," // VX
                << pt.p[5] << "," // VY
                << (pt.is_valid ? 1 : 0) // 把 bool 转成 1 或 0 
                << endl;

        if (pt.is_valid) valid_count++;
    }

    outFile.close();
    cout << "表格大小:" << pointcount_rows << " x " << pointcount_cols << endl;
    cout << "总点数: " << fullFieldResults.points.size() << endl;
    cout << "有效点数: " << valid_count << " (包含已修点)" << endl;
    cout << "数据已保存至文件: " << output_filename << endl;

    system("python D:/DIC_App/plot_dic.py");

    return 0;
}