#include "Utils.h"
#include <filesystem>
#include <map>
#include <iostream>
#include "DeviceFactory/CameraCalibration.h"
// #include <windows.h>


std::vector<std::string> Utils::loadImages(const std::string& folderPath)
{
	std::map<int, std::string> filenames;
	for (const auto& entry : std::filesystem::directory_iterator(folderPath))
	{
		if (entry.is_directory() || entry.is_other())
			continue;

		std::string stem = entry.path().stem().string();

		if (!std::isdigit(stem[0]))
			continue;

		int patternId = atoi(stem.c_str());
		filenames[patternId] = entry.path().string();
	}

	std::vector<std::string> paths;
	for (auto pair : filenames)
	{
		std::cout << "Loaded: " << pair.second << std::endl;
		paths.push_back(pair.second);
	}
	return paths;
}

cv::Matx44d Utils::extrinsicFromRt(cv::Matx33d R, cv::Matx31d t)
{
	cv::Matx44d extrinsic = cv::Matx44d::eye();
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			extrinsic(i, j) = R(i, j);
		}
	}
	extrinsic(0, 3) = t(0);
	extrinsic(1, 3) = t(1);
	extrinsic(2, 3) = t(2);

	return extrinsic;
}

void Utils::flip2dPoints(std::vector<cv::Point2f>& points2d, int imgWidth)
{
	for (auto& p : points2d)
	{
		p.x = imgWidth - p.x;
	}
}

bool Utils::readJSONFileToCameraCalibration(const std::string& filename, CameraCalibration& camCalib)
{
	cv::FileStorage fs(filename, cv::FileStorage::READ + cv::FileStorage::FORMAT_JSON);
    if (!fs.isOpened())
    {
        std::cerr << "[CameraCalibration] Error: Could not open the input file " << filename << std::endl;
        return false;
    }

	int m_width, m_height;
	cv::Matx33d m_K;	
	std::vector<double> m_dists;
	double RMS;
	bool m_fishEye;

    fs["cam_width"] >> m_width;
    fs["cam_height"] >> m_height;
    fs["cam_int"] >> m_K;
    fs["cam_dist"] >> m_dists;
    fs["cam_RMS"] >> RMS;
    fs["cam_fisheye"] >> m_fishEye;
    fs.release();

	camCalib.loadCalibration(m_K(0,0), m_K(1,1), m_K(0,2), m_K(1,2), m_dists, m_width, m_height);
	camCalib.setFishEye(m_fishEye);

    return true;
}

void Utils::verifyDirectories(const std::string& filePath)
{
	std::filesystem::path path = filePath;
	path = path.parent_path();
	if (!std::filesystem::exists(path))
	{
		std::filesystem::create_directories(path);
	}
}

// std::vector<MONITORINFO> Utils::getMonitors() {
// 	std::vector<MONITORINFO> monitors;

// 	auto monitorEnumProc = [](HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) -> BOOL {
// 		MONITORINFOEX mi;
// 		mi.cbSize = sizeof(mi);
// 		if (GetMonitorInfo(hMonitor, &mi)) {
// 			auto monitors = reinterpret_cast<std::vector<MONITORINFO>*>(dwData);
// 			monitors->push_back(mi);
// 		}
// 		return TRUE;
// 		};

// 	EnumDisplayMonitors(NULL, NULL, monitorEnumProc, reinterpret_cast<LPARAM>(&monitors));
// 	return monitors;
// }

std::ostream& operator<<(std::ostream& os, const CameraCalibration& camCalib)
{
    os << std::endl << "Camera\n----------------\nIntrinsics: " << std::endl << camCalib.getIntrinsicsMatrix() << std::endl
        << "Distortion coeffs:" << std::endl << "[ " << camCalib.getDistortionParameters()[0] << ", " << camCalib.getDistortionParameters()[1] << ", " << camCalib.getDistortionParameters()[2] << ", " << camCalib.getDistortionParameters()[3] << ", " << camCalib.getDistortionParameters()[4] << " ] " << std::endl;
    return os;
}