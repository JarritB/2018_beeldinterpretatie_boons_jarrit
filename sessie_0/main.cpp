#include <iostream>
#include <opencv2/opencv.hpp>   //project->linkersettings -> other -> `pkg-config opencv --libs`

using namespace std;
using namespace cv; //in this order

int main(int argc,const char** argv)
{
    CommandLineParser parser(argc,argv,
    "{help h usage ?   |  |show this message }"
    "{image_gray ig    |  |(required) path to gray image }"
    "{image_color ic   |  |(required) path to color image }"
    );

    if(parser.has("help")){
    parser.printMessage();
    return 0;
    }

    string image_gray_location(parser.get<string>("image_gray"));
    string image_color_location(parser.get<string>("image_color"));
    ///colect data from arguments

    if(image_gray_location.empty() || image_color_location.empty()){
        cerr << "wrong arg" << endl;
        parser.printMessage();
        return -1;
    }
    ///check given arguments


    Mat gimg,cimg;
    gimg = imread(image_gray_location);
    cimg = imread(image_color_location);
    ///read images in Matrix var

    if(gimg.empty() && cimg.empty()){
        cerr <<"Image not found";
        return -1;
    }
    ///stop if both reads fail

    imshow("eavise logo - gray",gimg);
    imshow("eavise logo - color",cimg);
    ///open images in original resolution

    Mat bgr[3];    //new matrix to save channels in
    split(cimg,bgr); //splits img into BGR
    imshow("Channel-Blue",bgr[0]);
    imshow("Channel-Green",bgr[1]);
    imshow("Channel-Red",bgr[2]);
    ///display each channel: three grayscale images representing the colors in the original images

    Mat gscaled;
    cvtColor(cimg,gscaled, COLOR_BGR2GRAY); //convert colored to grayscaled
    imshow("Grayscaled",gscaled);
    ///display colored image in grayscale

    for(int row=0;row<gscaled.rows;row++){
        for(int col=0;col<gscaled.cols;col++){
            cout <<(int)gscaled.at<uchar>(row,col); //needs to be displayed as integer
            cout <<" "; //space for readability
        }
    }
    ///print pixelvalue of grayscaled image using dubble iteration

    Mat canvas = Mat::zeros(256,256,CV_8UC3);//create empty matrix:res 256x256,filled black,3 channels,1byte

    line(canvas,Point(20,140),Point(236,20),Scalar(255,255,255),5);//white line
    circle(canvas,Point(128,128),30,Scalar(0,255,255),3);//yellow circle
    putText(canvas,"Text",Point(180,180),1,1,Scalar(255,128,255),1);//text
    imshow("Canvas",canvas);
    ///make a canvas on which are drawn some basic shapes

    waitKey(0);
    ///wait for user input to show images
}


