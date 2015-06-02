#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>

int main()
{
	int x=0 , y=0;
	int Cr=0, Cb=0, w=0, h=0;

	IplImage* image = 0;
	image = cvLoadImage("./test1.jpeg",-1);

	cvNamedWindow("image",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("YCrCb",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Skin",CV_WINDOW_AUTOSIZE);

	IplImage* YCrCb = cvCreateImage(cvGetSize(image),8,3);
	IplImage* Skin = cvCreateImage(cvGetSize(image),8,1);

	cvCvtColor(image,YCrCb,CV_RGB2YCrCb);
	
	w=YCrCb->width;
	h=YCrCb->height;

	for(y=0;y<h;y++)
		for(x=0;x<w;x++)
		{
			Cr=(int)((unsigned char*)(YCrCb->imageData+YCrCb->widthStep*(y)))[(x)*3+1];
			Cb=(int)((unsigned char*)(YCrCb->imageData+YCrCb->widthStep*(y)))[(x)*3+2];
			
			//printf("Cr [%d][%d] = %d Cb [%d][%d] = %d \n",x,y,Cr,x,y,Cb);

			if((Cr>133&&Cr<173)&&(Cb>77&&Cb<127))
			{
				((unsigned char*)(Skin->imageData + Skin->widthStep*(y)))[(x)]=255;
			//	printf("여기가 피부 [%d][%d] \n",x,y);
			}	
			else
				((unsigned char*)(Skin->imageData + Skin->widthStep*(y)))[(x)]=0;	
				
		}

	cvShowImage("Image",image);
	cvShowImage("YCrCb",YCrCb);
	cvShowImage("Skin",Skin);

	cvWaitKey(0);
	cvDestroyAllWindows();

	cvReleaseImage(&image);
	cvReleaseImage(&YCrCb);
	cvReleaseImage(&Skin);

	return 0;
}
