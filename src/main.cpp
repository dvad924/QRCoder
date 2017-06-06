#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>
#include "./qr.h"

using namespace std;

int main(){


  cv::Mat image;
  image = cv::imread( "/Users/dvad/Desktop/exampleqr.jpg",1);

  if ( !image.data ){
    cout << "No image data" << endl;
    return -1;
  }
  
  qr::QRLocator qrLoc(image);
  cv::namedWindow("QR Code", cv::WINDOW_AUTOSIZE );
  cv::imshow("QR Code", image);
  
  cv::waitKey(1000);
  qrLoc.convertBW();
  
  qrLoc.locateFinders();

  qrLoc.createPatchFrame();
  return 0;
}
