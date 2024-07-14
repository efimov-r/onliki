//============================================================================
// Name        : onliki.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <libfreenect.h>
#include <pthread.h>
#define CV_NO_BACKWARD_COMPATIBILITY
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <unistd.h>
#include "render/render.hpp"
#include <thread>
#include <chrono>
#include <boost/exception/all.hpp>


#define FREENECTOPENCV_RGB_DEPTH 3
#define FREENECTOPENCV_DEPTH_DEPTH 1
#define FREENECTOPENCV_RGB_WIDTH 640
#define FREENECTOPENCV_RGB_HEIGHT 480
#define FREENECTOPENCV_DEPTH_WIDTH 640
#define FREENECTOPENCV_DEPTH_HEIGHT 480

using namespace std;
using namespace cv;

Mat depthimg, rgbimg, tempimg;
uint16_t center_value;

pthread_mutex_t mutex_depth = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_rgb = PTHREAD_MUTEX_INITIALIZER;
pthread_t cv_thread, render_thread;

Mat renderimg;
int ld[640];


void depth_cb(freenect_device *dev, void *depth, uint32_t timestamp)
{

	Mat renderimg = Mat(Size(FREENECTOPENCV_DEPTH_WIDTH, FREENECTOPENCV_DEPTH_HEIGHT), CV_8UC3, Scalar(0, 0, 0));
		vector<array<short, 3>> listOfArrays;

		for (int i = 0; i < FREENECTOPENCV_DEPTH_HEIGHT; i++) {
			for (int j = 0; j < FREENECTOPENCV_DEPTH_WIDTH; j++) {
				uint16_t value = *((uint16_t*) depth + (i * FREENECTOPENCV_DEPTH_WIDTH + j));
				if(value < 1100) {
//				if(value < 924) {
					short sm = (short)(35565.702733/(1100.64694 - value))/2;
					renderimg.at<cv::Vec3b>(i, j) = Vec3b(0, (short)(sm), (short)(255 - sm));

					listOfArrays.push_back({(short)(j-320), (short)(i-240), (short)(35565.702733/(1100.64694 - value)/2)});
//					listOfArrays.push_back({(short)(j-320), (short)(i-240), (short)value});
				}
			}
		}

		for (int j = 0; j < FREENECTOPENCV_DEPTH_WIDTH; j++) {
			ld[j] = *((uint16_t*) depth + (240 * FREENECTOPENCV_DEPTH_WIDTH + j));
			ld[j] = (int)(35565.702733/(1100.64694 - ld[j]));
		}
		render::setData(listOfArrays);



//    cv::Mat depth8;
//    cv::Mat mydepth = cv::Mat( FREENECTOPENCV_DEPTH_WIDTH,FREENECTOPENCV_DEPTH_HEIGHT, CV_16UC1, depth);

//    mydepth.convertTo(depth8, CV_8UC1, 1.0/4.0);
//    memcpy(depthimg.data, depth8.data, 640*480);
	pthread_mutex_lock( &mutex_depth );
    memcpy(depthimg.data, renderimg.data, 640*480*3);
    center_value = *((uint16_t*) depth + (240 * FREENECTOPENCV_DEPTH_WIDTH + 320));
    // unlock mutex
    pthread_mutex_unlock( &mutex_depth );

}

void rgb_cb(freenect_device *dev, void *rgb, uint32_t timestamp)
{
    pthread_mutex_lock( &mutex_rgb );
    memcpy(rgbimg.data,rgb,(FREENECTOPENCV_RGB_WIDTH+0)*(FREENECTOPENCV_RGB_HEIGHT+950));
    pthread_mutex_unlock( &mutex_rgb );
}

void cv_threadfunc () {
	try{
		while (1)
		{
			pthread_mutex_lock( &mutex_depth );
//			cvtColor(depthimg,tempimg,CV_BGR2RGB);
//			cvtColor(tempimg,tempimg,CV_HSV2BGR);

			float ed = (float)center_value;
			float sm = 35565.70273305854789214209/(1100.64694429075643711258 - ed);
			int smi = sm;


			cv::putText(depthimg, to_string(smi), Point(50, 100), FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0, 0, 255), 2, 1, false);

			cv::line(depthimg, cv::Point(0, 240), cv::Point(639, 240), {255, 0, 0}, 1);

			imshow("Depthimage",depthimg);

			renderimg = Mat(Size(640, 700), CV_8UC3, Scalar(0, 0, 0));
			for (int j = 0; j < FREENECTOPENCV_DEPTH_WIDTH; j++) {
				if(ld[j] < 640) {
					renderimg.at<cv::Vec3b>(690 - ld[j], j) = {255, 0, 0};
				}
			}
			renderimg.at<cv::Vec3b>(690, 320) = {255, 255, 0};
			renderimg.at<cv::Vec3b>(690, 321) = {255, 255, 0};
			renderimg.at<cv::Vec3b>(691, 320) = {255, 255, 0};
			renderimg.at<cv::Vec3b>(691, 321) = {255, 255, 0};

			cv::line(renderimg, cv::Point(0, 590), cv::Point(639, 590), {255, 255, 0}, 1);
			cv::line(renderimg, cv::Point(0, 490), cv::Point(639, 490), {255, 255, 0}, 1);
			cv::line(renderimg, cv::Point(0, 390), cv::Point(639, 390), {255, 255, 0}, 1);
			cv::line(renderimg, cv::Point(0, 290), cv::Point(639, 290), {255, 255, 0}, 1);
			cv::line(renderimg, cv::Point(0, 190), cv::Point(639, 190), {255, 255, 0}, 1);
			imshow("lidar",renderimg);


			pthread_mutex_unlock( &mutex_depth );

			pthread_mutex_lock( &mutex_rgb );
			cvtColor(rgbimg,tempimg,CV_BGR2RGB);

//			imshow("Normalimage", tempimg);

			pthread_mutex_unlock( &mutex_rgb );


	        if(cvWaitKey(15) == 27) {
	        	printf("breack!!!\n");
	        	break;
	        }



		}
	} catch(const boost::exception& e) {
		cout << boost::diagnostic_information(e) << endl;
	}
//    pthread_exit(NULL);

//    return NULL;

}

void render_threadfunc () {
//	void *render_threadfunc (void *ptr) {
	render::doThread();
    pthread_exit(NULL);
//    return NULL;
}

int main() {
	depthimg = cv::Mat(FREENECTOPENCV_DEPTH_HEIGHT, FREENECTOPENCV_DEPTH_WIDTH, CV_8UC3);
	rgbimg = cv::Mat(FREENECTOPENCV_RGB_HEIGHT, FREENECTOPENCV_RGB_WIDTH, CV_8UC3);
	tempimg = cv::Mat(FREENECTOPENCV_RGB_HEIGHT, FREENECTOPENCV_RGB_WIDTH, CV_8UC3);

	freenect_context *f_ctx;
	freenect_device *f_dev;


	if (freenect_init(&f_ctx, NULL) < 0)
	{
		printf("freenect_init() failed\n");
		return 1;
	}

	freenect_select_subdevices(f_ctx, FREENECT_DEVICE_CAMERA);

	if (freenect_open_device(f_ctx, &f_dev, 0) < 0)
	{
		printf("Could not open device\n");
		return 1;
	}

	freenect_set_depth_callback(f_dev, depth_cb);
	freenect_set_video_callback(f_dev, rgb_cb);
	freenect_video_format video_format = FREENECT_VIDEO_RGB; // выбираем формат RGB
	freenect_resolution resolution = FREENECT_RESOLUTION_MEDIUM; // выбираем среднее разрешение
	freenect_frame_mode video_mode = freenect_find_video_mode(resolution, video_format); // находим подходящий режим видео
	freenect_set_video_mode(f_dev, video_mode);

	thread t(cv_threadfunc);
//	thread g(render_threadfunc);


	printf("init done\n");

	freenect_start_depth(f_dev);
	freenect_start_video(f_dev);

	while(freenect_process_events(f_ctx) >= 0 );

	printf("End\n");
	return 0;
}
