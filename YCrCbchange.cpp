#include <cv.h>
#include <highgui.h>

int main()
{
	IplImage* src_image = 0;
	IplImage* dst_image = 0;

	src_image = cvLoadImage("./cat.jpg",-1);

	cvNamedWindow("RGV Color Image",0);
	cvNamedWindow("HSV Colar Image",0);

	dst_image = cvCreateImage(cvGetSize(src_image),IPL_DEPTH_8U,3);
	cvCvtColor(src_image,dst_image,CV_RGB2YCrCb);

	cvShowImage("RGB Color Image",src_image);
	cvShowImage("YCRCB colar Image",dst_image);

	cvWaitKey(0);

	cvReleaseImage(&src_image);
	cvReleaseImage(&dst_image);

	return 0;
}

