//�O���[�X�P�[����2�l������@臒l���g���b�N�o�[�ŕύX�ł���悤�ɂ���
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
	Mat img, gray,dst;
	int thresh;
	img = imread("Lenna.bmp");

	if (img.empty()) {
		cout << "�摜���J���܂���";
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