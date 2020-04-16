//動画像を読み込んで静止画連番ファイルを保存する
#include <sstream>
#include<iomanip>
#include <opencv2/opencv.hpp>


using namespace std;
using namespace cv;

int main() {
	VideoCapture cap("sample.mp4");

	if (!cap.isOpened()) {
		cout << "ビデオが開けません";
		cin.get();
		return -1;
	}

	Mat img;
	int n = 0;

	while (1) {

		cap >> img;
		if (img.empty()) break;

		ostringstream oss;
		oss << setfill('0') << setw(3) << n++;

		imwrite(oss.str()+".jpg", img);

		if(waitKey(30) == 27) break;
	}
	return 0;
}