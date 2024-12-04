// SPDX-License-Identifier: GPL-2.0-only

#include "driver_ipc.hpp"
#include "driver_tracked_device.h"
#include "driverlog.h"

IpcServer::IpcServer(uint16_t nPort)
    : olc::net::server_interface<HeaderStatus>(nPort)
{
}

bool IpcServer::OnClientConnect(std::shared_ptr<olc::net::connection<HeaderStatus>> client)
{
    // For now we will allow all
    return true;
}

void IpcServer::OnClientValidated(std::shared_ptr<olc::net::connection<HeaderStatus>> client)
{
    // Client passed validation check, so send them a message informing
    // them they can continue to communicate
    olc::net::message<HeaderStatus> msg;
    msg.header.id = HeaderStatus::Client_Accepted;
    client->Send(msg);
}

void IpcServer::OnClientDisconnect(std::shared_ptr<olc::net::connection<HeaderStatus>> client)
{
    if (client) {
        if (m_mapPlayerRoster.find(client->GetID()) == m_mapPlayerRoster.end()) {
            // client never added to roster, so just let it disappear
        } else {
            auto& pd = m_mapPlayerRoster[client->GetID()];
            DriverLog("[UNGRACEFUL REMOVAL]: %s", std::to_string(pd.nUniqueID).c_str());
            m_mapPlayerRoster.erase(client->GetID());
            m_vGarbageIDs.push_back(client->GetID());
            OnDeviceRemove(client->GetID());
        }
    }
}

void IpcServer::OnMessage(std::shared_ptr<olc::net::connection<HeaderStatus>> client, olc::net::message<HeaderStatus>& msg)
{
    if (!m_vGarbageIDs.empty()) {
        for (auto pid : m_vGarbageIDs) {
            olc::net::message<HeaderStatus> m;
            m.header.id = HeaderStatus::Client_RemoveDevice;
            m << pid;
            DriverLog("Removing %lu", pid);
            MessageAllClients(m);
        }
        m_vGarbageIDs.clear();
    }

    switch (msg.header.id) {
    case HeaderStatus::Client_RegisterWithServer: {
        sDeviceNetPacket desc;
        msg >> desc;
        desc.nUniqueID = client->GetID();
        m_mapPlayerRoster.insert_or_assign(desc.nUniqueID, desc);

        olc::net::message<HeaderStatus> msgSendID;
        msgSendID.header.id = HeaderStatus::Client_AssignID;
        msgSendID << desc.nUniqueID;
        MessageClient(client, msgSendID);
        OnDeviceAdded(client, desc);

        olc::net::message<HeaderStatus> msgAddPlayer;
        msgAddPlayer.header.id = HeaderStatus::Client_AddDevice;
        msgAddPlayer << desc;
        MessageAllClients(msgAddPlayer);

        for (const auto& player : m_mapPlayerRoster) {
            olc::net::message<HeaderStatus> msgAddOtherPlayers;
            msgAddOtherPlayers.header.id = HeaderStatus::Client_AddDevice;
            msgAddOtherPlayers << player.second;
            MessageClient(client, msgAddOtherPlayers);
        }

        break;
    }

    case HeaderStatus::Client_UpdateDevice: {
        // Simply bounce update to everyone except incoming client
        MessageAllClients(msg, client);
        OnDeviceUpdate(client, msg);
        break;
    }
    default:
        break;
    }
}

void IpcServer::OnDeviceAdded(std::shared_ptr<olc::net::connection<HeaderStatus>> client, const sDeviceNetPacket& desc)
{
    std::unique_ptr<IHvrTrackedDevice> tracker_device;
    if (m_deactivated_devices.empty()) {
        // ignore client if its device type is not supported
        if (std::find(
                m_supported_device_types.begin(),
                m_supported_device_types.end(),
                desc.eDeviceType)
                == m_supported_device_types.end()
            || desc.eDeviceType == DeviceType::Invalid) {
            DriverLog("REQUESTED DEVICE TYPE NOT SUPPORTED!!!");
            return;
        }

        switch (desc.eDeviceType) {
        case DeviceType::Tracker:
        default: // tracker is the default type
            tracker_device = std::make_unique<MyTrackerDeviceDriver>(client->GetID());
            break;
        }

        // Now we need to tell vrserver about our controllers.
        // The first argument is the serial number of the device, which must be unique across all devices.
        // We get it from our driver settings when we instantiate,
        // And can pass it out of the function with MyGetSerialNumber().
        // Let's add the left hand controller first (there isn't a specific order).
        // make sure we actually managed to create the device.
        // TrackedDeviceAdded returning true means we have had our device added to SteamVR.
        if (!vr::VRServerDriverHost()->TrackedDeviceAdded(tracker_device->hGetSerialNumber().c_str(),
                vr::TrackedDeviceClass_GenericTracker, tracker_device.get())) {
            DriverLog("Failed to create device!");
            // We failed? Return early.
            return;
        }
    } else {
        tracker_device = std::move(m_deactivated_devices.back());
        tracker_device->hTurnOn();
        m_deactivated_devices.pop_back();
    }

    my_tracker_devices.emplace(client->GetID(), std::move(tracker_device));
}

void IpcServer::OnDeviceUpdate(std::shared_ptr<olc::net::connection<HeaderStatus>> client, olc::net::message<HeaderStatus>& msg)
{
    const auto res = my_tracker_devices.find(client->GetID());
    if (res != my_tracker_devices.end()) {
        res->second->hProcessMsg(msg);
    } else {
        DriverLog("DEVICE %s MISSING!!!", std::to_string(client->GetID()).c_str());
    }
}

void IpcServer::OnDeviceRemove(const uint32_t pid)
{
    const auto res = my_tracker_devices.find(pid);
    if (res == my_tracker_devices.end())
        return;

    res->second->hTurnOff();

    m_deactivated_devices.emplace_back(std::move(res->second));
    my_tracker_devices.erase(res);
}

void IpcServer::OnVRevent(const vr::VREvent_t& event)
{
    for (const auto& devices : my_tracker_devices) {
        devices.second->hProcessEvent(event);
    }
}

void IpcServer::StopAllDevices()
{
    for (auto& tracker : my_tracker_devices) {
        olc::net::message<HeaderStatus> m;
        m.header.id = HeaderStatus::Client_RemoveDevice;
        m << tracker.first;
        DriverLog("Removing %lu", tracker.first);
        MessageAllClients(m);
        tracker.second = nullptr;
    }
}
