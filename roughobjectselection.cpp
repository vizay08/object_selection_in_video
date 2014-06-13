#include<opencv2\opencv.hpp>
#include<stdio.h>

int rectStartx = -1,rectStarty = -1,rectEndx =-1,rectEndy = -1;
bool down = false;
bool stored = false;
bool isPressed = false;
int presentX = -1,presentY = -1;
void mouseCallback(int event ,int x ,int y,int flags,void * param)
{

	///////////////////////////////
	switch(event)
	{
	case CV_EVENT_LBUTTONDOWN:
		rectStartx = x;
		rectStarty  = y;
		isPressed = true;
		printf("mouse is down %d %d \n",x,y);
		break;
	case CV_EVENT_LBUTTONUP:
		rectEndx = x;
		rectEndy  = y;
		stored  = true;
		isPressed = false;
		printf("mouse is up %d %d\n",x,y);
		break;

	case CV_EVENT_MOUSEMOVE:
		if(isPressed)
		{
		 presentX  = x;
		 presentY = y;
		printf("mouse is moving %d %d \n",x,y);
		}
		break;

	}
}

cv::Point getPresentPoint()
{
	return cv::Point(presentX,presentY);
}

void setCallBack(char* windowName)
{//sets callback to that window

	cv::namedWindow(windowName);
	cv::setMouseCallback(windowName,mouseCallback);

}

