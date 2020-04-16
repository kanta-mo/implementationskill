//2値化された画像をラベリングする
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
	Mat img, gray, dst, label;
	int nlabel, x, y;
	img = imread("Lenna.bmp");
	if (img.empty()) {
		cout << "画像が開けません";
		cin.get();
		return -1;
	}

	cvtColor(img, gray, COLOR_BGR2GRAY);

	threshold(gray, dst, 0, 255, THRESH_OTSU);

	nlabel = connectedComponents(dst, label);

	vector<Vec3b> colors(nlabel);
	colors[0] = Vec3b(0, 0, 0);
	for (int i = 1; i < nlabel; i++) {
		colors[i] = Vec3b((rand() & 255), (rand() & 255), (rand() & 255));
	}

	for (y = 0; y < img.rows; y++) {
		for (x = 0; x < img.cols; x++) {
			if(label.at<int>(y, x) > 0) {
				img.at<Vec3b>(y, x) = colors[label.at<int>(y, x)];
			}
			else{
				img.at<Vec3b>(y, x) = (0, 0, 0);
			}
		}
	}
	imshow("threshold", dst);
	imshow("labeling", img);
	waitKey(0);
}