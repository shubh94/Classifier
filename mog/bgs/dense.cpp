
    #include  "opencv2/opencv.hpp"  
    #include "opencv2/core/core.hpp"  
    #include "opencv2/highgui/highgui.hpp"  
    #include "opencv2/video/background_segm.hpp"  
     #include <stdio.h>  
    #include <iostream> 
    #include <cmath>
    #include <stdlib.h>
    #include <string>

    #ifdef _DEBUG          
    #pragma comment(lib, "opencv_core247d.lib")  
    #pragma comment(lib, "opencv_imgproc247d.lib")   //MAT processing  
    #pragma comment(lib, "opencv_objdetect247d.lib") //HOGDescriptor  
    //#pragma comment(lib, "opencv_gpu247d.lib")  
    //#pragma comment(lib, "opencv_features2d247d.lib")  
    #pragma comment(lib, "opencv_highgui247d.lib")  
    #pragma comment(lib, "opencv_ml247d.lib")  
    //#pragma comment(lib, "opencv_stitching247d.lib");  
    //#pragma comment(lib, "opencv_nonfree247d.lib");  
    #pragma comment(lib, "opencv_video247d.lib")  
    #else  
    #pragma comment(lib, "opencv_core247.lib")  
    #pragma comment(lib, "opencv_imgproc247.lib")  
    #pragma comment(lib, "opencv_objdetect247.lib")  
    //#pragma comment(lib, "opencv_gpu247.lib")  
    //#pragma comment(lib, "opencv_features2d247.lib")  
    #pragma comment(lib, "opencv_highgui247.lib")  
    #pragma comment(lib, "opencv_ml247.lib")  
    //#pragma comment(lib, "opencv_stitching247.lib");  
    //#pragma comment(lib, "opencv_nonfree247.lib");  
    #pragma comment(lib, "opencv_video247d.lib")  
    #endif   
      
    using namespace cv;  
    using namespace std;  
    
    int fno=0;
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
    void drawOptFlowMap (const Mat& flow, Mat& cflowmap, int step, const Scalar& color) {  
     for(int y = 0; y < cflowmap.rows; y += step)  
            for(int x = 0; x < cflowmap.cols; x += step)  
            {  
                const Point2f& fxy = flow.at< Point2f>(y, x);  
                if(cvRound(fxy.x)!=0||cvRound(fxy.y)!=0)
                {
                 // line(cflowmap, Point(x,y), Point(cvRound(x+fxy.x), cvRound(y+fxy.y)),  
                 //    color);  
                //  circle(cflowmap, Point(cvRound(x+fxy.x), cvRound(y+fxy.y)), sqrt(fxy.x*fxy.x + fxy.y*fxy.y), color, -1);
                    circle(cflowmap, Point(x,y), 6, color, -1);
              //    cout<<sqrt(fxy.x*fxy.x + fxy.y*fxy.y)<<endl;
                }  
            }  
        }  
      
      
    int main(int argc, const char** argv)  
    {  
     int s=2;  
     //global variables  
     Mat GetImg;  
     Mat prvs, next; //current frame  
       
     VideoCapture stream1(argv[1]);   //0 is the id of video device.0 if you have only one camera     
      
     if(!(stream1.read(GetImg))) //get one frame form video  
      return 0;  
       resize(GetImg, prvs, Size(GetImg.size().width/s, GetImg.size().height/s) );  
     cvtColor(prvs, prvs, CV_BGR2GRAY);  
      
     //unconditional loop     
     while (true) {     
        
      if(!(stream1.read(GetImg))) //get one frame form video     
       break;  
      //Resize 
     Mat final;
     GetImg.copyTo(final);
     resize(final, final, Size(final.size().width/s, final.size().height/s) );
  cvtColor(GetImg, GetImg, CV_BGR2GRAY); 
      resize(GetImg, next, Size(GetImg.size().width/s, GetImg.size().height/s) );  
        
      ///////////////////////////////////////////////////////////////////  
      Mat flow;  
      calcOpticalFlowFarneback(prvs, next, flow, 0.5, 3, 15, 3, 5, 1.2, 0);  
      
      Mat cflow1;  
      cvtColor(prvs, cflow1, CV_GRAY2BGR);  
    /* for(int y = 0; y < cflow.rows; y ++)  
            for(int x = 0; x < cflow.cols; x ++)  
            {  //cflow.at<uchar>(x,y)=0;
              cflow.at<cv::Vec3b>(y,x)[0] = 0;
        cflow.at<cv::Vec3b>(y,x)[1] = 0;
        cflow.at<cv::Vec3b>(y,x)[2] = 0;
            }*/
        cv::Mat cflow( cflow1.rows, cflow1.cols, CV_8UC1, cv::Scalar(0) );
    //  imshow("flow",flow);
      drawOptFlowMap(flow, cflow, 10, CV_RGB(255, 255, 255));  
        for ( int i = 1; i < 10; i = i + 2 )
        { blur( cflow, cflow, Size( i, i ),  Point(-1,-1) );}
      imshow("OpticalFlowFarneback", cflow);  
      std::vector<std::vector<cv::Point> > contours;
      cv::findContours( cflow, // binary input image
                               contours, // vector of vectors of points
                               CV_RETR_EXTERNAL, // retrieve only external contours
                               CV_CHAIN_APPROX_NONE); // detect all pixels of each contour

      string path = "images/used/";
      string path1="images/unused/";
      bool flag = false;
      fno++;
	if(fno>2000) break;
       for (int i = 0; i < contours.size(); i++)
    {   
        if(contourArea(contours[i])>4000)
        {
              cv::drawContours(final, // draw contours here
                                  contours, i, // draw these contours
                                  cv::Scalar(0,0,255), // set color
                                  2);
         //     cout << i <<" "<<contourArea(contours[i])<<endl;
              flag = true;
        }
      }
      if(flag) 
{   cout<<fno<<"\n";
    cv::imwrite( path + make_str(fno) + ".jpg",final);
    //cout<<path + make_str(fno) + ".jpg"<<endl;
}
    else
    {
      //cout<<"unsed"<<fno<<endl;
        cv::imwrite( path1 + make_str(fno) + ".jpg", final);
         //cout<<path1 + make_str(fno) + ".jpg"<<endl;
    }
 /*   cv::drawContours(final, // draw contours here
                                  contours, // draw these contours
                                  -1, // draw all contours
                                  cv::Scalar(0,0,255), // set color
                                  2); // set thickness*/
      ///////////////////////////////////////////////////////////////////  
      //Display  
      imshow("prvs", prvs);  
      imshow("next", next);  
      imshow("f",final);
      if (waitKey(5) >= 0)     
       break;  
      
      prvs = next.clone();  
     }  
      
    }  
