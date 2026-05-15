# Traffic Sign Detection and Recognition using Image Processing

A C++ / OpenCV application for detecting and recognizing a predefined set of traffic signs from images using classical image processing techniques.

The project focuses on color segmentation, contour extraction, geometric shape analysis, and rule-based validation. It does not rely on machine learning or deep learning. Instead, it uses interpretable image processing steps to identify traffic signs based on their visual characteristics.

## Project Overview

Traffic signs are important elements in road environments because they communicate restrictions, priorities, obligations, and navigation information. Automatic traffic sign detection is relevant for driver assistance systems, traffic monitoring, and autonomous vehicle research.

This project implements a traffic sign recognition pipeline that detects candidate regions in an input image and classifies them into one of several supported traffic sign classes.

The system follows a classical image processing approach:

1. The image is preprocessed to reduce noise.
2. Candidate regions are extracted based on color.
3. Each candidate is analyzed according to its geometric shape.
4. Final recognition is performed using additional rules specific to each traffic sign class.

## Supported Traffic Signs

The current implementation supports the following traffic signs:

- STOP
- Yield
- Priority Road
- Pedestrian Crossing
- Mandatory Ahead
- No Entry
- One Way
- No Stopping

## Main Features

- Image loading through a console-based application
- Color-based segmentation in HSV color space
- Detection of red, blue, and yellow traffic sign regions
- Gaussian filtering for noise reduction
- Morphological operations for cleaning binary masks
- Contour extraction
- Candidate filtering based on size, aspect ratio, and position
- Geometric shape recognition
- Rule-based traffic sign classification
- Visual output with bounding boxes, contours, and class labels
- Additional image processing utilities for testing and experimentation

## Technologies Used

- C++
- OpenCV
- Visual Studio
- Classical image processing techniques

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
```

## How the Algorithm Works

The recognition pipeline follows several sequential image processing stages.

### 1. Image Loading

The user selects an input image from disk. The image is loaded using OpenCV and passed to the recognition pipeline.

### 2. Image Resizing

The input image is resized to make processing faster and more stable across different image sizes.

### 3. Lower Region Removal

The bottom part of the image is ignored or filled with black. This reduces false detections caused by car hoods, reflections, or objects close to the camera.

### 4. Gaussian Filtering

A Gaussian blur is applied to reduce noise before color segmentation.

### 5. BGR to HSV Conversion

The image is converted from the BGR color space to HSV.

HSV is useful because it separates color information from brightness, making it easier to segment traffic signs by color.

### 6. Color Segmentation

The application creates binary masks for the main colors used by the supported traffic signs:

- red
- blue
- yellow

Red is handled using two hue intervals because red appears at both ends of the HSV hue range.

### 7. Morphological Processing

Morphological operations are applied to clean the binary masks.

The project uses operations such as:

- opening, to remove small noise regions
- closing, to fill small gaps
- dilation, especially for blue regions

### 8. Contour Extraction

External contours are extracted from each binary mask. These contours represent possible traffic sign regions.

### 9. Candidate Filtering

Each contour is filtered using several criteria:

- minimum area
- minimum width and height
- aspect ratio
- position in the image
- whether the candidate touches the image border

This step removes regions that are unlikely to represent real traffic signs.

### 10. Shape Analysis

For each valid candidate, the application computes geometric properties such as:

- area
- perimeter
- bounding rectangle
- polygonal approximation
- number of corners
- circularity

Based on these properties, the candidate is classified into one of the following general shapes:

- triangle
- quadrilateral
- octagon
- circle

### 11. Sign-Specific Validation

After the general shape is determined, additional rules are applied for each traffic sign class.

Examples:

- A STOP sign must be red, octagonal, and contain enough white pixels inside.
- A Yield sign must be red, triangular, and contain a significant white interior region.
- A Priority Road sign must be yellow and approximately quadrilateral.
- A No Entry sign must be red and circular, with a white horizontal bar inside.
- A No Stopping sign must contain both red and blue regions.
- A Mandatory Ahead sign must be blue, circular, and contain a white vertical arrow.
- A One Way sign must be blue, quadrilateral, and contain a white arrow.
- A Pedestrian Crossing sign must be blue, quadrilateral, and contain white and dark interior details.

### 12. Result Display

If a traffic sign is recognized, the application displays the final image with:

- a bounding box around the detected sign
- the approximated contour
- the detected class name

## General Pipeline

```text
Input image
    ↓
Resize image
    ↓
Remove lower image region
    ↓
Apply Gaussian blur
    ↓
Convert BGR to HSV
    ↓
Segment red, blue, and yellow regions
    ↓
Apply morphological operations
    ↓
Extract contours
    ↓
Filter candidates
    ↓
Determine candidate shape
    ↓
Apply sign-specific validation rules
    ↓
Display final result
```

## Implemented Modules

### Interface and Control Module

The interface and control module is implemented mainly through the console menu in `main()`.

It displays the application menu, reads the user option, and calls the selected function. The traffic sign detection functionality is launched from the project-specific menu option.

### Image Utility Module

The project contains several helper functions for basic image processing operations.

These include functions for:

- opening images
- saving images
- converting images to grayscale
- converting BGR images to HSV
- resizing images
- applying Canny edge detection
- displaying histograms
- testing mouse clicks on image pixels
- processing video sequences
- capturing images from a camera

These functions do not all belong directly to the final traffic sign recognition pipeline, but they were useful during development and testing.

### Candidate Filtering Module

This module eliminates invalid regions before classification.

Typical rejected regions include:

- very small objects
- objects touching the image border
- objects with unrealistic width-to-height ratios
- objects located in ignored image areas

This filtering step reduces the number of false candidates before shape and content analysis.

### Shape Analysis Module

The shape analysis module determines the approximate shape of each candidate region using contour approximation and circularity.

The recognized shape categories are:

- triangle
- quadrilateral
- octagon
- circle

This module is important because many traffic signs have standardized geometric shapes.

### Color and Interior Analysis Module

This module computes color ratios inside candidate regions.

The application checks for colors such as:

- white
- red
- blue
- black

These ratios are used to verify whether the internal structure of a detected object matches a real traffic sign.

For example:

- the STOP sign must contain white letters
- the No Entry sign must contain a white horizontal bar
- the No Stopping sign must contain blue inside the red border
- the Pedestrian Crossing sign must contain white and black details
- the Mandatory Ahead and One Way signs must contain a white arrow

### Class-Specific Validation Module

This module contains rules for specific traffic sign classes.

It improves accuracy by preventing simple color or shape matches from being accepted too easily.

For example, a red octagonal object is not automatically classified as STOP unless it also contains enough white pixels inside. Similarly, a red circular object is not automatically classified as No Entry unless a white horizontal bar is detected.

### Final Recognition Module

The final recognition module combines:

- detected color
- geometric shape
- contour properties
- color ratios
- internal visual structures

If no class-specific rule is satisfied, the candidate is marked as unknown and is not displayed as a recognized traffic sign.

## Traffic Sign Recognition Rules

### STOP

The STOP sign is detected using the following properties:

- dominant red color
- octagonal shape
- suitable size
- width-to-height ratio close to 1
- good object fill ratio
- presence of white pixels inside the sign

The white interior check is important because the real STOP sign contains white text. This reduces false positives from other red octagonal or nearly octagonal objects.

### Yield

The Yield sign is detected using:

- red color
- triangular shape
- suitable proportions
- enough white pixels inside the sign

The white interior region helps distinguish the real Yield sign from other red triangular objects in the image.

### Priority Road

The Priority Road sign is detected using:

- yellow color
- quadrilateral shape
- approximately square proportions

This sign is easier to separate from the others because yellow is the dominant color and is unique among the selected traffic signs.

### No Entry

The No Entry sign is detected using:

- red color
- circular shape
- white horizontal bar inside the sign

The white horizontal bar is the main visual element used to distinguish it from other red circular signs.

### No Stopping

The No Stopping sign is detected using:

- circular shape
- red border
- blue interior region
- absence of a white horizontal bar

The blue interior helps distinguish it from the No Entry sign.

### Mandatory Ahead

The Mandatory Ahead sign is detected using:

- blue color
- circular shape
- white vertical arrow inside the sign

The algorithm checks for a dominant white component inside the sign that resembles an upward arrow.

### One Way

The One Way sign is detected using:

- blue color
- quadrilateral shape
- white arrow inside the sign

This sign is separated from the Mandatory Ahead sign mainly through the exterior shape. Mandatory Ahead is circular, while One Way is quadrilateral.

### Pedestrian Crossing

The Pedestrian Crossing sign is detected using:

- blue color
- quadrilateral shape
- white triangular region inside the sign
- dark details inside the candidate region

The black details help identify the pedestrian symbol and crossing lines.

## Installation and Setup

### Requirements

Before running the project, make sure the following tools are installed:

- Windows operating system
- Visual Studio
- OpenCV for C++
- C++ compiler supported by Visual Studio

### OpenCV Configuration

The project uses OpenCV. Make sure OpenCV is correctly configured in Visual Studio.

Typical setup steps:

1. Add the OpenCV include directory to the project.
2. Add the OpenCV library directory to the project.
3. Link the required OpenCV `.lib` files.
4. Make sure the required OpenCV `.dll` files are available in the system path or near the executable.

Typical OpenCV modules used by the project include:

- `opencv_core`
- `opencv_imgproc`
- `opencv_highgui`
- `opencv_imgcodecs`
- `opencv_videoio`

The exact library names depend on the installed OpenCV version.

## How to Run

Clone the repository:

```bash
git clone https://github.com/vasibreban27/Sign-Detection-Image-Processing.git
```

Open the Visual Studio solution:

```text
OpenCVApplication.sln
```

Then:

1. Build the project in Visual Studio.
2. Run the application.
3. In the console menu, select the traffic sign detection option.
4. Choose an input image when the file dialog appears.
5. View the intermediate masks and the final detection result.

## Usage Example

After running the application, select the project option from the console menu.

```text
Select menu option 13 - Detect and recognize traffic signs
Choose an image from disk
Wait for processing
View the detection result
```

The result image displays recognized signs with labels such as:

```text
STOP
Yield
No Entry
No Stopping
Mandatory Ahead
Pedestrian Crossing
Priority Road
One Way
```

## Detection Output

The final output image contains:

- a green bounding box around the detected sign
- an approximated contour drawn around the sign
- a text label showing the recognized sign class

The project documentation includes successful detection examples for:

- Yield sign
- Mandatory Ahead sign
- No Stopping sign
- No Entry sign
- STOP sign

## Strengths of the Approach

- Does not require a training dataset
- Easy to understand and explain
- Works well for a limited number of predefined signs
- Uses visual properties that are meaningful for traffic signs
- Combines multiple criteria instead of relying only on color or shape
- Suitable for educational image processing projects

## Limitations

The current implementation has several limitations:

- It uses fixed color thresholds.
- Performance may decrease under strong lighting changes.
- Very small signs may not be detected.
- Partially occluded signs may be missed.
- Complex backgrounds may produce false candidates.
- The system is designed for a limited set of traffic signs.
- It is not a general-purpose traffic sign recognition system.
- It does not use machine learning, so it cannot easily generalize to unseen classes.

## Possible Improvements

Future versions could improve the system by adding:

- adaptive HSV thresholds
- better illumination normalization
- perspective correction
- more robust shape descriptors
- template matching for internal symbols
- support for more traffic sign classes
- real-time video detection
- camera input support for live road scenes
- machine learning or deep learning classification
- quantitative evaluation metrics such as precision, recall, and accuracy

## Educational Purpose

This project was developed as an image processing application. It demonstrates how classical computer vision techniques can be combined to solve a practical recognition problem.

The project is useful for understanding:

- HSV color segmentation
- binary masks
- morphological operations
- contour extraction
- polygonal approximation
- shape descriptors
- rule-based classification
- traffic sign visual analysis

## Authors

- Breban Vasile Sergiu
- Pal Roland

## Documentation

The repository includes a PDF documentation file describing the theoretical background, design, implementation, testing, and validation of the traffic sign detection system.

Documentation file:

```text
Detectare semne de circulatie_Pal_Roland_Breban_Vasile.pdf
```

## Acknowledgements

This project uses OpenCV and concepts from image processing laboratories, including:

- color spaces
- binary object features
- connected components
- contour following
- morphological operations
- noise filtering
- edge detection
- shape descriptors

## Conclusion

This project implements a complete classical image processing pipeline for detecting and recognizing selected traffic signs.

By combining HSV color segmentation, contour analysis, geometric shape recognition, and sign-specific validation rules, the application can identify several common traffic signs in static images. The system is especially suitable for educational purposes because every step of the recognition process is interpretable and based on fundamental image processing techniques.
