#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;
using namespace cv;

static void help()
{
 printf("\nDo background segmentation, especially demonstrating the use of cvUpdateBGStatModel().\n"
"Learns the background at the start and then segments.\n"
"Learning is togged by the space key. Will read from file or camera\n"
"Usage: \n"
"			./bgfg_segm [--camera]=<use camera, if this key is present>, [--file_name]=<path to movie file> \n\n");
}

const char* keys =
{
    "{c |camera   |true    | use camera or not}"
    "{fn|file_name|tree.avi | movie file             }"
};

 int fno=0;
string make_str(int n)      
{
    string ans = "";
    if(n == 0) return "0";
    while(n > 0) {
        ans.push_back((char)(n%10 + '0'));
        n /= 10;
    }
    reverse(ans.begin(), ans.end());
    return ans;
}

//this is a sample for foreground detection functions
int main(int argc, const char** argv)
{
    //help();
    //freopen("bgfg_segm.txt", "w", stdout) ;
    ofstream fout;
    fout.open("a.txt");
   // CommandLineParser parser(argc, argv, keys);
   // bool useCamera = parser.get<bool>("camera");
  //  string file = parser.get<string>("file_name");
    VideoCapture cap;
    bool update_bg_model = true;

   
    cap.open(argv[1]);
   // parser.printParams();

    if( !cap.isOpened() )
    {
        printf("can not open camera or video file\n");
        return -1;
    }

    namedWindow("image", WINDOW_NORMAL);
    namedWindow("foreground mask", WINDOW_NORMAL);
   namedWindow("mean background image", WINDOW_NORMAL);
   namedWindow("threshold", WINDOW_NORMAL);
    BackgroundSubtractorMOG2 bg_model;//(100, 3, 0.3, 5);

    Mat img, fgmask, fgimg,reimg;                 //img input image, fgmask: foreground mask
    int s = 2;
    for(;;)
    {
        cap >> img;

        if( img.empty() )
            break;

        resize(img, reimg, Size(img.size().width/s, img.size().height/s) );  

        //cvtColor(_img, img, COLOR_BGR2GRAY);

        if( fgimg.empty() )
          fgimg.create(reimg.size(), reimg.type());
          
        //update the model
        bg_model(reimg, fgmask, update_bg_model ? -1 : 0);

    //    Mat bgimg;                              // to get background image
    //    bg_model.getBackgroundImage(bgimg);

        imshow("image", img);
        imshow("foreground mask", fgmask);
    //    cv::threshold(fgmask, fgmask, 10, 255, cv::THRESH_BINARY);
   //      imshow("threshold",fgmask);
        cv::Mat img_mask1;
    	  fgmask.copyTo(img_mask1);

	      std::vector<std::vector<cv::Point> > contours;

   	    cv::erode(img_mask1,img_mask1,cv::Mat());
   	    cv::dilate(img_mask1,img_mask1,cv::Mat());
      
         imshow("mean background image",img_mask1);
        for ( int i = 1; i < 10; i = i + 2 )
        { 
          // blur( img_mask1, img_mask1, Size( i, i ), Point(-1,-1) );
            GaussianBlur( img_mask1, img_mask1, Size( i, i ), 0, 0 );
        }
                imshow("threshold",img_mask1);

        cv::findContours( img_mask1, // binary input image
                               contours, // vector of vectors of points
                               CV_RETR_EXTERNAL, // retrieve only external contours
                               CV_CHAIN_APPROX_NONE); // detect all pixels of each contour
   
   	/// Get the moments
	  vector<Moments> mu(contours.size() );
	  for( int i = 0; i < contours.size(); i++ )
	     { mu[i] = moments( contours[i], false ); }

	  ///  Get the mass centers:
	  vector<Point2f> mc( contours.size() );
	  for( int i = 0; i < contours.size(); i++ )
	     { mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 ); }


        string path = "../output/used/";
        string path1="../output/unused/";
        bool flag = false;
        fno++;
        //if(fno > 2000) break;
        for (int i = 0; i < contours.size(); i++)
        {
            
            if(contourArea(contours[i])>1500)
            {
               drawContours(reimg,              // draw contours here
                                contours, i,    // draw these contours
                                Scalar(0, 0, 255),  // set color
                                2);
               circle( reimg, mc[i], 4,Scalar(0, 255, 0), -1, 8, 0 );
               if(mc[i].y>120 && mc[i].y < 300 )
               flag = true;
                
            }
        }
        if(flag) 
        {   cout << fno << "\n";
        	  fout << fno << "\n";
            cv::imwrite( path + make_str(fno) + ".jpg",img);
        } 
        else 
        {
            cv::imwrite( path1 + make_str(fno) + ".jpg",img);
        }
	        imshow("Masked", reimg);
        char k = (char)waitKey(1);
        if( k == 27 ) break;
        if( k == ' ' )
        {
            update_bg_model = !update_bg_model;
            if(update_bg_model)
                printf("Background update is on\n");
            else
                printf("Background update is off\n");
        }
    }
	fout.close();
    return 0;
}