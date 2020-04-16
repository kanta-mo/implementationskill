//1枚の静止画を読み込んで表示する
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main() {
	Mat img;
	img = imread("Lenna.bmp");

	if (img.empty()) {
		cout << "ファイルが読めません";
		cin.get();
		return -1;
	}

	imshow("入力画像", img);
	waitKey();
	return 0;
}