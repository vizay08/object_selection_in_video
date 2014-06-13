#include <iostream>
#include <opencv2\opencv.hpp>
#include "imageop.h" 
#include "videoprocessor.h"
// #include "objectselection.h"

using namespace std;

cv::Point initialPosition ;
bool mouseOn = false;
bool isSelected = false;

cv::Point setPosition(int x,int y)
{
	return cv::Point(x,y);
}



void onMouse(int event ,int x ,int y,int flags,void * param)
{
	/*
	if(param != NULL){
	cv::Mat* im = (cv::Mat*)param;
	if(im != NULL)
		cv::imshow("someimage",im[0]);
	}*/
	if(!isSelected)
	switch(event)
	{
		case CV_EVENT_LBUTTONDOWN:
			//cout<<"left mouse down at "<<x<<" "<<y<<endl;
			initialPosition = setPosition(x,y);
			mouseOn = true;

			break;
		case CV_EVENT_LBUTTONUP:
			//cout<<"left mouse up at "<<x<<" "<<y<<endl;
			mouseOn = false;
			isSelected = true;
			
			break;
		
		case CV_EVENT_MOUSEMOVE:
			if(mouseOn)
			cout<<"mouse moving "<<x<<" "<<y<<endl;
			break;
	}
}

int main()
{
	cv::Mat image[1];
	char* path = (char*)malloc(sizeof(char)*30);
/*	cout<<"Enter the path for the image"<<endl;
	
	gets(path);
	image[0] = cv::imread(path);
	
	
	cv::namedWindow("video");
	cout<<path<<endl;
	//display(path);
	//cv::setMouseCallback("video",onMouse,(void*)image);

	cv::setMouseCallback("video",onMouse);
	*/
	rough_object_selection("video");

	cout<<"Enter the path for the video"<<endl;
	gets(path);
	
	start_video(path);

	cv::waitKey();

	return 0;
}


