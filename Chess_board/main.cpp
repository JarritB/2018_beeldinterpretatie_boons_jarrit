#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>   //project->linkersettings -> other -> `pkg-config opencv --libs`

using namespace std;
using namespace cv; //in this order

//global variables and constants
const int fcbflags = CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK;
const Size cbsize = Size(7,7);
const Scalar black = Scalar(0,0,0);
const Scalar white = Scalar(255,255,255);
int thresholdValue = 150;
int slider_th = 150;

//functions
Mat detectBoard(Mat img);

static void on_trackbar1(int, void*)
{
    thresholdValue = slider_th; //set the global variable h1 to the current slidervalue;
}

int main(int argc,const char** argv)
{
    CommandLineParser parser(argc,argv,
    "{help h usage ?   |  |show this message }"
    "{video source s   |  |(required) absolute path to recorded video or empty for capturing device}"
    );
    ///chess matches can be found at: http://www.chessgames.com/perl/chess.pl?page=1&playercomp=either&year=2018&yearcomp=ge&moves=30&movescomp=le
    if(parser.has("help")){
    parser.printMessage();
    return 0;
    }


    string source_location(parser.get<string>("source"));
    ///colect data from arguments
    VideoCapture cap;
    if(source_location.empty()){
        cout << "Using default camera as video source." << endl;
        cap = VideoCapture(1);
        /// open the default camera
        if(!cap.isOpened()){
            cerr << "Default camera not found, exiting..." << endl;
            return -1;
        }
        /// check if we succeeded

    }
    else{
        cap = VideoCapture(source_location);
        ///read video in from given path
        if(!cap.isOpened()){
            cerr <<"Video recording not found, check the file path...";
            return -1;
        }
        ///check if we succeeded
    }
    Mat frame,board,concat;
    namedWindow("Detect chess board");
    createTrackbar("Threshold", "Detect chess board", &slider_th, 250, on_trackbar1);
    while(1)
    {
        cap >> frame;                   // Advance frame

        if (frame.empty())              // If the frame is empty, break immediately
            break;
        board = detectBoard(frame.clone()
        );
        char c=(char)waitKey(25);
        if(c==27)                       // Press  ESC on keyboard to exit
            break;
        //hconcat(frame,board,concat);
        imshow("Detect chess board",board);
    }


}


Mat detectBoard(Mat img){
    vector<Point2f> corners;
    Mat detected = img.clone();
    cvtColor(detected,detected,COLOR_RGB2GRAY);
    equalizeHist(detected,detected);
    normalize(detected, detected, 0, 255, NORM_MINMAX,CV_8UC1, Mat());
    threshold(detected, detected, slider_th, 255, 0);
    erode(detected,detected,Mat(),Point(-1,-1),1);
    dilate(detected,detected,Mat(),Point(-1,-1),1); //erode and dilate to remove noise

    Mat mask = Mat::zeros(detected.rows,detected.cols,CV_8UC1);
    Mat res = Mat::zeros(detected.rows,detected.cols,CV_8UC1);
    vector <vector<Point>>contours;
    vector<Point> hull;
    findContours(detected.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    convexHull(contours[0],hull);
    vector<Point> big_blob = hull;
    for (int i = 0; i < contours.size(); i++){                  //using connected componenent analysis to retain only the stop sign
        if (contourArea(contours[i]) > contourArea(big_blob)){
            convexHull(contours[i], hull);
            big_blob = hull;
        }
    }
    vector<vector<Point>> temp;
    temp.push_back((big_blob));
    drawContours(mask, temp, -1, 255, -1);
    detected.copyTo(res,mask); //apply mask to set the region of interest
    detected = res;
    Mat dst = detected.clone();

    if(!findChessboardCorners(dst,cbsize,corners,fcbflags)){               //check for chess board pattern
        putText(img,"No board detected",Point(50,50),1, 3,Scalar(0,0,255),3);
        return img;
    }
    mask = Mat::zeros(detected.rows,detected.cols,CV_8UC1);
    goodFeaturesToTrack(dst,corners,90,0.01,10,Mat(),3,false,0.04 ); //apply Shi-Thomas corner detection
    for( int i = 0; i < corners.size(); i++ ){
        circle(mask, corners[i],3, Scalar(255),-1);
    }


    /*
    normalize( mask, mask, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
    convertScaleAbs(dst, dst);
    cvtColor(dst,dst,CV_GRAY2BGR);
    for( int j = 0; j < dst.rows ; j++ )
    {
        for( int i = 0; i < dst.cols; i++ )
          {
            if( (int) mask.at<float>(j,i) > 150 )
              {
               circle( dst, Point( i, j ), 5,  Scalar(0,0,255));
              }
          }
     }

    //imshow("peip",dst);


    if(!findChessboardCorners(detected,cbsize,corners,fcbflags)){               //finds inner corners based on touching corners of black tiles
        putText(img,"No board detected",Point(50,50),1, 3,Scalar(0,0,255),3);
        return img;
    }
    ///found at https://docs.opencv.org/3.4.5/d9/d0c/group__calib3d.html#ga93efa9b0aa890de240ca32b11253dd4a

    vector<vector<Point2f>> tiles;
    vector<Point2f> tile;
    Point2f p;
    for(int i=0;i<36;i++){
        if(i%5 == 0 && i!=0)
            continue;
        p = corners[i];
        tile.push_back(p);
        p = corners[i+1];
        tile.push_back(p);
        p = corners[i+7];
        tile.push_back(p);
        p = corners[i+8];
        tile.push_back(p);
        tiles.push_back(tile);
        tile.clear();
    }
    cvtColor(detected,detected,CV_GRAY2BGR);
    for(int i=0;i<tiles.size();i++){
        int e = (i%2);
        line(detected,tiles[i][0],tiles[i][1],Scalar(255*(1-e),0,255*e),2);
        line(detected,tiles[i][0],tiles[i][2],Scalar(255*(1-e),0,255*e),2);
        line(detected,tiles[i][1],tiles[i][3],Scalar(255*(1-e),0,255*e),2);
        line(detected,tiles[i][2],tiles[i][3],Scalar(255*(1-e),0,255*e),2);
    }


    */
    return mask;

}

