#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <math.h>

int main()
{
	int R_color = 0, G_color = 0, B_color = 0;
	int w=0, h=0;

	IplImage* image = 0;
	image = cvLoadImage("./test1.jpeg",-1);

	printf("here 1 \n");
	cvNamedWindow("image",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Skin",CV_WINDOW_AUTOSIZE);

	printf("here 2 \n");
	IplImage* R =  cvCreateImage(cvGetSize(image),8,1);
	IplImage* G = cvCreateImage(cvGetSize(image),8,1);
	IplImage* B = cvCreateImage(cvGetSize(image),8,1);
	IplImage* Skin = cvCreateImage(cvGetSize(image),8,1);

	printf("here 3 \n");
	cvSplit(image,R,G,B,NULL);

	w=R->width;
	h=R->height;

	printf("here 4 \n");
	for(int i=0;i<h;i++)
		for(int j=0;j<w;j++)
		{
			R_color = R->imageData[i*R->widthStep + j];
			G_color = G->imageData[i*G->widthStep + j];
			B_color = B->imageData[i*B->widthStep + j];
			printf("%d \n",R_color);
			if(R_color > 95 && G_color> 40 && B_color > 20 && (std::max(R_color,std::max(G_color,B_color))-std::min(R_color,std::min(G_color,B_color))) > 15 && abs(R_color - G_color) > 15 && R_color > G_color && R_color > B_color)
				Skin->imageData[i*Skin->widthStep + j] = 255;
			else
				Skin->imageData[i*Skin->widthStep + j] = 0;
		}	


	printf("here 5 \n");
	cvShowImage("Image",image);
	cvShowImage("Skin",Skin);

	printf("here 6 \n");
	cvWaitKey(0);
	cvDestroyAllWindows();	

	printf("here 7 \n");
	cvReleaseImage(&image);
	cvReleaseImage(&Skin);

	return 0;
}	
