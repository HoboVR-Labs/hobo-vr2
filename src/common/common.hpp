// SPDX-License-Identifier: GPL-2.0-only

#ifndef COMMON_HEADER_HELPER_HPP
#define COMMON_HEADER_HELPER_HPP

#include <cmath>
#include <cstdint>

#define OLC_PGEX_NETWORK
#include "olcPGEX_Network.h"

#ifndef NOOPENVR
#include <openvr_driver.h>
#endif // #ifndef NOOPENVR

#include <array>
#include <bitset>

#include "hvr_math.hpp"

enum class HeaderStatus : uint32_t {
    Server_GetStatus,
    Server_GetPing,

    Client_Accepted,
    Client_AssignID,
    Client_RegisterWithServer,

    Client_AddDevice,
    Client_RemoveDevice,
    Client_UpdateDevice,
};

enum class DeviceType : uint8_t {
    Hmd,
    HmdDirectDisplay,
    HmdVirtualDisplay,

    ControllerViveLike,
    ControllerQuestLike,
    ControllerIndexLike,

    Tracker,
    FaceTracker,
    EyeTracker,

    BaseStation,

    Invalid
};

enum class DeviceRole : uint8_t {
    Left,
    Right,
    Neither,
    Stylus,
    Treadmill,
    Invalid
};

#ifndef NOOPENVR
inline vr::ETrackedControllerRole toVr(const DeviceRole value)
{
    switch (value) {
    case DeviceRole::Left:
        return vr::ETrackedControllerRole::TrackedControllerRole_LeftHand;
    case DeviceRole::Right:
        return vr::ETrackedControllerRole::TrackedControllerRole_RightHand;
    case DeviceRole::Neither:
        return vr::ETrackedControllerRole::TrackedControllerRole_OptOut;
    case DeviceRole::Stylus:
        return vr::ETrackedControllerRole::TrackedControllerRole_Stylus;
    case DeviceRole::Treadmill:
        return vr::ETrackedControllerRole::TrackedControllerRole_Treadmill;
    case DeviceRole::Invalid:
    default:
        return vr::ETrackedControllerRole::TrackedControllerRole_Invalid;
    }
}

inline vr::ETrackedDeviceClass toVr(const DeviceType value)
{
    switch (value) {
    case DeviceType::Hmd:
    case DeviceType::HmdDirectDisplay:
    case DeviceType::HmdVirtualDisplay:
        return vr::ETrackedDeviceClass::TrackedDeviceClass_HMD;
    case DeviceType::ControllerViveLike:
    case DeviceType::ControllerIndexLike:
    case DeviceType::ControllerQuestLike:
        return vr::ETrackedDeviceClass::TrackedDeviceClass_Controller;
    case DeviceType::Tracker:
    case DeviceType::EyeTracker:
    case DeviceType::FaceTracker:
        return vr::ETrackedDeviceClass::TrackedDeviceClass_GenericTracker;
    case DeviceType::BaseStation:
        return vr::ETrackedDeviceClass::TrackedDeviceClass_TrackingReference;
    case DeviceType::Invalid:
    default:
        return vr::ETrackedDeviceClass::TrackedDeviceClass_Invalid;
    }
}
#endif // #ifndef NOOPENVR

struct sDeviceNetPacket {
    uint32_t nUniqueID = 0;
    DeviceType eDeviceType = DeviceType::Tracker;
    DeviceRole eDeviceRole = DeviceRole::Invalid;

    hvr::math::vec3d vPos = {};
    hvr::math::vec3d vVel = {};

    hvr::math::quatd vRot = { 1, 0, 0, 0 };
    hvr::math::vec3d vAngVel = {};

    // this should cover most button configurations on any controller
    std::bitset<16> bBoolStates = 0U;

    // this should be enough to cover both scalar inputs,
    // like triggers on controllers, as well as skeletal inputs
    // the skeletal input in the openvr repo inly uses 39 floats per hand
    std::array<float, 64> aFloatStates = { {} };

    // reserved extra to pad the packet to 512 bytes, this takes into account the extra 8 bytes
    // in the header
    std::array<uint8_t, 136> reserved;
};

#endif // #ifndef COMMON_HEADER_HELPER_HPP
