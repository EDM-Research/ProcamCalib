#pragma once
#include <array>
#include <ostream>
#include <opencv2/core.hpp>

class MirrorPlane
{
private:
	cv::Vec4f planeParams;
	cv::Point3f getPointOnPlane();
	void transformPlane(const cv::Matx44f& transformationMatrix);
	cv::Vec4f fitPlane(std::vector<cv::Point3f> points);
	void fromFile(const std::string& fileName);

public:
	MirrorPlane();
	MirrorPlane(const std::string& fileName);
	MirrorPlane(const std::vector<cv::Point3f>& points);

	cv::Vec4f getPlaneParams() const;

	void fromPoints(std::vector<cv::Point3f> points, int iterations = 1000, float inlierThreshold = 0.005);

	std::vector<cv::Point3f> reflectPoints(std::vector<cv::Point3f> points);
	cv::Matx44d reflectPose(cv::Matx44d pose);

	void saveToJSON(const std::string& fileName);
	friend std::ostream& operator<<(std::ostream& os, const MirrorPlane& mp);
};

