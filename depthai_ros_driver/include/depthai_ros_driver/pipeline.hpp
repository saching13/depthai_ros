#pragma once

#include <vector>

#include <depthai/depthai.hpp>

namespace rr {
struct PipelineConfig {
    using NodeConstPtr = std::shared<const dai::Node>;

    /**
     * @brief The first function to get called, should have the setup ready for any future calls
     */
    virtual void configure = 0;

    /**
     * @brief interface called by the driver to get a ready pipeline
     */
    virtual dai::Pipeline getPipeline() = 0;

    // TODO: Do these need to be virtual? Can it be implemented independently?
    /**
     * @brief returns the nodes that access the raw camera data. Not used for ROS interface
     */
    virtual std::vector<NodeConstPtr> getCameras() = 0;
    /**
     * @brief returns the raw streams exposed by the ROS interface
     */
    virtual std::vector<NodeConstPtr> getRawVideoStreams() = 0;

    /**
     * @brief returns the compressed streams exposed by the ROS interface
     */
    virtual std::vector<NodeConstPtr> getCompressedVideoStreams() = 0;

    /**
     * @brief returns the camera control streams exposed by the ROS interface
     */
    virtual std::vector<NodeConstPtr> getControlStreams() = 0;

    /**
     * @brief returns the streams required to pass information into the device, via ROS interface
     */
    virtual std::vector<NodeConstPtr> getInputStreams() = 0;

    /**
     * @brief returns the streams that return a tensor, for interfacing with ROS
     */
    virtual std::vector<NodeConstPtr> getTensorOutStreams() = 0;
};
}  // namespace rr
