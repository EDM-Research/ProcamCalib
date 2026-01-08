#include "MirrorPlane.h"
#include <opencv2/calib3d.hpp>
#include <iostream>
#include <filesystem>
#include <random>
#include "Config.h"
#include "Utils.h"

using namespace cv;

std::ostream& operator<<(std::ostream& os, const MirrorPlane& mp)
{
    os << "{ a: " << mp.planeParams[0] << ", b: " << mp.planeParams[1] << ", c: " << mp.planeParams[2] << ", d: " << mp.planeParams[3];
    return os;
}

Point3f MirrorPlane::getPointOnPlane()
{
    Point3f p = Point3f{ 0,0,0 };
    if (planeParams[3] == 0)
    {
        return p;
    }
    else if (planeParams[0] != 0)
    {
        p.x = -(planeParams[1] * p.y + planeParams[2] * p.z + planeParams[3]) / planeParams[0];
    }
    else if (planeParams[1] != 0)
    {
        p.y = -(planeParams[0] * p.x + planeParams[2] * p.z + planeParams[3]) / planeParams[1];
    }
    else if (planeParams[2] != 0)
    {
        p.z = -(planeParams[0] * p.x + planeParams[1] * p.y + planeParams[3]) / planeParams[2];
    }
    return p;
}

void MirrorPlane::transformPlane(const Matx44f& transformationMatrix)
{
    planeParams = transformationMatrix.t() * planeParams;
    planeParams = normalize(planeParams);
}

Vec4f MirrorPlane::fitPlane(std::vector<cv::Point3f> points)
{
    Point3f centroid = Point3f{ 0,0,0 };

    for (auto& point : points)
    {
        centroid.x += point.x;
        centroid.y += point.y;
        centroid.z += point.z;
    }

    centroid /= (float)points.size();

    for (auto& point : points)
    {
        point -= centroid;
    }

    Mat U, S, Vt;
    Mat pointsMat = Mat(points.size(), 3, CV_32F, points.data());

    SVD::compute(pointsMat, S, U, Vt);

    Vec3f normal = Vt.row(Vt.rows - 1);

    normal = normalize(normal);

    if (normal[0] > 0)
        normal = -normal;

    float d = -centroid.dot(normal);

   return Vec4f(normal[0], normal[1], normal[2], d);
}

void MirrorPlane::fromFile(const std::string& filePath)
{
    FileStorage fs(filePath, FileStorage::READ + FileStorage::FORMAT_JSON);
    if (!fs.isOpened())
    {
        std::cerr << "[MirrorPlane] Error: Could not open the input file " << filePath << std::endl;
        exit(1);
    }

    fs["plane"] >> planeParams;
    fs.release();

    std::cout << "Read plane from file " << filePath << " with params " << planeParams << std::endl;
}

MirrorPlane::MirrorPlane(const std::vector<Point3f>& points)
{
    fromPoints(points);
}

cv::Vec4f MirrorPlane::getPlaneParams() const
{
    return planeParams;
}

void MirrorPlane::fromPoints(std::vector<cv::Point3f> points, int iterations, float inlierThreshold)
{
    int bestInliers = 0;
    Vec4f bestPlane;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, points.size() - 1);

    for (int i = 0; i < iterations; ++i)
    {
        std::vector<Point3f> sample;
        while (sample.size() < 3)
        {
            sample.push_back(points[dis(gen)]);
        }

        Vec4f plane = fitPlane(sample);

        int inliers = 0;
        for (const auto& p : points)
        {
            float distance = abs(plane[0] * p.x + plane[1] * p.y + plane[2] * p.z + plane[3])
                / sqrt(plane[0] * plane[0] + plane[1] * plane[1] + plane[2] * plane[2]);
            if (distance < inlierThreshold)
            {
                ++inliers;
            }
        }

        if (inliers > bestInliers)
        {
            bestInliers = inliers;
            bestPlane = plane;
        }
    }

    if ((float)bestInliers / points.size() < 0.25)
    {
        std::cerr << "[MirrorPlane]: Failed to find a suitable candidate for mirror calibration. Only " << std::setprecision(2) << ((float)bestInliers / points.size()) * 100.0f <<  "% was considered an inlier. Aborting..\n";
        exit(1);
    }

    planeParams = bestPlane;
}

MirrorPlane::MirrorPlane()
{
}

MirrorPlane::MirrorPlane(const std::string& filePath)
{
    fromFile(filePath);
}

std::vector<Point3f> MirrorPlane::reflectPoints(std::vector<Point3f> points)
{
    Point3f planePt = getPointOnPlane();

    for (auto& point : points)
    {
        point -= planePt;
    }

    Mat reflectionMatrix = Mat::eye(4,4,CV_32F);
    Rect roi{ 0,0,3,3 };
    Vec3f firstThree{ planeParams[0], planeParams[1], planeParams[2] };
    reflectionMatrix(roi) = reflectionMatrix(roi) - 2 * firstThree * firstThree.t();
    
    std::vector<Point3f> pointsTransformed;
    perspectiveTransform(points, pointsTransformed, reflectionMatrix);

    for (auto& point : pointsTransformed)
    {
        point -= planePt;
    }

    return pointsTransformed;
}

Matx44d MirrorPlane::reflectPose(Matx44d pose)
{
    Point3f planePt = getPointOnPlane();

    Mat poseMat = Mat(pose);
    Mat posMat = poseMat.col(3).rowRange(0,3);
    Mat planePtMat = Mat(planePt);

    posMat -= planePtMat;

    Mat reflectionMatrix = Mat::eye(4, 4, CV_64F);
    Rect roi{ 0,0,3,3 };
    Vec3d firstThree{ planeParams[0], planeParams[1], planeParams[2] };
    reflectionMatrix(roi) = reflectionMatrix(roi) - 2 * firstThree * firstThree.t();

    Mat reflectedPose = reflectionMatrix * poseMat;

    posMat = reflectedPose.col(3).rowRange(0, 3);
    posMat += planePtMat;

    return reflectedPose;
}

void MirrorPlane::saveToJSON(const std::string& filePath)
{
    Utils::verifyDirectories(filePath);
    FileStorage fs{ filePath, FileStorage::WRITE + FileStorage::FORMAT_JSON };
    if (!fs.isOpened())
    {
        std::cerr << "[MirrorPlane] Error: Could not open the input file " << filePath << std::endl;
        exit(1);
    }

    fs << "plane" << planeParams;
    fs.release();

    std::cout << "Saved plane to file " << filePath << " with params " << planeParams << std::endl;
}


