// first phase to select the image -- mouse or touch listener
#include<opencv2\opencv.hpp>

bool isOn = false,notHappened = true,isMoving = false; // mouse button is pressed and input is being taken
int startX = -1,startY = -1,endX = -1,endY = -1,presentX = -1,presentY =-1;
vector<cv::Point> movingPoints;

bool isMouseDown()
{
	return isOn;
}

bool itsMoving()
{
	return isMoving;
}


void mouseCallback(int event,int x,int y,int flags,void* params)
{
	switch(event){
	case CV_EVENT_LBUTTONDOWN:
		startX = x;
		startY = y;
		isOn = true;
		break;
	case CV_EVENT_LBUTTONUP:
		endX = x;
		endY = y;
		isOn = false;
		notHappened = false;

		break;
	case CV_EVENT_MOUSEMOVE:
		if(isOn && notHappened)
		{
			isMoving = true;
			presentX = x;
			presentY = y;

			movingPoints.push_back(cv::Point(x,y));

		cout<<"baby its moving.... "<<x<<" "<<y<<endl;
		}
		isMoving = false;
		break;
	}
}
void rough_object_selection(char* windowName)
{

	cv::namedWindow(windowName);

	cv::setMouseCallback(windowName,mouseCallback);
}

cv::Point getPresentPoint()
{
	return cv::Point(presentX,presentY);
}

cv::Point getStartPoint()
{
	return cv::Point(startX,startY);
}

cv::Point getEndPoint()
{
	return cv::Point(endX,endY);
}


vector<cv::Point> getMovingPoints()
{
	return movingPoints;
}

