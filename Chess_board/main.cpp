#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>   //project->linkersettings -> other -> `pkg-config opencv --libs`
#include <algorithm>

using namespace std;
using namespace cv; //in this order

//strcuts, global variables and constants
typedef struct tile {   //structure of one tile of the board
    Point2f p1;             // top left corner
    Point2f p2;             // top right corner
    Point2f p3;             // bottom left corner
    Point2f p4;             // bottom right corner
    char piece;             // piece that holds this position
} Tile;

int thresholdValue = 150;
int slider_1 = 150;
float dth = 5;
int nth = 8;
int slider_2 = 1;
const string WNAME = "Finding keypoints, Enter to continue";
const string WNAME2 = "Add (LMB) remove(RMB) continue(ENTER)";
const string WNAME3 = "Select leftmost point (LMB)";
const string WNAME4 = "Select the one right under (LMB)";
Point2f lmp;
Point2f ulmp;
vector<Point2f> lock;
bool changed = false;
bool proceed = false;
bool proceed2 = false;
Rect ROI;
Tile Board[64];


//functions
void detectBoard(Mat img);
vector<Point2f> selectPoints(vector<Point2f> corners);
bool removePoint(int x,int y);
void sortPoints();
bool getPoint(int x,int y);
void initBoard();


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
void mClick2(int met,int x,int y,int flags,void *)
{
        switch(met)
        {
            case EVENT_LBUTTONDOWN:
            {
                if(getPoint(x,y)){
                    if(proceed){
                        proceed2 = true;
                    }
                    else{
                        proceed = true;
                    }
                }
                else{
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
    string pcount;
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
            pcount = std::to_string(lock.size()) + "/81";
            circle(res, lock[i],4, Scalar(100,255,100),-1);
            rectangle(res, ROI, Scalar(255,100,100),2);
            putText(res,pcount ,Point(50,50), 1, 2,Scalar(100,255,100),1);
        }
        imshow(WNAME,res);
        c = (char)waitKey(25);
        if(c==27){
            cout << "Escape was pressed, program exitted.."  <<endl;
            exit(0);
        }
        else if(c == 13 && lock.size() > 70){
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
                pcount = std::to_string(lock.size()) + "/81";
                circle(res, lock[i],4, Scalar(100,255,100),-1);
                rectangle(res, ROI, Scalar(255,100,100),2);
                putText(res,pcount ,Point(50,50), 1, 2,Scalar(100,255,100),1);
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
    namedWindow(WNAME3,WINDOW_AUTOSIZE);
    setMouseCallback(WNAME3,mClick2,0);
    imshow(WNAME3,res);
    while(1){
        c = (char)waitKey(25);
        if(c==27){
            cout << "Escape was pressed, program exitted.."  <<endl;
            exit(0);
        }
        if(proceed){
            circle(res,lmp,4, Scalar(0,0,255),-1);
            destroyAllWindows();
            break;
        }

    }
    namedWindow(WNAME4,WINDOW_AUTOSIZE);
    setMouseCallback(WNAME4,mClick2,0);
    imshow(WNAME4,res);
    while(1){
        c = (char)waitKey(25);
        if(c==27){
            cout << "Escape was pressed, program exitted.."  <<endl;
            exit(0);
        }
        if(proceed2){
            break;
        }
    }

    sortPoints();
    res = frame.clone();
    for( int i = 0; i < lock.size(); i++ ){
            putText(res,std::to_string(i),lock[i],1,1,Scalar(100,255,100),1);
    }
    initBoard();

    res = frame.clone();
    /*
    for(int i=0;i<64;i++)
    {
        line(res,Board[i].p1,Board[i].p2,Scalar(255,255,255));
        line(res,Board[i].p1,Board[i].p3,Scalar(255,255,255));
        line(res,Board[i].p2,Board[i].p4,Scalar(255,255,255));
        line(res,Board[i].p3,Board[i].p4,Scalar(255,255,255));
    }
    */
    imshow("Tiles created",res);
    waitKey(0);



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
    ROI = boundingRect(big_blob);
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
    Point2f p;
    for(int i=0;i<lock.size();i++){
        p = lock[i];
        if((p.x-20 < x && x < p.x+20) && (p.y-20 < y && y < p.y+20)){
            lock.erase(lock.begin() + i);
            return true;
        }
    }
    return false;
}

void sortPoints(){
    ///using a sliding windows detect order of points from L to R and T to B
    vector<Point2f> temp;
    int sw = (ulmp.y-lmp.y)/2;

    int xmin = ROI.tl().x + sw;
    int xmax = xmin + ROI.width - sw;
    int ymin = ROI.tl().y + sw;
    int ymax = ymin + ROI.height - sw;
    Point2f p;

    for(int y=ymin;y<ymax;y += sw){
        for(int x=xmin;x<xmax;x += sw){
            for(int i=0;i<lock.size();i++){
                p = lock[i];
                if((x-sw < p.x && p.x < x+sw) && (y-sw < p.y && p.y < y+sw)){
                    temp.push_back(p);
                    lock.erase(lock.begin() + i);
                }
            }
        }
    }
    cout << "Points sorted" << endl;
    lock = temp;
}
bool getPoint(int x,int y){
    Point2f p;
    for(int i=0;i<lock.size();i++){
        p = lock[i];
        if((p.x-20 < x && x < p.x+20) && (p.y-20 < y && y < p.y+20)){
            if(!proceed){
                lmp = p;
            }
            else{
                ulmp = p;
            }
            return true;
        }
    }
    return false;
}

void initBoard(){
    int n = 0;
    for(int i=0;i<lock.size() - 9;i++){
        if(i%8 == 0 && i != 0){
            continue;
        }
        Board[n].p1 = lock[i];
        Board[n].p2 = lock[i+1];
        Board[n].p2 = lock[i+8];
        Board[n].p2 = lock[i+9];
        n++;
    }
    cout << "Board initiated" << endl;
}
