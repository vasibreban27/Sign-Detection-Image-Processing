# Traffic Sign Detection and Recognition using Image Processing

A C++ / OpenCV application for detecting and recognizing a predefined set of traffic signs from images using classical image processing techniques.

The project focuses on color segmentation, contour extraction, geometric shape analysis, and rule-based validation. It does not rely on machine learning or deep learning; instead, it uses interpretable image processing steps to identify signs based on their visual characteristics.

## Project Overview

Traffic signs are important visual elements in road environments because they communicate restrictions, priorities, obligations, and navigation information. Automatic detection of traffic signs is relevant for driver assistance systems, traffic monitoring, and autonomous vehicle research.

This project implements a traffic sign recognition pipeline that detects candidate regions in an input image and classifies them into one of several supported traffic sign classes.

## Supported Traffic Signs

The current implementation supports the following traffic signs:

1. STOP
2. Yield
3. Priority Road
4. Pedestrian Crossing
5. Mandatory Ahead
6. No Entry
7. One Way
8. No Stopping

## Main Features

- Image loading through a console-based application
- Color-based segmentation in HSV color space
- Detection of red, blue, and yellow traffic sign regions
- Morphological filtering for noise reduction
- Contour extraction and candidate filtering
- Geometric shape recognition:
  - triangle
  - quadrilateral
  - octagon
  - circle
- Rule-based traffic sign classification
- Visual output with:
  - bounding boxes
  - approximated contours
  - detected sign labels
- Additional laboratory-style image processing utilities:
  - image opening and saving
  - grayscale conversion
  - BGR to HSV conversion
  - image resizing
  - Canny edge detection
  - video sequence processing
  - mouse-based pixel inspection

## Technologies Used

- C++
- OpenCV
- Visual Studio
- Classical image processing methods

## Repository Structure

```text
Sign-Detection-Image-Processing/
│
├── Images/                                  # Test images and image resources
├── Videos/                                  # Video resources used by auxiliary functions
├── OpenCV/                                  # OpenCV-related project files/dependencies
├── doc/                                     # Documentation files
│
├── OpenCVApplication.cpp                    # Main application source file
├── common.cpp                               # Utility implementation file
├── common.h                                 # Utility header file
├── stdafx.cpp                               # Precompiled header source
├── stdafx.h                                 # Precompiled header
├── targetver.h                              # Target Windows version header
│
├── OpenCVApplication.sln                    # Visual Studio solution
├── OpenCVApplication.vcxproj                # Visual Studio C++ project file
├── clean.bat                                # Cleanup script
├── .gitignore
│
└── Detectare semne de circulatie_Pal_Roland_Breban_Vasile.pdf
