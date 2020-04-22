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
#include <glm/gtc/type_ptr.hpp>
#include "shader.hpp"
#include "objloader.hpp"


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
			//cout << i << "...ok" << endl;
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

	//imshow("opencv",srcimg[0]);
	//回転行列(3*3)
	Mat rvecs_mat;
	//回転ベクトルを回転行列に変換
	Rodrigues(rvecs2, rvecs_mat);
	Mat tvecs_nom = tvecs2 / 17.0;	//チェスボードの大きさで割る

	Mat view_cv = Mat::zeros(4,4,CV_64F);
	view_cv.at<double>(0, 0) = rvecs_mat.at<double>(0, 0);	view_cv.at<double>(0, 1) = rvecs_mat.at<double>(0, 1);	view_cv.at<double>(0, 2) = rvecs_mat.at<double>(0, 2); view_cv.at<double>(0, 3) = tvecs_nom.at<double>(0);
	view_cv.at<double>(1, 0) = rvecs_mat.at<double>(1, 0);	view_cv.at<double>(1, 1) = rvecs_mat.at<double>(1, 1);	view_cv.at<double>(1, 2) = rvecs_mat.at<double>(1, 2); view_cv.at<double>(1, 3) = tvecs_nom.at<double>(1);
	view_cv.at<double>(2, 0) = rvecs_mat.at<double>(2, 0);	view_cv.at<double>(2, 1) = rvecs_mat.at<double>(2, 1);	view_cv.at<double>(2, 2) = rvecs_mat.at<double>(2, 2); view_cv.at<double>(2, 3) = tvecs_nom.at<double>(2);
	view_cv.at<double>(3, 3) = 1;

	Mat Rot_x = Mat::zeros(4, 4, CV_64F);
	Rot_x.at<double>(0, 0) = 1; Rot_x.at<double>(1, 1) = -1; Rot_x.at<double>(2, 2) = -1; Rot_x.at<double>(3, 3) = 1;
	view_cv= Rot_x * view_cv;

	transpose(view_cv, view_cv);

	/*FileStorage fs("external parameter.xml", cv::FileStorage::WRITE);
	if (!fs.isOpened())
	{
		cerr << "File can not be opened." << endl;
		return -1;
	}

	fs << "rotation" << rvecs2;
	fs << "translation" << tvecs2;
	fs.release();
	//cout << tvecs2/17.0;*/
	
	//GLFWの初期化
	if (!glfwInit()) {
		cerr << "Failed to initialize GLFW\n";
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//ウィンドウを開く
	GLFWwindow* window = glfwCreateWindow(srcimg[0].cols, srcimg[0].rows, "Marker capture",NULL,NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	//レンダリングを行うウィンドウを指定する
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW.\n";
		return -1;
	}
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	GLuint Background_VertexArrayID;	//頂点配列オブジェクト(VAO)
	glGenVertexArrays(1, &Background_VertexArrayID);	//VAO作成
	glBindVertexArray(Background_VertexArrayID);	//頂点配列オブジェクトを結合

	//モデル用のVAO
	GLuint Model_VertexArrayID;	
	glGenVertexArrays(1, &Model_VertexArrayID);
	glBindVertexArray(Model_VertexArrayID);

	GLuint programID = LoadShaders("background.vert", "background.frag");
	GLuint model_programID = LoadShaders("model.vert", "model.frag");

	GLuint MatrixID = glGetUniformLocation(model_programID, "MVP");
	//プロジェクション行列
	mat4 Projection;
	cameraFrustumRH(cam_mat, srcimg[0], Projection, 0.1, 500);

	//mat4 Projection = perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 100.0f);

	mat4 View=make_mat4(reinterpret_cast<GLdouble*>(view_cv.data));

	/*mat4 View = glm::lookAt(
		glm::vec3(4, 3, 3), // ワールド空間でカメラは(4,3,3)
		glm::vec3(0, 0, 0), // 原点を見ている
		glm::vec3(0, 1, 0)  // 頭が上方向(0,-1,0にセットすると上下逆転する)
	);*/


	mat4 Model = mat4(1.0f);	//単位行列
	Model[1][2] = 1.0; Model[2][1] = -1.0; Model[1][1] = 0.0; Model[2][2] = 0.0;

	mat4 MVP = Projection * View * Model;

	const GLfloat background_vertices[] = {
		-1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
	};

	const GLfloat background_uv[] = {
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
	};

	GLuint texture;
	glGenTextures(1, &texture);	//テクスチャオブジェクトを生成
	glBindTexture(GL_TEXTURE_2D, texture);	//テクスチャの次元を指定、指名したテクスチャを有効にする
	cvtColor(srcimg[0], srcimg[0], cv::COLOR_BGR2RGBA);
	//画像データと関連付ける
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, srcimg[0].cols, srcimg[0].rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, srcimg[0].data);
	//画像を補完する際のパラメータ指定
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint textureID = glGetUniformLocation(programID, "texture");

	GLuint bg_vertexbuffer;
	glGenBuffers(1, &bg_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, bg_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(background_vertices), background_vertices, GL_STATIC_DRAW);

	GLuint background_uvbuffer;
	glGenBuffers(1, &background_uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, background_uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(background_uv), background_uv, GL_STATIC_DRAW);

	//objファイルの読み込み
	vector<vec3> vertices;
	vector<vec2> uvs;
	vector<vec3> normals;
	bool res = loadOBJ("axis.obj", vertices,uvs, normals);

	//VBOに読み込む
	GLuint model_vertexbuffer;
	glGenBuffers(1, &model_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, model_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);

	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(programID);
	

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(textureID, 0);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, bg_vertexbuffer);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, background_uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			2,                                // size : U+V => 2
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glUseProgram(model_programID);
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, model_vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	glDeleteBuffers(1, &bg_vertexbuffer);
	glDeleteBuffers(1, &background_uvbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &texture);
	glDeleteVertexArrays(1, &Background_VertexArrayID);
	glfwTerminate();
	return 0;
}

//プロジェクション行列の計算
void cameraFrustumRH(Mat camMat, Mat srcimg, mat4& projMat, double znear, double zfar) {
	// Load camera parameters
	double fx = camMat.at<double>(0, 0);
	double fy = camMat.at<double>(1, 1);
	double cx = camMat.at<double>(0, 2);
	double cy = camMat.at<double>(1, 2);
	double w = srcimg.cols, h = srcimg.rows;

	mat4 projection(
		2.0 * fx / w,		 0,					 0,									   0,
		0,					 2.0 * fy / h,	     0,									   0,
		1.0 - 2.0 * cx / w, -1.0 + 2.0 * cy / h, -(zfar + znear) / (zfar - znear),	  -1.0,
		0,					 0,					 -2.0 * zfar * znear / (zfar - znear), 0);
	projMat = projection;
}
