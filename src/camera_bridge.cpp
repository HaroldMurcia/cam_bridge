
#include "ros/ros.h"
// %EndTag(ROS_HEADER)%
// %Tag(MSG_HEADER)%
#include "std_msgs/String.h"
// %EndTag(MSG_HEADER)%
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

static const std::string OPENCV_WINDOW = "Image window";

using namespace std;
using namespace cv;
using namespace ros;


int main(int argc, char** argv){
  // Create a VideoCapture object and open the input file
  // If the input is the web camera, pass 0 instead of the video file name
  //VideoCapture cap("tcambin serial=26810384 ! video/x-raw, format=BGRx,width=(int)1280,height=(int)960, framerate=(fraction)30/1 ! videoconvert  ! video/x-raw, format=(string)BGR ! appsink", CAP_GSTREAMER);
  //VideoCapture cap("tcambin serial=26810384 ! video/x-raw, width=1280, height=960, framerate=60/1 ! videoconvert ! appsink", CAP_GSTREAMER);
  VideoCapture cap("v4l2src device=/dev/video6 ! video/x-bayer, format=(string)grbg, width=(int)1280,height=(int)960, framerate=(fraction)60/1  ! bayer2rgb ! videoconvert  ! appsink", CAP_GSTREAMER);
  if(!cap.isOpened())
  {
      cout<<"VideoCapture or VideoWriter not opened"<<endl;
      exit(-1);
  }
  Mat frame;

  // ROS
  ros::init(argc, argv, "image_publisher");
  ros::NodeHandle nh;
  image_transport::ImageTransport it(nh);
  image_transport::Publisher pub = it.advertise("camera/image", 1);
  sensor_msgs::ImagePtr msg;
  //
  ros::Rate loop_rate(60);
  while( nh.ok() ){
    cap.read(frame);
    // If the frame is empty, break immediately
    if (frame.empty())
      break;
    // Display the resulting frame
    //imshow( "Frame", frame );
    //to ROS
    msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame).toImageMsg();
    ros::Time now = ros::Time::now();
    msg->header.frame_id = "camera";
    msg->header.stamp = now;
    //
    pub.publish(msg);
    cv::waitKey(1);
    //
    ros::spinOnce();
    loop_rate.sleep();
  }

  // When everything done, release the video capture object
  cap.release();
  // Closes all the frames
  destroyAllWindows();

  return 0;
}
