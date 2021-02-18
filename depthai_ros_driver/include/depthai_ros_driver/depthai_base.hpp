#pragma once

// self-message includes
#include <depthai_ros_msgs/AutoFocusCtrl.h>
#include <depthai_ros_msgs/Object.h>
#include <depthai_ros_msgs/Objects.h>
#include <depthai_ros_msgs/TriggerNamed.h>
#include <depthai_ros_msgs/StringArray.h>

#include <depthai_ros_driver/pipeline.hpp>
// core ROS dependency includes
#include <camera_info_manager/camera_info_manager.h>
#include <cv_bridge/cv_bridge.h>
#include <image_transport/camera_publisher.h>
#include <ros/ros.h>

// ROS1 messages
#include <sensor_msgs/Image.h>
#include <std_msgs/Float32.h>
//ros pluginlib
#include "pluginlib/class_loader.hpp"

// relevant 3rd party includes
// #include <depthai/device.hpp>
// #include <depthai/host_data_packet.hpp>
// #include <depthai/nnet/nnet_packet.hpp>
// #include <depthai/pipeline/cnn_host_pipeline.hpp>
#include <depthai/depthai.hpp>
#include <depthai-shared/datatype/RawImgFrame.hpp>


// general 3rd party includes
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <opencv2/opencv.hpp>

// std includes
#include <algorithm>
#include <array>
#include <memory>
#include <string>

namespace rr {

template <class Node>
class DepthAIBase : public Node {
public:
    DepthAIBase() = default;
    ~DepthAIBase() = default;

private:
    // encoding make_pair
    const std::unordered_map<int, std::string> encoding_enum_map({
        {dai::RawImgFrame::Type::YUV422i        : "yuv422"    },
        {dai::RawImgFrame::Type::RGBA8888       : "rgba8"     },
        {dai::RawImgFrame::Type::RGB161616      : "rgb16"     }, // TODO(Sachin): This might be buggy
        {dai::RawImgFrame::Type::RGB888i        : "rgb8"      },
        {dai::RawImgFrame::Type::BGR888i        : "bgr8"      },
        {dai::RawImgFrame::Type::RGBF16F16F16i  : "16FC3"     }, // FIXME(Sachin): There is no 16FC in opencv. THink of fix later
        {dai::RawImgFrame::Type::BGRF16F16F16i  : "16FC3"     }, // FIXME(Sachin): There is no 16FC in opencv. THink of fix later
        {dai::RawImgFrame::Type::GRAY8          : "8UC1"      },
        {dai::RawImgFrame::Type::GRAYF16        : "16FC1"     }, // FIXME(Sachin): There is no 16FC in opencv. THink of fix later
        {dai::RawImgFrame::Type::RAW8           : "8UC1"      },
        {dai::RawImgFrame::Type::RAW16          : "16UC1"     },
        {dai::RawImgFrame::Type::RGBF16F16F16i  : "16FC3"     }, // FIXME(Sachin): There is no 16FC in opencv. THink of fix later
        {dai::RawImgFrame::Type::RGBF16F16F16i  : "16FC3"     }  // FIXME(Sachin): There is no 16FC in opencv. THink of fix later
        {dai::RawImgFrame::Type::NV12           : "CV_bridge" },
    });


    std::unordered_map<std::string, std::unique_ptr<ros::Publisher>> _stream_publishers;
    std::unordered_map<std::string, std::unique_ptr<ros::Publisher>> _camera_info_publishers;

    std::unordered_map<std::string,std::unique_ptr<camera_info_manager::CameraInfoManager>> _camera_info_manager;
    std::unique_ptr<camera_info_manager::CameraInfoManager> _defaultManager;
    ros::ServiceServer _camera_info_default, _start_device_srv, _get_available_device_srv, _get_all_devices_srv, _start_pipeline_srv;
    pluginlib::ClassLoader<rr::Pipeline> pipeline_loader;

    ros::Publisher _camera_info_pub;

    ros::Subscriber _af_ctrl_sub;
    ros::Subscriber _disparity_conf_sub;

    ros::Timer _cameraReadTimer;

    std::string _pipeline_config_json;

    // Parameters
    std::string _camera_name = "";
    std::string _camera_param_uri = "package://depthai_ros_driver/params/camera/";
    std::string _cmd_file = "";
    std::string _calib_file = "";
    std::string _blob_file = "";
    std::string _blob_file_config = "";

    bool _depthai_block_read = false;
    bool _request_jpegout = false;
    bool _sync_video_meta = false;
    bool _full_fov_nn = false;
    bool _force_usb2 = false;
    bool _compute_bbox_depth = false;

    int _rgb_height = 1080;
    int _rgb_fps = 30;
    int _depth_height = 720;
    int _depth_fps = 30;

    int _shaves = 14;
    int _cmx_slices = 14;
    int _nn_engines = 2;

    int _queue_size = 10;

    std::vector<std::string> _stream_list = {"video", "left", "depth"};

    ros::Time _stamp;
    double _depthai_ts_offset = -1;  // sadly, we don't have a way of measuring drift

    std::unique_ptr<PipelineConfig> _config_plugin; 
    std::shared_ptr<dai::Pipeline> _pipeline; 
    std::unique_ptr<dai::Device> _depthai;
    std::map<std::string, int> _nn2depth_map;
    std::vector<std::string> _available_streams;

    void prepareStreamConfig();
    void processPacketsAndPub();

    void afCtrlCb(const depthai_ros_msgs::AutoFocusCtrl msg);
    void disparityConfCb(const std_msgs::Float32::ConstPtr& msg);

    void publishImageMsg(const HostDataPacket& packet, Stream type, const ros::Time& stamp);
    void publishObjectInfoMsg(const dai::Detections& detections, const ros::Time& stamp);
    void publishCameraInfo(ros::Time stamp);

    void cameraReadCb(const ros::TimerEvent&);
    bool defaultCameraInfo(depthai_ros_msgs::TriggerNamed::Request& req, depthai_ros_msgs::TriggerNamed::Response& res);

    void getPackets();
    void createPipeline();
    void getAvailableStreams();

    void onInit() override;
};
}  // namespace rr

#include <node_interface/ros1_node_interface.hpp>
#include <nodelet/nodelet.h>
namespace rr {
using DepthAINode = DepthAIBase<ROS1Node<>>;
using DepthAINodelet = DepthAIBase<nodelet::Nodelet>;
}  // namespace rr
