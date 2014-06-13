#include<opencv2\opencv.hpp>

int main()
{
	cv::Mat image1,image2,imageop;
	char * path = (char*)malloc(sizeof(char)*20);
	cout<<"enter image1 path"<<endl;
	gets(path);
	image1  = cv::imread(path);

	cout<<"enter image2 path"<<endl;
	gets(path);
	image2  = cv::imread(path);

	
	imageop = image1 & image2;

	cv::imshow("image1",image1);
	cv::imshow("image2",image2);

	cv::imshow("imageop",imageop);

	cv::waitKey();
	return 0;
}