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
    waitKey(0);
    destroyAllWindows();
    ///print mask
    Mat resimg;
    img1.copyTo(resimg,mask1); //apply mask to original img
    imshow("Resulterende pixels",resimg);
    ///print resulting img
    waitKey(0);
    destroyAllWindows();


    resize(img2,img2,Size (img2.cols/3,img2.rows/3));
    Mat img2gs,th1;
    cvtColor(img2,img2gs, COLOR_BGR2GRAY);
    threshold(img2gs,th1,0,255,THRESH_OTSU | THRESH_BINARY);
    imshow("Threshheld img",th1);
    waitKey(0);
    destroyAllWindows();

    Mat img2eq;
    equalizeHist(img2gs,img2eq);
    imshow("Equalized img",img2eq);
    waitKey(0);
    destroyAllWindows();

    Mat th2;
    threshold(img2eq,th2,0,255,THRESH_OTSU | THRESH_BINARY);
    imshow("Thresheld equ img",th2);
    waitKey(0);
    destroyAllWindows();

    Mat img2cla;
    Ptr<CLAHE> c_ptr = createCLAHE();
    c_ptr -> setTilesGridSize(Size(15,15));
    c_ptr -> setClipLimit(1);
    c_ptr -> apply(img2gs,img2cla);
    imshow("CLAHE result img",img2cla);
    waitKey(0);
    destroyAllWindows();

    Mat th3;
    threshold(img2cla,th3,200,255,THRESH_OTSU | THRESH_BINARY);
    imshow("Thresheld CLAHE img",th3);
    waitKey(0);
    destroyAllWindows();

    ///*******************************************************************************

    ///1.2
    split(img3,bgr); //splits img into BGR
    Mat mask = Mat::zeros(img3.rows,img1.cols,CV_8UC1);
    mask = (bgr[2]>95) & (bgr[1]>40) & (bgr[0]>20) & ((max(bgr[2],max(bgr[1],bgr[0])) - min(bgr[2],min(bgr[1],bgr[0])))>15) & (abs(bgr[2]-bgr[1])>15) & (bgr[2]>bgr[1]) & (bgr[2]>bgr[0]);
    ///gives binary values
    mask *= 255; //make ones white
    imshow("Huidskleursmasker",mask);
    waitKey(0);
    destroyAllWindows();
    ///print mask
    img1.copyTo(resimg,mask); //apply mask to original img
    imshow("Resulterende pixels",resimg);
    ///print resulting img
    waitKey(0);
    destroyAllWindows();


    erode(mask,mask,Mat(),Point(-1,-1),2);
    dilate(mask,mask,Mat(),Point(-1,-1),2);
    ///remove noise
    imshow("Remove noise",mask);
    ///print resulting img
    waitKey(0);
    destroyAllWindows();

    dilate(mask,mask,Mat(),Point(-1,-1),5);
    erode(mask,mask,Mat(),Point(-1,-1),5);
    ///connect blob
    imshow("Remove noise and connect blobs",mask);
    ///print resulting img
    waitKey(0);
    destroyAllWindows();




    ///convex hull approach
    vector<vector<Point>> contours;
    findContours(mask,contours,RETR_EXTERNAL,CHAIN_APPROX_NONE); //finds and accents contours
    ///convex hull approach
    vector<vector<Point>> hulls;
    for(size_t i=0;i<contours.size();i++){
        vector<Point> hull;
        convexHull(contours[i],hull);
        hulls.push_back(hull);
    }
    drawContours(mask,hulls,-1,255,-1);
    ///Draw filled hulls
    imshow("hulls",mask);
    ///print resulting img
    waitKey(0);

    Mat colored_hulls;
    vector<Mat> fin;
    Mat Blue = bgr[0] & mask;
    Mat Green = bgr[1] & mask;
    Mat Red = bgr[2] & mask;
    fin.push_back(Blue);
    fin.push_back(Green);
    fin.push_back(Red);

    merge(fin, colored_hulls);

    imshow("Hulls on img!",colored_hulls);
    waitKey(0);



    waitKey(0);
}


