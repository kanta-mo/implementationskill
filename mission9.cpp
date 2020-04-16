//�t�������m�C�Y����������
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
	Mat img, gray,gray_blur,gray_med;
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

	blur(gray, gray_blur,Size(3,3));
	medianBlur(gray, gray_med, 3);

	imshow("blur", gray_blur);
	imshow("median", gray_med);
	waitKey(0);
	return 0;

}