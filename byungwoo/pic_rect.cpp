#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
void Find_ROI(CvRect* rect,int x, int y);
CvRect rect[10];
int INDEX = -1;
int DIST = 15;
int main(){

	int x=0, y=0;
	int Cr=0, Cb=0, w=0, h=0;

	IplImage* Image = 0;
	Image = cvLoadImage("./test.1.jpeg",-1);

	cvNamedWindow("Image",0);
	cvNamedWindow("YCbCr",0);
	cvNamedWindow("Binary",0);

	cvResizeWindow("Image",340,300);
	cvResizeWindow("YCbCr",340,300);
	cvResizeWindow("Binary",340,300);

	IplImage* YCbCr = cvCreateImage(cvGetSize(Image),8,3);
	IplImage* Binary = cvCreateImage(cvGetSize(Image),8,1);

	cvCvtColor(Image,YCbCr,CV_RGB2YCrCb);

	w = YCbCr->width;
	h = YCbCr->height;

	printf("width = %d, height = %d\n",w,h);
	
	memset(rect,0,10*sizeof(CvRect));	
	printf("x = %d, y = %d, width = %d, height = %d\n",rect[0].x,rect[0].y, rect[0].width,rect[0].height);
	
	for(y=0; y<h; y++){
		for(x=1; x<w; x++){

			Cr = (int)((unsigned char*)(YCbCr->imageData+YCbCr->widthStep*(y)))[(x)*3+1];
			Cb = (int)((unsigned char*)(YCbCr->imageData+YCbCr->widthStep*(y)))[(x)*3+2];

			
			if((Cr>110 && Cr<173)&&(Cb>77&&Cb<138)){
				Find_ROI(rect,x,y);
				((unsigned char*)(Binary->imageData + Binary->widthStep*(y)))[(x)]=255;

			}
			else
				((unsigned char*)(Binary->imageData + Binary->widthStep*(y)))[(x)]=0;
		}
	}

	for(x=0; x<INDEX+1;x++){
		printf("x = %d\n y = %d\n width = %d\n, height = %d\n",rect[x].x, rect[x].y, rect[x].width, rect[x].height);
	}
	cvShowImage("Image",Image);
	cvShowImage("YCbCr",YCbCr);
	cvShowImage("Binary",Binary);

//	if(cvWaitKey(33)==27)
//		break;

	cvWaitKey(0);
}
void  Find_ROI(CvRect* rect,int x, int y)
{
	int i=0;
	int flag=0;

	if(INDEX == -1 ){
		rect[++INDEX].x = x;
		rect[INDEX].y = y;
	}

	// find i-rect		i is current index
	while(i<INDEX+1)
	{
		if((rect[i].x-DIST<x) && (x<rect[i].x+DIST))		//if x is in range of i-rect.x
			if(y<rect[i].y+DIST){//if y is in range of i-rect.y
				flag = 1;
				break;
			}
		i++;
	}

	if(flag ==1){

		if( x < rect[i].x ){	// x <
			rect[i].width = rect[i].width + rect[i].x - x;
			rect[i].x = x;
		}
				
		else if(rect[i].x < x ){
			rect[i].width = x - rect[i].x;
		}
		if(y<rect[i].y+DIST)
			rect[i].height = y - rect[i].y;

		if(rect[i].y+DIST>y)
			rect[i].height = rect[i].height + y - rect[i].y;
	}
	else
	{
		rect[++INDEX].x = x;
		rect[INDEX].y = y;

		printf("INDEX = %d\n",INDEX);
	}

	 

	
/* case 1. normal one
   case 2. x< index and it is left part if rect
   case 3. x is not a part of rect

   have to check in all of the rects using for loop in i<index;

*/
}






