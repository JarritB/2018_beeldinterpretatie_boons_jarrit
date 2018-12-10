#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>   //project->linkersettings -> other -> `pkg-config opencv --libs`

using namespace std;
using namespace cv; //in this order
const string CASNAMEH = "haarcascade_frontalface_alt.xml";
const string CASNAMEL = "lbpcascade_frontalface_improved.xml";
const float THRESH = 1.5;
const int SCALE=2;
bool lbp = 0;
string mode="HAAR";
vector<Point> traject;

void detectAndDraw( Mat img, HOGDescriptor hog,Scalar color);

int main(int argc,const char** argv)
{
    CommandLineParser parser(argc,argv,
    "{help h usage ?   |  |show this message }"
    "{video_1 v1       |  |(required) absolute path to first video }"
    "{video_2 v2       |  |(required) absolute path to second video }"
    );

    if(parser.has("help")){
    parser.printMessage();
    return 0;
    }

    string video_1_location(parser.get<string>("video_1"));
    string video_2_location(parser.get<string>("video_2"));
    ///colect data from arguments

    if(video_1_location.empty() || video_2_location.empty()){
        cerr << "wrong arg" << endl;
        parser.printMessage();
        return -1;
    }
    ///check given arguments
    VideoCapture cap[2];
    cap[0] = VideoCapture(video_1_location);
    cap[1] = VideoCapture(video_2_location);

    ///read videos in

    if(!cap[0].isOpened()){
        cerr <<"Video 1 not found";
        return -1;
    }
    if(!cap[1].isOpened()){
        cerr <<"Video 2 not found";
        return -1;
    }
    ///stop if any of the videos is read wrongly and return which one
    HOGDescriptor hog;
    vector<Point> traject;
    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
    while(1)
    {

        Mat frame;
        // Capture frame-by-frame
        cap[1] >> frame;

        // If the frame is empty, break immediately
        if (frame.empty())
            break;
        resize(frame, frame, Size(SCALE*frame.cols, SCALE*frame.rows));
        detectAndDraw(frame,hog,Scalar(0,0,255));
        // Press  ESC on keyboard to exit
        char c=(char)waitKey(25);
        if(c==27)
            break;
        if(c==13)
        {
            for(int i=0;i<20;i++)
            {
                cap[1] >> frame;
            }
        }
        if(c==32)
            lbp = !lbp;
    }
    ///play video frame by frame source:https://www.learnopencv.com/read-write-and-display-a-video-using-opencv-cpp-python/
    ///for each frame execute frame detection
    ///video plays very slowly because the calculations takes a certain amount of time during which the frame wont proceed

}

void detectAndDraw( Mat img, HOGDescriptor hog,Scalar color)
{
    vector<Rect> detections;
    vector<double> weights;


    hog.detectMultiScale(img,detections,weights);
    ///detect the person
    for (int i = 0; i < detections.size(); i++)
    {
        Point center;
        int radius;
        Rect r = detections.at(i);
        if(weights.at(i) >= THRESH)
        {
            center.x = cvRound((r.x + r.width*0.5));
            center.y = cvRound((r.y + r.height*0.5));
            traject.push_back(center);
            putText(img, std::to_string(weights[i]),Point(center.x+r.width, center.y), 1, 2,color,2);
            rectangle(img,r,color,2);
        }

    }
    ///draw the bounding box and show the score

    for(int i=1;i<traject.size();i++)
    {
        line(img,traject.at(i),traject.at(i-1),color,2);
    }
    ///draw a line following the persons trajectory, only works for one person

    imshow( "Person detection (esc:exit | enter:skip 20 frames | space:switch mode)", img );
}

