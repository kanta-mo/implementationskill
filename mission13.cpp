//�ē��e�_����͉摜��ɕ\������E�ϑ��Ώۂɑ΂���J�����̈ʒu�p�����擾����

#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
	int  i, j, k;
	Size pattern_size = Size2i(10, 7);
	vector<Mat>srcimg;
	vector<Point2f> corners;
	vector<vector<Point2f>>img_points;

	//�L�����u���[�V�����摜�̓ǂݍ���
	for (i = 0; i < 10; i++) {
		ostringstream oss;
		oss << i;
		Mat img = imread(oss.str() + ".jpg");
		if (img.empty()) {
			cerr << "�摜���ǂݍ��߂܂���ł���" << endl;
		}
		else {
			srcimg.push_back(img);
		}
	}

	//3������ԍ��W�̐ݒ�
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

	//�`�F�X�{�[�h(�L�����u���[�V�����p�^�[��)�̃R�[�i�[���o
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

		//�R�[�i�[�ʒu���T�u�s�N�Z�����x�ɏC���A�`��
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

	//�����p�����[�^�A�c�݌W���̌���
	Mat cam_mat; //�J���������p�����[�^�s��
	Mat dist_coefs;	//�c�݌W��
	vector<Mat> rvecs, tvecs; //�e�r���[�̉�]�x�N�g���ƕ��i�x�N�g��
	Mat rvecs2, tvecs2;

	calibrateCamera(
		obj_points,
		img_points,
		srcimg[0].size(),
		cam_mat,
		dist_coefs,
		rvecs,
		tvecs
	);



	vector<Point2f>img_point2;	//�ē��e����摜��̍��W

	projectPoints(obj_points[0], rvecs[0], tvecs[0], cam_mat, dist_coefs, img_point2);

	for (i = 0; i < 70; i++) {
		circle(srcimg[0], img_point2[i], 3, Scalar(0, 0, 200), 1, 4);
	}

	imshow("projectPoint", srcimg[0]);
	waitKey(0);

	//���͉摜�ɂ������]�x�N�g��,���i�x�N�g�������߂�
	solvePnP(
		obj_points[0],
		img_points[0],
		cam_mat,
		dist_coefs,
		rvecs2,
		tvecs2
	);

	FileStorage fs("external parameter.xml", cv::FileStorage::WRITE);
	if (!fs.isOpened())
	{
		cerr << "File can not be opened." << endl;
		return -1;
	}

	fs << "rotation" << rvecs2;
	fs << "translation" << tvecs2;
	fs.release();

	return 0;
}