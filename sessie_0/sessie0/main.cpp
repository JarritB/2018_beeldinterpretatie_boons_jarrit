#include <iostream>
#include <opencv2/opencv.hpp>   //project->linkersettings -> other -> `pkg-config opencv --libs`

using namespace std;
using namespace cv; //in deze volgorde

int main(int argc,const char** argv)
{
    CommandLineParser parser(argc,argv,
    "{help h usage ?  |  |show this message }"
    "{image_gray ig   |  |(required) path to image }"
    "{image_color ic   |  |(required) path to image }"
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


    Mat gimg,cimg;
    gimg = imread(image_gray_location);
    cimg = imread(image_color_location);
    if(gimg.empty() && cimg.empty()){
        cerr <<"Image not found";
        return -1;
    }
    imshow("eavise logo - gray",gimg);
    imshow("eavise logo - color",cimg);
    waitKey(0);


    //todo imshow in versch kanalen
    //todo adhv cvtColor(input,output,COLOR_BGR2GRAY)
    //todo dubbele for loop itereren over image.rows en image.cols en telkens (int value = image.at<uchar>(row,col))
    //todo canvas
}


