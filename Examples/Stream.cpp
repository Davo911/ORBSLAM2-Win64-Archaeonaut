#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

using namespace cv;
using namespace std;
//#include <ffmpeg/avformat.h>
//#include <ffmpeg/avcodec.h>
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main( int argc, char** argv )
{
    /*if( argc != 2)
    {
        cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
        return -1;
    }

    Mat image;
    //image = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file

    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

*/
    Mat image;
    namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
    //imshow( "Display window", image );                   // Show our image inside it.


   /* VideoCapture cap("udp://:8554");
    waitKey(0);
    while (true)
    {

        cap >> image;
        if (image.rows != 0)
            imshow( "Display window", image );
        waitKey(0);                                          // Wait for a keystroke in the window

    }*/


    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,
                       (struct sockaddr *) &cli_addr,
                       &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");
    bzero(buffer,256);
    string xxx;
    while (true) {
        n = read(newsockfd, buffer, 255);
        if (n < 0) error("ERROR reading from socket");
        printf("Here is the message: %s\n", xxx);
    }
    n = write(newsockfd,"I got your message",18);
    if (n < 0) error("ERROR writing to socket");
    close(newsockfd);
    close(sockfd);
    return 0;




}























/*
int main(int argc, char** argv)
{
    namedWindow("frame");
    boost::thread capThread {captureFunc};
    boost::thread keepAliveThread {keepAlive};
    for (;;)
    {
        boost::unique_lock<boost::mutex> lock(mtx);
        while (!newFrame)
        {
            cond.wait(lock);
        }
        imshow("frame", frame);
        newFrame = false;
        if (27 == waitKey(10)) break; // the thread sleeps for longer
    }

    keepAliveThread.interrupt();
    cap->release();
    capThread.join();
    return 0;
}
#include <cstdlib>
#include <pthread.h>

using namespace std;

#define NUM_THREADS 5

void *GoProStream(void *threadid) {
    system("python GoProStream.py");
    pthread_exit(NULL);
}

int main () {
    pthread_t threads[NUM_THREADS];
    int rc;
   // rc = pthread_create(&threads[0], NULL, GoProStream, (void *)0);



    cout << "STart" << endl;
//VideoCapture capture(0);
    VideoCapture capture("udp://:8554");
//VideoCapture capture("input.mp4");


// create mat to fill by external source
    Mat frame;

    for(;;)

    {
        bool OK = capture.grab();

        if (OK == false){
            cout << "cannot grab" << endl;
        }
        else{
            // retrieve a frame of your source
            cout << "grab" << endl;
            //capture.read(frame);
            //OR
            // capture >> frame;



            capture >> frame;
            if( frame.empty() ) break; // end of video stream
            imshow("this is you, smile! :)", frame);
        }
    }


    return 1;



}

*/
