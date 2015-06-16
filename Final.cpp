/*
   최종 코드
   함수는 Blackbox.h에
*/


#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cxcore.h>
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
#include <wiringPi.h>
#include <wiringSerial.h>

using namespace std;
using namespace cv;

string cascadeName = "../opencv-2.4.11/data/haarcascades/haarcascade_frontalface_alt.xml";
string nestedCascadeName = "../opencv-2.4.11/data/haarcascades/haarcascade_eye_tree_eyeglasses.xml";
int main()
{
	int count=0;
	int fd;
	char data;
	Mat Cut, ROICopy;

	if((fd = serialOpen("/dev/ttyAMA0",9600))<0)
	{
		fprintf(stderr,"Unable to open serial device: %s\n",strerror(errno));
		return 1;
	}
	if(wiringPiSetup() == -1 )
	{
		fprintf(stderr,"Unable to start wiringPu: %s\n",strerror(errno));
		return 1;
	}
	IplImage* frame = 0;

	CvCapture* capture = cvCaptureFromCAM(-1);
	cvGrabFrame(capture);
	frame = cvRetrieveFrame(capture); 

	IplImage* bkgImage = cvCreateImage(cvGetSize(frame),8,1) ; // 배경을 저장한다.

	cvCvtColor(frame,bkgImage,CV_RGB2GRAY); // 처음배경을 그레이로 변환하
	CascadeClassifier cascade, nestedCascade;
	double scale =1;

	
	if(!cascade.load(cascadeName))
		cerr <<"ERROR: could not load classifier cascade" << endl;
	

	cvNamedWindow("Origin",0);
	cvNamedWindow("ROI",CV_WINDOW_AUTOSIZE);

	IplImage* ROI = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3); // 잘라진 영상
	
	while(1)
	{
		clock_t start;
		data =1;
		start=clock();
		cvGrabFrame(capture);
		frame = cvRetrieveFrame(capture);
		cvShowImage("Origin",frame);
		
		cvSmooth(frame,frame,CV_GAUSSIAN,3); // 필터를 통해 노이즈를 없앤다

		data=getROI(frame,bkgImage,ROI);
		cvShowImage("ROI",ROI);
		Cut = ROI;
		Cut.copyTo(ROICopy);

		count=detectAndDraw(ROICopy, cascade, nestedCascade, scale );
		printf("number of counted faces so far : %d \n",count);
		if(count)
			data=2;
		printf("number of counted faces so  : %d \n",data);
		serialPutchar(fd,data);
		printf("data=%d\n",data);
		fflush(stdout);
		cvResetImageROI(ROI); // 이미지 리셋 
		
		if(cvWaitKey(33)==27)
			break;
		printf("%f sec\n",(double)(clock()-start)/CLOCKS_PER_SEC);
	}
	cvReleaseImage(&frame);
	cvReleaseCapture(&capture);
//	cvDestroyWindow("Original");
//	cvDestroyWindow("Result");
	cvDestroyWindow("ROI");

	return 0;
}
