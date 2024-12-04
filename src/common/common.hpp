// SPDX-License-Identifier: GPL-2.0-only

#ifndef COMMON_HEADER_HELPER_HPP
#define COMMON_HEADER_HELPER_HPP

#include <cmath>
#include <cstdint>

#define OLC_PGEX_NETWORK
#include "olcPGEX_Network.h"

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

struct sDeviceNetPacket {
    uint32_t nUniqueID = 0;
    DeviceType eDeviceType = DeviceType::Tracker;

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

    // reserved extra to pad the packet payload to 512 bytes
    std::array<uint8_t, 145> reserved;
};

#endif // #ifndef COMMON_HEADER_HELPER_HPP
