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

//functions
Mat detectBoard(Mat img);
//float findBoardRotation(vector<Point2f> corners)

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
        hconcat(frame,board,concat);
        imshow("Video stream",concat);
    }


}


Mat detectBoard(Mat img){
    vector<Point2f> corners;
    Mat detected = img.clone();
    //threshold(detected, detected, 10, 255, 0);
    cvtColor(detected,detected,COLOR_RGB2GRAY);
        if(!findChessboardCorners(detected,cbsize,corners,fcbflags)){
            putText(img,"No board detected",Point(50,50),1, 3,Scalar(0,0,255),3);
            return img;
        }

    //drawChessboardCorners(detected, cbsize, Mat(corners),1);  //draw points on the 49 inner corners

    cvtColor(detected,detected,CV_GRAY2BGR);
    for(int i=0;i<6;i++){
        for(int j=0;j<6;j++){
            int e = j%2;
            line(detected,corners[i*7+j],corners[i*7+j+1],Scalar(255*(1-e),0,255*e),3);
            line(detected,corners[i*7+j],corners[i*7+j+7],Scalar(255*(1-e),0,255*e),3);
            line(detected,corners[i*7+j+1],corners[i*7+j+8],Scalar(255*(1-e),0,255*e),3);
            line(detected,corners[i*7+j+7],corners[i*7+j+8],Scalar(255*(1-e),0,255*e),3);
        }
    }

    //float angle = findBoardRotation(corners);

    return detected;
    ///found at https://docs.opencv.org/3.4.5/d9/d0c/group__calib3d.html#ga93efa9b0aa890de240ca32b11253dd4a
}

/*float findBoardRotation(vector<Point2f> corners){
    float angle =
}
*/
