//カラー画像を読み込んでグレースケールに変換する
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
	Mat img,dst;
	img = imread("Lenna.bmp");
	
	if (img.empty()) {
		cout << "画像が読み込めません";
		cin.get();
		return -1;
	}

	cvtColor(img, dst, COLOR_BGR2GRAY);
	imshow("グレースケール", dst);

	waitKey(0);
	return 0;
}