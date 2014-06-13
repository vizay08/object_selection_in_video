#include<opencv2\opencv.hpp>

#define fun(x,y,k,l)  
cv::Mat decreaseResolution(cv::Mat src ,cv::Rect part)
{
	///////////////////////decrease resolution


	int i,j,r,c,rp,cp,px,py;
	r=src.rows;
	c= src.cols;
	rp= part.width;
	cp = part.height;
	px = part.x;
	py = part.y;

	///////////////////////////////////

	for(i=py;i<py+cp;i++)
		for(j=px;j<px+rp;j++)
		{

			
		}


}

int main()
{
    cv::Mat image,image1,lowres;
	char *s= (char *)malloc(sizeof(char)*30);
	gets(s);
	image=cv:: imread(s);
	cv::imshow("figure1",image);
	
	cv::cvtColor(image,image1,CV_RGB2GRAY);
	cv::imshow("figure2",image1);
	cv::Rect part = cv::Rect(45,90,(int)image1.rows/4,(int)image1.cols/4);
	///////////////////////////////////////////////////////////////////////
	// decrease the resolution of the selected part //////////////////////
	/////////////////////////////////////////////////////////////////////


	lowres = decreaseResolution(image1,part);


	cv::waitKey();
	return 0;
	
}