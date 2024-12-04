// SPDX-License-Identifier: GPL-2.0-only

#pragma once

#include "common.hpp"
#include "openvr_driver.h"
#include "tracked_device_interfaces.hpp"

#include <array>
#include <atomic>
#include <string>

enum MyComponent {
    MyComponent_a_touch,
    MyComponent_a_click,

    MyComponent_trigger_value,
    MyComponent_trigger_click,

    MyComponent_MAX
};

//-----------------------------------------------------------------------------
// Purpose: Represents a single tracked device in the system.
// What this device actually is (controller, hmd) depends on the
// properties you set within the device (see implementation of Activate)
//-----------------------------------------------------------------------------
class MyTrackerDeviceDriver : public IHvrTrackedDevice {
public:
    MyTrackerDeviceDriver(unsigned int my_tracker_id);

    vr::EVRInitError Activate(uint32_t unObjectId) override;

    void EnterStandby() override;

    void* GetComponent(const char* pchComponentNameAndVersion) override;

    void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) override;

    vr::DriverPose_t GetPose() override;

    void Deactivate() override;

    // ----- Functions we declare ourselves below -----

    const std::string& hGetSerialNumber() override;

    void MyRunFrame();
    void hProcessEvent(const vr::VREvent_t& vrevent) override;
    void hProcessMsg(olc::net::message<HeaderStatus>& msg) override;
    void hTurnOff() override;
    void hTurnOn() override;
    DeviceType hGetDeviceType() override;

    void MyPoseUpdateThread();

private:
    unsigned int my_tracker_id_;

    std::atomic<vr::TrackedDeviceIndex_t> my_device_index_;

    std::string my_device_model_number_;
    std::string my_device_serial_number_;

    std::array<vr::VRInputComponentHandle_t, MyComponent_MAX> input_handles_;
};
