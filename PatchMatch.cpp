//+++++++++++++++++++++++++++++++++++
//++++ Patch Match by Barnes	+++++
//++++ Authors: Damien Thome	+++++
//++++			Christian Kohl	+++++	
//++++ Date:	18.02.2012		+++++
//+++++++++++++++++++++++++++++++++++

#include "stdafx.h"
//#include "cv.h"
#include <opencv2\opencv.hpp>
//#include "highgui.h"
#include <sstream>
#include <iostream>
#include <math.h>

using namespace std;
using namespace cv;

int mousepress = 0;
bool IsLMouseDown = false;
bool IsRMouseDown = false;
int ROIxUpCorner = 0;
int ROIyUpCorner = 0;
int ROIxDownCorner = 0;
int ROIyDownCorner = 0;
int NewXOfROI = 0;
int NewYOfROI = 0;
int sizeOfPatch = 1;
bool withConstraints = false;
vector<Point2f> points(6);

Mat Simg;
Mat Timg;
Mat newTimg;
Rect ImageRoi;

Mat Scale(int numberOfScales, Mat image)
{
	Mat temp = image.clone();
	for (int i=0;i<numberOfScales;i++)
	{
		pyrDown(temp, temp);
	}
	return temp;
}

void DisplayImage(Mat Image, Size orgSize, int sessionId)
{
	stringstream stream, streamfile;
	stream << "New Image " << sessionId;;
	string session = stream.str();
	streamfile << session << ".jpg";
	string file = streamfile.str();
	Mat tempImage = Image.clone();
	cvtColor(tempImage, tempImage, CV_Lab2RGB);
	tempImage.convertTo(tempImage,CV_8UC3,255,0);
	Mat resizeImage;
    resize( tempImage, resizeImage, orgSize, 0, 0, INTER_NEAREST );
	
	imshow(session, resizeImage);
	imwrite(file, resizeImage);
	//cvWaitKey(0);
}

Mat FindPatch(int x, int y, Mat image)
{
	//Find Range
	//+1 to get all the pixels around the x/y pixel. because through rounding we lose one pixel
	int xmin = x - floor((float)sizeOfPatch/2);
	int xmax = x + floor((float)sizeOfPatch/2) + 1;
	if (xmin < 0)
	{
		xmin = 0;
		xmax = sizeOfPatch;
	}
	if (xmax > image.cols)
	{
		xmax = image.cols;
		xmin = image.cols-sizeOfPatch;
	}

	int ymin = y - floor((float)sizeOfPatch/2);
	int ymax = y + floor((float)sizeOfPatch/2) + 1;
	if (ymin < 0)
	{
		ymin = 0;
		ymax = sizeOfPatch;
	}
	if (ymax > image.rows)
	{
		ymax = image.rows;
		ymin = image.rows-sizeOfPatch;
	}
	return image(Range(ymin, ymax), Range(xmin, xmax));
}

float calcDistance(Mat firstPatchLAB, Mat secondPatchLAb)
{
	assert(firstPatchLAB.cols == secondPatchLAb.cols);
	assert(firstPatchLAB.rows == secondPatchLAb.rows);

	float dif=0;
	
	for(int x=0;x< firstPatchLAB.cols;x++)
	{
		for(int y=0;y< firstPatchLAB.rows;y++)
		{
			double dif0,dif1,dif2;
			dif0 = firstPatchLAB.at<Vec3f>(y,x)[0] - secondPatchLAb.at<Vec3f>(y,x)[0];
			dif0 *= dif0;
			dif1 = firstPatchLAB.at<Vec3f>(y,x)[1] - secondPatchLAb.at<Vec3f>(y,x)[1];
			dif1 *= dif1;
			dif2 = firstPatchLAB.at<Vec3f>(y,x)[2] - secondPatchLAb.at<Vec3f>(y,x)[2];
			dif2 *= dif2;
   
			dif += sqrt(float( dif0 + dif1 + dif2));
			//printf("dif0: %d dif1: %d dif2: %d dif: %f \n",dif0,dif1,dif2,dif);
		}
	}
	return dif;
}

int calcDistance(Mat FirstPatch, Mat SecondPatch, Mat ThirdPatch, Mat FourthPatch)
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

int calcDistance(Mat FirstPatch, Mat SecondPatch, Mat ThirdPatch)
{
	float first2Second = 0;
	float first2Third = 0;
	first2Second = calcDistance(FirstPatch, SecondPatch);
	first2Third = calcDistance(FirstPatch, ThirdPatch);
	if (first2Second <= first2Third)
		return 1;
	return 2;
}


Mat Propagation(Mat OffsetMap, Mat SourceImage, Mat TargetImage, bool odd)
{
	int x,y,i=0;
	for(x=1;x<TargetImage.cols;x++)
	{
		for(y=1;y<TargetImage.rows;y++)
		{
			Mat TargetPatch, SourcePatch, SourcePatchUpDown, SourcePatchLeftRight;
			int xT,yT,xUpDown,yUpDown,xLeftRight,yLeftRight;
			if (odd)
			{
				xT = x;
				yT = y;
				xUpDown = xT;
				if (y==0)
					yUpDown = yT;
				else
					yUpDown = yT-1;
				if (x==0)
					xLeftRight = xT;
				else
					xLeftRight = xT-1;
				yLeftRight = yT;
			}
			else
			{
				xT = OffsetMap.cols - x -1;
				yT = OffsetMap.rows - y -1;
				xUpDown= xT;
				if (x==0)
					xLeftRight = xT;
				else
					xLeftRight = xT + 1;
				if (y==0)
					yUpDown = yT;
				else
					yUpDown = yT + 1;
				yLeftRight = yT;
			}
		
			if (withConstraints && OffsetMap.at<Vec3f>(yT,xT)[2] == 255){
				i++;
				printf("x %f y %f\n",(OffsetMap.at<Vec3f>(yT,xT)[0]), (OffsetMap.at<Vec3f>(yT,xT)[1]));
			}
			else
			{
				int a,b,c,d,e,f,g,h;
				a = OffsetMap.at<Vec3f>(yT,xT)[1] + yT;
				if (a >= SourceImage.rows)
					a=SourceImage.rows -1;
				b = OffsetMap.at<Vec3f>(yT,xT)[0] + xT;
				if (b >= SourceImage.cols)
					b=SourceImage.cols-1;
				e = OffsetMap.at<Vec3f>(yLeftRight,xLeftRight)[1] + yT;//yLeftRight;
				if (e >= SourceImage.rows)
					e=SourceImage.rows -1;
				f = OffsetMap.at<Vec3f>(yLeftRight,xLeftRight)[0] + xT;//xLeftRight;
				if (f >= SourceImage.cols)
					f=SourceImage.cols-1;
				g = OffsetMap.at<Vec3f>(yUpDown,xUpDown)[1] + yT;//yUpDown;
				if (g >= SourceImage.rows)
					g=SourceImage.rows -1;
				h = OffsetMap.at<Vec3f>(yUpDown,xUpDown)[0] + xT;//xUpDown;
				if (h >= SourceImage.cols)
					h=SourceImage.cols-1;
				c = yT;
				if (c >= TargetImage.rows)
					c=TargetImage.rows -1;
				d = xT;
				if (d >= TargetImage.cols)
					d=TargetImage.cols-1;

				TargetPatch = FindPatch(d,c,TargetImage);
				SourcePatch = FindPatch(b,a,SourceImage);
				SourcePatchLeftRight = FindPatch(f,e, SourceImage);
				SourcePatchUpDown = FindPatch(h,g, SourceImage);

				
				//calculate minimale distance
				int patchNumber = calcDistance(TargetPatch, SourcePatch, SourcePatchUpDown,SourcePatchLeftRight);
				switch(patchNumber)
				{
				case 2:
					OffsetMap.at<Vec3f>(yT,xT)[0] = h -xT;//- xUpDown;
					OffsetMap.at<Vec3f>(yT,xT)[1] = g -yT;//- yUpDown -1;
					break;
				case 3:
					OffsetMap.at<Vec3f>(yT,xT)[0] = f -xT;//- xLeftRight +1;
					OffsetMap.at<Vec3f>(yT,xT)[1] = e -yT;//- yLeftRight;
					break;
				}
			}
		}
	}
	//printf("%i\n",i);
	return OffsetMap;
}

Mat RandomSearch(Mat OffsetMap, Mat SourceImage, Mat TargetImage, double w, double alpha)
{
	//double alpha=0.95;
	int x,y, l = 0;
	for(x=0;x<OffsetMap.cols;x++)
	{
		for(y=0;y<OffsetMap.rows;y++)
		{
			if (withConstraints && OffsetMap.at<Vec3f>(y,x)[2] == 255)
			{
				l++;
			}
			else
			{
				//random value between [-1;1]
				float rx= rand() % 2000 /1000.f - 1.f;
				float ry= rand() % 2000 /1000.f - 1.f;
				int i=0;
				while(true)
				{
					float tx = w*pow(alpha,i)*rx;
					float ty = w*pow(alpha,i)*ry;
					float hypo = sqrt(pow(tx,2) + pow(ty,2));
					if (hypo <1)	//if below than one pixel break the loop
						break;
					int tempx = OffsetMap.at<Vec3f>(y,x)[0] + x + tx;
					int tempy = OffsetMap.at<Vec3f>(y,x)[1] + y + ty;
					if ((tempx>=0 && tempx <= OffsetMap.cols -1) && (tempy>=0 && tempy <= OffsetMap.rows-1))
					{
						Mat patchTarget = FindPatch(x, y, TargetImage);
						Mat patchSourceNew = FindPatch(tempx, tempy,SourceImage);
						Mat patchSourceOld = FindPatch(OffsetMap.at<Vec3f>(y,x)[0] + x, OffsetMap.at<Vec3f>(y,x)[1] +y, SourceImage);
						//calculate Distance
						int patchnumber = calcDistance(patchTarget, patchSourceNew, patchSourceOld);
						switch (patchnumber)
						{
						case 1:
							OffsetMap.at<Vec3f>(y,x)[0] = tempx - x;
							OffsetMap.at<Vec3f>(y,x)[1] = tempy - y;
							break;
						}
					}
					i++;
				}
			}
		}
	}
	
	return OffsetMap;
}

Mat Generate(Mat OffsetMap, Mat SourceImage)
{
	Mat Inc(SourceImage.rows, SourceImage.cols, CV_32FC1, Scalar::all(0));
	Mat targetImage(SourceImage.rows, SourceImage.cols, CV_32FC3, Scalar::all(0));
	
	float a, b;
	int x,y;
	for (x=0;x<targetImage.cols;x++)
	{
		for (y=0;y<targetImage.rows;y++)
		{
			a = y + OffsetMap.at<Vec3f>(y,x)[1];
			if (a >= SourceImage.rows)
				a=SourceImage.rows -1;
			b = x+ OffsetMap.at<Vec3f>(y,x)[0];
			if (b >= SourceImage.cols)
				b=SourceImage.cols-1;
			
			Mat SubInc = FindPatch(x,y, Inc);
			SubInc +=1;
			/*c = y;
			if (c >= TargetImage.rows)
				c=TargetImage.rows -1;
			d = x;
			if (d >= TargetImage.cols)
				d=TargetImage.cols-1;
			targetImage.at<Vec3f>(y,x)[0] = (SourceImage.at<Vec3f>(a,b)[0]);// + TargetImage.at<Vec3f>(c,d)[0]) / 2;
			targetImage.at<Vec3f>(y,x)[1] = (SourceImage.at<Vec3f>(a,b)[1]);// + TargetImage.at<Vec3f>(c,d)[1]) / 2;
			targetImage.at<Vec3f>(y,x)[2] = (SourceImage.at<Vec3f>(a,b)[2]);// + TargetImage.at<Vec3f>(c,d)[2]) / 2;*/
			Mat SourcePatch = FindPatch(b,a,SourceImage);
			Mat TargetPatch = FindPatch(x,y,targetImage);
			
			for(int j=0;j < sizeOfPatch;j++)
            {
                for(int k=0;k < sizeOfPatch;k++)
                {
					/*if (OffsetMap.at<Vec3f>(y,x)[2] == 255)
			{
				TargetPatch.at<Vec3f>(k,j)[0] += 1;
					TargetPatch.at<Vec3f>(k,j)[1] += 1;
					TargetPatch.at<Vec3f>(k,j)[2] += 1;
					}
					else
					{*/
					TargetPatch.at<Vec3f>(k,j)[0] += SourcePatch.at<Vec3f>(k,j)[0];
					TargetPatch.at<Vec3f>(k,j)[1] += SourcePatch.at<Vec3f>(k,j)[1];
					TargetPatch.at<Vec3f>(k,j)[2] += SourcePatch.at<Vec3f>(k,j)[2];
					//}
					//TargetPatch.at<Vec3f>(k,j)[0] /= 2;
					//TargetPatch.at<Vec3f>(k,j)[1] /= 2;
					//TargetPatch.at<Vec3f>(k,j)[2] /= 2;
				}
			}
		}
	}
	//calculate the average by dividing colorchannel by the amount of patch overlapping
	//at each pixel
	std::vector<Mat> rgb(3);
	split(targetImage,rgb);
	rgb[0] /= Inc;
	rgb[1] /= Inc;
	rgb[2] /= Inc;
	merge(rgb,targetImage); 
	return targetImage;
}

void RandomValuesForOffset(Mat OffsetMap, bool WithReserve)
{
	int x,y;
	int Offset = -10;
	int NROIxUpCorner = ROIxUpCorner - Offset;
	int NROIyUpCorner = ROIyUpCorner;// - Offset;
	for (x=0;x<OffsetMap.cols;x++)
	{
		for (y=0;y<OffsetMap.rows;y++)
		{
			//reserve drag area and put perfect match in offsetmap
			if (WithReserve && (x >= NewXOfROI - Offset && x <= (NewXOfROI + Offset + (ROIxDownCorner - ROIxUpCorner)))
				&& (y >= NewYOfROI - Offset && y <= (NewYOfROI + Offset + (ROIyDownCorner - ROIyUpCorner))))
			{
				OffsetMap.at<Vec3f>(y,x)[0] = (NROIxUpCorner - NewXOfROI);
				OffsetMap.at<Vec3f>(y,x)[1] = (NROIyUpCorner - NewYOfROI);
				OffsetMap.at<Vec3f>(y,x)[2] = 255;
			}
			else
			{
				//x Coordinate
				int randomX = (rand() % OffsetMap.cols)- x;// ;
				//y Coordinate
				int randomY = (rand() % OffsetMap.rows)- y;// ;
				OffsetMap.at<Vec3f>(y,x)[0] = randomX;
				OffsetMap.at<Vec3f>(y,x)[1] = randomY;
				OffsetMap.at<Vec3f>(y,x)[2] = 0;
			}
		}
	}
}

//Merge two OffsetMap and take those values which are more similar to the Target Image
void MergeOffsetMaps(Mat OffsetMap, Mat newOffsetMap, Mat TargetImage, Mat SourceImage)
{
	Mat TargetPatch, SourcePatch, NewSourcePatch;
	for (int x=0;x<OffsetMap.cols;x++)
	{
		for (int y=0;y<OffsetMap.rows;y++)
		{
			if (withConstraints && OffsetMap.at<Vec3f>(y,x)[2]==255)
			{
			}
			else
			{
				TargetPatch = FindPatch(x,y, TargetImage);
				SourcePatch = FindPatch(OffsetMap.at<Vec3f>(y,x)[0] + x,OffsetMap.at<Vec3f>(y,x)[1] + y, SourceImage);
				NewSourcePatch = FindPatch(newOffsetMap.at<Vec3f>(y,x)[0] +x,newOffsetMap.at<Vec3f>(y,x)[1] + y, SourceImage);
				//find minimum distance
				int patchnumber = calcDistance(TargetPatch, NewSourcePatch, SourcePatch);
				switch (patchnumber)
				{
				case 1:
 					OffsetMap.at<Vec3f>(y,x)[0] = newOffsetMap.at<Vec3f>(y,x)[0];
					OffsetMap.at<Vec3f>(y,x)[1] = newOffsetMap.at<Vec3f>(y,x)[1];
					break;
				}
			}
		}
	}
}

bool myKeyBoardWait(bool nested);

void StartPatchMatch(Mat SourceImageRGB, Mat TargetImageRGB)
{

	cout<<"patch match started ...ooollasaas"<<endl;
	SYSTEMTIME start,end;
	GetLocalTime(&start);
	int i,x,y,o;
	Mat SourceImage, TargetImage, NewOffsetMap;
	bool randomIter = true;
	bool scaleCorrect = true;
	withConstraints = true;
	sizeOfPatch = 5;
	//Convert to LAB to compare Distances
	SourceImageRGB.convertTo(SourceImage,CV_32FC3,1/255.f);
	cvtColor(SourceImage,SourceImage,CV_RGB2Lab);
	TargetImageRGB.convertTo(TargetImage,CV_32FC3,1/255.f);
	cvtColor(TargetImage,TargetImage,CV_RGB2Lab);

	Mat orgTargetImage = TargetImage.clone();
	int numberOfScales =4;
	int numberOfIterationPerScale = 5;
	int numberOfSession = 1; //to see if alpha value affect algorithm

	//CV_32FC3 because we need to store x and y coordinate and reserved area
	Mat OffsetMap(orgTargetImage.rows, orgTargetImage.cols, CV_32FC3);
	//random OffsetMap Initialization
	RandomValuesForOffset(OffsetMap, true);

	//scale Target Image Down
	Size orgSize = Size(TargetImage.size());
	Mat temp = TargetImage.clone();
	for (i=0;i<numberOfScales;i++)
	{
		pyrDown(temp, temp);
		NewOffsetMap = Mat(temp.rows,temp.cols, OffsetMap.type()); 
		for (x=0;x<NewOffsetMap.cols;x++)
		{
			int a = x*2;
			if (a>= OffsetMap.cols-1)
				a = OffsetMap.cols-2;
			for (y=0;y<NewOffsetMap.rows;y++)
			{
				int b = y*2;
				if (b >= OffsetMap.rows-1)
					b = OffsetMap.rows -2;
				NewOffsetMap.at<Vec3f>(y,x)[0] = OffsetMap.at<Vec3f>(b,a)[0]/2;
				NewOffsetMap.at<Vec3f>(y,x)[1] = OffsetMap.at<Vec3f>(b,a)[1]/2;
				if ((OffsetMap.at<Vec3f>(b, a)[2] == 255)||
					(OffsetMap.at<Vec3f>(b+1, a)[2] == 255)||
					(OffsetMap.at<Vec3f>(b, a+1)[2] == 255)||
					(OffsetMap.at<Vec3f>(b +1 , a +1)[2] == 255))
				{
					if (OffsetMap.at<Vec3f>(b+1, a)[2] == 255)
					{
						NewOffsetMap.at<Vec3f>(y,x)[0] = OffsetMap.at<Vec3f>(b+1,a)[0]/2;
						NewOffsetMap.at<Vec3f>(y,x)[1] = OffsetMap.at<Vec3f>(b+1,a)[1]/2;
					}
					else if (OffsetMap.at<Vec3f>(b, a+1)[2] == 255)
					{
						NewOffsetMap.at<Vec3f>(y,x)[0] = OffsetMap.at<Vec3f>(b,a+1)[0]/2;
						NewOffsetMap.at<Vec3f>(y,x)[1] = OffsetMap.at<Vec3f>(b,a+1)[1]/2;
					}
					else if (OffsetMap.at<Vec3f>(b +1 , a +1)[2] == 255)
					{
						NewOffsetMap.at<Vec3f>(y,x)[0] = OffsetMap.at<Vec3f>(b+1,a+1)[0]/2;
						NewOffsetMap.at<Vec3f>(y,x)[1] = OffsetMap.at<Vec3f>(b+1,a+1)[1]/2;
					}
					NewOffsetMap.at<Vec3f>(y, x)[2] = 255;
				}
				else
					NewOffsetMap.at<Vec3f>(y, x)[2] = 0;
			}
		}
		OffsetMap = NewOffsetMap.clone();
	}
	
	for (o=0;o<numberOfSession;o++)
	{
		OffsetMap = NewOffsetMap.clone();
		//scale Source Image Down
		Mat tempSource = SourceImage.clone();
		tempSource = Scale(numberOfScales, tempSource);

		//TargetImage = Scale(numberOfScales, TargetImage);
		Mat TempTarget;
		temp.convertTo(TempTarget, CV_32FC3, 1);
		double alpha;
		switch (o)
		{
		case 0:
			alpha=0.25;
			break;
		case 1:
			alpha = 0.5;
			break;
		case 2:
			alpha=0.95;
			break;
		}
		for(i=numberOfScales;i>0;i--)
		{
			//RandomSearch
			double w= i*50; //50
			if (i == numberOfScales)
			{
				for (int k= 0;k<numberOfIterationPerScale;k++)
				{
					//Propagation
					Propagation(OffsetMap, tempSource, TempTarget, k%2);

					//RandomSearch
					RandomSearch(OffsetMap, tempSource, TempTarget, w, alpha);
				}
			}
			else
			{
				Mat newOffsetMap(OffsetMap.rows, OffsetMap.cols, OffsetMap.type());
				//make sure that the constraints get written in newoffsetmap
				if (randomIter)
				{
					RandomValuesForOffset(newOffsetMap, false);
					std::vector<Mat> offs(3);
					split(OffsetMap,offs);
					std::vector<Mat> noffs(3);
					split(newOffsetMap,noffs);
					noffs[2] = offs[2];
					merge(noffs,newOffsetMap); 
				}
				else
					newOffsetMap = OffsetMap.clone();
				for (int k= 0;k<numberOfIterationPerScale;k++)
				{
					//Propagation
  					Propagation(newOffsetMap, tempSource, TempTarget, k%2);

					//RandomSearch
					RandomSearch(newOffsetMap, tempSource, TempTarget, w, alpha);
				}
				//merge Offsetmap
				if (randomIter)
					MergeOffsetMaps(OffsetMap, newOffsetMap, TempTarget, tempSource);
				else
					OffsetMap = newOffsetMap.clone();
			}
			/*stringstream stream;
			stream << "Offset" << i << ".xml";
			CvMat cvmat2 = OffsetMap;
			string file = stream.str();
			cvSave( file.c_str(), &cvmat2);*/
		
			//Scale OffsetMap up
			if (!scaleCorrect)
			{
				pyrUp(OffsetMap, OffsetMap);
				OffsetMap*=2;
			}
			else
			{
				Mat NewOffset = Mat(OffsetMap.rows*2,OffsetMap.cols*2,OffsetMap.type());
				for (x = 0; x<OffsetMap.cols; x++) 
				{
					for (y = 0; y<OffsetMap.rows; y++) 
					{
						NewOffset.at<Vec3f>(y*2,x*2)[0] = OffsetMap.at<Vec3f>(y,x)[0] * 2;
						NewOffset.at<Vec3f>(y*2,x*2)[1] = OffsetMap.at<Vec3f>(y,x)[1] * 2;
						NewOffset.at<Vec3f>(y*2,x*2)[2] = OffsetMap.at<Vec3f>(y,x)[2];
						NewOffset.at<Vec3f>(y*2+1,x*2)[0] = OffsetMap.at<Vec3f>(y,x)[0] * 2;
						NewOffset.at<Vec3f>(y*2+1,x*2)[1] = OffsetMap.at<Vec3f>(y,x)[1] * 2;
						NewOffset.at<Vec3f>(y*2+1,x*2)[2] = OffsetMap.at<Vec3f>(y,x)[2];
						NewOffset.at<Vec3f>(y*2,x*2+1)[0] = OffsetMap.at<Vec3f>(y,x)[0] * 2;
						NewOffset.at<Vec3f>(y*2,x*2+1)[1] = OffsetMap.at<Vec3f>(y,x)[1] * 2;
						NewOffset.at<Vec3f>(y*2,x*2+1)[2] = OffsetMap.at<Vec3f>(y,x)[2];
						NewOffset.at<Vec3f>(y*2+1,x*2+1)[0] = OffsetMap.at<Vec3f>(y,x)[0] * 2;
						NewOffset.at<Vec3f>(y*2+1,x*2+1)[1] = OffsetMap.at<Vec3f>(y,x)[1] * 2;
						NewOffset.at<Vec3f>(y*2+1,x*2+1)[2] = OffsetMap.at<Vec3f>(y,x)[2];
					}
				}
				OffsetMap = NewOffset.clone();
			}

			tempSource = SourceImage.clone();
			//scale Source Image Down
			tempSource = Scale(i-1, tempSource);

			TempTarget = Generate(OffsetMap, tempSource);
		
			//Display new TargetImage
			DisplayImage(TempTarget, orgSize ,o);

			//remove constraints to adapt pixels of the environment
			if (i == numberOfScales -1 && withConstraints){
				for (x = 0; x<OffsetMap.cols; x++) 
				{
					for (y = 0; y<OffsetMap.rows; y++) 
					{
						if (OffsetMap.at<Vec3f>(y,x)[2] == 255)
						{
							OffsetMap.at<Vec3f>(y,x)[2] = 0;
						}
					}
				}
			}
		
			int isBreak = myKeyBoardWait(true);
			if(isBreak)
			{
				printf("Finished PatchMatch Before end\n");
				break;
			}
		}
		if (numberOfSession-1 == o && i ==1);
			printf("Finished");
	}
	GetLocalTime(&end);
	
	printf("Zeit: %02d:%02d\n", start.wMinute, start.wSecond);
	printf("Zeit: %02d:%02d\n", end.wMinute, end.wSecond);
}

//funtion used to respond to keyboard input from the user
//esc - leave iteration of patchMatch or leave program
//r - restart program to select a new ROY area or use the same is mouse is not pressed
//space - start PatchMatch or go the the next iteration
//parameter nested: means if is inside one iteration of patchmatch
bool myKeyBoardWait(bool nested)
{

	//cout<<"in mykeeyboardwait"<<endl;
	bool isBreak = false;

	int key = waitKey();
	if (key==27)//esc
		isBreak = true;
	else if ((key == 'r')||(key=='R'))//restart
	{
		mousepress = 0;
		printf("define a new drag area\n");
	}
	else if ((key==32)&&(!nested))//space
		StartPatchMatch(Simg, newTimg);

	return isBreak;
}

void mouseHandlerSource(int event, int x, int y,int flags,void* param)
{
	/* //try to cut out a polygon
	Point minPoint(1000,1000);
	Point maxPoint(0,0);
	switch(event)
	{
	case CV_EVENT_LBUTTONDOWN:
		printf("left down");
		points[mousepress] = Point(x,y);
		mousepress = (mousepress+1)%6;
		IsLMouseDown = true;
		break;
	case CV_EVENT_LBUTTONUP:
			IsLMouseDown = false;
			break;
	case CV_EVENT_RBUTTONDOWN:
		
		for( int j = 0; j < 6; j++ )
		{ 
			minPoint.x = MIN(minPoint.x, points[j].x);
			minPoint.y = MIN(minPoint.y, points[j].y);
			maxPoint.x = MAX(minPoint.x, points[j].x);
			maxPoint.y = MAX(minPoint.y, points[j].y);
			line( Timg, points[j],  points[(j+1)%6], Scalar( 255 ), 1, 8 ); 
		}
		ROIxUpCorner = minPoint.x;
		ROIyUpCorner = minPoint.y;
		ROIxDownCorner = maxPoint.x;
		ROIyDownCorner = maxPoint.y;
		ImageRoi = Rect(ROIxUpCorner, ROIyUpCorner,ROIxDownCorner-ROIxUpCorner, ROIyDownCorner-ROIyUpCorner);
		imshow("Target image", Timg);
		IsRMouseDown = true;
		break;
	case CV_EVENT_MOUSEMOVE:
		 if (IsLMouseDown && IsRMouseDown)
		 {
			NewXOfROI = x;
			NewYOfROI = y;
            Rect r(NewXOfROI,NewYOfROI,ImageRoi.width,ImageRoi.height);
			newTimg = Timg.clone();
			Mat ROI(newTimg(r));
			Simg(ImageRoi).copyTo(ROI);
			imshow("Target image", newTimg);
		 }
		break;
	}*/

	int dx,dy,p1x,p1y,width,height,xpoint,ypoint;
	switch(event)
	{
	case CV_EVENT_LBUTTONDOWN:
		printf("left down");
		if (mousepress ==0)
		{
			ROIxUpCorner = x;
			ROIyUpCorner = y;
		}
		break;
	case CV_EVENT_LBUTTONUP:
		printf("left up");
		if (mousepress ==0)
		{
			ROIxDownCorner = x;
			ROIyDownCorner = y;
		}
		//printf("ROI: %i / %i, %i / %i", ROIxUpCorner, ROIyUpCorner, ROIxDownCorner, ROIyDownCorner);
		//rectangle(Timg, cvPoint(ROIxUpCorner, ROIyUpCorner), cvPoint(ROIxDownCorner, ROIyDownCorner), CV_RGB(255,0,0),1);
		//must be sure that the widht and the height are not negative values
		dx = ROIxDownCorner-ROIxUpCorner;
		dy = ROIyDownCorner-ROIyUpCorner;
		p1x = ROIxUpCorner;
		p1y = ROIyUpCorner;
		width = dx;
		height = dy;
		xpoint = p1x;
		ypoint = p1y;
		if( dx < 0)
		{
			width = -dx;
			xpoint = p1x - width;
		}
		if(dy < 0)
		{
			height = -dy;
			ypoint = p1y - height;
		}
		if((xpoint + width >= Timg.cols) || (ypoint + height >= Timg.rows))
			printf("wrong dragged image\n");
		else
			ImageRoi = Rect(xpoint, ypoint,width, height);
		imshow("Target image", Timg);
		mousepress++;
		break;
	}
}

void mouseHandlerTarget(int event, int x, int y,int flags,void* param)
{
	switch(event)
	{
		case CV_EVENT_LBUTTONDOWN:
			IsLMouseDown = true;
			break;
		case CV_EVENT_LBUTTONUP:
			IsLMouseDown = false;
			break;
		case CV_EVENT_MOUSEMOVE:
			if (mousepress != 0 && IsLMouseDown)
			{
				if (x >= 0 && x + ImageRoi.width<= Simg.cols-1 && y >= 0 && y + ImageRoi.height <= Simg.rows)
				{
					NewXOfROI = x;
					NewYOfROI = y;
					Rect r(NewXOfROI,NewYOfROI,ImageRoi.width,ImageRoi.height);
					newTimg = Timg.clone();
					Mat ROI(newTimg(r));
					Simg(ImageRoi).copyTo(ROI);
					imshow("Target image", newTimg);
				}
				else
					printf("draggging image out of bounds\n");
			}
			break;
	}
}

int main(int argc, _TCHAR* argv[])
{
	//string filenameSource = "E:\\Neuschwanstein.jpg";
	//string filenameTaget = "E:\\Neuschwanstein.jpg";
	//string filenameSource = "E:\\Pict0222.jpg";
	//string filenameTaget = "E:\\Pict0223.jpg";
	//string filenameSource = "E:\\lena.bmp";
	//string filenameTaget = "E:\\lena.bmp";
	//string filenameSource = "E:\\earth.jpg";
	//string filenameTaget = "E:\\earth.jpg";
	//string filenameSource = "E:\\Sonne2.png";
	//string filenameTaget = "E:\\Sonne1.png";
	char* filenameSource =(char*)malloc(sizeof(char)*100);
	char* filenameTaget  =(char*)malloc(sizeof(char)*100);



	gets(filenameSource);
	gets(filenameTaget);
	Mat src,tgt;
	//string filename = "D:\\nat.jpg";
	

	src = imread(filenameSource);
	Simg = src;
	cout<<"read source"<<endl;
	Timg = imread(filenameTaget);

	cout<<"read target"<<endl;
	newTimg = Timg.clone();
	cout<<"go goa gone...."<<endl;


	
	//namedWindow("Source image");
	if(!src.empty())
		imshow("Source image", src);
	else
	{	
		cout<<"empty image the fuck"<<endl;
	}
	int mouseParam = 5;

	cout<<"go goa gone.. "<<endl;
	cvNamedWindow("Target image", CV_WINDOW_AUTOSIZE);
	imshow("Target image", Timg);
	
	cout<<"upto here"<<endl;

	cvSetMouseCallback("Source image", mouseHandlerSource, &mouseParam);
	cvSetMouseCallback("Target image", mouseHandlerTarget, &mouseParam);

	

	cout<<"upto here"<<endl;

	
	while(1)
	{
		int isBreak = myKeyBoardWait(false);
		if(isBreak)
			break;
	}
	return 0;
}

