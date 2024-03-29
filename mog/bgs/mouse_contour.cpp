#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/background_segm.hpp"
#include <stdio.h>
#include <string>
#include <iostream>

using namespace cv;
using namespace std;

Mat outline;
Point prev,curr;
Point outline_pts[1][30];
int pts;
int flag = 0;
Mat img ;
int thresh = 100;
static void onMouse( int event, int x, int y, int, void* )
{
	if(event == CV_EVENT_RBUTTONDOWN)
	{
		const Point* ppt[1] = { outline_pts[0] };
  			int npt[] = { pts };

  		//	fillPoly(outline, ppt, npt ,1,Scalar(255,255,255),8 );
  		//	imwrite("1.png",outline);
  		Mat src_gray;
            //	  cvtColor( src_gray, outline, CV_BGR2GRAY );
             	
             	Mat canny_output;
  		vector<vector<Point> > contours;
  		vector<Vec4i> hierarchy;

  		/// Detect edges using canny
  		Canny( outline, canny_output, thresh, thresh*2, 3 );
  		/// Find contours
  		findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
  		drawContours( img, contours, -1, Scalar(255,0,0), CV_FILLED, 8, hierarchy, 0, Point() );
		imshow("uh",img);

	}
    if( event != CV_EVENT_LBUTTONDOWN )
        return;
   	
   	Point seed = Point(x,y);
   	if(curr.x == 0 && curr.y == 0 && prev.x == 0 && prev.y == 0)
   	{
   		curr = seed;
   		prev = seed;
   	}
   	else
   	{
   		prev = curr;
   		curr = seed;
   	}
   	line(outline, prev, curr,Scalar(255,255,255), 10, 4, 0);

   	outline_pts[0][pts] = seed;
   	pts ++;
}
int main(int argc, const char** argv){

	VideoCapture cap;
	cap.open(argv[1]);

    if( !cap.isOpened() )
    {
        printf("can not open camera or video file\n");
        return -1;
    }
   
    namedWindow("image", WINDOW_NORMAL);
        namedWindow("outline", WINDOW_NORMAL);
    setMouseCallback( "image", onMouse, 0 );

    
    cap >> img;

    curr = Point(0,0);
    prev = Point(0,0);
    outline = cv::Mat( img.rows, img.cols, CV_8UC3, cv::Scalar(0,0,0) );
    //outline = Mat::zeros(img.size(), CV_8UC3);
    for(;;)
    {
        cap >> img;
        if( img.empty() )
        break;

    	imshow("image",img);
    	imshow("outline",outline);

    	char k = (char)waitKey(30);
        if( k == 27 ) break;
        
    }
	return 0;
}
