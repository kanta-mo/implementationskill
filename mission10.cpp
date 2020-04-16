//グレースケールを2値化する　閾値をトラックバーで変更できるようにする
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
	Mat img, gray,dst;
	int thresh;
	img = imread("Lenna.bmp");

	if (img.empty()) {
		cout << "画像が開けません";
		cin.get();
		return -1;
	}

	cvtColor(img, gray, COLOR_BGR2GRAY);
	namedWindow("threshold");
	createTrackbar("threshold", "threshold", 0, 255);

	while (1) {
		if (waitKey(1) == 'q') break;

		thresh = getTrackbarPos("threshold", "threshold");
		threshold(gray,dst,thresh,255,THRESH_BINARY);

		imshow("threshold", dst);
	}
} 