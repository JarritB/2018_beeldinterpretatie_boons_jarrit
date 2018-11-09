#include <iostream>
#include <opencv2/opencv.hpp>   //project->linkersettings -> other -> `pkg-config opencv --libs`

using namespace std;
using namespace cv; //in this order


static void on_trackbar(int,void* ){


}

int main(int argc,const char** argv)
{
    CommandLineParser parser(argc,argv,
    "{help h usage ?   |  |show this message }"
    "{image_1 i1       |  |(required) absolute path to first image }"
    "{image_2 i2       |  |(required) absolute path to second image }"
    "{image_3 i3       |  |(required) absolute path to third image }"
    "{image_4 i4       |  |(required) absolute path to fourth image }"
    );

    if(parser.has("help")){
    parser.printMessage();
    return 0;
    }

    string image_1_location(parser.get<string>("image_1"));
    string image_2_location(parser.get<string>("image_2"));
    string image_3_location(parser.get<string>("image_3"));
    string image_4_location(parser.get<string>("image_4"));
    ///colect data from arguments

    if(image_1_location.empty() || image_2_location.empty() || image_3_location.empty()  || image_4_location.empty()){
        cerr << "wrong arg" << endl;
        parser.printMessage();
        return -1;
    }
    ///check given arguments
    cout<<image_3_location;

    Mat img[4];
    img[0] = imread(image_1_location);
    img[1] = imread(image_2_location);
    img[2] = imread(image_3_location);
    img[3] = imread(image_4_location);
    ///read images in Matrix var

    if(img[0].empty()){
        cerr <<"Image1 not found";
        return -1;
    }
    if(img[1].empty()){
        cerr <<"Image2 not found";
        return -1;
    }
    if(img[2].empty()){
        cerr <<"Image3 not found";
        return -1;
    }
    if(img[3].empty()){
        cerr <<"Image3 not found";
        return -1;
    }
    ///stop if any of the images is read wrongly and return which one
    Mat bgr[3],redth,res;    //new matrix to save channels in


    for(int i=0;i<4;i++){
        split(img[i],bgr); //splits img into BGR
        threshold(bgr[2],redth,120,255,THRESH_BINARY); ///threshhold red values
        img[i].copyTo(res,redth); //apply mask to original img
        imshow("imgRed",res);
        waitKey(0);
        destroyAllWindows();
    }
    ///shows images in bgr, white stays white so no distinction between red and white

    const int max_value_H = 360/2;
    const int max_value = 255;
    Mat hsv[4],mask,mask2;

    for(int i=0;i<4;i++){
        cvtColor(img[i],hsv[i],COLOR_BGR2HSV);
        inRange(hsv[i], Scalar(160, 100, 100), Scalar(179, 255, 255), mask);
        inRange(hsv[i], Scalar(0, 100, 100), Scalar(5, 255, 255), mask2);
        mask = mask|mask2;
        erode(mask,mask,Mat(),Point(-1,-1),1);
        dilate(mask,mask,Mat(),Point(-1,-1),3); //erode and dilate to remove noise
        img[i].copyTo(res,mask); //apply mask to original img
        imshow("imgHSVthresheld",res);
        waitKey(0);
        destroyAllWindows();
    }
    ///
    while(true){


    }

    /**
    TODO
    findcountours
    zoek grootste blob (CountourArea)
    **/

    waitKey(0);
}


