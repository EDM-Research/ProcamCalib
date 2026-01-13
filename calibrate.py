import os
import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser("calibrate.py", "Calibrate a full system: camera calibration, mirror calibration and procam calibration")
    parser.add_argument("recording", type=str, help="Path to the recording folder (will save calibration images here, or read them from here). The folder name should start with 'S' for a mirrored setup.")
    parser.add_argument("patterns", type=str, help="Path to the patterns folder to use for the procam calibration (patterns that are projected, or were projected on the calibration board)")
    parser.add_argument("-m", "--mirrorRecording", type=str, help="Path to the recording folder for mirror calibration (will save calibration images here, or read them from here). Only needed for mirrored setups (recording folder name starts with 'S'). The folder name should start with 'F' for full view and with 'M' for Real-Virtual observations.")
    parser.add_argument("-d", "--debug", action="store_true", help="Enable debug mode (shows images during calibration)")
    
    args = parser.parse_args()

    sequenceName = os.path.basename(args.recording)

    if sequenceName.startswith("S") and args.mirrorRecording is None:
        print("A mirrored recording requires a mirrorRecording..")
        exit(1)

    os.system("CamCalib %s %s"%(args.recording, "-d" if args.debug else ""))
    if sequenceName.startswith("S"):
        os.system("MirrorCalib %s %s %s"%(args.mirrorRecording, "./data/estimation/camCalib/"+sequenceName+".json", "-d" if args.debug else ""))
        os.system("ProcamCalib %s %s %s --mirrorcalib %s %s"%(args.recording, args.patterns, "./data/estimation/camCalib/"+sequenceName+".json", "./data/estimation/mirrorCalib/"+sequenceName+".json", "-d" if args.debug else ""))
    else:
        os.system("ProcamCalib %s %s %s %s"%(args.recording, args.patterns, "./data/estimation/camCalib/"+sequenceName+".json", "-d" if args.debug else ""))
