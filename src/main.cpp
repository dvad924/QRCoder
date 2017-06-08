#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>
#include "QRLocator.h"
#include "QRParser.h"
#include "QRDecoder.h"
using namespace std;

int main(){


  cv::Mat image;
  image = cv::imread( "/Users/dvad/Desktop/exampleqr4.jpg",1);

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
  qr::QRParser parser(qrLoc.getQRFrame());
  parser.descretizeQR();
  qr::QRDecoder decoder(parser.getCode());
  decoder.parseMsg();
  decoder.getMsg();
  return 0;
}
