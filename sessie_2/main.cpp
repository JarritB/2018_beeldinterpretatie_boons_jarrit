#include <iostream>
#include <opencv2/opencv.hpp>   //project->linkersettings -> other -> `pkg-config opencv --libs`

using namespace std;
using namespace cv; //in this order

//slidervalues and maximum slidervalues for the sliders later on
const int alpha_slider_max1 = 180;
int alpha_slider1 = 169;
const int alpha_slider_max2 = 180;
int alpha_slider2 = 10;
const int alpha_slider_max3 = 255;
int alpha_slider3 = 115;
int h1;
int h2;
int s1;

//functions needed for the trackbars
static void on_trackbar1(int, void*)
{
    h1 = alpha_slider1; //set the global variable h1 to the current slidervalue;

}

static void on_trackbar2(int, void*)
{
    h2 = alpha_slider2; //set the global variable h2 to the current slidervalue;
}

static void on_trackbar3(int, void*)
{
    s1 = alpha_slider3; //set the global variable s1 to the current slidervalue;
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
        threshold(bgr[2],redth,120,255,THRESH_BINARY); //threshhold red values
        img[i].copyTo(res,redth); //apply mask to original img
        imshow("imgRed (using threshold)",res);
        waitKey(0);
        destroyAllWindows();
        inRange(img[i], Scalar(0, 0, 160), Scalar(50, 50, 255), res);//only show colors in interval
        imshow("imgRed (unsing inRange)",res);
        waitKey(0);
        destroyAllWindows();
    }
    /// threshold: automatically get all red values but also get white
    /// inRange: only get red values but harder to find since you need to adjust blue and green values too
    ///segmenting in bgr colorspace: more comprehensible colorspace, but hard to adjust values

    Mat hsv[4],mask,mask2;

    for(int i=0;i<4;i++){
        cvtColor(img[i],hsv[i],COLOR_BGR2HSV);
        inRange(hsv[i], Scalar(160, 100, 100), Scalar(180, 255, 255), mask);
        inRange(hsv[i], Scalar(0, 100, 100), Scalar(5, 255, 255), mask2);
        mask = mask|mask2;
        erode(mask,mask,Mat(),Point(-1,-1),1);
        dilate(mask,mask,Mat(),Point(-1,-1),3); //erode and dilate to remove noise
        dilate(mask, mask, Mat(), Point(-1, -1), 5);
        erode(mask, mask, Mat(), Point(-1, -1), 5);// dilate and erode to connect blobs
        img[i].copyTo(res,mask); //apply mask to original img
        imshow("imgHSVthresheld",res);
        waitKey(0);
        destroyAllWindows();
    }
    ///segmenting in HSV colorspace: more complex colorspace but easier to segment on color (hue)


    inRange(hsv[0], Scalar(160, 100, 100), Scalar(180, 255, 255), mask);
    inRange(hsv[0], Scalar(0, 100, 100), Scalar(5, 255, 255), mask2);
    mask = mask|mask2;
    erode(mask,mask,Mat(),Point(-1,-1),1);
    dilate(mask,mask,Mat(),Point(-1,-1),3); //erode and dilate to remove noise
    dilate(mask, mask, Mat(), Point(-1, -1), 5);
    erode(mask, mask, Mat(), Point(-1, -1), 5);
    imshow("mask",mask);
    waitKey(0);
    destroyAllWindows();


    vector <vector<Point>>contours;
    findContours(mask.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    vector<Point> hull;
    convexHull(contours[0],hull);
    vector<Point> big_blob = hull;
    for (int i = 0; i < contours.size(); i++){
        if (contourArea(contours[i]) > contourArea(big_blob)){
            convexHull(contours[i], hull);
            big_blob = hull;
        }
    }


    vector<vector<Point>> temp;
    temp.push_back((big_blob));
    drawContours(mask, temp, -1, 255, -1);
    Mat contourmask(Mat(img[0].size(), CV_8UC3));
    imshow("Contoured mask",mask);
    waitKey(0);
    destroyAllWindows();
    img[0].copyTo(res,mask); //apply mask to the image
    imshow("Contoured image", res); //show the result image
    waitKey(0);
    destroyAllWindows();
    Rect bound = boundingRect(mask); //get bounding rec
    Mat crop = res(bound); //crop result image to bounding rect
    imshow("Cropped", crop); //show the cropped image
    waitKey(0);
    destroyAllWindows();

    ///using connected componenent analysis to retain only the stop sign

    namedWindow("Sliders"); // Create Window
    resizeWindow("Sliders", 200, 200);//resize it (it gets auto-sized later)
    createTrackbar("max H", "Sliders", &alpha_slider1, alpha_slider_max1, on_trackbar1); //add on the three trackbars
    createTrackbar("min H", "Sliders", &alpha_slider2, alpha_slider_max2, on_trackbar2);
    createTrackbar("S", "Sliders", &alpha_slider3, alpha_slider_max3, on_trackbar3);
    on_trackbar1(alpha_slider1,0);
    on_trackbar2(alpha_slider2,0);
    on_trackbar3(alpha_slider3,0);

    cout <<"Press enter to continue";
    while (true) //in while loop to constantly change mask
    {
        inRange(hsv[0], Scalar(h1, s1, 100), Scalar(180, 255, 255), mask);
        inRange(hsv[0], Scalar(0, s1, 100), Scalar(h2, 255, 255), mask2); //change mask with slider values
        mask = mask|mask2;
        erode(mask,mask,Mat(),Point(-1,-1),1);
        dilate(mask,mask,Mat(),Point(-1,-1),3); //erode and dilate to remove noise
        dilate(mask, mask, Mat(), Point(-1, -1), 5);
        erode(mask, mask, Mat(), Point(-1, -1), 5);// dilate and erode to connect blobs
        res = mask;
        img[0].copyTo(res,mask); //apply mask to original img
        imshow("Sliders",res); //show result
        int key = waitKey(10);

        if (key == 13) //break loop on enter
        {
            break;
        }
    }
}


