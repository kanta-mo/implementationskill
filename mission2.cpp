//���摜��ǂݍ���ŕ\������
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int main() {
	VideoCapture cap("sample.mp4");

	if (!cap.isOpened()) {
		cout << "�r�f�I�t�@�C�����J���܂���";
		cin.get();
		return -1;
	}

	Mat img;

	while (1) {
		cap >> img;	//1�t���[���ǂݍ���
		if (img.empty()) break;
		imshow("video", img);
		if (waitKey(30) == 27) break;
	} 
	return 0;
}