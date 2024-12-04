// SPDX-License-Identifier: GPL-2.0-only

#ifndef TRACKED_DEVICES_INTERFACES_HPP
#define TRACKED_DEVICES_INTERFACES_HPP

#include "common.hpp"
#include "openvr_driver.h"
#include <string>

class IHvrTrackedDevice : public vr::ITrackedDeviceServerDriver {

public:
    virtual const std::string& hGetSerialNumber() = 0;
    virtual DeviceType hGetDeviceType() = 0;

    virtual void hProcessEvent(const vr::VREvent_t& vrevent) = 0;
    virtual void hProcessMsg(olc::net::message<HeaderStatus>& msg) = 0;

    virtual void hTurnOff() = 0;
    virtual void hTurnOn() = 0;
};

#endif // #ifndef TRACKED_DEVICES_INTERFACES_HPP
