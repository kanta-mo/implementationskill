//観測対象に対するカメラの位置姿勢を取得する・OpenGLを用いて座標軸を描画する
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include "shader.hpp"

using namespace std;
using namespace cv;
using namespace glm;

void cameraFrustumRH(Mat camMat, Mat srcimg, mat4& projMat, double znear, double zfar);

int main(int argc, char* argv[]) {
	int  i, j, k;
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
		img_points.push_back(corners);
	}

	if (found_num != 10) {
		cerr << "Caribration Images are insufficient" << endl;
		return -1;
	}

	//内部パラメータ、歪み係数の決定
	Mat cam_mat; //カメラ内部パラメータ行列
	Mat dist_coefs;	//歪み係数
	vector<Mat> rvecs, tvecs; //各ビューの回転ベクトルと並進ベクトル
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

	//入力画像における回転ベクトル,並進ベクトルを求める
	solvePnP(
		obj_points[0],
		img_points[0],
		cam_mat,
		dist_coefs,
		rvecs2,
		tvecs2
	);


	/*
	GLuint programID = LoadShaders("SimpleTransform.vertexshader","SingleColor.fragmentshader");
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	//プロジェクション行列
	mat4 Projection;
	cameraFrustumRH(cam_mat, srcimg[0],Projection,0.1,5000);

	//カメラビュー行列
	mat4 View = mat4(1.0)
				*lookAt(
					vec3(0, 0, 0),	//カメラの位置
					vec3(0, 0, 1),	//原点を見ている
					vec3(0,-1, 0)	//カメラの上方向
				);

	//モデル行列
	mat4 Model = mat4(1.0f);

	mat4 MVP = Projection * View * Model;
	/*FileStorage fs("PV", cv::FileStorage::WRITE);
	if (!fs.isOpened())
	{
		cerr << "File can not be opened." << endl;
		return -1;
	}

	fs << "プロジェクション行列" << Projection;
	fs << "カメラビュー行列" << View;
	fs.release();*/

	//cout << to_string(View);

	while (1) {
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_RGBA);
		glutInitWindowSize(srcimg[0].cols, srcimg[0].rows);
		glutCreateWindow("marker capture");
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(-1, 1, -1, 1);
		glViewport(-1, -1, srcimg[0].cols, srcimg[0].rows);

		flip(srcimg[0], srcimg[0], 0);
		cvtColor(srcimg[0], srcimg[0], COLOR_BGR2RGB);

		glClear(GL_COLOR_BUFFER_BIT);
		glDrawPixels(srcimg[0].cols, srcimg[0].rows, GL_RGB, GL_UNSIGNED_BYTE, srcimg[0].data);
		glFlush();

		glutMainLoop();
	}

	return 0;
}

void cameraFrustumRH(Mat camMat, Mat srcimg, mat4& projMat, double znear, double zfar) {
	// Load camera parameters
	double fx = camMat.at<double>(0, 0);
	double fy = camMat.at<double>(1, 1);
	double cx = camMat.at<double>(0, 2);
	double cy = camMat.at<double>(1, 2);
	double w = srcimg.rows, h = srcimg.cols;

	mat4 projection(
		2.0 * fx / w, 0, 0, 0,
		0, 2.0 * fy / h, 0, 0,
		1.0 - 2.0 * cx / w, -1.0 + 2.0 * cy / h, -(zfar + znear) / (zfar - znear), -1.0,
		0, 0, -2.0 * zfar * znear / (zfar - znear), 0);
	projMat = projection;
}