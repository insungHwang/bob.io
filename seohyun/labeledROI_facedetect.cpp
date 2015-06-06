/*
   테스트 19
   잘라낸 영상이 RGB가 되게 -> 성공
   이제부터 git을 사용할것입니다.
   얼굴인식 합치기!
*/


#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cxcore.h>
#include <stdio.h>
#include <time.h>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <cctype>
#include <iostream>
#include <iterator>

using namespace std;
using namespace cv;

void detectAndDraw( Mat& img, CascadeClassifier& cascade, CascadeClassifier& nestedCascade, double scale);
int Labeling(IplImage* Skin);
int doLabeling(int x,int y,IplImage* Skin,int** index_map, int INDEX);

CvRect rect[100]={9999}; 


string cascadeName = "../opencv-2.4.11/data/haarcascades/haarcascade_frontalface_alt.xml";
string nestedCascadeName = "../opencv-2.4.11/data/haarcascades/haarcascade_eye_tree_eyeglasses.xml";
int face_count = 0;
int main()
{
	int x = 0, y = 0;
	char Cr = 0, Cb = 0;
	int INDEX=0, iter;
	int width = 640, height = 320;
	int lefttop_x=0, lefttop_y=0, rightbottom_x=0,rightbottom_y=0; // 왼쪽 상단 좌표와 오른쪽 하단 좌표 선언
	int cnt = 0; // 첫 좌표를 저장하기 위한 카운트변수
	int start_x = 0, start_y = 0; // 잘라내려는 영역의 출발점
	int width_ROI = 0, height_ROI = 0; // 잘라내려는 영역의 높이와 너비
	Mat Cut, ROICopy;

	IplImage* frame = 0;

	CvCapture* capture = cvCaptureFromCAM(-1);
	cvGrabFrame(capture);
	frame = cvRetrieveFrame(capture); 

	IplImage* bkgImage = cvCreateImage(cvGetSize(frame),8,1) ; // 배경을 저장한다.

	cvCvtColor(frame,bkgImage,CV_RGB2GRAY); // 처음배경을 그레이로 변환하
	CascadeClassifier cascade, nestedCascade;
	double scale =1;

//	cvNamedWindow("Original",CV_WINDOW_AUTOSIZE);
	
	if(!cascade.load(cascadeName))
		cerr <<"ERROR: could not load classifier cascade" << endl;
	

	cvNamedWindow("Origin",0);
	cvNamedWindow("ROI",CV_WINDOW_AUTOSIZE);

	IplImage* Gray = cvCreateImage(cvGetSize(frame),8,1); // 카메라 이진화
	IplImage* YCrCb = cvCreateImage(cvGetSize(frame),8,3); // ycrcb 변환
	IplImage* Skin = cvCreateImage(cvGetSize(frame),8,1); // 피부 이진화
	IplImage* ROI = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3); // 잘라진 영상
	IplImage* Diff = cvCreateImage(cvGetSize(frame),8,1); // 카메라와 배경의 차이값을 가지고 있음
	
	while(1)
	{
		clock_t start;
		start=clock();
		cvGrabFrame(capture);
		frame = cvRetrieveFrame(capture);
		cvShowImage("Origin",frame);
		
		cvSmooth(frame,frame,CV_GAUSSIAN,3); // 필터를 통해 노이즈를 없앤다
		

		height = frame->height;
		width = frame->width;
		
		cvCvtColor(frame,Gray,CV_RGB2GRAY); // 카메라를 그래이로 바꿈

		cvAbsDiff(Gray,bkgImage,Diff);
		// 그레이랑 백그라운드랑 차이값을 다시 백그라운드에 저장 
		
		cvCvtColor(frame,YCrCb,CV_RGB2YCrCb);
		for(y=0; y<height;y++)
		{
			for(x=0; x<width; x++)
			{
				

				if((Diff->imageData[y*Diff->widthStep+x])>20) // 차이값이 5이상놈의 픽셀만 검사
				{
					Cb = (char)YCrCb->imageData[y*YCrCb->widthStep+3*x+2];
					Cr = (char)YCrCb->imageData[y*YCrCb->widthStep+3*x+1];
					
					if( (77<Cr && Cr<127) && (133<Cb && Cb<173) )
					{
						if(cnt == 0) // 초기에 한번만 살색인 영역의 값을 받아온다.
						{
							lefttop_x = x;
							lefttop_y = y;
							rightbottom_x = x;
							rightbottom_y = y;
							cnt++;
						}

						Skin->imageData[y*Skin->widthStep+x] = 0;
						if(lefttop_x >= x)
							lefttop_x = x; // 가장 큰 x좌표
						if(lefttop_y >= y)
							lefttop_y = y; // 가장 큰  y좌표
						if(rightbottom_x <= x)
							rightbottom_x = x; // 가장 작은 x좌표
						if(rightbottom_y <= y)
							rightbottom_y = y; // 가장 작은 y좌표
					}
					else
					{
						Skin->imageData[y*Skin->widthStep+x] = 255;
					}
				}
				
				else
					Skin->imageData[y*Skin->widthStep+x] = 255;
			}
		
		}
		INDEX = Labeling(Skin);	
	
		cvCopy(frame,ROI); // skin영상을 ROI에 복사한다. 
	
		for(iter = 1; iter<=INDEX; iter++)
		{
			start_x = rect[iter].x; // 시작점의 x값
			start_y = rect[iter].y; // 시작점의 y값
			width_ROI = rect[iter].width; // 잘라진 영역의 너비
			height_ROI = rect[iter].height; // 잘라진 영역의 높이
			if(width_ROI <= 10 || height_ROI <= 10)
			{
				width_ROI = 10;
				height_ROI = 10;
			}	
			else
			{
				printf("시작 x=%d y=%d \n",start_x,start_y); // 왼쪽 상단 좌표 출력  
				printf("너비 높이 wid=%d hei=%d \n",width_ROI,height_ROI); // 너비랑 높이 출력

				cvSetImageROI(ROI,cvRect(start_x,start_y,width_ROI,height_ROI)); // 영역 잘라냄

				cvCvtColor(frame,bkgImage,CV_RGB2GRAY); // 배경을 그레이로 변환하
				Cut = ROI;
				Cut.copyTo(ROICopy);

				detectAndDraw(ROICopy, cascade, nestedCascade, scale );
			}
			cnt = 0; //counter init
		}
		
		cvResetImageROI(ROI); // 이미지 리셋 
		
		if(cvWaitKey(33)==27)
			break;
		printf("%f sec\n",(double)(clock()-start)/CLOCKS_PER_SEC);
	}
	cvReleaseImage(&frame);
	cvReleaseCapture(&capture);
	cvDestroyWindow("ROI");

	return 0;
}

void detectAndDraw( Mat& img, CascadeClassifier& cascade,CascadeClassifier& nestedCascade,double scale )
{
	int i = 0;
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
	printf("number of counted faces so far : %d \n",face_count);
	cv::imshow( "result", img );
}
int Labeling(IplImage* Skin)
{
	int x=0,y=0;
	int w =(int) Skin->width;
	int h =(int) Skin->height;

	int INDEX=1;

	int i =0;
	int** index_map;


	//map init
	index_map = (int**)calloc(w,sizeof(int*));
	for(x=0; x<w; x++)
		index_map[x] = (int*)calloc(h,sizeof(int));

	printf("Labeling ::\n	getting ROI\n");

	for(x = 0; x<w; x++)
	{
		for(y = 0; y<h; y++)
		{
			if(doLabeling(x,y,Skin,index_map,INDEX))
			{
				INDEX++;
			}
		}
	}
	INDEX--;
	
	//rect init
	for(x=0; x<INDEX; x++)
	{
		rect[x].x = 999999;
		rect[x].y = 999999;
		rect[x].width =0;
		rect[x].height=0;
	}
	for(x = 0; x<w; x++)
	{
		for(y = 0; y<h; y++)
		{
			i = index_map[x][y]-1;

			if( i>-1)
			{
				if(rect[i].x>x)
					rect[i].x = x;
				else if(rect[i].x + rect[i].width< x)
					rect[i].width = x - rect[i].x;
				
				if(rect[i].y>y)
					rect[i].y = y;
				else if(rect[i].y + rect[i].height < y)
					rect[i].height = y - rect[i].y;
			}
		}
	}

	//memory free
	for(x=0;x<w;x++)
		free(index_map[x]);
	free(index_map);

	printf("INDEX = %d\n",INDEX);
	
	for(x=0;x<INDEX; x++)
		printf("x = %d, y = %d, width = %d, height = %d\n",rect[x].x, rect[x].y, rect[x].width, rect[x].height);

	return INDEX;
}

int doLabeling(int x,int y,IplImage* Skin, int** index_map, int INDEX)
{
	int i,j;

	if(index_map[x][y] == 0 )
	{
		if(((unsigned char*)(Skin->imageData + Skin->widthStep*(y)))[(x)]==0 )
		{
			index_map[x][y]=INDEX;
			for( i = x-5; i<x+5; i++)
				for(j = y-5; j<y+5; j++)
					if(i>0 && i<Skin->width && j>0&& j<Skin->height)
						doLabeling(i,j,Skin,index_map,INDEX);
			return 1;
		}
		else
		{
			index_map[x][y] =-1;
			return 0;
		}
	}
	else
		return 0;
}
