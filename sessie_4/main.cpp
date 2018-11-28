#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>   //project->linkersettings -> other -> `pkg-config opencv --libs`

using namespace std;
using namespace cv; //in this order

int kptype = 0;
int slider_kptype = 0;

Mat img[4],res;

void keyPointMatching(Mat obj,Mat img);
void bruteForceMatching(Mat obj,Mat img);
void ranSacMatching(Mat obj,Mat img);

static void on_trackbar1(int, void*)
{
    kptype = slider_kptype; //set the global variable kptype to the current slidervalue;
}





int main(int argc,const char** argv)
{
    CommandLineParser parser(argc,argv,
    "{help h usage ?   |  |show this message }"
    "{image_1 i1       |  |(required) absolute path to first image }"
    "{image_2 i2       |  |(required) absolute path to second image }"
    "{image_3 i3       |  |(required) absolute path to third image }"
    "{image_4 i4       |  |(required) absolute path to third image }"
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

    if(image_1_location.empty() || image_2_location.empty() || image_3_location.empty() || image_4_location.empty()){
        cerr << "wrong arg" << endl;
        parser.printMessage();
        return -1;
    }
    ///check given arguments


    img[0] = imread(image_1_location); //bueno obj
    img[1] = imread(image_2_location); //bueno img
    img[2] = imread(image_3_location); //fitness obj
    img[3] = imread(image_4_location); //fitness img
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

    //keyPointMatching(img[0],img[1]);
    //bruteForceMatching(img[0],img[1]);
    ranSacMatching(img[0],img[1]);

}

void keyPointMatching(Mat obj,Mat img)
{
    namedWindow("Keypoints of image",WINDOW_AUTOSIZE); // Create Window
    createTrackbar("ORB|BRISK|AKAZE", "Keypoints of image", &slider_kptype, 2, on_trackbar1);
    vector<KeyPoint> kp1,kp2;
    Mat img_res = img.clone(),obj_res = obj.clone();
    Mat des1,des2;
    while(1)
    {

        switch(kptype){
            case 0:
            {
                Ptr<ORB> orb = ORB::create();
                orb->detectAndCompute(obj,Mat(),kp1,des1);
                orb->detectAndCompute(img,Mat(),kp2,des2);
                drawKeypoints(obj,kp1,obj_res);
                drawKeypoints(img,kp2,img_res);
                break;
            }

            case 1:
            {
                Ptr<BRISK> brisk = BRISK::create();
                brisk->detectAndCompute(obj,Mat(),kp1,des1);
                brisk->detectAndCompute(img,Mat(),kp2,des2);
                drawKeypoints(obj,kp1,obj_res);
                drawKeypoints(img,kp2,img_res);
                break;
            }
            case 2:
            {
                Ptr<AKAZE> akaze = AKAZE::create();
                akaze->detectAndCompute(obj,Mat(),kp1,des1);
                akaze->detectAndCompute(img,Mat(),kp2,des2);
                drawKeypoints(obj,kp1,obj_res);
                drawKeypoints(img,kp2,img_res);
                break;
            }
            default: break;

        }
        imshow("Keypoints of image",img_res);
        imshow("Keypoints of image",obj_res);
        int k = waitKey(10);
        if(k == 27)
        {
            destroyAllWindows();
            break;
        }

    }

}

void bruteForceMatching(Mat obj,Mat img)
{
    vector<KeyPoint> kp1,kp2;
    Mat res;
    Mat des1,des2;

    Ptr<ORB> orb = ORB::create();
    orb->detectAndCompute(obj,Mat(),kp1,des1);
    orb->detectAndCompute(img,Mat(),kp2,des2);

    BFMatcher bfm = BFMatcher(NORM_L2);
    std::vector<DMatch> matches;

    bfm.match(des1,des2,matches);
    drawMatches(obj,kp1,img,kp2,matches,res);

    imshow("Matches",res);
    waitKey(0);
    destroyAllWindows();
}

void ranSacMatching(Mat obj,Mat img)
{
    vector<KeyPoint> kp1,kp2;
    Mat res;
    Mat des1,des2;

    Ptr<ORB> orb = ORB::create();
    orb->detectAndCompute(obj,Mat(),kp1,des1);
    orb->detectAndCompute(img,Mat(),kp2,des2);

    BFMatcher bfm = BFMatcher(NORM_L2);
    std::vector<DMatch> matches;

    bfm.match(des1,des2,matches);

    sort(matches.begin(),matches.end()); //sort matches by distance
    matches.erase(matches.begin()+matches.size()*0.15, matches.end()); // only keep matches


    drawMatches(obj, kp1, img, kp2, matches, res);

    vector<Point2f> objectLoc;
    vector<Point2f> inputLoc;

    for(int i=0; i < matches.size(); i++)
    {
        objectLoc.push_back(kp1[matches[i].queryIdx].pt);
        inputLoc.push_back(kp2[matches[i].trainIdx].pt);
    }

    Mat H = findHomography(objectLoc, inputLoc, RANSAC);

    // Find the corners of the object image
    vector<Point2f> sceneCorners(4),objectCorners(4);

    objectCorners[0] = cvPoint(0,0);
    objectCorners[1] = cvPoint(obj.cols, 0 );
    objectCorners[2] = cvPoint(obj.cols, obj.rows);
    objectCorners[3] = cvPoint(0, obj.rows);


    // Transform to scene
    perspectiveTransform(objectCorners, sceneCorners, H);

    // Draw a box around the object in the scene
    RNG rng(12345);
    Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
    line(res, sceneCorners[0] + Point2f(obj.cols, 0), sceneCorners[1] + Point2f(obj.cols, 0), color, 4);
    line(res, sceneCorners[1] + Point2f(obj.cols, 0), sceneCorners[2] + Point2f(obj.cols, 0), color, 4);
    line(res, sceneCorners[2] + Point2f(obj.cols, 0), sceneCorners[3] + Point2f(obj.cols, 0), color, 4);
    line(res, sceneCorners[3] + Point2f(obj.cols, 0), sceneCorners[0] + Point2f(obj.cols, 0), color, 4);


    imshow("Matches",res);
    waitKey(0);
    destroyAllWindows();

}

