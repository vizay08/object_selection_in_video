#include<opencv2\opencv.hpp>

void mouseCallback(int event ,int x,int y,int flags,void* param);
int main()
{
	int a;
	char *s= (char *)malloc(sizeof(char)*20);
	gets(s);
	cv::Mat image = cv::imread(s);
	cout<<"image created"<<endl;
	cv::imshow("image",image);
	cv::setMouseCallback("image",mouseCallback,(void *)&image);
	cv::rectangle(image,cv::Point(196,179),cv::Point(494,419),cv::Scalar(0,255,0));
	
	cv::waitKey();
	return 0;
}


void mouseCallback(int event ,int x ,int y,int flags,void * param)
{

			switch(event)
	{
	case CV_EVENT_LBUTTONDOWN:
		printf("mouse is down %d %d \n",x,y);
		break;
	case CV_EVENT_LBUTTONUP:
		printf("mouse is up %d %d\n",x,y);
		break;

	case CV_EVENT_MOUSEMOVE:
		printf("mouse is moving %d %d \n",x,y);
		break;

	}

		
}

