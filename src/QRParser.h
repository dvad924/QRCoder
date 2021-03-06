#include <opencv2/opencv.hpp>
#include <vector>


namespace qr{
  class QRParser {
    cv::Mat _qr;
    double _pheight;
    double _pwidth;
    cv::Mat _code;
  public:
    QRParser(cv::Mat qrcode);
    ~QRParser();
    void descretizeQR();
    void findPixelShape();
    cv::Mat getCode();
  };
};
