#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>   //project->linkersettings -> other -> `pkg-config opencv --libs`

using namespace std;
using namespace cv; //in this order

//global variables and constants
int thresholdValue = 150;
int slider_1 = 150;
float dth = 5;
int nth = 8;
int slider_2 = 2;
const string WNAME = "Finding keypoints, Enter to continue";
const string WNAME2 = "Add (LMB) remove(RMB) continue(ENTER)";
vector<Point2f> lock;
bool changed = 0;



//functions
void detectBoard(Mat img);
vector<Point2f> selectPoints(vector<Point2f> corners);
bool removePoint(int x,int y);

static void on_trackbar1(int, void*)
{
    thresholdValue = slider_1; //set the global variable to the current slidervalue;
}
static void on_trackbar2(int, void*)
{
    nth = (2 + 2*slider_2); //set the global variables to the current slidervalue;
    dth = (float)(5 + slider_2);
}

void mClick(int met,int x,int y,int flags,void *)
{
        switch(met)
        {
            case EVENT_LBUTTONDOWN:
            {
                if(lock.size() >= 81)
                {
                    cout <<"Already 81 points selected" << endl;
                }
                else
                {
                    cout <<"Point added" << endl;
                    Point temp = Point(x,y);
                    lock.push_back(temp);
                    changed = true;
                }
                break;
            }
            case EVENT_RBUTTONDOWN:
            {
                if(removePoint(x,y))
                {
                    cout <<"Point removed" << endl;
                    changed = true;
                }
                else
                {
                    cout <<"No point here" << endl;
                }
                break;
            }
            default:
                break;
        }
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
    Mat frame,board,res;
    char c;
    namedWindow(WNAME);
    createTrackbar("Threshold",WNAME, &slider_1, 250, on_trackbar1);
    createTrackbar("Severity of selection",WNAME, &slider_2, 4, on_trackbar2);
    while(1)
    {
        cap >> frame;                   // Advance frame

        if (frame.empty()){
            cout << "Video ended, program exitted.." <<endl;
            exit(0);
        }              // If the frame is empty, exit immediately
        detectBoard(frame.clone());
        res = frame.clone();
        for( int i = 0; i < lock.size(); i++ ){
            circle(res, lock[i],4, Scalar(100,255,100),-1);
        }
        imshow(WNAME,res);
        c = (char)waitKey(25);
        if(c==27){
            cout << "Escape was pressed, program exitted.."  <<endl;
            exit(0);
        }
        else if(c == 13){
            destroyAllWindows();
            break;
        }
    }
    namedWindow(WNAME2,WINDOW_AUTOSIZE);
    setMouseCallback(WNAME2,mClick,0);
    imshow(WNAME2,res);
    while(1){
        if(changed){
            res = frame.clone();
            for( int i = 0; i < lock.size(); i++ ){
                circle(res, lock[i],4, Scalar(100,255,100),-1);
            }
            imshow(WNAME2,res);
        }
        c = (char)waitKey(25);
        if(c==27){
            cout << "Escape was pressed, program exitted.."  <<endl;
            exit(0);
        }
        if(c == 13){
            if(lock.size() == 81){
                destroyAllWindows();
                break;
            }
            cout << "You need to select the 81 points" << endl;
        }


    }



}


void detectBoard(Mat img){
    vector<Point2f> corners;
    Mat detected = img.clone();
    cvtColor(detected,detected,COLOR_RGB2GRAY);
    equalizeHist(detected,detected);
    normalize(detected, detected, 0, 255, NORM_MINMAX,CV_8UC1, Mat());
    threshold(detected, detected, thresholdValue, 255, 0);
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
    mask = Mat::zeros(detected.rows,detected.cols,CV_8UC1);
    goodFeaturesToTrack(dst,corners,90,0.01,10,Mat(),3,false,0.04 ); //apply Shi-Thomasi corner detection
    lock = selectPoints(corners);
    cvtColor(mask,mask,CV_GRAY2BGR);
    return;

   /*
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

    for(int i=0;i<tiles.size();i++){
        int e = (i%2);
        line(detected,tiles[i][0],tiles[i][1],Scalar(255*(1-e),0,255*e),2);
        line(detected,tiles[i][0],tiles[i][2],Scalar(255*(1-e),0,255*e),2);
        line(detected,tiles[i][1],tiles[i][3],Scalar(255*(1-e),0,255*e),2);
        line(detected,tiles[i][2],tiles[i][3],Scalar(255*(1-e),0,255*e),2);
    }


    */


}

vector<Point2f> selectPoints(vector<Point2f> corners){
    vector<Point2f> res;
    Point2f p;
    int nx,ny;
    float x,y,x2,y2;
    for(int i=0;i<corners.size();i++){
        nx = 0;
        ny = 0;
        p = corners[i];
        x = p.x;
        y = p.y;
        for(int j=0;j<corners.size();j++){
            x2 = corners[j].x;
            y2 = corners[j].y;
            if(abs(x-x2) < dth)
                nx++;
            if(abs(y-y2) < dth)
                ny++;
        }
        if(nx > nth && ny > nth)
            res.push_back(p);
    }
    return res;
}

bool removePoint(int x,int y){
    cout << x << endl;
    cout << y << endl;
    Point2f p;
    for(int i=0;i<lock.size();i++){
        p = lock[i];
        if((p.x-20 < x && x < p.x+20) && (p.y-20 < y && y < p.y+20)){
            lock.erase(lock.begin() + i);
            cout << i << endl;
            cout << p.x-20 << endl;
            cout << p.y << endl;
            return true;
        }
    }
    return false;
}
