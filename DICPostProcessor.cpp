#pragma once
#include <vector>
#include <string>
#include "DICDataTypes.h"
#include "DICPostProcessor.h"

using namespace std;
using namespace cv;

DICPostProcessor::DICPostProcessor() {
};

void DICPostProcessor::fixInvalidHoles(DICMeshField& fullFieldResults) {
    for (int i = 0; i < fullFieldResults.rows * fullFieldResults.cols; i++) {   //坏点修复
        if (!fullFieldResults.points[i].is_valid) {
            bool is_top_edge = (i / fullFieldResults.cols == 0);
            bool is_bottom_edge = (i / fullFieldResults.cols == (fullFieldResults.rows - 1));
            bool is_left_edge = (i % fullFieldResults.cols == 0);
            bool is_right_edge = (i % fullFieldResults.cols == (fullFieldResults.cols - 1));
            if (is_top_edge || is_bottom_edge || is_left_edge || is_right_edge) {   //边缘不修
                continue;
            }
            else {
                vector<double> sum_p = { 0, 0, 0, 0, 0, 0 };
                int valid_neighbors = 0;
                vector<int> indexofneighbers = { i - 1, i + 1, i - fullFieldResults.cols, i + fullFieldResults.cols, i - fullFieldResults.cols - 1, i - fullFieldResults.cols + 1, i + fullFieldResults.cols - 1, i + fullFieldResults.cols + 1 };
                for (int n = 0; n < 8; n++) {
                    int idx = indexofneighbers[n];
                    if (fullFieldResults.points[idx].is_valid) {   //取好点数据
                        for (int j = 0; j < 6; j++) {
                            sum_p[j] += fullFieldResults.points[idx].p[j];
                        }
                        valid_neighbors++;
                    }
                }
                if (valid_neighbors > 0) {
                    for (int j = 0; j < 6; j++) {
                        fullFieldResults.points[i].p[j] = sum_p[j] / valid_neighbors;
                    }
                    fullFieldResults.points[i].is_valid = true;  //修好改标签
                }
            }
        }
    }
}
