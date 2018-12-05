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

void mClick(int met,int x,int y,int flags,void *);
void knear(Mat traindata, Mat labels,Mat samples,Mat hsv,Mat img);
void bayes(Mat traindata, Mat labels,Mat samples,Mat img);
void svm(Mat traindata, Mat labels,Mat samples,Mat img);


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
    ///Set mouse callback function so the user can select positive pixels

    p = 1;
    ///change global variable to change where the selected points are stored

    namedWindow("Select background (any key to continue)",WINDOW_AUTOSIZE);
    setMouseCallback("Select background (any key to continue)",mClick,0);
    imshow("Select background (any key to continue)",img[0]);
    waitKey();
    destroyAllWindows();
    ///Set mouse callback function so the user can select positive pixels

    Mat hsv = img[0].clone();
    cvtColor(hsv, hsv, COLOR_BGR2HSV);  //convert to HSV
    GaussianBlur(hsv, hsv,Size(5,5),0);     //blur the image to ensure no wrong pixels can be selected by mistake


    Mat traindata(pixels[0].size()+pixels[1].size(), 3, CV_32FC1);
    Mat labels(pixels[0].size()+pixels[1].size(), 1, CV_32SC1);

    if(pixels[0].size() == 0 || pixels[1].size() == 0)
    {
        cout<<"No pixels selected, program will exit.."<<endl;
        return -1;
    }
    ///exit if no pixels are selected

    for (int i = 0; i < pixels[0].size(); i++)
    {

        Vec3b hsv_values = hsv.at<Vec3b>(pixels[0][i].y, pixels[0][i].x);
        traindata.at<float>(i, 0) = hsv_values.val[0];
        traindata.at<float>(i, 1) = hsv_values.val[1];
        traindata.at<float>(i, 2) = hsv_values.val[2];
        labels.at<int>(i) = 1;
    }
    ///use the positive values as training data with label 1

    for (int i = 0; i < pixels[1].size(); i++)
    {

        Vec3b hsv_values = hsv.at<Vec3b>(pixels[1][i].y, pixels[1][i].x);

        traindata.at<float>(i, 0) = hsv_values.val[0];
        traindata.at<float>(i, 1) = hsv_values.val[1];
        traindata.at<float>(i, 2) = hsv_values.val[2];
        labels.at<int>(i) = 0;
    }
    ///use the negative values as training data with label 0
    cout << traindata.size() << endl;
    Mat samples(hsv.rows*hsv.cols, 3, CV_32FC1);
    for (int row = 0; row < hsv.rows; row++)
    {
        for (int col = 0; col < hsv.cols; col++)
        {
            int index = row*hsv.cols + col;

            Vec3b hsv_values = hsv.at<Vec3b>(col, row);
            samples.at<float>(index, 0) = hsv_values.val[0];
            samples.at<float>(index, 1) = hsv_values.val[1];
            samples.at<float>(index, 2) = hsv_values.val[2];
        }
    }
    ///load in all the pixel from the image as sample values
    //knear(traindata,labels,samples,hsv,img[0]);
    //bayes(traindata,labels,samples,img[0]);
    svm(traindata,labels,samples,img[0]);

}



void knear(Mat traindata, Mat labels,Mat samples,Mat hsv,Mat img)
{
    cout<<"executing knn"<<endl;
    Mat values,result;
    Mat mask = Mat::zeros(img.rows,img.cols,CV_8UC1);
    Ptr<ml::KNearest> KNN = ml::KNearest::create();
    KNN->setIsClassifier(true);
    KNN->setAlgorithmType(ml::KNearest::Types::BRUTE_FORCE);
    KNN->setDefaultK(3);
    ///set the needed values
    KNN->train(traindata, ml::SampleTypes::ROW_SAMPLE, labels);
    ///train with the given training data

    KNN->findNearest(samples, KNN->getDefaultK(), values);
    ///now we can try to find all the foreground pixels we want

    for(int r=0; r < img.rows; r++) {
        for(int c=0; c < img.cols; c++) {
            if(values.at<float>(r*img.cols+c) != 0) {
                mask.at<uchar>(r, c) = 255;
            }
        }
    }
    ///construct the mask based on the values we got from the predictor

    erode(mask, mask, Mat(), Point(-1, -1), 1);
    dilate(mask, mask, Mat(), Point(-1, -1), 1);
    dilate(mask, mask, Mat(), Point(-1, -1), 3);
    erode(mask, mask, Mat(), Point(-1, -1), 3);


    /// romve noise and connect blobs using erosion and dilation

    imshow("Mask using knn", mask);
    waitKey();
    destroyAllWindows();
    img.copyTo(result, mask);
    imshow("masked image using knn", result);
    waitKey();
    destroyAllWindows();

    ///show the images

    return;
}

void bayes(Mat traindata, Mat labels,Mat samples,Mat img)
{
    cout<<"executing bayes"<<endl;
    Mat values,result;
    Mat mask = Mat::zeros(img.rows,img.cols,CV_8UC1);
    Ptr<ml::NormalBayesClassifier> bayes = ml::NormalBayesClassifier::create();
    bayes->train(traindata, ml::SampleTypes::ROW_SAMPLE, labels);
    bayes->predict(samples, values);

    for(int r=0; r < img.rows; r++) {
        for(int c=0; c < img.cols; c++) {
            if(values.at<float>(r*img.cols+c) != 0) {
                mask.at<uchar>(r, c) = 255;
            }
        }
    }
    ///construct the mask based on the values we got from the predictor

    erode(mask, mask, Mat(), Point(-1, -1), 1);
    dilate(mask, mask, Mat(), Point(-1, -1), 1);
    dilate(mask, mask, Mat(), Point(-1, -1), 3);
    erode(mask, mask, Mat(), Point(-1, -1), 3);

    imshow("Mask using bayes", mask);
    waitKey();
    destroyAllWindows();
    img.copyTo(result, mask);
    imshow("masked image using bayes", result);
    waitKey();
    destroyAllWindows();

    ///show the images

    return;
}

void svm(Mat traindata, Mat labels,Mat samples,Mat img)
{
    cout<<"executing SVM"<<endl;
    Mat values,result;
    Mat mask = Mat::zeros(img.rows,img.cols,CV_8UC1);
    Ptr<ml::SVM> classifier = ml::SVM::create();
    classifier->setType(ml::SVM::C_SVC);
    classifier->setC(0.1);
    classifier->setKernel(ml::SVM::LINEAR);
    classifier->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, (int)1e7, 1e-6));
    classifier->train(traindata, ml::ROW_SAMPLE, labels);
    classifier->predict(samples, values);

     for(int r=0; r < img.rows; r++) {
        for(int c=0; c < img.cols; c++) {
            if(values.at<float>(r*img.cols+c) != 0) {
                mask.at<uchar>(r, c) = 255;
            }
        }
    }
    ///construct the mask based on the values we got from the predictor

    erode(mask, mask, Mat(), Point(-1, -1), 1);
    dilate(mask, mask, Mat(), Point(-1, -1), 1);
    dilate(mask, mask, Mat(), Point(-1, -1), 3);
    erode(mask, mask, Mat(), Point(-1, -1), 3);

    imshow("Mask using SVM", mask);
    waitKey();
    destroyAllWindows();
    img.copyTo(result, mask);
    imshow("masked image using SVM", result);
    waitKey();
    destroyAllWindows();

    ///show the images
}
