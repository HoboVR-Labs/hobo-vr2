// SPDX-License-Identifier: GPL-2.0-only

#pragma once

#include "driver_ipc.hpp"
#include "openvr_driver.h"

#include <atomic>
#include <thread>

class HvrDeviceProvider : public vr::IServerTrackedDeviceProvider {
public:
    vr::EVRInitError Init(vr::IVRDriverContext* pDriverContext) override;
    const char* const* GetInterfaceVersions() override;

    void RunFrame() override;

    bool ShouldBlockStandbyMode() override;
    void EnterStandby() override;
    void LeaveStandby() override;

    void Cleanup() override;

    void MyIpcThread();

private:
    std::atomic<bool> m_ipc_is_active;
    std::thread m_ipc_thread;

    std::unique_ptr<IpcServer> m_ipc_server;
};
