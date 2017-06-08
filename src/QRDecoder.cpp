#include "QRDecoder.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <bitset>
#define LOC_LEN 5
#define LOC_WHITE 3
#define LOC_MID 2
namespace qr{
  QRDecoder::QRDecoder(cv::Mat code){
    _code = code;
  }

  QRDecoder::~QRDecoder(){
    if(!_code.empty())
      _code.release();
  }

  void QRDecoder::findLocators(){
    cv::Mat locator = cv::Mat::zeros(LOC_LEN,LOC_LEN,CV_8UC1) ;
    locator(cv::Rect(1,1,LOC_WHITE,LOC_WHITE)) = 255;
    locator.at<uchar>(LOC_MID,LOC_MID) = 0;
    for (int r = LOC_MID; r < _code.rows-LOC_MID; r++){
      for (int c = LOC_MID; c < _code.cols-LOC_MID; c++){
        if( cv::sum(_code(cv::Rect(c-LOC_MID,r-LOC_MID,LOC_LEN,LOC_LEN)) - locator)[0] == 0 ){
          _locators.push_back(cv::Point(c,r));
          std::cout << cv::Point(c,r) << std::endl;
        }
      }
    }
  }

  void QRDecoder::defineReserved(){
    //cols,rows = (version-1)*4 + 21
    int vnum = (_code.cols - 21)/4 + 1;
    _reserved_map = cv::Mat::zeros(_code.size(),_code.type());

    if (vnum < 7){
      //upper locator
      _reserved_map(cv::Rect(0,0,9,9)) = 255;
      //right locator
      _reserved_map(cv::Rect(_code.cols-8,0,8,9)) = 255;
      //bottom locator
      _reserved_map(cv::Rect(0,_code.rows-8,9,8)) = 255;
    }else{
      //upper locator
      _reserved_map(cv::Rect(0,0,8,8)) = 255;
      //right locator
      _reserved_map(cv::Rect(_code.cols-8,0,8,8)) = 255;
      //bottom locator
      _reserved_map(cv::Rect(0,_code.rows-8,8,8)) = 255;
      _reserved_map(cv::Rect(_code.cols-11,0,3,6)) = 255;
      _reserved_map(cv::Rect(0,_code.rows-11,6,3)) = 255;
    }
    _reserved_map.at<uchar>(4*vnum + 9,8) = 255;
    //vertical timer
    _reserved_map.col(6) = 255;
    //horizontal timer
    _reserved_map.row(6) = 255;

    findLocators();
    //mark all the locators as reserved points in the code
    for(std::vector<cv::Point>::iterator it = _locators.begin();
        it != _locators.end();
        it++)
      {
        _reserved_map(cv::Rect((*it).x-2,(*it).y-2,5,5)) = 255;
      }
    
  }
  
  void QRDecoder::parseMsg(){
    int maxcol = _code.cols-1;
    int maxrow = _code.rows-1;
    bool up = true;
    cv::Mat display = cv::Mat::zeros(_code.size(),CV_8UC1);
    int c = maxcol;
    defineReserved();

    while(c >= 0){
      if (c == 6)
        c -= 1;  //skip the vertical timing pattern
      for (int k = (up)? maxrow : 0;
           (up)?k>0:(k<(maxrow+1));
           (up)?k--:k++)
      {
        for(int i = 0; i < 2; i++){
          if(_reserved_map.at<uchar>(k,c-i) == 0)
            _bitstring.push_back(_code.at<uchar>(k,c-i)>0);
          // display.at<cv::Vec3b>(k,c-i) = cv::Vec3b(0,255,0);
          // cv::imshow("Alg display",display);
          // cv::waitKey(50);
          // display.at<cv::Vec3b>(k,c-i) = cv::Vec3b(0,0,0);
        }
      }
      
      up = !up; //switch directions for then next pass
      c -= 2;   //bump over to the next column pair
    }
  }
  uchar QRDecoder::getMsg(){
    for(std::vector<bool>::iterator it = _bitstring.begin(); it != _bitstring.end(); it++)
      std::cout << ((*it)?1:0);
    std::cout << std::endl;
        
    return 0;
  }
};
