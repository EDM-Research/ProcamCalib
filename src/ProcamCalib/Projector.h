#pragma once
#include <vector>
#include <string>
#include <opencv2/core.hpp>

class Projector
{
private:
	std::vector<std::string> patternPaths;
	int currentPatternId;
	cv::Mat currentPattern;
	cv::Size circlesGridSize;

public:
	Projector(std::string patternFolder);

	void nextPattern();
	void showCurrentPattern(bool shortDelay = true);
	cv::Mat getCurrentPattern();
	int getNrPatterns();
	cv::Size getPatternSize();
};

