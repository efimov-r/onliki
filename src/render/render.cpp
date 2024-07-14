#include "render.hpp"

namespace render {
	Mat renderimg;

	bool gupdate;
	bool gneedClear;
	vector<array<short, 3>> glistOfArrays;
	pthread_mutex_t mutex_render = PTHREAD_MUTEX_INITIALIZER;
	const Vec3b render_point = Vec3b(255, 255, 255);
	const Vec3b xyz_point = Vec3b(XYZ_COLOR);

	void drowPoint(int x, int y) {
#if BIGSIZE_POINTS
		renderimg.at<cv::Vec3b>(y, x) = render_point;
		renderimg.at<cv::Vec3b>(y-1, x) = render_point;
		renderimg.at<cv::Vec3b>(y, x-1) = render_point;
		renderimg.at<cv::Vec3b>(y-1, x-1) = render_point;
		renderimg.at<cv::Vec3b>(y+1, x) = render_point;
		renderimg.at<cv::Vec3b>(y, x+1) = render_point;
		renderimg.at<cv::Vec3b>(y+1, x+1) = render_point;
		renderimg.at<cv::Vec3b>(y-1, x+1) = render_point;
		renderimg.at<cv::Vec3b>(y+1, x-1) = render_point;
#else
		renderimg.at<cv::Vec3b>(y, x) = render_point;
#endif
	}

	void drowX(int x, int y) {
		renderimg.at<cv::Vec3b>(y, x) = xyz_point;
		renderimg.at<cv::Vec3b>(y-1, x-1) = xyz_point;
		renderimg.at<cv::Vec3b>(y-2, x-2) = xyz_point;
		renderimg.at<cv::Vec3b>(y+1, x+1) = xyz_point;
		renderimg.at<cv::Vec3b>(y+2, x+2) = xyz_point;
		renderimg.at<cv::Vec3b>(y-1, x+1) = xyz_point;
		renderimg.at<cv::Vec3b>(y-2, x+2) = xyz_point;
		renderimg.at<cv::Vec3b>(y+1, x-1) = xyz_point;
		renderimg.at<cv::Vec3b>(y+2, x-2) = xyz_point;
	}

	void drowY(int x, int y) {
		renderimg.at<cv::Vec3b>(y, x) = xyz_point;
		renderimg.at<cv::Vec3b>(y+1, x-1) = xyz_point;
		renderimg.at<cv::Vec3b>(y+2, x-2) = xyz_point;
		renderimg.at<cv::Vec3b>(y-1, x+1) = xyz_point;
		renderimg.at<cv::Vec3b>(y-2, x+2) = xyz_point;
		renderimg.at<cv::Vec3b>(y-1, x-1) = xyz_point;
		renderimg.at<cv::Vec3b>(y-2, x-2) = xyz_point;
	}

	void drowZ(int x, int y) {
		renderimg.at<cv::Vec3b>(y, x) = xyz_point;
		renderimg.at<cv::Vec3b>(y+2, x-2) = xyz_point;
		renderimg.at<cv::Vec3b>(y+2, x-1) = xyz_point;
		renderimg.at<cv::Vec3b>(y+2, x) = xyz_point;
		renderimg.at<cv::Vec3b>(y+2, x+1) = xyz_point;
		renderimg.at<cv::Vec3b>(y-2, x-1) = xyz_point;
		renderimg.at<cv::Vec3b>(y-2, x) = xyz_point;
		renderimg.at<cv::Vec3b>(y-2, x+1) = xyz_point;
		renderimg.at<cv::Vec3b>(y-2, x+2) = xyz_point;
		renderimg.at<cv::Vec3b>(y-1, x+1) = xyz_point;
		renderimg.at<cv::Vec3b>(y-2, x+2) = xyz_point;
		renderimg.at<cv::Vec3b>(y+1, x-1) = xyz_point;
		renderimg.at<cv::Vec3b>(y+2, x-2) = xyz_point;
	}

	void drowClear()
	{
		renderimg = Mat(Size(RENDER_RGB_WIDTH, RENDER_RGB_HEIGHT), CV_8UC3, Scalar(0, 0, 0));
	}

	void drow_xyz(int w_offset, int h_offset, float rotateY, float rotateX) {
		int h_center = RENDER_RGB_HEIGHT/2 + h_offset + h_offset;
		int w_center = RENDER_RGB_WIDTH/2 + w_offset + w_offset;
		int x_hig = RENDER_RGB_WIDTH-4;
		int y_hig = RENDER_RGB_HEIGHT-4;
		float sin_y = sin(rotateY);
		float cos_y = cos(rotateY);
		float sin_x = sin(rotateX);
		float cos_x = cos(rotateX);
		for(int i = 1; i <= XYZ_LENGTH; i++) {
			int x = round((i*cos_y) + w_center);
			int y = round(h_center - (i*sin_y*sin_x));
			if(x>2 && x < x_hig && y > 2 && y < y_hig){
				renderimg.at<cv::Vec3b>(y, x) = xyz_point;
			}

			y = round((i*cos_x) + h_center);
			if(x>2 && x < x_hig && y > 2 && y < y_hig){
				renderimg.at<cv::Vec3b>(y, w_center) = xyz_point;
			}

			x = round(w_center - (i*sin_y));
			y = round(h_center - (i * cos_y*sin_x));
			if(x>2 && x < x_hig && y > 2 && y < y_hig){
				renderimg.at<cv::Vec3b>(y, x) = xyz_point;
			}
		}

		int x = round((XYZ_LENGTH*cos_y) + w_center) + 4;
		int y = round(h_center - (XYZ_LENGTH*sin_y*sin_x)) + 4;
		if(x>9 && x < RENDER_RGB_WIDTH-9 && y > 9 && y < RENDER_RGB_HEIGHT-9){
			drowX(x, y);
		}

		x = round(w_center) + 4;
		y = round((XYZ_LENGTH*cos_x) + h_center) + 4;
		if(x>9 && x < RENDER_RGB_WIDTH-9 && y > 9 && y < RENDER_RGB_HEIGHT-9){
			drowY(x, y);
		}

		x = round((-1 * (XYZ_LENGTH*sin_y)) + w_center) + 4;
		y = round((-1 * ((XYZ_LENGTH * cos_y)*sin_x)) + h_center) + 4;
		if(x>9 && x < RENDER_RGB_WIDTH-9 && y > 9 && y < RENDER_RGB_HEIGHT-9){
			drowZ(x, y);
		}
	}

	void do3Dto2D(vector<array<short, 3>> input3D, float zoom, int w_offset, int h_offset, float rotateY, float rotateX, int z_z) {
		float sin_y = sin(rotateY);
		float cos_y = cos(rotateY);
		float sin_x = sin(rotateX);
		float cos_x = cos(rotateX);
		int mam_z_min_z_z = CAM_Z - z_z;

		short h_center = RENDER_RGB_HEIGHT/2 + h_offset;
		short w_center = RENDER_RGB_WIDTH/2 + w_offset;
#if BIGSIZE_POINTS
		int x_hig = RENDER_RGB_WIDTH-4;
		int y_hig = RENDER_RGB_HEIGHT-4;
#else
		int x_hig = RENDER_RGB_WIDTH-1;
		int y_hig = RENDER_RGB_HEIGHT-1;
#endif

#if DROW_ORTO
		zoom = START_SIZE;
#endif



		for (const auto& arr : input3D) {
			short in0 = arr[0];
			short in1 = arr[1];
			short in2 = arr[2];
			float x = (in0*cos_y) - (in2 * sin_y);
			float z = (in0*sin_y) + (in2 * cos_y);
			float y = (in1*cos_x) - (z*sin_x);
#if DROW_ORTO
			z = (in1*sin_x) + (z * cos_x);
			if(z < z_z) {
				continue;
			}
			float co = mam_z_min_z_z/(CAM_Z - z);
			x = x * co;
			y = y * co;
#endif
			int x2d = round((x*zoom) + w_center);
			int y2d = round((y*zoom) + h_center);
			if(x2d>2 && x2d < x_hig && y2d > 2 && y2d < y_hig){
				drowPoint(x2d, y2d);
			}
		}
#if DROW_XYZ
		drow_xyz(w_offset, h_offset, rotateY, rotateX);
#endif
		imshow("Render", renderimg);
	}

	void addData(vector<array<short, 3>> data) {
		pthread_mutex_lock( &mutex_render );
		gupdate = true;
		glistOfArrays = data;
		pthread_mutex_unlock( &mutex_render );
	}

	void setData(vector<array<short, 3>> data) {
		pthread_mutex_lock( &mutex_render );
		gupdate = true;
		gneedClear = true;
		glistOfArrays = data;
		pthread_mutex_unlock( &mutex_render );
	}


	void doThread() {
		pthread_mutex_lock( &mutex_render );
		gupdate = true;
		gneedClear = true;
		pthread_mutex_unlock( &mutex_render );

		bool update;
		bool needClear;
		vector<array<short, 3>> listOfArrays;

	#if DROW_ORTO
		float size = Z_Z;
	#else
		float size = START_SIZE;
	#endif
		int w_offset = 0;
		int h_offset = 0;
		float rotate_horizont = 0;
		float rotate_vertical = 0;

		while(true) {
			pthread_mutex_lock( &mutex_render );
			update = gupdate;
			needClear = gneedClear;
			listOfArrays = glistOfArrays;
			pthread_mutex_unlock( &mutex_render );
			auto start = std::chrono::high_resolution_clock::now(); // сохраняем время начала выполнения функции
			if(update) {
				update = false;
				if(needClear) {
					needClear = false;
					drowClear();
				}
				do3Dto2D(listOfArrays, size, w_offset, h_offset, rotate_horizont ,rotate_vertical, size);
			}
			auto end = std::chrono::high_resolution_clock::now(); // сохраняем время окончания выполнения функции
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start); // вычисляем продолжительность выполнения функции в микросекундах
			std::cout << "Duration: " << duration.count() << " milllisec" << std::endl; // выводим продолжительность выполнения функции в консоль
			int kk = cvWaitKey(WAIT_KEY_MS);
	//		if(kk != -1) {
	//			cerr << kk <<endl;
	//		}
			if(kk == 1048667) { //[ size+
				update = true;
				needClear = true;
	#if DROW_ORTO
				size = size + Z_STEP;
	#else
				size = size + SIZE_STEP;
	#endif
			}
			if(kk == 1048635) {//; size-
				update = true;
				needClear = true;
	#if DROW_ORTO
				size = size - Z_STEP;
	#else
				size = size - SIZE_STEP;
	#endif
			}
			if(kk == 1048622) {//> right
				update = true;
				needClear = true;
				w_offset = w_offset + OFFSET_STEP;
			}
			if(kk == 1048620) {//< left
				update = true;
				needClear = true;
				w_offset = w_offset - OFFSET_STEP;
			}
			if(kk == 1048669) {//] top
				update = true;
				needClear = true;
				h_offset = h_offset - OFFSET_STEP;
			}
			if(kk == 1048615) {//' bottom
				update = true;
				needClear = true;
				h_offset = h_offset + OFFSET_STEP;
			}
			if(kk == 1048676) {//d
				update = true;
				needClear = true;
				rotate_horizont = rotate_horizont + ROTATE_STEP;
			}
			if(kk == 1048673) {//a
				update = true;
				needClear = true;
				rotate_horizont = rotate_horizont - ROTATE_STEP;
			}
			if(kk == 1048695) {//w top
				update = true;
				needClear = true;
				rotate_vertical = rotate_vertical + ROTATE_STEP;
			}
			if(kk == 1048691){//s down
				update = true;
				needClear = true;
				rotate_vertical = rotate_vertical - ROTATE_STEP;
			}
		}
	}
}
