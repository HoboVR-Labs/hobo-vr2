// SPDX-License-Identifier: GPL-2.0-only

#include "driver_provider.h"

#include "driverlog.h"

//-----------------------------------------------------------------------------
// Purpose: This is called by vrserver after it receives a pointer back from HmdDriverFactory.
// You should do your resources allocations here (**not** in the constructor).
//-----------------------------------------------------------------------------
vr::EVRInitError HvrDeviceProvider::Init(vr::IVRDriverContext* pDriverContext)
{
    // We need to initialise our driver context to make calls to the server.
    // OpenVR provides a macro to do this for us.
    VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);

    m_ipc_server = std::make_unique<IpcServer>(60000);
    if (!m_ipc_server) {
        DriverLog("COULD NOT INIT SERVER!!!");
        return vr::VRInitError_IPC_NamespaceUnavailable;
    }

    m_ipc_is_active = true;

    m_ipc_server->Start();

    m_ipc_thread = std::thread(&HvrDeviceProvider::MyIpcThread, this);

    return vr::VRInitError_None;
}

//-----------------------------------------------------------------------------
// Purpose: Tells the runtime which version of the API we are targeting.
// Helper variables in the header you're using contain this information, which can be returned here.
//-----------------------------------------------------------------------------
const char* const* HvrDeviceProvider::GetInterfaceVersions()
{
    return vr::k_InterfaceVersions;
}

//-----------------------------------------------------------------------------
// Purpose: This function is deprecated and never called. But, it must still be defined, or we can't compile.
//-----------------------------------------------------------------------------
bool HvrDeviceProvider::ShouldBlockStandbyMode()
{
    return false;
}

//-----------------------------------------------------------------------------
// Purpose: This is called in the main loop of vrserver.
// Drivers *can* do work here, but should ensure this work is relatively inexpensive.
// A good thing to do here is poll for events from the runtime or applications
//-----------------------------------------------------------------------------
void HvrDeviceProvider::RunFrame()
{
    if (!m_ipc_server)
        return;

    // Now, process events that were submitted for this frame.
    vr::VREvent_t vrevent {};
    while (vr::VRServerDriverHost()->PollNextEvent(&vrevent, sizeof(vr::VREvent_t))) {
        m_ipc_server->OnVRevent(vrevent);
    }
}

void HvrDeviceProvider::MyIpcThread()
{
    if (!m_ipc_server)
        return;

    while (m_ipc_is_active) {
        m_ipc_server->Update(-1, false);
    }
}

//-----------------------------------------------------------------------------
// Purpose: This function is called when the system enters a period of inactivity.
// The devices might want to turn off their displays or go into a low power mode to preserve them.
//-----------------------------------------------------------------------------
void HvrDeviceProvider::EnterStandby()
{
}

//-----------------------------------------------------------------------------
// Purpose: This function is called after the system has been in a period of inactivity, and is waking up again.
// Turn back on the displays or devices here.
//-----------------------------------------------------------------------------
void HvrDeviceProvider::LeaveStandby()
{
}

//-----------------------------------------------------------------------------
// Purpose: This function is called just before the driver is unloaded from vrserver.
// Drivers should free whatever resources they have acquired over the session here.
// Any calls to the server is guaranteed to be valid before this, but not after it has been called.
//-----------------------------------------------------------------------------
void HvrDeviceProvider::Cleanup()
{
    // Our tracker devices will have already deactivated. Let's now destroy them.
    if (m_ipc_is_active.exchange(false)) {
        m_ipc_thread.join();
    }

    if (m_ipc_server) {
        m_ipc_server->StopAllDevices();
    }
    m_ipc_server.reset(nullptr);
}
