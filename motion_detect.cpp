//
// Compile:
//
// g++ -std=c++20 motion_detect.cpp -o motion_detect \
//     `pkg-config --cflags --libs opencv4` \
//     `pkg-config --cflags --libs libcamera` \
//     -llccv



#include <opencv2/opencv.hpp>
#include <lccv.hpp>
#include <iostream>

int main() {
    // Configure camera
    lccv::Camera cam;
    cam.options->video_width = 640;
    cam.options->video_height = 480;
    cam.options->framerate = 30;
    cam.options->verbose = false;

    if (!cam.startVideo()) {
        std::cerr << "Failed to start camera\n";
        return -1;
    }

    cv::Mat frame, gray, prev_gray, diff, thresh;
    bool first = true;

    std::cout << "Press 'q' to quit\n";

    while (true) {
        if (!cam.getVideoFrame(frame, 1000)) {
            std::cerr << "Timeout waiting for frame\n";
            continue;
        }

        if (frame.empty()) continue;

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(gray, gray, cv::Size(21, 21), 0);

        if (first) {
            prev_gray = gray.clone();
            first = false;
            continue;
        }

        cv::absdiff(prev_gray, gray, diff);
        cv::threshold(diff, thresh, 25, 255, cv::THRESH_BINARY);
        cv::dilate(thresh, thresh, cv::Mat(), cv::Point(-1, -1), 2);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(thresh, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        bool motion = false;
        for (const auto& contour : contours) {
            if (cv::contourArea(contour) < 500) continue;
            motion = true;
            cv::Rect box = cv::boundingRect(contour);
            cv::rectangle(frame, box, cv::Scalar(0, 0, 255), 2);
        }

        if (motion) {
            cv::putText(frame, "MOTION DETECTED", cv::Point(10, 30),
                        cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 255), 2);
        }

        cv::imshow("Motion Detection", frame);
        prev_gray = gray.clone();

        if (cv::waitKey(1) == 'q') break;
    }

    cam.stopVideo();
    return 0;
}
