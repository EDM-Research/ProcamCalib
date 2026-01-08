#pragma once
#include <string>

class Config
{
public:
	inline static const std::string baseFolderEstimation = "./data/estimation/";
	inline static const std::string cameraCalibrationFolder = baseFolderEstimation + "camCalib/";
	inline static const std::string mirrorCalibrationFolder = baseFolderEstimation + "mirrorCalib/";
	inline static const std::string procamCalibrationFolder = baseFolderEstimation + "procamCalib/";
};

