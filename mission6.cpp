//�J���[�摜��ǂݍ���ŃO���[�X�P�[���ɕϊ�����
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
	Mat img,dst;
	img = imread("Lenna.bmp");
	
	if (img.empty()) {
		cout << "�摜���ǂݍ��߂܂���";
		cin.get();
		return -1;
	}

	cvtColor(img, dst, COLOR_BGR2GRAY);
	imshow("�O���[�X�P�[��", dst);

	waitKey(0);
	return 0;
}