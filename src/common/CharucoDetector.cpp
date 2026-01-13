#include "CharucoDetector.h"

using namespace cv;

CharucoDetector::CharucoDetector(int rowCount, int colCount, aruco::PredefinedDictionaryType dictionaryType, float squareLength, float markerLength)
{
	aruco::DetectorParameters detectorParams = aruco::DetectorParameters();
	aruco::CharucoParameters charucoParams = aruco::CharucoParameters();
	charucoParams.tryRefineMarkers = true;
	
	charucoBoard = std::make_unique<cv::aruco::CharucoBoard>(Size(rowCount, colCount), squareLength, markerLength, aruco::getPredefinedDictionary(dictionaryType));
	charucoDetector = std::make_unique<cv::aruco::CharucoDetector>(*charucoBoard, charucoParams, detectorParams);
}

void CharucoDetector::detectCharucoCorners(Mat gray, std::vector<Point2f>& corners, std::vector<int>& cornerIds)
{
	std::vector<int> markerIds;
	std::vector<std::vector<Point2f>> markerCorners;
	std::vector<std::vector<Point2f>> rejectedImgPoints;

	charucoDetector->detectBoard(gray, corners, cornerIds, markerCorners, markerIds);
}

Size CharucoDetector::getBoardSize()
{
	Size wrongSize = charucoDetector->getBoard().getChessboardSize();
	Size size{ wrongSize.width - 1, wrongSize.height - 1 };
	return size;
}

std::vector<Point3f> CharucoDetector::getObjectPoints()
{
	return charucoDetector->getBoard().getChessboardCorners();
}

void CharucoDetector::getMatchingPoints(const std::vector<Point2f> & charucoCorners, const std::vector<int> & charucoIds,
					   std::vector<cv::Point3f>& objectPoints, std::vector<cv::Point2f>& imagePoints)
{
	charucoBoard->matchImagePoints(charucoCorners, charucoIds, objectPoints, imagePoints);
}