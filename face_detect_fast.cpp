//
// To compile:
//
// g++ face_detect.cpp -o face_detect \
//    `pkg-config --cflags --libs opencv4` \
//    `pkg-config --cflags --libs libcamera` \
//    -llccv


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

    // Load cascade
    cv::CascadeClassifier face_cascade;
    if (!face_cascade.load("/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml")) {
        std::cerr << "Failed to load cascade\n";
        return -1;
    }

    cv::Mat frame, gray;
    std::cout << "Press 'q' to quit\n";

    while (true) {
        if (!cam.getVideoFrame(frame, 1000)) {  // 1 second timeout
            std::cerr << "Timeout waiting for frame\n";
            continue;
        }

        if (frame.empty()) continue;

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(gray, gray);

        std::vector<cv::Rect> faces;
        face_cascade.detectMultiScale(gray, faces, 1.1, 5, 0, cv::Size(40, 40));

        for (const auto& face : faces) {
            cv::rectangle(frame, face, cv::Scalar(0, 255, 0), 2);
            cv::putText(frame, "Face", cv::Point(face.x, face.y - 10),
                        cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);
        }

        cv::imshow("Face Detection", frame);
        if (cv::waitKey(1) == 'q') break;
    }

    cam.stopVideo();
    return 0;
}