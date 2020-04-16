//動画像を読み込んで表示する
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int main() {
	VideoCapture cap("sample.mp4");

	if (!cap.isOpened()) {
		cout << "ビデオファイルが開けません";
		cin.get();
		return -1;
	}

	Mat img;

	while (1) {
		cap >> img;	//1フレーム読み込む
		if (img.empty()) break;
		imshow("video", img);
		if (waitKey(30) == 27) break;
	} 
	return 0;
}