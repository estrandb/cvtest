#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <iostream>
#include <boost/thread.hpp>
#include <stdio.h>

#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART

#include <string>
#include <vector>

using namespace std;
using namespace cv;

int GatherFrames(VideoCapture& cap);
int DetectFaceAndDrawRect();
int DrawFrame();

boost::mutex mtx;

vector<Rect_<int> > faces;
double min_face_size=30;
double max_face_size=70;

int counter = 0;

std::list<Mat> framesIn;
std::list<Mat> framesOut;

int main(int argc, char** argv){

    //-------------------------
	//----- SETUP USART 0 -----
	//-------------------------
	//At bootup, pins 8 and 10 are already set to UART0_TXD, UART0_RXD (ie the alt0 function) respectively
	int uart0_filestream = -1;

	//OPEN THE UART
	//The flags (defined in fcntl.h):
	//	Access modes (use 1 of these):
	//		O_RDONLY - Open for reading only.
	//		O_RDWR - Open for reading and writing.
	//		O_WRONLY - Open for writing only.
	//
	//	O_NDELAY / O_NONBLOCK (same function) - Enables nonblocking mode. When set read requests on the file can return immediately with a failure status
	//											if there is no input immediately available (instead of blocking). Likewise, write requests can also return
	//											immediately with a failure status if the output can't be written immediately.
	//
	//	O_NOCTTY - When set and path identifies a terminal device, open() shall not cause the terminal device to become the controlling terminal for the process.
	uart0_filestream = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
	if (uart0_filestream == -1)
	{
		//ERROR - CAN'T OPEN SERIAL PORT
		printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
	}

	//CONFIGURE THE UART
	//The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
	//	Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
	//	CSIZE:- CS5, CS6, CS7, CS8
	//	CLOCAL - Ignore modem status lines
	//	CREAD - Enable receiver
	//	IGNPAR = Ignore characters with parity errors
	//	ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
	//	PARENB - Parity enable
	//	PARODD - Odd parity (else even)
	struct termios options;
	tcgetattr(uart0_filestream, &options);
	options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;		//<Set baud rate
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart0_filestream, TCIFLUSH);
	tcsetattr(uart0_filestream, TCSANOW, &options);

    VideoCapture cap(-1);
    if (!cap.isOpened())
    {
        cout << "Cannot open camera" << endl;
        return -1;
    }

   cap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
   cap.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
   cap.set(CV_CAP_PROP_FPS, 30);
   CascadeClassifier frontalface = CascadeClassifier("/home/pi/projects/cvtest/classifiers/haarcascade_frontalface_alt2.xml");
   //CascadeClassifier frontalface = CascadeClassifier("/home/pi/projects/cvtest/classifiers/haarcascade_profileface.xml");

   namedWindow("Output",CV_WINDOW_AUTOSIZE);

    //boost::thread GatherThread(GatherFrames, cap);
    //boost::thread ProcessThread(DetectFaceAndDrawRect);
    //boost::thread DrawThread(DrawFrame);

    //----- TX BYTES -----
	unsigned char tx_buffer[20];
	unsigned char *p_tx_buffer;

	p_tx_buffer = &tx_buffer[0];
	*p_tx_buffer++ = 255;
	*p_tx_buffer++ = 19;
	*p_tx_buffer++ = 120;


	if (uart0_filestream != -1)
	{
		int count = write(uart0_filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));		//Filestream, bytes to write, number of bytes to write
		if (count < 0)
		{
			printf("UART TX error\n");
		}
	}

    while(1)
    {

        Mat frame;
        bool bSuccess = cap.read(frame);
        if (bSuccess)
        {
            //frontalface.detectMultiScale(frame,faces,1.3,3,0|CASCADE_SCALE_IMAGE, Size(30,30));
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

        //cout << "Cannot read a frame from camera" << endl;
        //break;

        if (waitKey(1) == 27)
        {
        break;
        }
    }
    return 0;
}

int GatherFrames(VideoCapture& cap) {
    cout << "Gather Frames Thread Started." << '\n';
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
    cout << "Process Thread Started." << '\n';
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
    cout << "Draw Frame Thread Started." << '\n';
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
