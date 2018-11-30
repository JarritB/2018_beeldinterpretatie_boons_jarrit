#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>   //project->linkersettings -> other -> `pkg-config opencv --libs`

using namespace std;
using namespace cv; //in this order

int kptype = 0;
int slider_kptype = 0;
vector<Point> pixels[2];
Mat img[2],res;
int p=0;

void mClick(int met,int x,int y,int flags,void *)
{
        switch(met)
        {
            case EVENT_LBUTTONDOWN:
            {
                cout <<"Pixel added" << endl;
                Point temp = Point(x,y);
                pixels[p].push_back(temp);
                break;
            }
            case EVENT_RBUTTONDOWN:
            {
                if(pixels[p].size()==0)
                {
                    cout <<"list is empty" << endl;
                }
                else
                {
                    cout <<"Pixel removed" << endl;
                    pixels[p].pop_back();
                }
                break;
            }
            case EVENT_MBUTTONDOWN:
            {
                if(pixels[p].size()==0)
                {
                    cout <<"list is empty" << endl;
                    break;
                }
                for(int i=0;i<pixels[p].size();i++)
                {
                    cout << "Point at ";
                    cout << pixels[p][i].x;
                    cout << " , ";
                    cout << pixels[p][i].y << endl;
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
    "{image_1 i1       |  |(required) absolute path to first image }"
    "{image_2 i2       |  |(required) absolute path to second image }"
    );

    if(parser.has("help")){
    parser.printMessage();
    return 0;
    }

    string image_1_location(parser.get<string>("image_1"));
    string image_2_location(parser.get<string>("image_2"));
    ///colect data from arguments

    if(image_1_location.empty() || image_2_location.empty()){
        cerr << "wrong arg" << endl;
        parser.printMessage();
        return -1;
    }
    ///check given arguments


    img[0] = imread(image_1_location); //strawberry1
    img[1] = imread(image_2_location); //strawberry2
    ///read images in Matrix var

    if(img[0].empty()){
        cerr <<"Image1 not found";
        return -1;
    }
    if(img[1].empty()){
        cerr <<"Image2 not found";
        return -1;
    }
    ///stop if any of the images is read wrongly and return which one


    namedWindow("Select foreground (any key to continue)",WINDOW_AUTOSIZE);
    setMouseCallback("Select foreground (any key to continue)",mClick,0);
    imshow("Select foreground (any key to continue)",img[0]);
    waitKey();
    destroyAllWindows();

    p = 1;

    namedWindow("Select background (any key to continue)",WINDOW_AUTOSIZE);
    setMouseCallback("Select background (any key to continue)",mClick,0);
    imshow("Select background (any key to continue)",img[0]);
    waitKey();
    destroyAllWindows();







}

