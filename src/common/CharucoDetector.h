#pragma once
#include <opencv2/objdetect/aruco_board.hpp>
#include <opencv2/objdetect/charuco_detector.hpp>

class CharucoDetector
{
private:
	std::unique_ptr<cv::aruco::CharucoDetector> charucoDetector;
	std::unique_ptr<cv::aruco::CharucoBoard> charucoBoard;

public:
	CharucoDetector(int rowCount = 8, int colCount = 6, cv::aruco::PredefinedDictionaryType dictionaryType = cv::aruco::DICT_5X5_50, float squareLength = 1.65f, float markerLength = 1.65f/2.0f);

	void detectCharucoCorners(cv::Mat img, std::vector<cv::Point2f>& corners, std::vector<int>& cornerIds);
	cv::Size getBoardSize();
	std::vector<cv::Point3f> getObjectPoints();
	void getMatchingPoints(const std::vector<cv::Point2f> & charucoCorners, const std::vector<int> & charucoIds,
					   std::vector<cv::Point3f>& objectPoints, std::vector<cv::Point2f> & imagePoints);
};

