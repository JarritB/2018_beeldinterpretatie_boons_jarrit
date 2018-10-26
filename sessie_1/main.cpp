#include <iostream>
#include <opencv2/opencv.hpp>   //project->linkersettings -> other -> `pkg-config opencv --libs`

using namespace std;
using namespace cv; //in this order

int main(int argc,const char** argv)
{
    CommandLineParser parser(argc,argv,
    "{help h usage ?   |  |show this message }"
    "{image_1 i1       |  |(required) absolute path to first image }"
    "{image_2 i2       |  |(required) absolute path to second image }"
    "{image_3 i3       |  |(required) absolute path to third image }"
    );

    if(parser.has("help")){
    parser.printMessage();
    return 0;
    }

    string image_1_location(parser.get<string>("image_1"));
    string image_2_location(parser.get<string>("image_2"));
    string image_3_location(parser.get<string>("image_3"));
    ///colect data from arguments

    if(image_1_location.empty() || image_2_location.empty() || image_3_location.empty()){
        cerr << "wrong arg" << endl;
        parser.printMessage();
        return -1;
    }
    ///check given arguments
    cout<<image_3_location;

    Mat img1,img2,img3;
    img1 = imread(image_1_location);
    img2 = imread(image_2_location);
    img3 = imread(image_3_location);
    ///read images in Matrix var

    if(img1.empty()){
        cerr <<"Image1 not found";
        return -1;
    }
    if(img2.empty()){
        cerr <<"Image2 not found";
        return -1;
    }
    if(img3.empty()){
        cerr <<"Image3 not found";
        return -1;
    }
    ///stop if any of the images is read wrongly and return which one

    //imshow("img1",img1);
    //imshow("img2",img2);
    //imshow("img3",img3);
    ///open images in original resolution to test

    Mat bgr[3];    //new matrix to save channels in
    split(img1,bgr); //splits img into BGR
    Mat mask1 = Mat::zeros(img1.rows,img1.cols,CV_8UC1);
    Mat mask2 = mask1.clone();
/*  test each pixel seperatly
    for(int row=0;row<img1.rows;row++){
        for(int col=0;col<img1.cols;col++){
            if(test){
                pixel = 255
            }
        }
    }
*/
    mask1 = (bgr[2]>95) & (bgr[1]>40) & (bgr[0]>20) & ((max(bgr[2],max(bgr[1],bgr[0])) - min(bgr[2],min(bgr[1],bgr[0])))>15) & (abs(bgr[2]-bgr[1])>15) & (bgr[2]>bgr[1]) & (bgr[2]>bgr[0]);
    ///gives binary values
    mask1 *= 255; //make ones white
    imshow("Huidskleursmasker",mask1);
    ///print mask
    Mat resimg;
    img1.copyTo(resimg,mask1); //apply mask to original img
    imshow("Resulterende pixels",resimg);
    ///print resulting img



    waitKey(0);
    ///wait for user input to show images
}


