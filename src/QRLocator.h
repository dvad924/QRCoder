#include <opencv2/opencv.hpp>
#include <vector>

namespace qr{
class QRLocator {

  cv::Mat img;
  cv::Mat patch;
  std::vector<cv::Point> finders;
  
 public:
  QRLocator(cv::Mat i);
  ~QRLocator();
  void convertBW();
  void locateFinders();
  void createPatchFrame();
  cv::Mat getImage();
  cv::Mat getQRFrame();
};
};
