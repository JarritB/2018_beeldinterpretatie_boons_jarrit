#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>   //project->linkersettings -> other -> `pkg-config opencv --libs`

using namespace std;
using namespace cv; //in this order
const string CASNAMEH = "haarcascade_frontalface_alt.xml";
const string CASNAMEL = "lbpcascade_frontalface_improved.xml";
const int SCALE=1;
bool lbp = 0;
string mode="HAAR";
void detectAndDraw( Mat img, CascadeClassifier cascade,Scalar color);

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
    bool tryflip;
    CascadeClassifier cascadehaar,cascadelbp;
    double scale = 1;
    cascadehaar.load(CASNAMEH);
    cascadelbp.load(CASNAMEL);
    while(1)
    {

        Mat frame;
        // Capture frame-by-frame
        cap[0] >> frame;

        // If the frame is empty, break immediately
        if (frame.empty())
            break;
        if(lbp)
            detectAndDraw( frame.clone(), cascadelbp,Scalar(255,0,0));
        else
            detectAndDraw( frame.clone(), cascadehaar,Scalar(0,0,255));
        // Press  ESC on keyboard to exit
        char c=(char)waitKey(25);
        if(c==27)
            break;
        if(c==13)
        {
            for(int i=0;i<20;i++)
            {
                cap[0] >> frame;
            }
        }
        if(c==32)
            lbp = !lbp;
    }
    ///play video frame by frame source:https://www.learnopencv.com/read-write-and-display-a-video-using-opencv-cpp-python/
    ///for each frame execute frame detection
    ///--differnce between HAAR en LBP: tradoff accuracy for speed

}

void detectAndDraw( Mat img, CascadeClassifier cascade,Scalar color)
{
    double t = 0;
    vector<Rect> faces, faces2;
    vector<int> rejects;
    vector<double> weights;
    Mat gray, smallImg;
    cvtColor( img, gray, COLOR_BGR2GRAY );
    double fx = 1 / SCALE;
    resize( gray, smallImg, Size(), fx, fx, INTER_LINEAR_EXACT );
    equalizeHist( smallImg, smallImg );
    cascade.detectMultiScale( smallImg, faces,
        1.1, 2, 0
        |CASCADE_SCALE_IMAGE,
        Size(30, 30) );
    cascade.detectMultiScale(smallImg, faces, rejects,weights, 1.1, 2,0, Size(), Size(), true);
    for ( size_t i = 0; i < faces.size(); i++)
    {
        Rect r = faces[i];
        Mat smallImgROI;
        Point center;
        int radius;
        double aspect_ratio = (double)r.width/r.height;
        if( 0.75 < aspect_ratio && aspect_ratio < 1.3 )
        {
            center.x = cvRound((r.x + r.width*0.5)*SCALE);
            center.y = cvRound((r.y + r.height*0.5)*SCALE);
            radius = cvRound((r.width + r.height)*0.25*SCALE);
            circle( img, center, radius, color,2);
            putText(img, std::to_string(weights[i]),
                        Point(center.x, center.y-1.2*radius), 1, 2,color,2);

        }
        smallImgROI = smallImg(r);

    }
    if(lbp)
        mode = "LBP";
    else
        mode = "HAAR";
    putText(img, mode,Point(20,20), 1, 2,color,2);
    imshow( "Facedetection (esc:exit | enter:skip 20 frames | space:switch mode)", img );
}

