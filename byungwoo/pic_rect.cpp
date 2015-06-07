#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <string.h>
int Labeling(IplImage* Skin);
int doLabeling(int x,int y,IplImage* Skin);

CvRect rect[100]={9999}; 

int INDEX = 1;
int ** map;
int main(){

	int x=0, y=0;
	int Cr=0, Cb=0, w=0, h=0;


	IplImage* Image = 0;
	Image = cvLoadImage("../test.1.jpeg",-1);
	IplImage* YCbCr = cvCreateImage(cvGetSize(Image),8,3);
	IplImage* Binary = cvCreateImage(cvGetSize(Image),8,1);
	IplImage* init = cvCreateImage(cvGetSize(Image),8,3);

	cvNamedWindow("Image",0);
	cvNamedWindow("YCbCr",0);
	cvNamedWindow("Binary",0);
	cvNamedWindow("initcheck",0);

	cvResizeWindow("Image",340,300);
	cvResizeWindow("YCbCr",340,300);
	cvResizeWindow("Binary",340,300);
	cvResizeWindow("initcheck",340, 300);

	cvCvtColor(Image,YCbCr,CV_RGB2YCrCb);

	h = Image->height;
	w = Image->width;

	// map init
	map = (int**) calloc(w,sizeof(int*));
	for (x=0; x<w;  x++)
		map[x] = (int*)calloc(h,sizeof(int));

	printf("width = %d, height = %d\n",w,h);
	
	/// init check ///
	/*
	cvZero(init);
	cvShowImage("initcheck",init);
	printf("init done!!?\n");
	
	for(x=0;x<w;x++)
		for(y=0;y<h;y++)
			if(map[x][y]!=0){
				printf("shit\n");
				break;
		}
*/
	//////////////////


	for(y=0; y<h; y++){
		for(x=0; x<w; x++){

			Cr = (int)((unsigned char*)(YCbCr->imageData+YCbCr->widthStep*(y)))[(x)*3+1];
			Cb = (int)((unsigned char*)(YCbCr->imageData+YCbCr->widthStep*(y)))[(x)*3+2];

			
			if((Cb>133 && Cb<173)&&(Cr>77&&Cr<127)){
				((unsigned char*)(Binary->imageData + Binary->widthStep*(y)))[(x)]=0;

			}
			else
				((unsigned char*)(Binary->imageData + Binary->widthStep*(y)))[(x)]=255;
		}
	}

	Labeling(Binary);

	for(y = 0; y<h; y++)
	{
		for(x=0; x<w; x++)
		{
			if(map[x][y] >0)
			{

				((unsigned char*)(init->imageData + init->widthStep*(y)))[(x)*3] =map[x][y]*50+100 ;
				((unsigned char*)(init->imageData + init->widthStep*(y)))[(x)*3+1] = 0;//(map[x][y]* 30) ;
				((unsigned char*)(init->imageData + init->widthStep*(y)))[(x)*3+2] =0;//100 - map[x][y]*30;// (map[x][y]* 20)+100 ;
			}
			else
			{				
				((unsigned char*)(init->imageData + init->widthStep*(y)))[(x)*3] = 0;
				((unsigned char*)(init->imageData + init->widthStep*(y)))[(x)*3+1] = 0;
				((unsigned char*)(init->imageData + init->widthStep*(y)))[(x)*3+2] = 0;
			}
		}
//		printf("\n");
	}


	printf("INDEX = %d\n",INDEX);
	for(x = 0; x<INDEX; x++)
		printf("x = %d, y = %d, width = %d, height = %d\n",rect[x].x,rect[x].y,rect[x].width,rect[x].height);

	cvShowImage("Image",Image);
	cvShowImage("YCbCr",YCbCr);
	cvShowImage("Binary",Binary);
	cvShowImage("initcheck",init);

	cvWaitKey(0);
	for(x=0;x<w;x++)
		free(map[x]);
	free(map);
}
int Labeling(IplImage* Skin){
	int x=0,y=0;
	int w =(int) Skin->width;
	int h =(int) Skin->height;

	int i =0;

	printf("Labeling ::\n	getting ROI\n");

	for(x = 0; x<w; x++)
	{
		for(y = 0; y<h; y++)
		{
			if(doLabeling(x,y,Skin))
			{
				INDEX++;
			}
		}
	}

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
			i = map[x][y]-1;

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
	
	printf("INDEX = %d\n",INDEX);
}

int doLabeling(int x,int y,IplImage* Skin)
{
	int i,j;


	if(map[x][y] == 0 )
	{
		if(((unsigned char*)(Skin->imageData + Skin->widthStep*(y)))[(x)]==0 )
		{
			map[x][y]=INDEX;

//		if(x>5 && x<Skin->width-5 && y<Skin->height-5 && y>5)
//		{
			for( i = x-3; i<x+3; i++)
				for(j = y-3; j<y+3; j++)
					if(i>0 && i<Skin->width && j>0&& j<Skin->height)
				//	if(map[i][j] == 0 && ((unsigned int*)(Skin->imageData + Skin->widthStep*(y)))[(x)]==0)
						doLabeling(i,j,Skin);
//		}
		
			return 1;
		}
		else
		{
			map[x][y] =-1;
			return 0;
		}
	}
	else
		return 0;
		
}
