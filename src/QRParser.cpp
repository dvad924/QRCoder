#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include "./QRParser.h"
#include <vector>
#include <math.h>


namespace qr{

  QRParser::QRParser(cv::Mat qrcode){
    _qr = cv::Mat::zeros(qrcode.size(),CV_8UC1);
    cv::threshold(qrcode,_qr,
                  128,255,cv::THRESH_BINARY_INV);
  }

  QRParser::~QRParser(){
    _qr.release();
  }

  void QRParser::findPixelShape(){
    int width = 0;
    int height = 0;
    int i = 0;
    uchar v;
    while ((v=_qr.at<uchar>(5,i)) ==0){
      width++;
      i++;
      std::cout<<i<<std::endl;
    }
    std::cout << v+0 << std::endl;
    i = 0;

    while ((v = _qr.at<uchar>(i,5)) == 0){
      height++;
      i++;
    }
    
    _pwidth=width/7.0 ;
    _pheight=height/7.0 ;
    std::cout << width << " " << height << std::endl;

  }
  
  void QRParser::descretizeQR(){
    findPixelShape();
    
    double mindist = 100;
    double bestlen = 0;
    for(int v = 1; v < 40; v++){
      int len = (v-1)*4 + 21;
      double size = (_qr.cols/_pwidth + _qr.rows/_pheight)/2;
      double dist = std::abs(len - size);

      if( dist < mindist ){
        mindist = dist;
        bestlen = len;
      }
    }

    double pixelwidth = _qr.cols/bestlen;
    double pixelheight= _qr.rows/bestlen;
    std::cout << pixelwidth << " , " << pixelheight << std::endl;
    //descend the cross section of find
    // a good center of the white
    // for ( double i = 0; i< _qr.cols; i+=pixelwidth)
    //   _qr.col(floor(i)) = cv::Scalar(0);
    // for (double i = 0; i< _qr.rows; i+=pixelheight)
    //   _qr.row(floor(i)) = cv::Scalar(0);
    using namespace std;
    cv::Mat code = cv::Mat::zeros(bestlen,bestlen,CV_8UC1);
    for (int i = 0; i < bestlen; i++){
      for(int j = 0; j< bestlen; j++){
        int y1 = floor(i*pixelheight);
        int x1 = floor(j*pixelwidth);

        cv::Rect region(x1,y1,pixelwidth,pixelheight);
        cout << region <<endl; 
        code.at<uchar>(i,j) = cv::sum( _qr(region) )[0];
      }
    }
  }
};
