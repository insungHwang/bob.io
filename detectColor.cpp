/*
   테스트 14
   좌표를 이용해서 잘라진 영상을 출력하게 한다.
*/



#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cxcore.h>

int main()
{
	int x = 0, y = 0;
	char Cr = 0, Cb = 0;
	int width = 640, height = 320;
	
	IplImage* frame = 0;

	CvCapture* capture = cvCaptureFromCAM(-1);

	cvNamedWindow("Result",0);

	while(1)
	{
		cvGrabFrame(capture);
		frame = cvRetrieveFrame(capture);
		
		IplImage* YCrCb = cvCreateImage(cvGetSize(frame),8,3); // ycrcb 변환
		IplImage* Skin = cvCreateImage(cvGetSize(frame),8,1); // 피부 이진화

		height = frame->height;
		width = frame->width;
		
		cvCvtColor(frame,YCrCb,CV_RGB2YCrCb);
		for(y=0; y<height;y++)
		{
			for(x=0; x<width; x++)
			{
					Cr = (char)YCrCb->imageData[y*YCrCb->widthStep+3*x+1];
					Cb = (char)YCrCb->imageData[y*YCrCb->widthStep+3*x+2];

					if( (77<Cr && Cr<127) && (133<Cb && Cb<173) )
					{
						Skin->imageData[y*Skin->widthStep+x] = 0;
					}
					else
					{
						Skin->imageData[y*Skin->widthStep+x] = 255;
					}
			}
		
		}

		cvShowImage("Result",Skin);
		cvSetZero(frame);
		cvSetZero(YCrCb);
		cvSetZero(Skin);

		if(cvWaitKey(33)==27)
			break;
	}

	cvDestroyWindow("Result");
	cvReleaseCapture(&capture);

	return 0;
}
