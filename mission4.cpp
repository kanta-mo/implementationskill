//�J���[�摜��ǂݍ����RGB�̃`�����l�������ւ���
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {

	Mat img,dst;
	img = imread("Lenna.bmp");

	if (img.empty()) {
		cout << "�摜���J���܂���";
		cin.get();
		return -1;
	}

	cvtColor(img,dst,COLOR_BGR2RGB);
	imshow("RGB����ւ�", dst);

	waitKey();
	return 0;
}