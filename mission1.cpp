//1���̐Î~���ǂݍ���ŕ\������
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main() {
	Mat img;
	img = imread("Lenna.bmp");

	if (img.empty()) {
		cout << "�t�@�C�����ǂ߂܂���";
		cin.get();
		return -1;
	}

	imshow("���͉摜", img);
	waitKey();
	return 0;
}