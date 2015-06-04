#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <time.h>

int main()
{
	int x = 0, y = 0;
	char Cr = 0, Cb = 0;
	int width = 640, height = 320;

	IplImage* frame = 0;

	CvCapture* capture = cvCaptureFromCAM(-1);
	cvGrabFrame(capture);
	frame = cvRetrieveFrame(capture); 

	IplImage* bkgImage = cvCreateImage(cvGetSize(frame),8,1) ; // 배경을 저장한다.
	cvSmooth(frame,frame,CV_GAUSSIAN,3);
	cvCvtColor(frame,bkgImage,CV_RGB2GRAY); // 처음배경을 그레이로 변환하
	
	cvNamedWindow("Original",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Result",0);

	while(1)
	{
		clock_t start;
		start=clock();
		cvGrabFrame(capture);
		frame = cvRetrieveFrame(capture);
//		cvShowImage("Original",frame);
	cvSmooth(frame,frame,CV_GAUSSIAN,3);
//	cvSmooth(frame,frame, CV_MEDIAN, 3);
		
		IplImage* Gray = cvCreateImage(cvGetSize(frame),8,1); // 카메라 이진화
		IplImage* YCrCb = cvCreateImage(cvGetSize(frame),8,3); // ycrcb 변환
		IplImage* Skin = cvCreateImage(cvGetSize(frame),8,1); // 피부 이진화
		IplImage* Diff = cvCreateImage(cvGetSize(frame),8,1); // 카메라와 배경의 차이값을 가지고 있음

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
				if((Diff->imageData[y*Diff->widthStep+x])>1) // 차이값이 1이상놈의 픽셀만 검사
				{
					Cr = (char)YCrCb->imageData[y*YCrCb->widthStep+3*x+1];
					Cb = (char)YCrCb->imageData[y*YCrCb->widthStep+3*x+2];
	
					if( (77<Cr&&Cr<127) && (133<Cb&&Cb<173) )
						Skin->imageData[y*Skin->widthStep+x] = 0;
					else
	
						Skin->imageData[y*Skin->widthStep+x] = 255;
				}
				
				else
					Skin->imageData[y*Skin->widthStep+x] = 255;
			}
		}

		cvCvtColor(frame,bkgImage,CV_RGB2GRAY); // 배경을 그레이로 변환하
		cvSmooth(Skin,Skin,CV_GAUSSIAN,1);
	//	cvSmooth(Skin,Skin,CV_MEDIAN,1);
		printf("%f sec\n",(double)(clock()-start)/CLOCKS_PER_SEC);
		
		cvErode(Skin, Skin, NULL, 4);  //침식
    	cvDilate(Skin, Skin, NULL,1 ); //팽창
	    //cvSmooth(Skin,Skin,CV_GAUSSIAN,1);
		cvShowImage("Result",Skin);
		if(cvWaitKey(33)==27)
			break;
	}
	cvReleaseImage(&frame);
	cvReleaseCapture(&capture);
	cvDestroyWindow("Original");
	cvDestroyWindow("Result");

	return 0;
}
