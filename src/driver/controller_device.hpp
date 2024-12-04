
#pragma once

#include "tracked_device_interfaces.hpp"

#include <array>
#include <atomic>
#include <string>

enum MyControllerComponent {
    MyControllerComponent_a_touch,
    MyControllerComponent_a_click,

    MyControllerComponent_trigger_value,
    MyControllerComponent_trigger_click,

    MyControllerComponent_haptic,

    MyControllerComponent_MAX
};

class MyControllerDeviceDriver : public IHvrTrackedDevice {
public:
    MyControllerDeviceDriver(unsigned int my_controller_id, const DeviceRole my_role);

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
    unsigned int my_controller_id_;

    std::atomic<vr::TrackedDeviceIndex_t> my_device_index_;

    vr::ETrackedControllerRole my_controller_role_;

    std::string my_device_model_number_;
    std::string my_device_serial_number_;

    std::array<vr::VRInputComponentHandle_t, MyControllerComponent_MAX> input_handles_;
};
