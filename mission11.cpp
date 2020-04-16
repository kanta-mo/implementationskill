//2�l�����ꂽ�摜�����x�����O����
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
	Mat img ,gray,dst,label;
	int thresh,nlabel,x,y;
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
		threshold(gray, dst, thresh, 255, THRESH_BINARY);

		nlabel = connectedComponents(dst, label);

		vector<Vec3b> colors(nlabel);
		colors[0] = Vec3b(0, 0, 0);
		for (int i = 1; i < nlabel; i++) {
			colors[i] = Vec3b((rand() & 255), (rand() & 255), (rand() & 255));
		}

		for (y = 0; y < img.rows; y++) {
			for (x = 0; x < img.cols; x++) {
				if (label.at<int>(y, x) > 0) {
					img.at<Vec3b>(y, x) = colors[label.at<int>(y, x)];
				}
				else {
					img.at<Vec3b>(y, x) = (0, 0, 0);
				}
			}
		}
		imshow("threshold", dst);
		imshow("labeling", img);
	}
}