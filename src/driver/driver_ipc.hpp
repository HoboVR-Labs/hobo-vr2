// SPDX-License-Identifier: GPL-2.0-only

#pragma once

#include <memory>
#include <unordered_map>

#include "tracked_device_interfaces.hpp"

class IpcServer : public olc::net::server_interface<HeaderStatus> {
public:
    IpcServer(uint16_t nPort);

    std::unordered_map<uint32_t, sDeviceNetPacket> m_mapPlayerRoster;
    std::vector<uint32_t> m_vGarbageIDs;
    std::unordered_map<uint32_t, std::unique_ptr<IHvrTrackedDevice>> my_tracker_devices;
    std::vector<std::unique_ptr<IHvrTrackedDevice>> m_deactivated_devices;

    static constexpr const std::array<DeviceType, 2> m_supported_device_types = { { DeviceType::Tracker, DeviceType::ControllerViveLike } };

protected:
    bool OnClientConnect(std::shared_ptr<olc::net::connection<HeaderStatus>> client) override;

    void OnClientValidated(std::shared_ptr<olc::net::connection<HeaderStatus>> client) override;

    void OnClientDisconnect(std::shared_ptr<olc::net::connection<HeaderStatus>> client) override;

    void OnMessage(std::shared_ptr<olc::net::connection<HeaderStatus>> client, olc::net::message<HeaderStatus>& msg) override;

    void OnDeviceAdded(std::shared_ptr<olc::net::connection<HeaderStatus>> client, const sDeviceNetPacket& desc);

    void OnDeviceUpdate(std::shared_ptr<olc::net::connection<HeaderStatus>> client, olc::net::message<HeaderStatus>& msg);

    void OnDeviceRemove(const uint32_t pid);

public:
    void OnVRevent(const vr::VREvent_t& event);

    void StopAllDevices();
};
