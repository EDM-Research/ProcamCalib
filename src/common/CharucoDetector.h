#pragma once
#include <opencv2/objdetect/aruco_board.hpp>
#include <opencv2/objdetect/charuco_detector.hpp>

class CharucoDetector
{
private:
	cv::aruco::CharucoDetector charucoDetector;
	cv::aruco::ArucoDetector arucoDetector;

public:
	CharucoDetector(int rowCount = 8, int colCount = 6, cv::aruco::PredefinedDictionaryType dictionaryType = cv::aruco::DICT_5X5_50, float squareLength = 4.43, float markerLength = 2.22);

	void detectCharucoCorners(cv::Mat img, std::vector<cv::Point2f>& corners, std::vector<int>& cornerIds);
	cv::Size getBoardSize();
	std::vector<cv::Point3f> getObjectPoints();
};

