#include "CharucoDetector.h"

using namespace cv;

CharucoDetector::CharucoDetector(int rowCount, int colCount, aruco::PredefinedDictionaryType dictionaryType, float squareLength, float markerLength) :
	arucoDetector{ aruco::getPredefinedDictionary(dictionaryType) },
	charucoDetector{ aruco::CharucoBoard(Size(rowCount, colCount), squareLength, markerLength, aruco::getPredefinedDictionary(dictionaryType)) }
{
}

void CharucoDetector::detectCharucoCorners(Mat gray, std::vector<Point2f>& corners, std::vector<int>& cornerIds)
{
	std::vector<int> markerIds;
	std::vector<std::vector<Point2f>> markerCorners;
	std::vector<std::vector<Point2f>> rejectedImgPoints;

	arucoDetector.detectMarkers(gray, markerCorners, markerIds, rejectedImgPoints);
	arucoDetector.refineDetectedMarkers(gray, charucoDetector.getBoard(), markerCorners, markerIds, rejectedImgPoints);

	charucoDetector.detectBoard(gray, corners, cornerIds, markerCorners, markerIds);
}

Size CharucoDetector::getBoardSize()
{
	Size wrongSize = charucoDetector.getBoard().getChessboardSize();
	Size size{ wrongSize.width - 1, wrongSize.height - 1 };
	return size;
}

std::vector<Point3f> CharucoDetector::getObjectPoints()
{
	return charucoDetector.getBoard().getChessboardCorners();
}
