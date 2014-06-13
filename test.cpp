#include<opencv2\opencv.hpp>
#include "camera.h"


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
	cout<<get_camera_id(6)<<endl;
	cv::waitKey();
	return 0;
}

				
}

