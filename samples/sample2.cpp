//Sample 2: Test opencv
//
#include "opencv_header.h"
using namespace cv;
using namespace std;
int main() {
	VideoCapture capture(0);
	Mat rgb_pic;
	Mat gray_pic;
	Mat gauss_pic;
	Mat edge_pic;
	while (1) {
		// Cap camera's picture
		capture >> rgb_pic;
		rgb_pic.at<Vec3b>(100, 100) = Vec3f(0, 0, 0);
		rgb_pic.at<Vec3b>(100, 101) = Vec3f(0, 0, 0);
		rgb_pic.at<Vec3b>(101, 100) = Vec3f(0, 0, 0);
		rgb_pic.at<Vec3b>(101, 101) = Vec3f(0, 0, 0);
		imshow("RGB pic", rgb_pic);
		// Change to gray picture
		cvtColor(rgb_pic, gray_pic, CV_BGR2GRAY);
		imshow("Gray pic", gray_pic);
		// Gauss filter
    	blur(gray_pic, gauss_pic, Size(3,3));
		imshow("Gauss pre-process pic", gauss_pic);
		// Use Canny to find edges
    	Canny(gauss_pic, edge_pic, 3, 9, 3);
		imshow("Canny pic", edge_pic);
		std::vector<Vec2f> lines;
		// Hough transform
		HoughLines(edge_pic, lines, 1, CV_PI / 180, 150);
		for (Vec2f line : lines) {
			float rho = line[0];
			float theta = line[1];
			cv::Point pt1, pt2;
			double a = cos(theta), b = sin(theta);
			double x0 = a*rho, y0 = b * rho;
			pt1.x = cvRound(x0 + 1000 * (-b));
			pt1.y = cvRound(y0 + 1000 * (a));
			pt2.x = cvRound(x0 - 1000 * (-b));
			pt2.y = cvRound(y0 - 1000 * (a));
			cv::line(edge_pic, pt1, pt2, cv::Scalar(55, 100, 195), 1);
		}
		imshow("Houghline pic", edge_pic);
		cvWaitKey(1);
	}
	return 0;
}