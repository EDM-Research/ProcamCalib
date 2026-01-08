#pragma once
#include "MirrorPlane.h"
#include "CharucoDetector.h"
#include "DeviceFactory/CameraCalibration.h"
#include <vector>
#include "Config.h"
#include "DeviceFactory/Device.h"

class MirrorCalibrator
{
private:
	std::string imgsFolder;
	std::string camCalibName;

	MirrorPlane mp;
	CharucoDetector detector;
	CameraCalibration camCalib;

	std::vector<cv::Point3f> objp;

	std::vector<cv::Point3f> from2dToCamSpace(std::vector<cv::Point2f> points2d, std::vector<int>& ids);

	std::vector<cv::Point3f> getPlanePointsFull(int debugDelay = -1);
	std::vector<cv::Point3f> getPlanePointsRV(int debugDelay = -1);

	std::vector<cv::Point3f> getPlanePointsFull(std::shared_ptr<DeviceFactory::Device> cam, int debugDelay = -1);
	std::vector<cv::Point3f> getPlanePointsRV(std::shared_ptr<DeviceFactory::Device> cam, int patterns, int debugDelay = -1);

	bool detectFull(cv::Mat img, std::vector<cv::Point3f>& planePoints, int debugDelay = -1);
	bool detectRV(cv::Mat img, std::vector<cv::Point3f>& planePoints, int debugDelay = -1);

public:
	MirrorCalibrator();

	void init(const std::string& recording, const std::string& camCalibPath = "camGT");

	void calibrate(bool debug = false);
	void calibrate(std::shared_ptr<DeviceFactory::Device> cam, int patterns);
	void saveToJSON();
};

