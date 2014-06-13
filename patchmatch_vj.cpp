
#include <iostream>
#include <math.h>
#include<opencv2\opencv.hpp>

#define sametosame(x,y) if((x)==(y)) return -1;  //aithe aa quicke verappa
#define pow2(x) (x)=(x)*(x);
#define WHITE 255
#define BLACK 0


bool isMouseDown = false; // true if left mouse is  down 
bool isStarted = false; // represents whether drawing on window is started

     cv::Mat image //image is grayscale image used for masking ...
	,colorDup,imageDup,colorImage,randomMappedImage,randomPatchImage,pixelUpdationImage,randomPatchDup,finalImage; //pixelUpdationImage is used to stor the number of times a pixel is updated

int patchSize = 3;   //patch is a sqaure window default size is 3 pixels
const char* sourceWindow ="Source Window";
const char* targetWindow = "Target Window";



void sourceImageMouseCallback(int event,int x,int y,int flags,void *params)   //mouse call back to source image to 
{																			 // draw on image....................
	switch(event){
		case CV_EVENT_LBUTTONDOWN:
			if(!isStarted)
			{
				cv::Mat *images =(cv::Mat*)params;
				colorImage = images[0];
				imageDup = colorImage.clone();
				colorDup = colorImage.clone();
				image = images[1];
			}
			isMouseDown = true;
			isStarted = true;
			break;
		case CV_EVENT_LBUTTONUP:
			isMouseDown = false;
			break;
		case CV_EVENT_MOUSEMOVE:
			if(isMouseDown)
			{//draw onto the image
			
				cv::circle(image,cv::Point(x,y),10,cv::Scalar(255),-1);
				cv::circle(colorImage,cv::Point(x,y),10,cv::Scalar(0,255,255),-1);
			//	cv::imshow("masked window",image);
				cv::imshow(sourceWindow,colorImage);
			}
			break;
	}
}


cv::Mat findPatch(int x,int y,cv::Mat baseImage)  // returns the patch considering x,y as center
{
	int xmin = x - floor((float)patchSize/2);
	int xmax = x + floor((float)patchSize/2)+1;

	if(xmin < 0)
	{
		xmin = 0;
		xmax = patchSize;
	}

	if(xmax >baseImage.cols)
	{
		xmax = baseImage.cols;
		xmin = baseImage.cols - patchSize;
	}

	////y checking

	int ymin = y - floor((float)patchSize/2);
	int ymax = y + floor((float)patchSize/2)+1;

	if(ymin < 0)
	{
		ymin = 0;
		ymax = patchSize;
	}

	if(ymax >baseImage.rows)
	{
		ymax = baseImage.rows;
		ymin = baseImage.rows - patchSize;
	}


return baseImage(cv::Range(ymin,ymax),cv::Range(xmin,xmax));
}

float calcDistance(cv::Mat firstPatch,cv::Mat secondPatch)   //calculates distance between two patches....
{
	float dist  = 0;
	for(int x =0;x<firstPatch.cols;x++)
	{
		for(int y=0;y<firstPatch.rows;y++)
		{
			double dr,dg,db;
			dr = firstPatch.at<cv::Vec3b>(y,x)[0] - secondPatch.at<cv::Vec3b>(y,x)[0];
			dg = firstPatch.at<cv::Vec3b>(y,x)[1] - secondPatch.at<cv::Vec3b>(y,x)[1];
			db = firstPatch.at<cv::Vec3b>(y,x)[2] - secondPatch.at<cv::Vec3b>(y,x)[2];


			pow2(dr);
			pow2(dg);
			pow2(db);


			dist += sqrt(float(dr+dg+db));
		}
	}


	return dist;
}


int calcDistance(cv::Mat FirstPatch, cv::Mat SecondPatch, cv::Mat ThirdPatch, cv::Mat FourthPatch)
{
	float first2Second = calcDistance(FirstPatch, SecondPatch);
	float first2Third = calcDistance(FirstPatch, ThirdPatch);
	float first2Fourth = calcDistance(FirstPatch, FourthPatch);
	if (first2Second<=first2Third)
	{
		if (first2Second<=first2Fourth)
			return 1;
		else
			return 3;
	}
	else if (first2Third<= first2Fourth)
		return 2;
	else
		return 3;
}

int calcDistance(cv::Mat FirstPatch, cv::Mat SecondPatch, cv::Mat ThirdPatch)
{
	float first2Second = 0;
	float first2Third = 0;
	first2Second = calcDistance(FirstPatch, SecondPatch);
	first2Third = calcDistance(FirstPatch, ThirdPatch);
	if (first2Second <= first2Third)
		return 1;
	return 2;
}


double calculatePixelColorTq()   //T(q) is calulated bt formula sigma i=1 to n on (wi*s(pi))/sigma 1 to n on wi;
{
	double wi =0.0;

	for(int i=0;i<9;i++)
	{
		
	}
	return 0.0;
}

double errorOfPixel(int x, int y,cv::Mat image)
{
	return 0.0;
}


double inline calculatePatchWeight(cv::Mat sourcePatch,cv::Mat targetPatch)      //this function calculates the weight for the patch in the target image
{
	double w =0.0;
	int rows = sourcePatch.rows,cols = sourcePatch.cols;
	int k,t=0;//,l,m;


	for(int x=0;x < cols ; x++)
	{
		for(int y=0;y < rows ;y++)
		{
			//for color
			/*
			k=	sourcePatch.at<cv::Vec3b>(y,x)[0]-targetPatch.at<cv::Vec3b>(y,x)[0];
			k*=k;
			l=	sourcePatch.at<cv::Vec3b>(y,x)[1]-targetPatch.at<cv::Vec3b>(y,x)[1];
			l*=l;
			m=	sourcePatch.at<cv::Vec3b>(y,x)[2]-targetPatch.at<cv::Vec3b>(y,x)[2];
			m*=m;
			*/

			//for grayscale

			k = sourcePatch.at<uchar>(y,x)-targetPatch.at<uchar>(y,x);

			k*=k;

			t+= k;
			

		}
	}

	w=exp(-1*sqrt((float)t));
	return w;
}

void createOffsetMap()
{
	int rows = image.rows;
	int cols = image.cols;
	randomMappedImage = cv::Mat(image.rows,image.cols,colorImage.type());
	
	int temp,tempX,tempY;
	for(int x = 0;x<cols;x++)
	{
		for(int y=0;y<rows;y++)
		{
			temp  = image.at<uchar>(y,x);
			if(temp == WHITE)
			{
				do{


					tempY = (int)abs(rand()%rows - y) ;
					tempX = (int)abs(rand()%cols - x);
					//cout<<"OK upto here"<<tempY<<" "<<tempX<<endl;
					temp = image.at<uchar>(tempY,tempX);
					
				}while(temp ==  WHITE);
				//cout<<"bachana ae haseeno"<<endl;
				randomMappedImage.at<cv::Vec3b>(y,x)[0]=	tempY;// rand()%rows - y;//tempY;
				randomMappedImage.at<cv::Vec3b>(y,x)[1]= tempX; //rand()%cols - x;//tempX;
				randomMappedImage.at<cv::Vec3b>(y,x)[2] = 255;					

				
			}
			else
			{
				
				randomMappedImage.at<cv::Vec3b>(y,x)[0]= -1;
				randomMappedImage.at<cv::Vec3b>(y,x)[1]= -1;
				randomMappedImage.at<cv::Vec3b>(y,x)[2] = -1;					
			}
		}

	}
}




void randomPatchesCreation()   // creates a random patch with the randomImageOffset
{
	randomPatchImage = cv::Mat::zeros(image.rows,image.cols,colorImage.type());
	int k = (int)patchSize/2;

	int rows = colorImage.rows,cols = colorImage.cols;
	int tempX,tempY;
	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<cols;j++)
		{
			if(image.at<uchar>(i,j)==WHITE)
			{
				tempY = randomMappedImage.at<cv::Vec3b>(i,j)[0];
				tempX = randomMappedImage.at<cv::Vec3b>(i,j)[1];


				//getting things under control
				if(tempX<=0)
				{
					tempX = 1;
				}
				if(tempX>=cols-1)
				{
				
					tempX = cols - 1;
				}

				if(tempY<=0)
				{
					tempY = 1;
				}
				if(tempY>=rows-1)
				{
				
					tempY = rows - 1;
				}
				////everything under control

				for(int pX=-k;pX<=k;pX++)
				{
					for(int pY = -k;pY<=k;pY++)
					{
						randomPatchImage.at<cv::Vec3b>(i+pX,j+pY)[0]  = imageDup.at<cv::Vec3b>(tempY+pX,tempX+pY)[0];
						randomPatchImage.at<cv::Vec3b>(i+pX,j+pY)[1]  = imageDup.at<cv::Vec3b>(tempY+pX,tempX+pY)[1];
						randomPatchImage.at<cv::Vec3b>(i+pX,j+pY)[2]  = imageDup.at<cv::Vec3b>(tempY+pX,tempX+pY)[2];
					}
				}

			}
		}
	}
}

void initiation()   // inititation step in which the masked image is scanned from left to right for white pixels if it is white
{					//then  the patch is considered around it and particular patch is to be considered with the outer image patches
	/*
	int rows=image.rows,cols = image.rows;
	

	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<cols;j++)
		{
			if(image.at<uchar>(i,j)==WHITE)
			{//now construct a patch around the 

			
			}
		}
	}*/

	createOffsetMap();
	cout<<"offset creation completed"<<endl;
	cv::imshow("offswet map",randomMappedImage);
	randomPatchesCreation();

	cout<<"random patches created"<<endl;
	cv::imshow("random Patches",randomPatchImage);
}

void propogation()   //propogation step in which

{

	int k = (int)patchSize/2;
	int rows = image.rows,cols = image.cols;
	double distance1,distance2;
	int temp1r,temp1g,temp1b,temp2r,temp2g,temp2b,sum1=0,sum2=0;
	randomPatchDup = randomPatchImage.clone();
	for(int i=k+1;i<rows-k-1;i++)
	{
		for(int j=k+1;j<cols-k-1;j++)
		{//comparing the best among the left and top patches;
			if(image.at<uchar>(i,j)== WHITE)   //if white then consider the patch  and compare patches
			{
				if(image.at<uchar>(i,j)==WHITE || image.at<uchar>(i,j)== WHITE)
					continue;

				for(int pX=-k;pX<=k;pX++)
				{
					for(int pY = -k ; pY<=k ; pY++)
					{
						//left patch distance.............
						temp1r = randomPatchImage.at<cv::Vec3b>(i+pX,j+pY)[0]-randomPatchImage.at<cv::Vec3b>(i+pX,j+pY-1)[0];		
						temp1r*=temp1r;
						temp1g = randomPatchImage.at<cv::Vec3b>(i+pX,j+pY)[1]-randomPatchImage.at<cv::Vec3b>(i+pX,j+pY-1)[1];		
						temp1g*=temp1g;
						temp1b = randomPatchImage.at<cv::Vec3b>(i+pX,j+pY)[2]-randomPatchImage.at<cv::Vec3b>(i+pX,j+pY-1)[2];		
						temp1b*=temp1b;

						//top patch distance..............
						temp2r = randomPatchImage.at<cv::Vec3b>(i+pX,j+pY)[0]-randomPatchImage.at<cv::Vec3b>(i+pX-1,j+pY)[0];		
						temp2r*=temp2r;
						temp2g = randomPatchImage.at<cv::Vec3b>(i+pX,j+pY)[1]-randomPatchImage.at<cv::Vec3b>(i+pX-1,j+pY)[1];		
						temp2g*=temp2g;
						temp2b = randomPatchImage.at<cv::Vec3b>(i+pX,j+pY)[2]-randomPatchImage.at<cv::Vec3b>(i+pX-1,j+pY)[2];		
						temp2b*=temp2b;


						///////sum ////////////////////////////

						sum1 += temp1r+temp1g+temp1b;
						sum2 += temp2r+temp2g+temp2b;
					}
					///// ssd distance of two ppatches
					distance1 = sqrt((float)sum1);  //left patch
					distance2 = sqrt((float)sum2);	//top patch

					if(distance1<distance2)
					{//left patch is best so replace the patch wih left patch......

						for(int pX=-k;pX<=k;pX++)
						{
							for(int pY = -k ; pY<=k ; pY++)
							{
								//left patch distance.............
								 randomPatchDup.at<cv::Vec3b>(i+pX,j+pY)[0] = randomPatchImage.at<cv::Vec3b>(i+pX,j+pY-1)[0];		
								
								randomPatchDup.at<cv::Vec3b>(i+pX,j+pY)[1] = randomPatchImage.at<cv::Vec3b>(i+pX,j+pY-1)[1];		
						
								randomPatchDup.at<cv::Vec3b>(i+pX,j+pY)[2] = randomPatchImage.at<cv::Vec3b>(i+pX,j+pY-1)[2];		
						

							}
						}
					}
					else
					{//top patch is best so replace the patch with roght patch.......

						for(int pX=-k;pX<=k;pX++)
						{
							for(int pY = -k ; pY<=k ; pY++)
							{
						
							//top patch distance..............
								 randomPatchDup.at<cv::Vec3b>(i+pX,j+pY)[0] = randomPatchImage.at<cv::Vec3b>(i+pX-1,j+pY)[0];		
								
								randomPatchDup.at<cv::Vec3b>(i+pX,j+pY)[1] = randomPatchImage.at<cv::Vec3b>(i+pX-1,j+pY)[1];		
						
								randomPatchDup.at<cv::Vec3b>(i+pX,j+pY)[2] = randomPatchImage.at<cv::Vec3b>(i+pX-1,j+pY)[2];		
						

							}

						}
					}
			}
		}
	}
}

}
void randomSearch(int radius) //random search step in which patches are searched around the pixel with radius
{///searching for best patch
	bool same = false;
	int k= (int)floor((float)(patchSize/2));
	cv::Mat imageDup1 = colorDup.clone();
	//cv::imshow("imageDup1",imageDup1);
	printf("random search is started and is processing ");

	int rows = image.rows,cols = image.cols,rs,re,cs,ce;
	int maxDist = 500, rd,gd,bd,tot;
	

	for(int i=1;i<rows-1;i++)
	{
		for(int j=1;j<cols-1;j++)
		{
			for(int r = radius ; r>1;r/=2)
			{
				maxDist = 500;

			//cout<<i<<" "<<j<<" "<<r<<endl;
			if(image.at<uchar>(i,j)==WHITE)
			{



				rs = i-r;
				re = i+r;
				cs = j-r;
				ce = j+r;

				if(i-r<1)
				{
					rs= 1;
				}
				if(i+r>rows-1)
				{
					re = rows-1;
				}
			
				if(j-r<1)
				{
					cs = 1;
				}
				if(j+r>cols-1)
				{
					ce = cols - 1;
				}

				same = false;


				for(int rx = rs;rx<=re;rx++)
				{
					for(int cx = cs;cx<=ce;cx++)
					{//choose the best patch and replace it
						if((rx == i && cx  == j) || image.at<uchar>(rx,cx)==WHITE)
						{
							same = true;
							continue;
						}
						rd=-1,bd = -1;gd = -1;
						for(int pX =-k;pX <=k ; pX++)
						{
							for(int pY =-k ;pY<=k ;pY++)
							{
								if(rx+pX >= 0 && rx+pX < rows && cx+pY >=0 && cx+pY <cols ){
								rd = imageDup.at<cv::Vec3b>(i+pX,j+pY)[0]-imageDup.at<cv::Vec3b>(rx+pX,cx+pY)[0];
								rd *= rd;
								gd = imageDup.at<cv::Vec3b>(i+pX,j+pY)[1]-imageDup.at<cv::Vec3b>(rx+pX,cx+pY)[1];
								gd *= gd;
								bd = imageDup.at<cv::Vec3b>(i+pX,j+pY)[2]-imageDup.at<cv::Vec3b>(rx+pX,cx+pY)[2];
								bd *= bd;
								}
								tot = rd+gd+bd;



								
							}	
			
						}
						tot = (int)sqrt((float)tot);

						

								if(tot < maxDist)
								{//update distance and patch  toooo....:) :)
						//			cout<<"patch updating"<<i<<","<<j<<" ||"<<rx<<","<<cx<<endl;

									maxDist = tot;

//									image.at<uchar>(i,j) = BLACK;
									for(int pX =-k;pX <=k ; pX++)
									{
										for(int pY =-k ;pY<=k ;pY++)
										{
											imageDup.at<cv::Vec3b>(i+pX,j+pY)[0] = imageDup.at<cv::Vec3b>(rx+pX,cx+pY)[0];
								
											imageDup.at<cv::Vec3b>(i+pX,j+pY)[1] = imageDup.at<cv::Vec3b>(rx+pX,cx+pY)[1];
								
											 imageDup.at<cv::Vec3b>(i+pX,j+pY)[2] = imageDup.at<cv::Vec3b>(rx+pX,cx+pY)[2];
											 printf(".");
								
										}	
			
									}
									
								}

			
					}
				}
			}//if
			
		}//radius
			
		//	printf("!");
	}
	
}

	cout<<"\n random search completed"<<endl;

	//cv::imshow("in random search",imageDup1);
}
void addtoImage()
{
	int rows = image.rows,cols = image.cols;
	for(int i=0;i<rows ; i++)
	{
		for(int j=0;j<cols;j++)
		{
			if(image.at<uchar>(i,j) == BLACK)
			{

			}
			else
			{
				
				imageDup.at<cv::Vec3b>(i,j)[0] = randomPatchDup.at<cv::Vec3b>(i,j)[0];
				imageDup.at<cv::Vec3b>(i,j)[1] = randomPatchDup.at<cv::Vec3b>(i,j)[1];
				imageDup.at<cv::Vec3b>(i,j)[2] = randomPatchDup.at<cv::Vec3b>(i,j)[2];
			}
		}
	}

	
}

void startProcess()
{
	cout<<"process is started"<<endl;
	cv::pyrDown(image,image);
	cv::pyrDown(imageDup,imageDup);
	cv::pyrDown(colorDup,colorDup);
	cv::pyrDown(colorImage,colorImage);
	cv::pyrDown(finalImage,finalImage);
	cv::pyrDown(randomPatchImage,randomPatchImage);
	//cv::pyrDown(randomPatchesCreation,randomPatchesCreation);
	cv::pyrDown(randomPatchDup,randomPatchDup);
	cv::pyrDown(randomMappedImage,randomMappedImage);
				
	initiation();
	propogation();

	cout<<"propogation phase is completed"<<endl;

	cv::imshow("random patches after propogation",randomPatchDup);
	
	addtoImage();

	cv::imshow("complete image with random patches after propogation",imageDup);
	randomSearch(40);


		cv::imshow("complete image after random search",imageDup);


}





int main()
{
	char choice;

	char *path = (char*)malloc(sizeof(char)*100);
	cout<<"Enter the image path"<<endl;
	gets(path);

	cv::Mat imageS = cv::imread(path);  // Source Image  ---- S
	

	cv::Mat imageT = imageS.clone();	//Target Image	------T

	cv::Mat masked = cv::Mat::zeros(imageS.rows,imageS.cols,CV_8UC1);
	
	cv::Mat images[2]={imageS,masked};

	cv::namedWindow(sourceWindow,CV_WINDOW_AUTOSIZE);
	cv::namedWindow(targetWindow,CV_WINDOW_AUTOSIZE);

	cv::imshow(sourceWindow,imageS);
	cv::setMouseCallback(sourceWindow,sourceImageMouseCallback,(void*)images);
	
	while(1)
	{
		choice = cv::waitKey();
		switch(choice)
		{
			case 27:
				exit(0);
				break;
			case 's':
				cout<<"Starting the process"<<endl;

				startProcess();
				break;
		}
	}
	return 0;
}