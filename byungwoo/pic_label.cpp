#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <string.h>
int Labeling(IplImage* Skin);
int doLabeling(int x,int y,IplImage* Skin);

CvRect rect[100]={0}; 
int INDEX = 1;
int ** map;
int main(){

	int x=0, y=0;
	int Cr=0, Cb=0, w=0, h=0;

//	int** map;

	IplImage* Image = 0;
	Image = cvLoadImage("../test.1.jpeg",-1);
	IplImage* YCbCr = cvCreateImage(cvGetSize(Image),8,3);
	IplImage* Binary = cvCreateImage(cvGetSize(Image),8,1);
	IplImage* init = cvCreateImage(cvGetSize(Image),8,1);

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

	map = (int**) calloc(w,sizeof(int*));
	for (x=0; x<w;  x++)
		map[x] = (int*)calloc(h,sizeof(int));

	printf("width = %d, height = %d\n",w,h);
	
	/// init check ///
	
	cvZero(init);
	cvShowImage("initcheck",init);
	printf("init done!!?\n");
	
	for(x=0;x<w;x++)
		for(y=0;y<h;y++)
			if(map[x][y]!=0){
				printf("shit\n");
				break;
		}

	//////////////////


	for(y=0; y<h; y++){
		for(x=1; x<w; x++){

			Cr = (int)((unsigned char*)(YCbCr->imageData+YCbCr->widthStep*(y)))[(x)*3+1];
			Cb = (int)((unsigned char*)(YCbCr->imageData+YCbCr->widthStep*(y)))[(x)*3+2];

			
			if((Cr>110 && Cr<173)&&(Cb>77&&Cb<138)){
				((unsigned char*)(Binary->imageData + Binary->widthStep*(y)))[(x)]=255;

			}
			else
				((unsigned char*)(Binary->imageData + Binary->widthStep*(y)))[(x)]=0;
		}
	}

	Labeling(Binary);

	printf("INDEX = %d\n",INDEX);
	for(x = 0; x<INDEX; x++)
		printf("x = %d, y = %d, width = %d, height = %d\n",rect[x].x,rect[x].y,rect[x].width,rect[x].height);

	cvShowImage("Image",Image);
	cvShowImage("YCbCr",YCbCr);
	cvShowImage("Binary",Binary);

	cvWaitKey(0);

	free(map);
}
int Labeling(IplImage* Skin){
	int x,y;
	int w =(int) Skin->width;
	int h =(int) Skin->height;

	printf("in labeling\n	width = %d, height = %d\n",Skin->width, Skin->height);

	for(x = 0; x<Skin->width; x++){
		for(y = 0; y<Skin->height; y++){
			if(doLabeling(x,y,Skin)){
//				printf("new index\n");
				INDEX++;
			}
//			else
//				printf("not found\n");
		}
	}

	for(x = 0; x<Skin->width; x++)
		for(y = 0; y<Skin->height; y++)
		{
			if(map[x][y]>0){
				if(rect[map[x][y]-1].x>x)
					rect[map[x][y]-1].x = x;
				else if(rect[map[x][y]-1].x+rect[map[x][y]-1].width<x)
					rect[map[x][y]-1].width = x - rect[map[x][y]-1].x;


				if(rect[map[x][y]-1].y>y)
					rect[map[x][y]-1].y = y;
				else if(rect[map[x][y]-1].y+rect[map[x][y]-1].height<y)
					rect[map[x][y]-1].height = y - rect[map[x][y]-1].y;
			}
		}
}
int doLabeling(int x, int y, IplImage* Skin){
	int w = Skin->width;	
//	printf("do labeling\n");
	if(map[x][y]>0)
	{
//		printf("checking non-checked pixel\n");
//		map[x][y] = -1;
		return 0;
	}
	else if(map[x][y]==-1)
		return 0;
	else
	{
		if((unsigned int)(Skin->imageData + Skin->widthStep*(y))[x] == 0)
		{
			map[x][y]=INDEX;

			if(x>1 && y>1)
				doLabeling(x-1,y-1,Skin);
			if(y>1)
				doLabeling(x,y-1,Skin);
			if(x<Skin->width && y>1)
				doLabeling(x+1,y-1,Skin);
			if(x>1)
				doLabeling(x-1,y,Skin);
			if(x<Skin->width)
				doLabeling(x+1,y,Skin);
			if(x>1 && y<Skin->height)
				doLabeling(x-1,y+1,Skin);
			if(y<Skin->height)
				doLabeling(x,y+1,Skin);
			if(x<Skin->width&& y<Skin->height)
				doLabeling(x+1,y+1,Skin);

			return 1;
		}
		else
		{
			map[x][y] = -1;
			return 0;
		}
	}

}
