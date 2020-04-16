//�O���[�X�P�[���̋P�x�l�Ƀ����_���m�C�Y��t������
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
	Mat img, gray;
	int w_x, w_y, b_x, b_y;

	img = imread("Lenna.bmp");

	if (img.empty()) {
		cout << "�摜���J���܂���";
		cin.get();
		return -1;
	}

	cvtColor(img, gray, COLOR_BGR2GRAY);
	srand(time(NULL));
	for (int i = 0; i < 500; i++) {

	//��
	w_x = rand() % img.rows;
	w_y = rand() % img.cols;
	gray.at<uchar>(w_y, w_x) = 0;

	//��
	b_x = rand() % img.rows;
	b_y = rand() % img.cols;
	gray.at<uchar>(b_y, b_x) = 255;
	}

	imshow("�����_���m�C�Y", gray);
	waitKey(0);
	return 0;

}