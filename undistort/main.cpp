#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>   //project->linkersettings -> other -> `pkg-config opencv --libs`
#include <algorithm>
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv; //in this order

int main()
{
    int numBoards = 3;
    Size board_sz = Size(7,7);
    VideoCapture capture = VideoCapture(1);
    vector<vector<Point3f>> object_points;
    vector<vector<Point2f>> image_points;
    vector<Point2f> corners;
    int successes=0;
    Mat image;
    Mat gray_image;
    capture >> image;
    vector<Point3f> obj;
    for(int j=0;j<49;j++)
        obj.push_back(Point3f(j/7, j%7, 0.0f));
    while(successes<numBoards)
    {
        cvtColor(image, gray_image, CV_BGR2GRAY);
        bool found = findChessboardCorners(image, board_sz, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

        if(found)
        {
            cornerSubPix(gray_image, corners, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
            drawChessboardCorners(gray_image, board_sz, corners, found);
        }
        imshow("win1", image);
        imshow("win2", gray_image);

        capture >> image;
        int key = waitKey(1);

        if(key==27)
            return 0;

        if(key==' ' && found!=0)
        {
            image_points.push_back(corners);
            object_points.push_back(obj);

            printf("Snap stored!");

            successes++;

            if(successes>=numBoards)
                break;
        }
    }

    Mat intrinsic = Mat(3, 3, CV_32FC1);
    Mat distCoeffs;
    vector<Mat> rvecs;
    vector<Mat> tvecs;
    intrinsic.ptr<float>(0)[0] = 1;
    intrinsic.ptr<float>(1)[1] = 1;
    calibrateCamera(object_points, image_points, image.size(), intrinsic, distCoeffs, rvecs, tvecs);
    //write out matrix
    FileStorage fs("intr.xml",FileStorage::WRITE);
    fs << "intrinsic" << intrinsic;
    FileStorage fs2("dist.xml",FileStorage::WRITE);
    fs2 << "distCoef" << distCoeffs;
    Mat imageUndistorted;
    while(1)
    {
        capture >> image;
        undistort(image, imageUndistorted, intrinsic, distCoeffs);

        imshow("win1", image);
        imshow("win2", imageUndistorted);
        waitKey(1);
    }

///Code found at http://aishack.in/tutorials/calibrating-undistorting-opencv-oh-yeah/
}
