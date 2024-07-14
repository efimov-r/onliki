#ifndef RENDER_RENDER_HPP_
#define RENDER_RENDER_HPP_

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#define CV_NO_BACKWARD_COMPATIBILITY
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <chrono>
#include <iostream>

namespace render {



	#define RENDER_RGB_WIDTH 1280
	#define RENDER_RGB_HEIGHT 960
	#define START_SIZE 2.0 //стартовый масштаб в НЕорто режиме
	#define SIZE_STEP 3 //шаг масштаба
	#define OFFSET_STEP 10 //шаг смещения
	#define ROTATE_STEP 0.05 //угол в рад для шага вращения
	#define DROW_XYZ true//рисовать нулевые векторы?
	#define XYZ_LENGTH 400 //длина нулевых векторов
	#define BIGSIZE_POINTS false //точка как 3*3 пикселя на экране?
	#define XYZ_COLOR 0, 0, 255 //цвет нулевых отрезков
	#define CAM_Z -800 //положение камеры в орто
	#define Z_Z -300 //положение проецирующей прямой в орто
	#define Z_STEP 10 //шаг изменения проецирующей плоскости в орто
	#define DROW_ORTO true //орторежим?
	#define WAIT_KEY_MS 15 //время слипа между отрисовками

	using namespace std;
	using namespace cv;

	void doThread();
	void addData(vector<array<short, 3>> data);
	void setData(vector<array<short, 3>> data);



}

#endif /* RENDER_RENDER_HPP_ */
