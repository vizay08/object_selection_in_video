#include<opencv2\opencv.hpp>
#include <iostream>
void display(char* path)
{//display the image 
	cout<<"in display function"<<endl;
	cout<<path<<endl;
	cv::Mat image = cv::imread(path);
	cv::imshow("image",image);

	
};


