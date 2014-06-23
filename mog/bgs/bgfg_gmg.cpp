/*
 * FGBGTest.cpp
 *
 *  Created on: May 7, 2012
 *      Author: Andrew B. Godbehere
 */

#include <opencv2/opencv.hpp>
 #include <stdio.h>  
    #include <iostream> 
    #include <cmath>
    #include <stdlib.h>
    #include <string>
using namespace cv;

int MAX_KERNEL_LENGTH = 31;

static void help()
{
    std::cout <<
    "\nA program demonstrating the use and capabilities of a particular BackgroundSubtraction\n"
    "algorithm described in A. Godbehere, A. Matsukawa, K. Goldberg, \n"
    "\"Visual Tracking of Human Visitors under Variable-Lighting Conditions for a Responsive\n"
    "Audio Art Installation\", American Control Conference, 2012, used in an interactive\n"
    "installation at the Contemporary Jewish Museum in San Francisco, CA from March 31 through\n"
    "July 31, 2011.\n"
    "Call:\n"
    "./BackgroundSubtractorGMG_sample\n"
    "Using OpenCV version " << CV_VERSION << "\n"<<std::endl;
}
 int fno=0;
string make_str(int n) ;   // return after converting n to str

int main(int argc, char** argv)
{
    help();

    initModule_video();
    setUseOptimized(true);
    setNumThreads(8);

    Ptr<BackgroundSubtractorGMG> fgbg = Algorithm::create<BackgroundSubtractorGMG>("BackgroundSubtractor.GMG");
    if (fgbg.empty())
    {
        std::cerr << "Failed to create BackgroundSubtractor.GMG Algorithm." << std::endl;
        return -1;
    }

    fgbg->set("initializationFrames", 120);
    fgbg->set("decisionThreshold", 0.8);

    VideoCapture cap;
    if (argc > 1)
        cap.open(argv[1]);
    else
        cap.open(0);

    if (!cap.isOpened())
    {
        std::cerr << "Cannot read video. Try moving video file to sample directory." << std::endl;
        return -1;
    }

    Mat frame, fgmask, segm, final;

    namedWindow("FG Segmentation", WINDOW_NORMAL);

    for (;;)
    {
        cap >> frame;

        if (frame.empty())
            break;

        (*fgbg)(frame, fgmask);
	imshow("Fore", fgmask);
	
	   erode(fgmask,fgmask,cv::Mat());
	   dilate(fgmask,fgmask,cv::Mat());

	//    for ( int i = 1; i < 10; i = i + 2 )
	 //       { GaussianBlur( fgmask, fgmask, Size( i, i ), 0, 0 );}
       std::vector<std::vector<cv::Point> > contours;
        cv::findContours( fgmask, // binary input image
                               contours, // vector of vectors of points
                               CV_RETR_EXTERNAL, // retrieve only external contours
                               CV_CHAIN_APPROX_NONE); // detect all pixels of each contour
        frame.copyTo(final);

        string path = "images/used/";
      string path1="images/unused/";
      bool flag = false;
      fno++;
      if(fno > 10000) break;
	   for (int i = 0; i < contours.size(); i++)
        {
            
            if(contourArea(contours[i])>5000)
            {
               drawContours(final,              // draw contours here
                                contours, i,    // draw these contours
                                Scalar(0, 0, 255),  // set color
                                2);
               flag = true;
                
            }
        }
        if(flag) 
        {   //cout<<fno<<"\n";
            cv::imwrite( path + make_str(fno) + ".jpg",final);
            //cout<<path + make_str(fno) + ".jpg"<<endl;
        } else {
            cv::imwrite( path1 + make_str(fno) + ".jpg",final);
        }
        imshow("output", final);
        frame.copyTo(segm);
        add(frame, Scalar(100, 100, 0), segm, fgmask);

        imshow("FG Segmentation", segm);

        int c = waitKey(30);
        if (c == 'q' || c == 'Q' || (c & 255) == 27)
            break;
    }

    return 0;
}

string make_str(int n)    // return after converting n to str
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
