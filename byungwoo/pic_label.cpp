#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
void Labeling(IplImage* Skin,int* map);
int doLabeling(int x,int y,int* map ,IplImage* Skin);
CvRect rect[100]; 
int INDEX = 0;
int main(){

	int x=0, y=0;
	int Cr=0, Cb=0, w=0, h=0;

	IplImage* Image = 0;
	Image = cvLoadImage("../test.1.jpeg",-1);

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

	int* map;
	map = (int*) malloc(w*h);
	memset(map,-1,w*h*sizeof(int));

	printf("width = %d, height = %d\n",w,h);
	
	memset(rect,0,10*sizeof(CvRect));	

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
	Labeling(Binary,map);

	printf("INDEX = %d\n",INDEX);
	for(x = 0; x<INDEX; x++)
		printf("x = %d, y = %d, width = %d, height = %d\n",rect[x].x,rect[x].y,rect[x].width,rect[x].height);

	cvShowImage("Image",Image);
	cvShowImage("YCbCr",YCbCr);
	cvShowImage("Binary",Binary);

	cvWaitKey(0);
}
void Labeling(IplImage* Skin,int* map){
	int x,y;
	int w =(int) Skin->width;
	int h =(int) Skin->height;

	//int map[w][h];

	//memset(map,-1,w*h*sizeof(int));

	for(x = 0; x<Skin->width; x++){
		for(y = 0; y<Skin->height; y++){
			if(doLabeling(x,y,map,Skin))
				INDEX++;
		}
}

	for(x = 0; x<Skin->width; x++)
		for(y = 0; y<Skin->height; y++)
		{
			if(map[x][y]!=-1){
				if(rect[map[x][y]].x>x)
					rect[map[x][y]].x = x;
				else if(rect[map[x][y]].x+rect[map[x][y]].width<x)
					rect[map[x][y]].width = x - rect[map[x][y]].x;


				if(rect[map[x][y]].y>y)
					rect[map[x][y]].y = y;
				else if(rect[map[x][y]].y+rect[map[x][y]].height<y)
					rect[map[x][y]].height = y - rect[map[x][y]].y;
			}
		}
}
int doLabeling(int x, int y, int* map,IplImage* Skin){
	
	if(map[x][y]==-1)
	{
		if(x>1 && y>1)
			doLabeling(x-1,y-1,map,Skin);
		if(y>1)
			doLabeling(x,y-1,map,Skin);
		if(x<Skin->width && y>1)
			doLabeling(x+1,y-1,map,Skin);
		if(x>1)
			doLabeling(x-1,y,map,Skin);
		if(x<Skin->width)
			doLabeling(x+1,y,map,Skin);
		if(x>1 && y<Skin->height)
			doLabeling(x-1,y+1,map,Skin);
		if(y<Skin->height)
			doLabeling(x,y+1,map,Skin);
		if(x<Skin->width&& y<Skin->height)
			doLabeling(x+1,y+1,map,Skin);

		if((unsigned int)(Skin->imageData + Skin->widthStep*(y))[x] == 0)
		{
			map[x][y]= INDEX;
			return 1;
		}
		else
			return 0;
	}
	else
		return 0;
}

