//カラー画像を読み込んでRGBのチャンネルを入れ替える
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {

	Mat img,dst;
	img = imread("Lenna.bmp");

	if (img.empty()) {
		cout << "画像が開けません";
		cin.get();
		return -1;
	}

	cvtColor(img,dst,COLOR_BGR2RGB);
	imshow("RGB入れ替え", dst);

	waitKey();
	return 0;
}