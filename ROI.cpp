/*
   테스트 14
   좌표를 이용해서 잘라진 영상을 출력하게 한다.
*/



#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cxcore.h>
#include <stdio.h>
#include <time.h>

int main()
{
	int x = 0, y = 0;
	char Cr = 0, Cb = 0;
	int width = 640, height = 320;
//	int lefttop_x=0, lefttop_y=0, rightbottom_x=0,rightbottom_y=0; // 왼쪽 상단 좌표와 오른쪽 하단 좌표 선언
//	int cnt = 0; // 첫 좌표를 저장하기 위한 카운트변수
//	int start_x = 0, start_y = 0; // 잘라내려는 영역의 출발점
//	int width_ROI = 0, height_ROI = 0; // 잘라내려는 영역의 높이와 너비

	IplImage* frame = 0;

	CvCapture* capture = cvCaptureFromCAM(-1);
	cvGrabFrame(capture);
	frame = cvRetrieveFrame(capture); 

//	IplImage* bkgImage = cvCreateImage(cvGetSize(frame),8,1) ; // 배경을 저장한다.

//	cvCvtColor(frame,bkgImage,CV_RGB2GRAY); // 처음배경을 그레이로 변환하


//	cvNamedWindow("Original",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Result",0);
//	cvNamedWindow("ROI",CV_WINDOW_AUTOSIZE);

	while(1)
	{
	//	clock_t start;
	//	start=clock();
		cvGrabFrame(capture);
		frame = cvRetrieveFrame(capture);
//		cvShowImage("Original",frame);
		
		IplImage* YCrCb = cvCreateImage(cvGetSize(frame),8,3); // ycrcb 변환
		IplImage* Skin = cvCreateImage(cvGetSize(frame),8,1); // 피부 이진화
	//	cvSmooth(frame,frame,CV_GAUSSIAN,3); // 필터를 통해 노이즈를 없앤다
		
	//	IplImage* Gray = cvCreateImage(cvGetSize(frame),8,1); // 카메라 이진화
	//	IplImage* ROI = cvCreateImage(cvGetSize(frame),8,1); // 잘라진 영상
	//	IplImage* Diff = cvCreateImage(cvGetSize(frame),8,1); // 카메라와 배경의 차이값을 가지고 있음

		height = frame->height;
		width = frame->width;
		
	//	cvCvtColor(frame,Gray,CV_RGB2GRAY); // 카메라를 그래이로 바꿈

//		cvAbsDiff(Gray,bkgImage,Diff);
		// 그레이랑 백그라운드랑 차이값을 다시 백그라운드에 저장 
		
		cvCvtColor(frame,YCrCb,CV_RGB2YCrCb);
		for(y=0; y<height;y++)
		{
			for(x=0; x<width; x++)
			{
				

			//	if((Diff->imageData[y*Diff->widthStep+x])>20) // 차이값이 5이상놈의 픽셀만 검사
			//	{
					Cr = (char)YCrCb->imageData[y*YCrCb->widthStep+3*x+1];
					Cb = (char)YCrCb->imageData[y*YCrCb->widthStep+3*x+2];

					if( (77<Cr && Cr<127) && (133<Cb && Cb<173) )
					{
					/*	if(cnt == 0) // 초기에 한번만 살색인 영역의 값을 받아온다.
						{
							lefttop_x = x;
							lefttop_y = y;
							rightbottom_x = x;
							rightbottom_y = y;
							cnt++;
						}*/
						Skin->imageData[y*Skin->widthStep+x] = 0;
					/*	if(lefttop_x <= x)
							lefttop_x = x; // 가장 작은 x좌표
						if(lefttop_y >= y)
							lefttop_y = y; // 가장 큰 y좌표
						if(rightbottom_x >= x)
							rightbottom_x = x; // 가장 큰 x좌표
						if(rightbottom_y <= y)
							rightbottom_y = y; // 가장 작은 y좌표*/
					}
					else
					{
						Skin->imageData[y*Skin->widthStep+x] = 255;
					}
			//	}
				
			//	else
			//		Skin->imageData[y*Skin->widthStep+x] = 255;
			}
		
		}
	//	cvCopy(Skin,ROI); // skin영상을 ROI에 복사한다. 

	//	start_x = lefttop_x ; // 시작점의 x값
	//	start_y = lefttop_y; // 시작점의 y값
	//	width_ROI = lefttop_x - rightbottom_x; // 잘라진 영역의 너비
	//	height_ROI = rightbottom_y - lefttop_y; // 잘라진 영역의 높이
		
//		printf("시작 x=%d y=%d \n",lefttop_x,lefttop_y); // 왼쪽 상단 좌표 출력  
//		printf("너비 높이 wid=%d hei=%d \n",width_ROI,height_ROI); // 너비랑 높이 출력

//		cvSetImageROI(ROI,cvRect(start_x,start_y,width_ROI,height_ROI)); // 영역 잘라냄

		//printf("왼쪽상단 x=%d y=%d \n",lefttop_x,lefttop_y); // 왼쪽 상단 좌표 출력  
		//printf("오른쪽하단 x=%d y=%d \n",rightbottom_x,rightbottom_y); // 오른쪽 상단 좌표 출력
	//	cnt = 0; // 카운터값 초기화

//		cvCvtColor(frame,bkgImage,CV_RGB2GRAY); // 배경을 그레이로 변환하

	//	printf("%f sec\n",(double)(clock()-start)/CLOCKS_PER_SEC);
		cvShowImage("Result",Skin);
//		cvShowImage("ROI",ROI);
//		cvResetImageROI(ROI); // 이미지 리셋 
		
		if(cvWaitKey(33)==27)
			break;
	}

//	cvDestroyWindow("Original");
	cvDestroyWindow("Result");
	cvReleaseImage(&frame);
	cvReleaseCapture(&capture);
//	cvDestroyWindow("ROI");

	return 0;
}
