//グレースケールのエッジを検出する
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
	Mat img, gray, gray_src1, gray_src2, gray_src3, dst1, dst2, dst3;
	img = imread("Lenna.bmp");

	if (img.empty()) {
		cout << "画像が開けません";
		cin.get();
		return -1;
	}

	cvtColor(img, gray, COLOR_BGR2GRAY);

	Sobel(gray, gray_src1, CV_32F, 1, 0, 3);
	Laplacian(gray, gray_src2, CV_32F, 3);
	Canny(gray, gray_src3, 50, 200, 3);

	convertScaleAbs(gray_src1, dst1);
	convertScaleAbs(gray_src2, dst2);
	convertScaleAbs(gray_src3, dst3);

	imshow("Sobel", dst1);
	imshow("Laplacian", dst2);
	imshow("Canny", dst3);

	waitKey(0);
	return 0;
}