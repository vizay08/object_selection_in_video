#include<opencv2\opencv.hpp>
#include<stdio.h>
#include<conio.h>
#include "camera.h"
#include "video.h"
#include "roughobjectselection.h"

/*
void camera_test(char *s)
{
	
	cv::Mat frame ;
	cv::Mat frame_gray,frame_canny,frame_blur;

	//char *s  = (char*)malloc(sizeof(char)*40);
	//gets(s);

	int i,framecount = 0;
	for(i=1;i<100;i++)
	{
		cv::VideoCapture cap(i);
		if(cap.isOpened())
			break;
		////////////////////////////
	}
	cout<<i<<endl;
		cv::VideoCapture cap(s);
	bool gotu= true;
	cv::namedWindow("video");
	if(cap.isOpened())
	{
		while(1)
		{
		//takes sequence of images from video
		gotu = cap.read(frame);
		cv::cvtColor(frame,frame_gray,CV_RGB2GRAY);
		GaussianBlur(frame_gray, frame_blur, cv::Size(7,7), 1.5, 1.5);
		cv::Canny(frame_blur,frame_canny,0,30,3);
		cv::namedWindow("video");
		cv::setMouseCallback("video",mouseCallback);
		if(gotu)
		{
			framecount++;
			if(rectStartx != -1 && rectEndx  != -1){
			cv::rectangle(frame,cv::Point(rectStartx,rectStarty),cv::Point(rectEndx,rectEndy),cv::Scalar(255,255,0,1));
			cv::Mat roi= cv::Mat(frame,cv::Rect(cv::Point(rectStartx,rectStarty),cv::Point(rectEndx,rectEndy)));
			cv::imwrite("selected_roi.png",roi);
			rectStartx = -1,rectStarty = -1,rectEndx =-1,rectEndy = -1;
			
			
			}
			cv::imshow("video",frame);
			cv::imshow("video_gray",frame_gray);
			cv::imshow("video_canny",frame_canny);
	

			if(framecount!=10)
				cv::imwrite("somefile.png",frame);
			//cv::destroyWindow("video");
		
			printf("got u \n");
					cv::waitKey(41);
		}
		else
			printf("didn't got u\n");
		}
	}

	else
		printf("capture is not happened");
	cap.release();
	cv::waitKey();
	
}
*/



int main()
{

	char *s= (char *)malloc(sizeof(char)*20);
	gets(s);
	
	int i=get_camera_id(20);
	
    //start_camera(i);
	//camera_test(s);
	setCallBack("video_from_file");

	start_video(s);

	cv::waitKey();
	
	getch();
	return 0;
}