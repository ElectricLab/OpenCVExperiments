// Compile: g++ -std=c++20 motion_detect_stop.cpp -o motion_detect_stop \
//     `pkg-config --cflags --libs opencv4` \
//     `pkg-config --cflags --libs libcamera` \
//     -llccv


#include <opencv2/opencv.hpp>
#include <lccv.hpp>
#include <iostream>

int main() {
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
    int warmup = 30;          // ignore the first 30 frames (~1 second at 30 fps)

    std::cout << "Warming up camera...\n";

    while (true) {
        if (!cam.getVideoFrame(frame, 1000)) {
            std::cerr << "Timeout waiting for frame\n";
            continue;
        }

        if (frame.empty()) continue;

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(gray, gray, cv::Size(21, 21), 0);

        // Skip the first N frames while exposure/gain settle
        if (warmup > 0) {
            warmup--;
            prev_gray = gray.clone();
            continue;
        }

        if (first) {
            prev_gray = gray.clone();
            first = false;
            std::cout << "Watching for motion...\n";
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
            break;
        }

        if (motion) {
            cv::imwrite("motion_diff.jpg", diff);
            cv::imwrite("motion_thresh.jpg", thresh);
            std::cout << "Motion detected — wrote motion_diff.jpg and motion_thresh.jpg\n";
            break;
        }

        prev_gray = gray.clone();
    }

    cam.stopVideo();
    return 0;
}
