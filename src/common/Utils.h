#pragma once
#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include "DeviceFactory/CameraCalibration.h"
#include <ostream>
// #include <Windows.h>

class Utils
{
public:
	static std::vector<std::string> loadImages(const std::string& folderPath);
	static cv::Matx44d extrinsicFromRt(cv::Matx33d R, cv::Matx31d t);
	static void flip2dPoints(std::vector<cv::Point2f>& points2d, int imgWidth);
	static void verifyDirectories(const std::string& filepath);
	static bool readJSONFileToCameraCalibration(const std::string& filename, CameraCalibration& camCalib);

	// static std::vector<MONITORINFO> getMonitors();
};

std::ostream& operator<<(std::ostream& os, const CameraCalibration& camCalib);