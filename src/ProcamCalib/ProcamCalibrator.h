#pragma once
#include "CharucoDetector.h"
#include "MirrorPlane.h"
#include "Projector.h"
#include "DeviceFactory/CameraCalibration.h"
#include <opencv2/opencv.hpp>
#include "Config.h"
#include "DeviceFactory/Device.h"

class ProcamCalibrator
{
private:
	std::string imgsFolder;
	std::string mirrorCalibName;
	std::string camCalibName;

	CharucoDetector detector;
	MirrorPlane mp;
	Projector* proj;
	CameraCalibration camCalib;

	cv::Ptr<cv::FeatureDetector> circlesDetector;
	
	std::vector <std::vector<cv::Point2f>> imgPointsVirtualProj;
	std::vector<std::vector<cv::Point2f>> imgPointsCamera;
	std::vector<std::vector<cv::Point3f>> objPointsVirtual;

	cv::Matx33d projInt;
	std::vector<double> projDist;
	float projRMS;
	cv::Matx44d cam2Proj;
	cv::Matx44d virtualProj2Cam;
	float stereoRMS;
	int detections;

	std::vector<cv::Point3f> objp;

	int capPerPattern;
	cv::Size circlesGridSize;

	std::vector<cv::Point3f> pointsToBoardSpace(std::vector<cv::Point2f> points2d, std::vector<cv::Point2f> refPoints2d, std::vector<cv::Point3f> objp, cv::Matx33d cameraIntrinsics, std::vector<double> distortionCoeffs);

	bool detectAll(cv::Mat pattern, cv::Mat img, bool mirrored, int debugDelay = -1);
	void calibrateInternal(bool mirrored, const cv::Size& projSize, const cv::Size& camSize);

	void init();

public:
	ProcamCalibrator();

	void init(const std::string& imgsFolder, const std::string& mirrorCalibName, Projector* proj, const std::string& camCalibName = "camGT");
	void init(const std::string& imgsFolder, Projector* proj, const std::string& camCalibName = "camGT");

	void setCapturesPerPattern(int capPerPattern);

	void calibrate(bool debug = false);
	void calibrate(std::shared_ptr<DeviceFactory::Device> physCamera, int capPerPattern);
	void saveToJSON();
};

