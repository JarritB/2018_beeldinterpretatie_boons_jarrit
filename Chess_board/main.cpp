#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>   //project->linkersettings -> other -> `pkg-config opencv --libs`

using namespace std;
using namespace cv; //in this order

int main(int argc,const char** argv)
{
    CommandLineParser parser(argc,argv,
    "{help h usage ?   |  |show this message }"
    "{video source s   |  |(required) absolute path to recorded video or empty for capturing device}"
    );

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
            cout << "Default camera not found, exiting..." << endl;
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



    while(1)
    {

        Mat frame;
        // Capture frame-by-frame
        cap >> frame;

        // If the frame is empty, break immediately
        if (frame.empty())
            break;
        // Press  ESC on keyboard to exit
        char c=(char)waitKey(25);
        if(c==27)
            break;
        if(c==13)
        {
            for(int i=0;i<20;i++)
            {
                cap >> frame;
            }
        }
        imshow("Vid",frame);
    }
    ///play video frame by frame source:https://www.learnopencv.com/read-write-and-display-a-video-using-opencv-cpp-python/
    ///for each frame execute frame detection
    ///--differnce between HAAR en LBP: tradoff accuracy for speed

}


