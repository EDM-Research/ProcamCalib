import cv2 as cv
import numpy as np
import random
from skspatial.objects import Line, Plane
import util
import pyrealsense2 as rs
import os
import sys
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.dirname(SCRIPT_DIR))

from config import *
import screeninfo
import argparse
from mirrorPlane import Mirror

def create_fullscreen_window(screen_id, window_name):
    if screen_id >= len(screeninfo.get_monitors()):
        return
    screen = screeninfo.get_monitors()[screen_id]

    cv.namedWindow(window_name, cv.WND_PROP_FULLSCREEN)
    cv.moveWindow(window_name, screen.x - 1, screen.y - 1)
    cv.setWindowProperty(window_name, cv.WND_PROP_FULLSCREEN,
                          cv.WINDOW_FULLSCREEN)
    return

def interpolatePoints(point1, point0):
    x0,y0,z0 = point0

    steps = 2000
    stepSize = (point1 - point0) / steps

    interpolatedPoints = []
    for i in range(steps + 1):
        xInterpolated = x0 + (i * stepSize[0])
        yInterpolated = y0 + (i * stepSize[1])
        zInterpolated = z0 + (i * stepSize[2])
        interpolatedPoints.append(np.array([xInterpolated, yInterpolated, zInterpolated]))
    return np.array(interpolatedPoints)

def drawPoints():
    global markings, frame
    ret, mask = cv.threshold(markings, 1, 255, cv.THRESH_BINARY_INV)

    mask = mask.astype(np.uint8)
    mask = cv.cvtColor(mask, cv.COLOR_BGR2GRAY)

    frame = cv.bitwise_and(frame, frame, mask=mask)
    
    markings = markings.astype(np.uint8)
    frame = cv.add(frame, markings)

def mouse_callback(event,x,y,flags, param):
    global camIntEST, camDistEST, projIntEST, projDistEST, markings, frame, pattern, mirrored, planeEST
    if event == cv.EVENT_LBUTTONUP:
        color = (random.randint(60,255),random.randint(60,255),random.randint(60,255))
        markings = cv.circle(markings, (x,y), 4, color, -1)

        drawPoints()

        # project point to cam space
        pixel = np.array([x,y])
        pixel = np.array([pixel]).astype("float32")
        pixel = cv.undistortPoints(pixel, camIntEST, camDistEST, None, None, camIntEST)
        pixel = cv.convertPointsToHomogeneous(pixel).reshape(3,)

        point3d = np.linalg.inv(camIntEST)@pixel

        cameraPos = np.array([0,0,0])

        if mirrored:
            ray = Line.from_points(point3d, cameraPos)
            mp = Mirror(planeEST, None)
            mirrorPlane = Plane(mp.getPointOnPlane(), mp.plane[:3])

            mirrorPoint = mirrorPlane.intersect_line(ray)

            reflectedCameraPos = mp.reflectPoints(np.array([cameraPos]))[0]

            points = np.array([mirrorPoint, reflectedCameraPos])
        else:
            points = np.array([point3d, cameraPos])

        # to projector space
        points = cv.convertPointsToHomogeneous(points).reshape((-1,4))
        points = (cam2projEST@points.T).T
        points = cv.convertPointsFromHomogeneous(points).reshape((-1,3))

        cameraRay = Line.from_points(points[1], points[0])

        topRight = np.linalg.inv(projIntEST)@np.array([1920,0,1])
        bottomRight = np.linalg.inv(projIntEST)@np.array([1920,1080,1])
        topLeft = np.linalg.inv(projIntEST)@np.array([0,0,1])
        bottomLeft = np.linalg.inv(projIntEST)@np.array([0,1080,1])

        planeTop = Plane.from_points(topLeft, topRight, np.array([0,0,0], dtype=np.float32))
        planeBottom = Plane.from_points(bottomLeft, bottomRight, np.array([0,0,0], dtype=np.float32))

        point0 = planeTop.intersect_line(cameraRay)
        point1 = planeBottom.intersect_line(cameraRay)

        point0 /= point0[2]
        point1 /= point1[2]

        points = interpolatePoints(point0, point1)

        # project back to projector
        projPoints, _ = cv.projectPoints(points, np.array([0,0,0], dtype=np.float32), np.array([0,0,0], dtype=np.float32), projIntEST, projDistEST)
        projPoints = projPoints.reshape((-1,2))

        # draw line
        for i in range(len(projPoints) - 1):
            point0 = projPoints[i]
            point1 = projPoints[i+1]
            if point0[0] >= 0 and point0[0] <= 1920 and point0[1] >= 0 and point0[1] <= 1080 and \
                point1[0] >= 0 and point1[0] <= 1920 and point1[1] >= 0 and point1[1] <= 1080:
                # pattern = cv.circle(pattern, (int(point0[0]), int(point0[1])), 5, color, -1)
                pattern = cv.line(pattern, (int(point0[0]), int(point0[1])),(int(point1[0]), int(point1[1])), color, 2)

        cv.imshow("frame", frame)
        cv.imshow("pattern", pattern)

if __name__ == "__main__":
    parser = argparse.ArgumentParser("empirical.py")
    parser.add_argument("estimation")
    
    args = parser.parse_args()

    mirrored = args.estimation.startswith('S') 

    os.chdir("..")
    camIntEST, camDistEST, camRMSEST, projIntEST, projDistEST, projRMSEST, \
            cam2projEST, virtualProj2camEST, stereoRMSEST, detectionsEST, planeEST = util.readEstFile(procamCalibrationFolder + args.estimation, mirrored)

    markings = np.zeros((800,1280,3))

    cv.namedWindow("frame")
    cv.setMouseCallback("frame", mouse_callback)
    create_fullscreen_window(1, "pattern")

    # pipeline = rs.pipeline()
    # config = rs.config()

    # config.enable_stream(rs.stream.color,1280,800,rs.format.bgr8, 30)

    # # Start streaming
    # prof = pipeline.start(config)
    # s = prof.get_device().query_sensors()[1]

    # pattern = np.zeros((1080,1920,3),np.uint8)#
    pattern = cv.imread(baseFolderPatterns + "Asym_4_9/01.png")
    while True:
        # frames = pipeline.wait_for_frames()
        # frame = np.asanyarray(frames.get_color_frame().get_data())
        frame = cv.imread(basefolderRecording + args.estimation.split("/")[0] + "/00.png")
        drawPoints()
        cv.imshow("frame", frame)
        c = cv.waitKey(1)
        if c == ord('q'):
            break
        elif c == ord('s'):
            cv.imwrite("eval.png", frame)