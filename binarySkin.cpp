#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#define CR_COLOR_FROM 77 
#define CR_COLOR_TO 127
#define CB_COLOR_FROM 133
#define CB_COLOR_TO 173

int main()
{
	unsigned char Cr_dt=0 , Cb_dt=0;
	int w=0, h=0;

	IplImage* image = 0;
	image = cvLoadImage("./test1.jpeg",-1);

//	printf("here 1 \n");
	cvNamedWindow("image",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("YCrCb",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Skin",CV_WINDOW_AUTOSIZE);
	//cvNamedWindow("Copy",CV_WINDOW_AUTOSIZE);

//	printf("here 2 \n");
	IplImage* YCrCb = cvCreateImage(cvGetSize(image),8,3);
	IplImage* Y = cvCreateImage(cvGetSize(image),8,1);
	IplImage* Cr = cvCreateImage(cvGetSize(image),8,1);
	IplImage* Cb = cvCreateImage(cvGetSize(image),8,1);
	IplImage* Skin = cvCreateImage(cvGetSize(image),8,1);
	//IplImage* Copy = cvCreateImage(cvGetSize(image),8,1);

//	printf("here 3 \n")
//	cvSmooth(image,image,CV_GAUSSIAN,3,3);
	cvCvtColor(image,YCrCb,CV_RGB2YCrCb);
	cvSplit(YCrCb,Y,Cr,Cb,NULL);


	w=Cr->width;
	h=Cr->height;

//	printf("here 4 \n");
	for(int i=0;i<h;i++)
		for(int j=0;j<w;j++)
		{
			Cr_dt = Cr->imageData[i*Cr->widthStep+j];
			Cb_dt = Cb->imageData[i*Cb->widthStep+j];

			if((Cr_dt>CR_COLOR_FROM && Cr_dt<CR_COLOR_TO))
			{
				if(Cb_dt>=CB_COLOR_FROM && Cb_dt<=CB_COLOR_TO)
				{
				//	YCrCb->imageData[i*YCrCb->widthStep+(j*3+2)]=(unsigned char)255;
				//	YCrCb->imageData[i*YCrCb->widthStep+(j*3+1)]=(unsigned char)255;
				//	YCrCb->imageData[i*YCrCb->widthStep+(j*3+0)]=(unsigned char)255;
				//	printf("4444 \n");
					Skin->imageData[i*Skin->widthStep+(j)]=(unsigned char)255;
				}
				else
				{
			//		YCrCb->imageData[i*YCrCb->widthStep+(j*3+2)]=(unsigned char)0;
			//		YCrCb->imageData[i*YCrCb->widthStep+(j*3+1)]=(unsigned char)0;
			//		YCrCb->imageData[i*YCrCb->widthStep+(j*3+0)]=(unsigned char)0;
					Skin->imageData[i*Skin->widthStep+(j)]=(unsigned char)0;
				}
			}
			else
			{
				Skin->imageData[i*Skin->widthStep+(j)]=(unsigned char)0;
			//	YCrCb->imageData[i*YCrCb->widthStep+(j*3+2)]=(unsigned char)0;
			//	YCrCb->imageData[i*YCrCb->widthStep+(j*3+1)]=(unsigned char)0;
			//	YCrCb->imageData[i*YCrCb->widthStep+(j*3+0)]=(unsigned char)0;
			}
		}	


//	printf("here 5 \n");
	cvShowImage("Image",image);
	cvShowImage("YCrCb",YCrCb);
	cvShowImage("Skin",Skin);

//	printf("here 6 \n");
	cvWaitKey(0);
	cvDestroyAllWindows();

//	printf("here 7 \n");
	cvReleaseImage(&image);
	cvReleaseImage(&YCrCb);
	cvReleaseImage(&Skin);
//	cvReleaseImage(&Copy);

	return 0;
}	
