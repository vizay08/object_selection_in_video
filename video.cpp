#include<opencv2\opencv.hpp>
#include<stdio.h>

void start_video(char* path)
{
     cv::VideoCapture cap ;
	
	char c;
	
	cap.open(path);
	cv::Mat frame;
	if(cap.isOpened())
	{
		while((c=cv::waitKey(40))!='q')
		{
			cap.read(frame);
			cv::imshow("video_from_file",frame);
			
		}
		if(c=='q')
		{
			cap.release();  
		    exit(0);
		}
	}

}