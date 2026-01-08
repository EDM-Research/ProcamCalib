#include <iostream>
#include <vector>
#include "Projector.h"
#include "ProcamCalibrator.h"
#include "DeviceFactory/DeviceFactory.h"
#include <filesystem>

using namespace std;

class CmdLineParser {
    
private:
    int argc; char** argv;

public: 
    CmdLineParser(int _argc, char** _argv) :argc(_argc), argv(_argv) {}  bool operator[] (string param) { int idx = -1;  for (int i = 0; i < argc && idx == -1; i++) if (string(argv[i]) == param) idx = i;	return (idx != -1); } string operator()(string param, string defvalue = "") { int idx = -1;	for (int i = 0; i < argc && idx == -1; i++) if (string(argv[i]) == param) idx = i; if (idx == -1) return defvalue;   else  return (argv[idx + 1]); }
      std::vector<std::string> getAllInstances(string str) 
      {
          std::vector<std::string> ret;
          for (int i = 0; i < argc - 1; i++) 
          {
              if (string(argv[i]) == str)
                  ret.push_back(argv[i + 1]);
          }
          return ret;
      }
};



int main(int argc, char** argv)
{
    CmdLineParser cml(argc, argv);
    if (argc < 3 || cml["-h"]) {
        cerr << std::endl << "Usage: ./ProcamCalib recording patterns camcalib mirrorcalib [-p] [-camid] [-d]" << std::endl;
        cerr << std::endl << "recording: folder containing the recording, or folder to save images to. Starts with 'S' if recording is mirrored." << std::endl;
        cerr << std::endl << "patterns: folder containing the patterns, folder name should end with _{width}_{height} of the circlegrid to detect." << std::endl;
        cerr << std::endl << "camcalib: path to camera calibration data." << std::endl;
        cerr << std::endl << "[--mirrorcalib]: path to mirror calibration data. Only needed when using a mirrored recording (S...)." << std::endl;
        cerr << std::endl << "[-p]: captures per pattern, only use when physical camera is connected." << std::endl;
        cerr << std::endl << "[-camid]: camera id to use. Only use when physical camera is connected." << std::endl;
        cerr << std::endl << "[-d]: whether to use debug mode or not." << std::endl;
        return 0;
    }

    std::string recordingFolder = argv[1];
    std::string patterns = argv[2];
    std::string camCalibPath = argv[3];
    std::string mirrorRecordingFolder;

    std::string seqName = std::filesystem::path(recordingFolder).filename().string();
    if (seqName[0] == 'S' && !cml["--mirrorcalib"])
    {
        cerr << std::endl << "[ProcamCalib] Mirror calibration [--mirrorcalib] should be specified when using a mirrored recording (S...)" << std::endl;
        exit(1);
    }

    Projector proj{ patterns };
    ProcamCalibrator calibrator;

    if (cml["--mirrorcalib"])
    {
        mirrorRecordingFolder = cml("--mirrorcalib");
        calibrator.init(recordingFolder, mirrorRecordingFolder, &proj, camCalibPath);
    }
    else
    {
        calibrator.init(recordingFolder, &proj, camCalibPath);
    }

    if (cml["-p"] && cml["-camid"])
    {
        DeviceFactory::DeviceFactory df;
        df.listAvailableDevices();
        std::shared_ptr<DeviceFactory::Device> cam = df.createDevices("RealSense2", cml("-camid"));
        if(cam.get() == nullptr)
            exit(1);
        calibrator.calibrate(cam, std::stoi(cml("-p")));
        
    }
    else if (!cml["-p"] && !cml["-camid"])
    {
        calibrator.calibrate(cml["-d"]);
    }
    else
    {
        cerr << "[ProcamCalib]: [-p] and [-camid] should be used simultaneously" << endl;
        exit(1);
    }

    calibrator.saveToJSON();

    return 0;
}

