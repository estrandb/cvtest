#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <iostream>
#include <boost/thread.hpp>
#include <stdio.h>

#include "ServoController.h"

#include <string>
#include <vector>

using namespace cv;

int GatherFrames(VideoCapture& cap);
int DetectFaceAndDrawRect();
int DrawFrame();



std::vector<Rect_<int> > faces;
double min_face_size=30;
double max_face_size=70;

int counter = 0;

std::list<Mat> framesIn;
std::list<Mat> framesOut;

int main(int argc, char** argv){

    VideoCapture cap(-1);
    if (!cap.isOpened())
    {
        std::cout << "Cannot open camera" << std::endl;
        return -1;
    }

    cap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
    cap.set(CV_CAP_PROP_FPS, 30);
    CascadeClassifier frontalface = CascadeClassifier("/home/pi/projects/cvtest/classifiers/haarcascade_frontalface_alt2.xml");
    //CascadeClassifier frontalface = CascadeClassifier("/home/pi/projects/cvtest/classifiers/haarcascade_profileface.xml");

    namedWindow("Output",CV_WINDOW_AUTOSIZE);

    ServoController* servoController = new ServoController();
    servoController->MovePanServo(0);
    servoController->MoveTiltServo(0);

    while(1)
    {

        Mat frame;
        bool bSuccess = cap.read(frame);
        if (bSuccess)
        {
            if (counter % 2 == 0)
            {
                frontalface.detectMultiScale(frame,faces,1.2,2,0|CV_HAAR_SCALE_IMAGE, Size(min_face_size,min_face_size), Size(max_face_size,max_face_size));
                for (unsigned int i = 0 ; i < faces.size(); ++i)
                {
                    Rect face = faces[i];
                    min_face_size = faces[0].width*0.8;
                    max_face_size = faces[0].width*1.2;
                    rectangle(frame,Point(face.x, face.y),Point(face.x+face.width, face.y+face.height),Scalar(255,0,0),1,4);
                }
                counter = 0;
            }
            if (faces.empty())
            {
                min_face_size = 30;
                max_face_size = 60;
            }
            counter++;
            imshow("Output", frame);
        }

        if (waitKey(1) == 27)
        {
        break;
        }
    }
    return 0;
}

int GatherFrames(VideoCapture& cap) {
    std::cout << "Gather Frames Thread Started." << '\n';
    while(1)
    {
        Mat frame;
        bool bSuccess = cap.read(frame);
        if (bSuccess)
        {
            //mtx.lock();
            framesIn.push_back(frame);
            //mtx.unlock();
        }

        //cout << "Cannot read a frame from camera" << endl;
        //break;
    }
    return 0;
}

int DetectFaceAndDrawRect() {
    std::cout << "Process Thread Started." << '\n';
    CascadeClassifier frontalface = CascadeClassifier("/home/pi/projects/cvtest/classifiers/haarcascade_frontalface_alt2.xml");
    while(1)
    {
        if (!framesIn.empty())
        {
                Mat frame = framesIn.front();
                framesIn.pop_front();
            //for (std::list<Mat>::iterator it = framesIn.begin() ; it != framesIn.end() ; it++)
            //{
                frontalface.detectMultiScale(frame,faces,1.3,3,0|CASCADE_SCALE_IMAGE, Size(30,30));
                for (unsigned int i = 0 ; i < faces.size(); ++i)
                {
                    Rect face = faces[i];
                    rectangle(frame,Point(face.x, face.y),Point(face.x+face.width, face.y+face.height),Scalar(255,0,0),1,4);
                }

                framesOut.push_back(frame);

            //}
        }
    }
    return 0;
}

int DrawFrame() {
    std::cout << "Draw Frame Thread Started." << '\n';
    while(1)
    {
        if (!framesOut.empty())
        {
            Mat frame = framesOut.front();
            framesOut.pop_front();
            imshow("Output", frame);
        }
    }
    return 0;
}
