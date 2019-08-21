#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <string.h>
#include<time.h>

cv::Mat PinP_point(const cv::Mat &srcImg, const cv::Mat &smallImg, const cv::Point2f p0, cv::Point2f p1);
cv::Mat Eyes(const cv::Mat &srcImg, const cv::Mat &smallImg);
cv::Mat Faces(const cv::Mat &srcImg, const cv::Mat &smallImg, const cv::Mat &backimg0);
void myMouseEventHandler(int event, int x , int y , int flags, void *);
cv::CascadeClassifier cascade;
double scale = 4.0;
cv::Mat frame;
cv::VideoCapture cap;
cv::Point2f p0;
cv::Point2f p1;

int main(int argc, char *argv[])
{
  cv::Mat original_image;
  cap.open(0);
  cap >> frame;
  
  cv::namedWindow("back", 1);
 
  cv::Mat gray, smallImg(cv::saturate_cast<int>(frame.rows/scale),
               cv::saturate_cast<int>(frame.cols/scale), CV_8UC1);

   while(1){
     cap >> frame;

     cv::imshow("back", frame);
      int key0 = cv::waitKey(1);
      if(key0 == 'b'){
	cv::imwrite("back.jpg",frame);
	cv::destroyWindow("back");
	break;
      }
      }
   cv::namedWindow("result", 1);
   cv::namedWindow("eyes", 0);
   char *filename = (char *)"back.jpg";
   original_image = cv::imread(filename);
   cv::Mat p_image;

  while(1){
    cap >> frame;
    cv::cvtColor(frame, gray, CV_BGR2GRAY);
    cv::resize(gray, smallImg, smallImg.size(), 0, 0, cv::INTER_LINEAR);
    cv::equalizeHist(smallImg, smallImg);   

    Eyes(frame, smallImg).copyTo(frame);

    Faces(frame, smallImg, original_image).copyTo(frame);
    
    cv::imshow("result", frame);

    int key = cv::waitKey(10);
    if(key == 'q' || key == 'Q'){
      cv::destroyWindow("result");
      cv::destroyWindow("faces");
      cv::destroyWindow("eyes");
        break;
    }
    else if(key = 's') {
      cv::imwrite("puri.jpg",frame);
    }
    
  }

  cv::namedWindow("paint", 1);
  char *filename2 = (char *)"puri.jpg";
  p_image = cv::imread(filename2);
  char *filename3 = (char *)"heart.jpg";
  cv::Mat heart_image;
  heart_image = cv::imread(filename3);
  char *filename4 = (char *)"star.png";
  cv::Mat star_image;
  star_image = cv::imread(filename4);
  // cv::imshow("paint", p_image);
  cv::setMouseCallback("paint", myMouseEventHandler, (void *)&p_image);//コールバック関数の設定
   while(1){
     cv::imshow("paint", p_image);
    int key = cv::waitKey(10);
    if(key == 'q') {
      break;
    }
    if(key == 'a'){
     cv::imwrite("puri0.jpg", p_image);
    break;
    }
    if(key == 's'){
      srand(time(NULL));
      p0.x = rand()%500;
      p0.y = rand()%500;
      p1.x = p0.x + 30.0;
      p1.y = p0.y + 30.0;
      
      cv::resize(p_image, p_image, cv::Size(), 1.0, 1.0);

      PinP_point(p_image, star_image, p0, p1).copyTo(p_image);
      
      
    }
    if(key == 'h'){
      srand(time(NULL));
      p0.x = rand()%500;
      p0.y = rand()%500;
      p1.x = p0.x + 30.0;
      p1.y = p0.y + 30.0;
      
      cv::resize(p_image, p_image, cv::Size(), 1.0, 1.0);

      PinP_point(p_image, heart_image, p0, p1).copyTo(p_image);
    }
   }
 return 0;
}


cv::Mat PinP_point(const cv::Mat &srcImg, const cv::Mat &smallImg, const cv::Point2f p0, cv::Point2f p1){

  cv::Mat dstImg;
  srcImg.copyTo(dstImg); //背景画像の作成

  std::vector<cv::Point2f> src, dst;
  src.push_back(cv::Point2f(0, 0));
  src.push_back(cv::Point2f(smallImg.cols, 0));
  src.push_back(cv::Point2f(smallImg.cols, smallImg.rows));
  dst.push_back(p0);
  dst.push_back(cv::Point2f(p1.x, p0.y));
  dst.push_back(p1);

  cv::Mat mat = cv::getAffineTransform(src, dst);

  cv::warpAffine(smallImg, dstImg, mat, dstImg.size(), CV_INTER_LINEAR, cv::BORDER_TRANSPARENT);
  
  return dstImg;

}
cv::Mat Eyes(const cv::Mat &srcImg, const cv::Mat &smallImg){
  //srcImgにframeを代入する

  std::string cascadeName2 = "/usr/share/opencv/haarcascades/haarcascade_eye.xml";

  if(!cascade.load(cascadeName2)){
    printf("ERROR: cascadeFile not found\n");
    return -1;
  }

  
std::vector<cv::Rect> eyes;

    cascade.detectMultiScale(smallImg, eyes,
			     1.1,
			     3,
			     CV_HAAR_SCALE_IMAGE,
			     cv::Size(10, 10));
  
 for(int i = 0;i < eyes.size();i++){
   
      cv::Mat roi_img2(srcImg, cv::Rect(eyes[i].x * scale, eyes[i].y * scale, eyes[i].width * scale, eyes[i].height * scale));
      cv::Mat eyes_img;
      cv::resize(roi_img2, roi_img2, cv::Size(), 1.0, 1.0);//目のサイズ変更
      cv::imshow("eyes",roi_img2);
      cv::Point2f pe2((eyes[i].x - 10)* scale, (eyes[i].y - 10) * scale);
      cv::Point2f pe3((eyes[i].x + eyes[i].width + 10) * scale, (eyes[i].y + eyes[i].height + 10) * scale );
      PinP_point(srcImg, roi_img2, pe2, pe3).copyTo(srcImg);

      
 }
 return srcImg;
}

cv::Mat Faces(const cv::Mat &srcImg, const cv::Mat &smallImg, const cv::Mat &backimg0){

  std::string cascadeName = "/usr/share/opencv/haarcascades/haarcascade_frontalface_alt.xml"; //Haar-like

   if(!cascade.load(cascadeName)){
    printf("ERROR: cascadeFile not found\n");
    return -1;
    }

  std::vector<cv::Rect> faces;
  cascade.detectMultiScale(smallImg, faces,
			     1.01,
			     3,
			     CV_HAAR_SCALE_IMAGE,
			     cv::Size(30, 30));
 
 for(int i = 0;i < faces.size();i++){
  
   cv::Mat roi_img(srcImg, cv::Rect(faces[i].x* scale, faces[i].y* scale, faces[i].width * scale, faces[i].height * scale));//顔の部分の画像を取り出す
    cv::imshow("faces",roi_img);
    cv::resize(roi_img, roi_img, cv::Size(), 1.0, 1.0);//顔のサイズ変更

    cv::Mat white_img;
    backimg0.copyTo(white_img);

    cv::Point2f p0((faces[i].x + (faces[i].width)*(0.05)) * scale, (faces[i].y + (faces[i].height)*(1.0/2.0)) * scale);//左上に貼り付ける
    cv::Point2f p1((faces[i].x + (faces[i].width)*(0.8)) * scale, (faces[i].y + faces[i].height*(1.2)) * scale);//右下
      cv::Point2f p2((faces[i].x - 10)* scale, (faces[i].y - 10) * scale);
      cv::Point2f p3((faces[i].x + faces[i].width + 10) * scale, (faces[i].y + faces[i].height + 10) * scale);

     
      PinP_point(PinP_point(srcImg, white_img, p2, p3), roi_img, p0, p1).copyTo(srcImg);//顔を貼り付ける
      
 }
 
 return srcImg;
}


void myMouseEventHandler(int event, int x , int y , int flags, void *userdata){
 
  cv::Mat &image = *(cv::Mat *)userdata;

  static bool isBrushDown = false;
  static cv::Point prevPt;
  cv::Point pt(x, y);

  bool isLButtonPressedBeforeEvent = (bool)(flags & CV_EVENT_FLAG_LBUTTON);
  if(isLButtonPressedBeforeEvent && isBrushDown){
    cv::line(image, prevPt, pt, cv::Scalar::all(255), 5, 8, 0);
    cv::imshow("paint", image);
  }

  bool isLButtonPressedAfterEvent = isLButtonPressedBeforeEvent
    ^ ((event == CV_EVENT_LBUTTONDOWN) || (event == CV_EVENT_LBUTTONUP));
  if(isLButtonPressedAfterEvent){
    prevPt = pt;
    isBrushDown = true;
  }else{
    isBrushDown = false;
  }
}
