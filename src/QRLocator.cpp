#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include "./QRLocator.h"
#include <vector>
#include <math.h>

int sqrdist(cv::Point p1,cv::Point p2){
  int xd = p2.x - p1.x;
  int yd = p2.y - p1.y;
  return xd*xd + yd*yd;
}

namespace qr{
  QRLocator::QRLocator(cv::Mat i){
    img = i.clone();
  }
  
  QRLocator::~QRLocator(){
    img.release();
  }

  cv::Mat QRLocator::getImage(){
    return img;
  }
  
  void QRLocator::convertBW(){
    cv::threshold(img,img,128,255, CV_THRESH_BINARY_INV);
    cv::cvtColor(img,img, CV_BGR2GRAY);
  }

  void QRLocator::locateFinders(){
    using namespace std;
    //Section to find the QR Finders
    vector<vector <cv::Point> > contours;
    vector<cv::Vec4i> hier;
    cv::findContours(img.clone(), contours, hier, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
    for ( vector< cv::Vec4i > ::iterator it = hier.begin(); it != hier.end(); it++){
      cout << *it << endl;
    }
    int idx = 0;
    vector<int> blockpts;
    cv::Mat dst = cv::Mat::zeros(img.rows,img.cols,CV_8UC3);
    
    cv::cvtColor(img,dst,CV_GRAY2BGR);
    
    for (; idx >= 0; idx = hier[idx][0]){
      int idj = hier[idx][2];
      for(; idj >=0; idj = hier[idj][2]){
        if ( hier[idj][2] >= 0 && hier[idj][3] >= 0){ // if it has a child and a parent
          cout << hier[idj][3] << " ";
          blockpts.push_back(hier[idj][3]); //push back the parent
        }
      }
    }
    cout << endl;
    //End of finding the blockpts
    //Begin Find the corner Finder
    vector <cv::Point> centroids;

    for ( int i = 0; i < 3; i++) {
      int fidx = blockpts[i];
      cv::Point cent;
      int len  = 0;
      for ( vector< cv::Point>::iterator it = contours[fidx].begin(); it != contours[fidx].end(); it++  ) {
        cv::Point p = *it;
        cent.x += p.x;
        cent.y += p.y;
        len += 1;
      }
      cent.x /= len;
      cent.y /= len;
      centroids.push_back(cent);

    }
    
    int maxd =0;
    int maxi =0;
    int noncorner[2];
    for ( int j = 0; j < 3; j++){
      int magd  = sqrdist(centroids[j%3],centroids[(j+1)%3]);
      cout<< "pairings : " << centroids[j%3]  << "," << centroids[(j+1)%3]<<endl;
      cout << magd << endl;
      if ( magd > maxd ){
        cout << "max,curr : " << maxd << "," << magd << endl;
        maxd = magd;
        maxi = j;
        noncorner[0] = j;
        noncorner[1] = (j+1)%3;
      }
    }
    for (int t = 0; t < 2; t++)
      cout<<noncorner[t] <<endl;
    int cornerfinderix,finder1,finder2;
    for(int i = 0; i< 3; i++){
      bool exists = false;
      for(int k=0; k < 2; k++){
        exists = (noncorner[k] == i) || exists;
      }
      if (!exists){
        cornerfinderix = i;
        finder1 = noncorner[0];
        finder2 = noncorner[1];
      }
    }    
    //Found Corner Finder
    //compute the signed angle between the non corner blockpts
    double sangle = atan2(centroids[finder1].y,centroids[finder1].x) -
      atan2(centroids[finder2].y, centroids[finder2].x);

    int bot,right;
    if (sangle > 0){
      bot = finder2;
      right = finder1;
    }else{
      bot = finder1;
      right = finder2;
    }
    cv::Point bot2corn = centroids[cornerfinderix] - centroids[bot];
    cv::Point right2corn = centroids[cornerfinderix] - centroids[right];
      
    // cv::drawContours(dst, contours, blockpts[cornerfinderix] ,cv::Scalar(0,255,0),1,8,hier,0);
    // cv::drawContours(dst, contours, blockpts[finder1] ,cv::Scalar(0,255,0),1,8,hier,0);
    // cv::drawContours(dst, contours, blockpts[finder2] ,cv::Scalar(0,255,0),1,8,hier,0);
    //find the two farthest points in the bottom and right oriented contours
    
    vector < cv::Point > boarders(3);
    int max = 0;
    bot = blockpts[bot];
    right = blockpts[right];
      
    for ( vector<cv::Point>::iterator itbot = contours[bot].begin(); itbot != contours[bot].end(); itbot++){
      for ( vector<cv::Point>::iterator itr = contours[right].begin(); itr != contours[right].end(); itr++){
        int d = sqrdist(*itbot,*itr);
        if ( d > max){
          max = d;
          boarders.at(1) = *itbot;
          boarders.at(2) = *itr;
        }
      }
    }

    //Use vector line equation to find the closest point in
    //corner contour to grab the upper corner
    //boarder1 + v1t = boarder2 + v2t => boarder 
    //use cramers rule to solve linear system
    double diffx = boarders.at(1).x - boarders.at(2).x;
    double diffy = boarders.at(1).y - boarders.at(2).y;
    double detA = bot2corn.x * -right2corn.y - bot2corn.y * -right2corn.x;
    double numbot = diffx* -right2corn.y - diffy*-right2corn.x;
    double numr = bot2corn.x * -diffy - bot2corn.y * diffx;
    double solbot = -numbot/detA;
    double solr = -numr/detA;
    cout << solbot << " "  << bot2corn<< " " << boarders.at(1);
    cout << endl;
    cout << boarders.at(1).x << "," << boarders.at(1).y;
    cv::Point corn = solbot*bot2corn + boarders.at(1);
    int cfi = blockpts[cornerfinderix];
    int min = 0;
    for (vector<cv::Point>::iterator it = contours[cfi].begin(); it != contours[cfi].end(); it++){
      int d = sqrdist(*it,corn);
      if (min == 0){
        min = d;
        boarders.at(0) = *it;
      }
      if ( d < min ){
        min = d;
        boarders.at(0) = *it;
      }
    }
    
    cout << boarders.at(0) << endl;
    cv::circle(dst,boarders.at(1),3,cv::Scalar(255,0,0));
    cv::circle(dst,boarders.at(2),3,cv::Scalar(0,0,255));
    cv::circle(dst,boarders.at(0),3,cv::Scalar(0,255,0));
    finders = boarders;

  }
  void QRLocator::createPatchFrame(){
    patch = cv::Mat(300,300,CV_8UC1);
    cv::Mat warpMat = cv::Mat(2,3,CV_32FC1);
    cv::Point2f srcPoints[3];
    cv::Point2f dstPoints[3];

    cv::Mat temp = cv::Mat::zeros(img.size(),img.type());
    dstPoints[0] = cv::Point2f(0,0);
    dstPoints[1] = cv::Point2f(patch.cols-1,0);
    dstPoints[2] = cv::Point2f(0,patch.rows-1);

    srcPoints[0] = finders.at(0); //corner
    srcPoints[1] = finders.at(1); //bottom
    srcPoints[2] = finders.at(2); //right

    warpMat = cv::getAffineTransform(srcPoints,dstPoints);
    
    cv::warpAffine(img,temp,warpMat,cv::Size(300,300));
    patch = temp.clone();
  }

  cv::Mat QRLocator::getQRFrame(){
    return patch;
  }
}
