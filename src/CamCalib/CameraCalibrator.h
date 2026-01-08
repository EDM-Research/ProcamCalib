#pragma once
#include <string>
#include <vector>
#include "CharucoDetector.h"
#include "MirrorPlane.h"
#include "DeviceFactory/CameraCalibration.h"
#include "DeviceFactory/Device.h"

class CameraCalibrator
{
private:
	std::string imgsFolder;

	CharucoDetector detector;
	CameraCalibration camCalib;

	std::vector<std::vector<cv::Point2f>> imgPoints;
	std::vector<std::vector<cv::Point3f>> objPoints;

	std::vector<cv::Point3f> objp;

	void init();
	void calibrateInternal(cv::Size camSize);

	bool detectAll(cv::Mat img, bool mirrored, int debug = -1);

public:
	CameraCalibrator();

	void init(const std::string& imgsFolder);

	void calibrate(bool debug = false);
	void calibrate(std::shared_ptr<DeviceFactory::Device> cam, int nrPatterns);
	void saveToJSON();
};

