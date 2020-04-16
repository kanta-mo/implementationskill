//�J���[�摜��ǂݍ����45�x��]������
#include <opencv2/opencv.hpp>

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

	float angle = 45.0, scale = 1.0;
	Point2f center(img.cols * 0.5, img.rows * 0.5);

	Mat affine_matrix = getRotationMatrix2D(center, angle, scale);
	
	warpAffine(img, dst, affine_matrix, img.size(), INTER_CUBIC);

	imshow("�A�t�B���ϊ�", dst);

	waitKey(0);
}