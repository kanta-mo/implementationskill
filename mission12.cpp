//カメラで撮影した画像からキャリブレーションを行う
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
	int  i,j,k;
	Size pattern_size = Size2i(10, 7);
	vector<Mat>srcimg;
	vector<Point2f> corners;
	vector<vector<Point2f>>img_points;

	//キャリブレーション画像の読み込み
	for (i = 0; i < 10; i++) {
		ostringstream oss;
		oss << i;
		Mat img = imread(oss.str() + ".jpg");
		if (img.empty()) {
			cerr << "画像が読み込めませんでした" << endl;
		}
		else {
			srcimg.push_back(img);
		}
	}

		//3次元空間座標の設定
	vector<Point3f> obj;
	for (j = 0; j < 7; j++) {
		for (k = 0; k < 10; k++) {
			Point3f p(j * 17.0, k * 17.0, 0.0);
			obj.push_back(p);
		}
	}

	vector<vector<Point3f>> obj_points;
	for (i = 0; i < 10; i++) {
		obj_points.push_back(obj);
	}

	//チェスボード(キャリブレーションパターン)のコーナー検出
	int found_num = 0;
	namedWindow("Calibration", WINDOW_AUTOSIZE);

	for (i = 0; i < 10; i++) {
		auto found = findChessboardCorners(srcimg[i], pattern_size, corners);
		if (found) {
			cout << i << "...ok" << endl;
			found_num++;
		}
		else {
			cerr << i << "...fail" << endl;
		}

		//コーナー位置をサブピクセル精度に修正、描画
		Mat src_gray = Mat(srcimg[i].size(), CV_8UC1);
		cvtColor(srcimg[i], src_gray, COLOR_BGR2GRAY);
		find4QuadCornerSubpix(src_gray, corners, Size(3, 3));
		drawChessboardCorners(srcimg[i], pattern_size, corners, found);
		img_points.push_back(corners);

		imshow("Calibration", srcimg[i]);
		waitKey(0);
	}

	destroyWindow("Calibration");

	if (found_num != 10) {
		cerr << "Caribration Images are insufficient" << endl;
		return -1;
	}

	//内部パラメータ、歪み係数の決定
	Mat cam_mat; //カメラ内部パラメータ行列
	Mat dist_coefs;	//歪み係数
	vector<Mat> rvecs, tvecs; //各ビューの回転ベクトルと並進ベクトル
	calibrateCamera(
		obj_points, 
		img_points, 
		srcimg[0].size(), 
		cam_mat, 
		dist_coefs, 
		rvecs, 
		tvecs
	);

	FileStorage fs("camera1.xml", cv::FileStorage::WRITE);
	if (!fs.isOpened())
	{
		cerr << "File can not be opened." << endl;
		return -1;
	}

	fs << "intrinsic" << cam_mat;
	fs << "distortion" << dist_coefs;
	fs.release();

	return 0;
}