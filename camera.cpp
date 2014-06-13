#include <opencv2\opencv.hpp>

int get_cam_id(int range)
{
	int i;
	cv::VideoCapture capture;
	for(i = 1 ; i<=range ; i++)
	{
		capture.open(i);
		if(capture.isOpened())
			return i;
	}
	return -1;
}