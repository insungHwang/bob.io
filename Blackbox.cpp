/*
   함수 :
   얼굴 찾기
   영역나누기
*/


#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <time.h>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "Blackbox.h"
#include <cctype>
#include <iostream>
#include <iterator>

#include <string.h>
#include <errno.h>

using namespace std;
using namespace cv;

int detectAndDraw( Mat& img, CascadeClassifier& cascade,CascadeClassifier& nestedCascade,double scale )
{
	int i = 0;
	int face_count = 0;
	vector<Rect> faces, faces2;
	char Cr_inCircle, Cb_inCircle;
	IplImage* YCrCb_inCircle = new IplImage(img);
//	IplImage* YCrCb_inCircle = cvCreateImage(Size(10,10),8,3);
	const static Scalar colors[] =  { CV_RGB(0,0,255),
	CV_RGB(0,128,255),
	CV_RGB(0,255,255),
	CV_RGB(0,255,0),
	CV_RGB(255,128,0),
	CV_RGB(255,255,0),
	CV_RGB(255,0,0),
	CV_RGB(255,0,255)} ;
	Mat gray, smallImg( cvRound (img.rows/scale), cvRound(img.cols/scale), CV_8UC1 );
	Mat temp(cvRound(img.rows/scale),cvRound(img.cols/scale),IPL_DEPTH_16U);
	IplImage* small_YCrCb = new IplImage(temp);
	cvtColor( img, gray, CV_BGR2GRAY );
//	YCrCb_inCircle = img;		//Data change (Mat->IplImage)
	cvCvtColor( YCrCb_inCircle, YCrCb_inCircle, CV_RGB2YCrCb ); //Cvt Color
	resize( gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR );
	equalizeHist( smallImg, smallImg );
	cvResize( YCrCb_inCircle, small_YCrCb, INTER_LINEAR ); //resize
	
	cascade.detectMultiScale( smallImg, faces,1.1, 2, 0
				//|CV_HAAR_FIND_BIGGEST_OBJECT
				//|CV_HAAR_DO_ROUGH_SEARCH
				|CV_HAAR_SCALE_IMAGE,
				Size(30, 30) );
	for( vector<Rect>::const_iterator r = faces.begin(); r != faces.end(); r++, i++ )
	{
		Mat smallImgROI;
		vector<Rect> nestedObjects;
		Point center;
		Scalar color = colors[i%8];
		int radius; 
		
		double aspect_ratio = (double)r->width/r->height;
		if( 0.75 < aspect_ratio && aspect_ratio < 1.3 )
		{
			center.x = cvRound((r->x + r->width*0.5)*scale);
			center.y = cvRound((r->y + r->height*0.5)*scale);
			radius = cvRound((r->width + r->height)*0.25*scale);
			circle( img, center, radius, color, 3,8, 0 );
			face_count++;	//face +1
		}
		else
		rectangle( img, cvPoint(cvRound(r->x*scale), cvRound(r->y*scale)),cvPoint(cvRound((r->x + r->width-1)*scale), cvRound((r->y + r->height-1)*scale)),color, 3, 8, 0);
		
		Cb_inCircle = (char)small_YCrCb->imageData[(center.y)*small_YCrCb->widthStep+3*(center.x)+2];
		Cr_inCircle = (char)small_YCrCb->imageData[(center.y)*small_YCrCb->widthStep+3*(center.x)+1];	//Color of circle center

		if(!( (77<Cr_inCircle && Cr_inCircle<127) && (133<Cb_inCircle && Cb_inCircle<173) ))	//Is not skin
			face_count--;	//face -1 (is not face)
		
		if( nestedCascade.empty() )
			continue;
		smallImgROI = smallImg(*r);
		nestedCascade.detectMultiScale( smallImgROI, nestedObjects,
				1.1, 2, 0
				//|CV_HAAR_FIND_BIGGEST_OBJECT
				//|CV_HAAR_DO_ROUGH_SEARCH
				//|CV_HAAR_DO_CANNY_PRUNING
				|CV_HAAR_SCALE_IMAGE,
				Size(30, 30) );
		for( vector<Rect>::const_iterator nr = nestedObjects.begin(); nr != nestedObjects.end(); nr++)
		{
			center.x = cvRound((r->x + nr->x + nr->width*0.5)*scale);
			center.y = cvRound((r->y + nr->y + nr->height*0.5)*scale);
			radius = cvRound((nr->width + nr->height)*0.25*scale);
			circle( img, center, radius, color, 3, 8, 0 );
		}
	}
	cv::imshow( "result", img );
	return face_count;
}

char getROI(IplImage* img, IplImage* background, IplImage* dst)
{
	int x = 0, y = 0;
	int height = 0, width = 0;
	int init = 1;
	int x1=0,x2=0,y1=0,y2=0;
	int height_ROI, width_ROI;
	char data=1;
	char Cr,Cb;
	IplImage* YCrCb = cvCreateImage(cvGetSize(img),8,3);
	IplImage* Gray = cvCreateImage(cvGetSize(img),8,1);
	IplImage* Skin = cvCreateImage(cvGetSize(img),8,1);
	IplImage* Diff = cvCreateImage(cvGetSize(img),8,1);

	height = img->height;
	width = img->width;

	cvCvtColor(img,Gray,CV_RGB2GRAY);
	cvCvtColor(img,YCrCb,CV_RGB2YCrCb);

	cvAbsDiff(Gray,background,Diff);

	for(y=0; y<height; y++)
	{
		for(x=0; x<width; x++)
		{
			if( (Diff->imageData[y*Diff->widthStep+x])>20 ) //차이값이 20이상 검출
			{
				Cr = (char)YCrCb->imageData[y*YCrCb->widthStep+3*x+1];
				Cb = (char)YCrCb->imageData[y*YCrCb->widthStep+3*x+2];

				if( (77<Cr && Cr<127) && (133<Cb && Cb<173) )
				{
					Skin->imageData[y*Skin->widthStep+x] = 0;
					if(init)
					{
						x1 = x;
						y1 = y;
						x2 = x;
						y2 = y;
						init = 0;
					}
					else
					{
						if(x1 >= x)
							x1 = x;
						if(y1 >= y)
							y1 = y;
						if(x2 <= x)
							x2 = x;
						if(y2 <= y)
							y2 = y;
					}

				}
				else
					Skin->imageData[y*Skin->widthStep+x] = 255;
			}
			else
				Skin->imageData[y*Skin->widthStep+x] = 255;
		}
	}

	cvCopy(img, dst); //원본을 모두 복사

	width_ROI = x2 - x1;
	height_ROI = y2 - y1;
	if(width_ROI <= 10 || height_ROI <=10)
	{
		data=0;
		width_ROI = 10;
		height_ROI = 10;
	}
	printf("x=%d y=%d w=%d h=%d \n",x1,y1,width_ROI,height_ROI);
	cvSetImageROI( dst,cvRect(x1,y1,width_ROI,height_ROI) );

	cvReleaseImage(&YCrCb);
	cvReleaseImage(&Gray);

	return data;
}	

