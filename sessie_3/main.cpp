#include <iostream>
#include <opencv2/opencv.hpp>   //project->linkersettings -> other -> `pkg-config opencv --libs`

using namespace std;
using namespace cv; //in this order
const float PI = 3.14159265359;
const int ASTEP = 10;
bool skip = false;


// Return the rotation matrices for each rotation
// The angle parameter is expressed in degrees!
void rotate(Mat& src, double angle, Mat& dst)
{
    Point2f pt(src.cols/2., src.rows/2.);
    Mat r = getRotationMatrix2D(pt, angle, 1.0);
    warpAffine(src, dst, r, cv::Size(src.cols, src.rows));
}
///used from: https://github.com/OpenCVBlueprints/OpenCVBlueprints/blob/master/chapter_5/source_code/rotation_invariant_detection/rotation_invariant_object_detection.cpp

int main(int argc,const char** argv)
{
    CommandLineParser parser(argc,argv,
    "{help h usage ?   |  |show this message }"
    "{image_1 i1       |  |(required) absolute path to first image }"
    "{image_2 i2       |  |(required) absolute path to second image }"
    "{image_3 i3       |  |(required) absolute path to third image }"
    );

    if(parser.has("help")){
    parser.printMessage();
    return 0;
    }

    string image_1_location(parser.get<string>("image_1"));
    string image_2_location(parser.get<string>("image_2"));
    string image_3_location(parser.get<string>("image_3"));
    ///colect data from arguments

    if(image_1_location.empty() || image_2_location.empty() || image_3_location.empty()){
        cerr << "wrong arg" << endl;
        parser.printMessage();
        return -1;
    }
    ///check given arguments

    Mat img[3];
    img[0] = imread(image_1_location); //template
    img[1] = imread(image_2_location); //straight
    img[2] = imread(image_3_location); //rotated
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
    ///stop if any of the images is read wrongly and return which one

    Mat res,matched=img[1].clone(),amatched=img[1].clone(),rmatched=img[2].clone();

    matchTemplate(img[1],img[0],res,TM_SQDIFF);
    normalize(res,res,0,1,NORM_MINMAX,-1,Mat());

    double minVal,maxVal;
    Point minLoc,maxLoc,matchLoc;

    minMaxLoc( res, &minVal, &maxVal, &minLoc, &maxLoc, Mat() ); //get location of match
    matchLoc = minLoc; //set origin of rectangle
    rectangle(matched, matchLoc, Point( matchLoc.x + img[0].cols , matchLoc.y + img[0].rows),Scalar(0,0,255),3); //draw rectangle
    imshow( "One Matched template", matched );
    waitKey(0);
    destroyAllWindows();
    ///Found one template match

    matchTemplate( img[1], img[0], res, TM_CCOEFF_NORMED);
    normalize(res, res, 0, 1, NORM_MINMAX, -1, Mat());
    Mat thresh;
    threshold(res,thresh,0.92,1,CV_THRESH_BINARY);//we get optimal results for 0.92
    thresh *= 255;
    thresh.convertTo(thresh,CV_8UC1);
    imshow( "Threshold", thresh );
    waitKey(0);
    destroyAllWindows();

    vector<vector<Point>> contours;
    findContours(thresh,contours, RETR_EXTERNAL,CHAIN_APPROX_NONE);
    for (int i = 0; i < contours.size(); i++)
    {
        Rect region = boundingRect(contours[i]); //find bounding rect
        rectangle(amatched, Point(region.x, region.y ),Point(region.x + img[0].cols, img[0].rows + region.y), Scalar(0,0,255), 3);
        //draw rect with bounding rect origin and size of template
    }
    imshow( "All Matched templates", amatched );
    waitKey(0);
    destroyAllWindows();
    ///Found multiple template matches


    vector<Mat> rotated;
    for(int angle=1; angle<=180;angle+= ASTEP)
    {
        Mat rot = img[2].clone();
        rotate(rot,angle,rot);
        rotated.push_back(rot);
    }
    vector<vector<Rect>> detected;
    for(int i=0;i<rotated.size();i++)
    {
        vector<Rect> obj;
        vector<vector<Point>> contours;
        matchTemplate( rotated[i], img[0], res, TM_CCOEFF_NORMED);
        normalize(res, res, 0, 1, NORM_MINMAX, -1, Mat());
        Mat thr;
        inRange(res, 0.999999, 1, thr);
        thr *= 255;
        thr.convertTo(thr,CV_8UC1);

        findContours(thr,contours, RETR_EXTERNAL,CHAIN_APPROX_NONE);
        for (int j = 0; j < contours.size(); j++)
        {
                obj.push_back(boundingRect(contours[j]));
        }
        detected.push_back(obj);
    }
    for(int i=0; i< detected.size();i++)
    {
        vector<Rect> temp = detected[i];
        for(int j=0;j< temp.size();j++)
        {
            Point p1 ( temp[j].x, temp[j].y ); // Top left
            Point p2 ( (temp[j].x + img[0].cols), temp[j].y ); // Top right
            Point p3 ( (temp[j].x + img[0].cols), (temp[j].y + img[0].rows) ); // Down right
            Point p4 ( temp[j].x, (temp[j].y + img[0].rows) ); // Down left

            Mat coordinates = (Mat_<double>(3,4) << p1.x, p2.x, p3.x, p4.x,p1.y, p2.y, p3.y, p4.y,1 , 1 , 1 , 1 );

            Point2f pt(rmatched.cols/2.,rmatched.rows/2.);
            Mat r = getRotationMatrix2D(pt,-(ASTEP*(i+1)),1.0);
            Mat result = r * coordinates;

            Point p1_back, p2_back, p3_back, p4_back;
            p1_back.x=(int)result.at<double>(0,0);
            p1_back.y=(int)result.at<double>(1,0);

            p2_back.x=(int)result.at<double>(0,1);
            p2_back.y=(int)result.at<double>(1,1);

            p3_back.x=(int)result.at<double>(0,2);
            p3_back.y=(int)result.at<double>(1,2);

            p4_back.x=(int)result.at<double>(0,3);
            p4_back.y=(int)result.at<double>(1,3);


            line(rmatched, p1_back, p2_back, Scalar(0,0,255), 3);
            line(rmatched, p2_back, p3_back, Scalar(0,0,255), 3);
            line(rmatched, p3_back, p4_back, Scalar(0,0,255), 3);
            line(rmatched, p4_back, p1_back, Scalar(0,0,255), 3);

        }
        if(!skip)
        {
            imshow( "All Rotated & Matched templates", rmatched );
            int k = waitKey(0);
            if(k == 27)
            {
                skip = true;
            }
            destroyAllWindows();
        }
    }
    imshow( "All Rotated & Matched templates", rmatched );
    waitKey(0);

    /**rotate the image and apply the template match
    Algorithm based on code found at https://github.com/OpenCVBlueprints/OpenCVBlueprints/blob/master/chapter_5/source_code/rotation_invariant_detection/rotation_invariant_object_detection.cpp

    **/
}


