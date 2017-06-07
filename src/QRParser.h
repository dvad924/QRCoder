#include <opencv2/opencv.hpp>
#include <vector>


namespace qr{
  class QRParser {
    cv::Mat _qr;
    double _pheight;
    double _pwidth;
  public:
    QRParser(cv::Mat qrcode);
    ~QRParser();
    void descretizeQR();
    void findPixelShape();
    cv::Mat getCode();
  };
};
