//カラー画像を読み込んで45度回転させる
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
	Mat img,dst;

	img = imread("Lenna.bmp");
	if (img.empty()) {
		cout << "画像が読み込めません";
		cin.get();
		return -1;
	}

	float angle = 45.0, scale = 1.0;
	Point2f center(img.cols * 0.5, img.rows * 0.5);

	Mat affine_matrix = getRotationMatrix2D(center, angle, scale);
	
	warpAffine(img, dst, affine_matrix, img.size(), INTER_CUBIC);

	imshow("アフィン変換", dst);

	waitKey(0);
}