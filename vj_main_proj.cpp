///source code final     do or die    ....////////

#include <opencv2/opencv.hpp>

bool lMouseDown  = false,started = false;
cv::Mat tgt1,tgt2,*tgt,src,*frames  = (cv::Mat*)malloc(sizeof(cv::Mat)*2);

void mouseCallback(int event,int x,int y,int flags,void * params)
{

	////mouse callback

	switch(event)
	{
	case CV_EVENT_LBUTTONDOWN:
		lMouseDown = true;
		if(!started)
		{
			cout<<"l mouse buton down"<<endl;
			tgt1 = frames[0];
			started = true;
		}
	case CV_EVENT_LBUTTONUP:
		lMouseDown = false;


	case CV_EVENT_MOUSEMOVE:
		if(lMouseDown)
			cout<<"mouse is  moving"<<endl;
		//cv::circle(tgt1,cv::Point(x,y),-1,cv::Scalar(0,255,255));

	}
}



int main()
{
	cv::VideoCapture cap ;
	cv::Mat frame;
	char choice;
	char *path = (char*)malloc(sizeof(char)*50);
	cout<<"enter the video path"<<endl;
	gets(path);

	cap.open(path);
	/*
	cap.read(frame);
	cv::namedWindow("video playback");
	
	cv::imshow("video playback",frame);
	frames[0] = frame;
	*/

	cv::setMouseCallback("video playback",mouseCallback);
	
	while(cap.read(frame))
	{

		//frames[0] = frame;
		
		cv::imshow("video playback",frame);
		choice = cv::waitKey(40);
		switch(choice)
		{
		case 27:
			exit(0);
			break;
		//coming soon
		}
		
	}
	return 0;
}