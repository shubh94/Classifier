    #include <stdio.h>  
    #include <iostream>  
      
    #include <opencv2/opencv.hpp>  
    #include <opencv2/core/core.hpp>  
    #include <opencv2/highgui/highgui.hpp>  
    #include <opencv2/video/background_segm.hpp>  
      
      
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
      
      int MAX_KERNEL_LENGTH = 31;
      
    int main(int argc, char **argv)  
    {  
      
     //global variables  
     Mat frame; //current frame  
     Mat resizeF;  
     Mat fgMaskMOG; //fg mask generated by MOG method  
     Mat fgMaskMOG2; //fg mask fg mask generated by MOG2 method  
     Mat fgMaskGMG; //fg mask fg mask generated by MOG2 method  
       
      
     Ptr< BackgroundSubtractor> pMOG; //MOG Background subtractor  
     Ptr< BackgroundSubtractor> pMOG2; //MOG2 Background subtractor  
     Ptr< BackgroundSubtractorGMG> pGMG; //MOG2 Background subtractor  
       
      
      
     pMOG = new BackgroundSubtractorMOG();  
     pMOG2 = new BackgroundSubtractorMOG2();  
     pGMG = new BackgroundSubtractorGMG();  
       
   //  char fileName[100] = "abc.3gp"; //Gate1_175_p1.avi"; //mm2.avi"; //";//_p1.avi";  
     VideoCapture stream1(argv[1]);   //0 is the id of video device.0 if you have only one camera     
      
     Mat element = getStructuringElement(MORPH_RECT, Size(3, 3), Point(1,1) );     
      
     //unconditional loop     
     while (true) {     
      Mat cameraFrame;     
      if(!(stream1.read(frame))) //get one frame form video     
       break;  
        
      resize(frame, resizeF, Size(frame.size().width/2, frame.size().height/2) );  
 //     pMOG->operator()(resizeF, fgMaskMOG);  
     // pMOG2->operator()(resizeF, fgMaskMOG2);  
     pGMG->operator()(resizeF, fgMaskGMG);  
     morphologyEx(fgMaskGMG, fgMaskGMG, CV_MOP_OPEN, element);   
    
      
      
      imshow("Origin", resizeF);  
    //  imshow("MOG", fgMaskMOG);  
    //  imshow("MOG2", fgMaskMOG2);  
      imshow("GMG", fgMaskGMG);  
        
 cv::Mat img_mask1;
    fgMaskGMG.copyTo(img_mask1);

    cv::Mat img_mask2;
    resizeF.copyTo(img_mask2);
       
//          if( display_dst( DELAY_BLUR ) != 0 ) { return 0; } }

        std::vector<std::vector<cv::Point> > contours;

   cv::erode(img_mask1,img_mask1,cv::Mat());
   cv::dilate(img_mask1,img_mask1,cv::Mat());

   for ( int i = 1; i < 10; i = i + 2 )
        { GaussianBlur( img_mask1, img_mask1, Size( i, i ), 0, 0 );}
        
    cv::findContours( img_mask1, // binary input image
                               contours, // vector of vectors of points
                               CV_RETR_EXTERNAL, // retrieve only external contours
                               CV_CHAIN_APPROX_NONE); // detect all pixels of each contour
 
    cv::drawContours(resizeF, // draw contours here
                                  contours, // draw these contours
                                  -1, // draw all contours
                                  cv::Scalar(0,0,255), // set color
                                  2); // set thickness
	
  cv::imshow("Frame",resizeF);
      
      if (waitKey(30) >= 0)     
       break;     
     }  
      
    }  