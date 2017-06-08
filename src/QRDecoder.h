#include <opencv2/opencv.hpp>
#include <vector>

namespace qr{

  class QRDecoder {
    cv::Mat _code;
    cv::Mat _reserved_map;
    std::vector<bool> _bitstring;
    std::vector<cv::Point> _locators;

    void defineReserved();
  public:
    QRDecoder(cv::Mat code);
    ~QRDecoder();
    void findLocators();
    void parseMsg();
    uchar getMsg();
  };
};
