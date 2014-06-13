#include<opencv2\opencv.hpp>

int get_camera_id(int range)
{
	cv::VideoCapture capture;
	printf("in get_camera_test");
	int i;
	for(i=1;i<=range;i++)
	{
		capture.open(i);
		if(capture.isOpened())
		{
			capture.release();
			return i;
		}
	}

	capture.release();
	return 0;
}

void start_camera(int deviceID)
{
	cv::VideoCapture cap ;
	char c;
	cap.open(deviceID);
	cv::Mat frame;
	if(cap.isOpened())
	{
		while((c=cv::waitKey(40))!='q')
		{
			cap.read(frame);
			cv::imshow("video_camera",frame);
		}
		if(c=='q')
		{
			cap.release();  
		    exit(0);
		}
	}
}
