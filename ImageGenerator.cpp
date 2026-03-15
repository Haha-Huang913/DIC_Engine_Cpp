#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
    theRNG().state = getTickCount();
    Mat noise(200, 200, CV_64F);
    randn(noise, 128, 50);

    Mat speckle;
    GaussianBlur(noise, speckle, Size(5, 5), 1.5);

    // 1. Anchor point (Center of ROI)
    double cx = 100.0;
    double cy = 100.0;

    // 2. Ground truth settings
    double u_true = theRNG().uniform(-15.0, 15.0);
    double v_true = theRNG().uniform(-15.0, 15.0);
    double ux_true = theRNG().uniform(-0.04, 0.04);
    double uy_true = theRNG().uniform(-0.02, 0.02);
    double vx_true = theRNG().uniform(-0.02, 0.02);
    double vy_true = theRNG().uniform(-0.04, 0.04);

    // 3. Matrix calculation for exact anchor point deformation
    double m00 = 1.0 + ux_true;
    double m01 = uy_true;
    double m02 = cx + u_true - m00 * cx - m01 * cy;

    double m10 = vx_true;
    double m11 = 1.0 + vy_true;
    double m12 = cy + v_true - m10 * cx - m11 * cy;

    Mat M = (Mat_<double>(2, 3) << m00, m01, m02, m10, m11, m12);

    Mat deformed_speckle;
    // No INVERSE_MAP, use forward mapping
    warpAffine(speckle, deformed_speckle, M, speckle.size(), INTER_CUBIC);

    // 4. ROI cropping
    Rect roi(49, 49, 101, 101);
    Mat refImg = speckle(roi).clone();
    Mat defImg = deformed_speckle(roi).clone();

    // 5. Convert and save
    Mat refImg_8u, defImg_8u;
    refImg.convertTo(refImg_8u, CV_8U);
    defImg.convertTo(defImg_8u, CV_8U);

    imwrite("D:/DIC_App/TEMP1.tif", refImg_8u);
    imwrite("D:/DIC_App/TEMP2.tif", defImg_8u);

    // 6. Print log
    cout << "[Expected ICGN u ] : " << u_true << endl;
    cout << "[Expected ICGN v ] : " << v_true << endl;
    cout << "[Expected ICGN ux] : " << ux_true << endl;
    cout << "[Expected ICGN uy] : " << uy_true << endl;
    cout << "[Expected ICGN vx] : " << vx_true << endl;
    cout << "[Expected ICGN vy] : " << vy_true << endl;

    return 0;
}