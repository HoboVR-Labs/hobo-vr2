// SPDX-License-Identifier: OLC

#define NOOPENVR
#include "common.hpp"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_TRANSFORMEDVIEW
#include "olcPGEX_TransformedView.h"

#include <chrono>
#include <unordered_map>

class Benchmark : public olc::net::client_interface<HeaderStatus> {
    std::unordered_map<uint32_t, sDeviceNetPacket> mapObjects;
    uint32_t nPlayerID = 0;
    sDeviceNetPacket descPlayer;

    bool bWaitingForConnection = true;

public:
    Benchmark() = default;

    void Init()
    {

        if (Connect("127.0.0.1", 60000)) {
        }
    }

    void Update()
    {
        const auto start = std::chrono::high_resolution_clock::now();
        // Check for incoming network messages
        if (IsConnected()) {
            while (!Incoming().empty()) {
                auto msg = Incoming().pop_front().msg;

                switch (msg.header.id) {
                case (HeaderStatus::Client_Accepted): {
                    std::cout << "Server accepted client - you're in!\n";
                    olc::net::message<HeaderStatus> msg;
                    msg.header.id = HeaderStatus::Client_RegisterWithServer;
                    descPlayer.vPos = { 3.0f, 0, 3.0f };
                    msg << descPlayer;
                    Send(msg);
                    break;
                }

                case (HeaderStatus::Client_AssignID): {
                    // Server is assigning us OUR id
                    msg >> nPlayerID;
                    std::cout << "Assigned Client ID = " << nPlayerID << "\n";
                    break;
                }

                case (HeaderStatus::Client_AddDevice): {
                    sDeviceNetPacket desc;
                    msg >> desc;
                    mapObjects.insert_or_assign(desc.nUniqueID, desc);

                    if (desc.nUniqueID == nPlayerID) {
                        // Now we exist in game world
                        bWaitingForConnection = false;
                    }
                    break;
                }

                case (HeaderStatus::Client_RemoveDevice): {
                    uint32_t nRemovalID = 0;
                    msg >> nRemovalID;
                    mapObjects.erase(nRemovalID);
                    break;
                }

                case (HeaderStatus::Client_UpdateDevice): {
                    sDeviceNetPacket desc;
                    msg >> desc;
                    mapObjects.insert_or_assign(desc.nUniqueID, desc);
                    break;
                }
                }
            }
        }

        // Send player description
        olc::net::message<HeaderStatus> msg;
        msg.header.id = HeaderStatus::Client_UpdateDevice;
        msg << mapObjects[nPlayerID];
        Send(msg);

        // check final frame time
        const auto end = std::chrono::high_resolution_clock::now();
        std::cout << "elapsed(ns): " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "\n";
    }
};

class MMOGame : public olc::PixelGameEngine, olc::net::client_interface<HeaderStatus> {
public:
    MMOGame(const DeviceType type, const DeviceRole role)
        : m_device_type(type)
        , m_device_role(role)
    {
        std::cout << "starting with device type: " << static_cast<int>(type) << "\n";
        sAppName = "MMO Client";
    }

private:
    DeviceType m_device_type;
    DeviceRole m_device_role;
    olc::TileTransformedView tv;

    std::string sWorldMap = "################################"
                            "#..............................#"
                            "#..............................#"
                            "#..............................#"
                            "#..............................#"
                            "#..............................#"
                            "#..............................#"
                            "#..............................#"
                            "#..............................#"
                            "#..............................#"
                            "#..............................#"
                            "#..............................#"
                            "#..........####...####.........#"
                            "#..........#.........#.........#"
                            "#..........#.........#.........#"
                            "#..........#.........#.........#"
                            "#..........##############......#"
                            "#..............................#"
                            "#..................#.#.#.#.....#"
                            "#..............................#"
                            "#..................#.#.#.#.....#"
                            "#..............................#"
                            "#..............................#"
                            "#..............................#"
                            "#..............................#"
                            "#..............................#"
                            "#..............................#"
                            "#..............................#"
                            "#..............................#"
                            "#..............................#"
                            "#..............................#"
                            "################################";

    olc::vi2d vWorldSize = { 32, 32 };

private:
    std::unordered_map<uint32_t, sDeviceNetPacket> mapObjects;
    uint32_t nPlayerID = 0;
    sDeviceNetPacket descPlayer;

    bool bWaitingForConnection = true;

public:
    bool OnUserCreate() override
    {
        tv = olc::TileTransformedView({ ScreenWidth(), ScreenHeight() }, { 8, 8 });

        // mapObjects[0].nUniqueID = 0;
        // mapObjects[0].vPos = { 3.0f, 3.0f };
        std::cout << "network packet size: " << sizeof(sDeviceNetPacket) << "\n";

        if (Connect("127.0.0.1", 60000)) {
            return true;
        }

        return false;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        // Check for incoming network messages
        if (IsConnected()) {
            while (!Incoming().empty()) {
                auto msg = Incoming().pop_front().msg;

                switch (msg.header.id) {
                case (HeaderStatus::Client_Accepted): {
                    std::cout << "Server accepted client - you're in!\n";
                    olc::net::message<HeaderStatus> msg;
                    msg.header.id = HeaderStatus::Client_RegisterWithServer;
                    descPlayer.vPos = { 3.0f, 0, 3.0f };
                    descPlayer.eDeviceType = m_device_type;
                    descPlayer.eDeviceRole = m_device_role;
                    msg << descPlayer;
                    Send(msg);
                    break;
                }

                case (HeaderStatus::Client_AssignID): {
                    // Server is assigning us OUR id
                    msg >> nPlayerID;
                    std::cout << "Assigned Client ID = " << nPlayerID << "\n";
                    break;
                }

                case (HeaderStatus::Client_AddDevice): {
                    sDeviceNetPacket desc;
                    msg >> desc;
                    mapObjects.insert_or_assign(desc.nUniqueID, desc);

                    if (desc.nUniqueID == nPlayerID) {
                        // Now we exist in game world
                        bWaitingForConnection = false;
                    }
                    break;
                }

                case (HeaderStatus::Client_RemoveDevice): {
                    uint32_t nRemovalID = 0;
                    msg >> nRemovalID;
                    mapObjects.erase(nRemovalID);
                    break;
                }

                case (HeaderStatus::Client_UpdateDevice): {
                    sDeviceNetPacket desc;
                    msg >> desc;
                    mapObjects.insert_or_assign(desc.nUniqueID, desc);
                    break;
                }
                }
            }
        }

        if (bWaitingForConnection) {
            Clear(olc::DARK_BLUE);
            DrawString({ 10, 10 }, "Waiting To Connect...", olc::WHITE);
            return true;
        }

        // Control of Player Object
        mapObjects[nPlayerID].vVel = { 0.0f, 0.0f, 0.0f };
        if (GetKey(olc::Key::W).bHeld)
            mapObjects[nPlayerID].vVel += hvr::math::vec3d { 0.0f, 0.0f, -1.0f };
        if (GetKey(olc::Key::S).bHeld)
            mapObjects[nPlayerID].vVel += hvr::math::vec3d { 0.0f, 0.0f, +1.0f };
        if (GetKey(olc::Key::A).bHeld)
            mapObjects[nPlayerID].vVel += hvr::math::vec3d { -1.0f, 0.0f, 0.0f };
        if (GetKey(olc::Key::D).bHeld)
            mapObjects[nPlayerID].vVel += hvr::math::vec3d { +1.0f, 0.0f, 0.0f };

        if (mapObjects[nPlayerID].vVel.mag2() > 0)
            mapObjects[nPlayerID].vVel = mapObjects[nPlayerID].vVel.norm() * 4.0f;

        // Update objects locally
        for (auto& object : mapObjects) {
            // Where will object be worst case?
            const auto tmp = object.second.vPos + object.second.vVel * fElapsedTime;
            olc::vf2d vPotentialPosition = olc::vf2d(tmp.x, tmp.z);

            // Extract region of world cells that could have collision this frame
            olc::vi2d vCurrentCell = olc::vi2d((object.second.vPos.floor()).x, (object.second.vPos.floor()).y);
            olc::vi2d vTargetCell = vPotentialPosition;
            olc::vi2d vAreaTL = (vCurrentCell.min(vTargetCell) - olc::vi2d(1, 1)).max({ 0, 0 });
            olc::vi2d vAreaBR = (vCurrentCell.max(vTargetCell) + olc::vi2d(1, 1)).min(vWorldSize);

            // Iterate through each cell in test area
            olc::vi2d vCell;
            for (vCell.y = vAreaTL.y; vCell.y <= vAreaBR.y; vCell.y++) {
                for (vCell.x = vAreaTL.x; vCell.x <= vAreaBR.x; vCell.x++) {
                    // Check if the cell is actually solid...
                    //	olc::vf2d vCellMiddle = vCell.floor();
                    if (sWorldMap[vCell.y * vWorldSize.x + vCell.x] == '#') {
                        // ...it is! So work out nearest point to future player position, around perimeter
                        // of cell rectangle. We can test the distance to this point to see if we have
                        // collided.

                        olc::vf2d vNearestPoint;
                        // Inspired by this (very clever btw)
                        // https://stackoverflow.com/questions/45370692/circle-rectangle-collision-response
                        vNearestPoint.x = std::max(float(vCell.x), std::min(vPotentialPosition.x, float(vCell.x + 1)));
                        vNearestPoint.y = std::max(float(vCell.y), std::min(vPotentialPosition.y, float(vCell.y + 1)));

                        // But modified to work :P
                        olc::vf2d vRayToNearest = vNearestPoint - vPotentialPosition;
                        float fOverlap = 0.5f - vRayToNearest.mag();
                        if (std::isnan(fOverlap))
                            fOverlap = 0; // Thanks Dandistine!

                        // If overlap is positive, then a collision has occurred, so we displace backwards by the
                        // overlap amount. The potential position is then tested against other tiles in the area
                        // therefore "statically" resolving the collision
                        if (fOverlap > 0) {
                            // Statically resolve the collision
                            vPotentialPosition = vPotentialPosition - vRayToNearest.norm() * fOverlap;
                        }
                    }
                }
            }

            // Set the objects new position to the allowed potential position
            object.second.vPos = hvr::math::vec3d(vPotentialPosition.x, 0, vPotentialPosition.y);
        }

        // Handle Pan & Zoom
        if (GetMouse(2).bPressed)
            tv.StartPan(GetMousePos());
        if (GetMouse(2).bHeld)
            tv.UpdatePan(GetMousePos());
        if (GetMouse(2).bReleased)
            tv.EndPan(GetMousePos());
        if (GetMouseWheel() > 0)
            tv.ZoomAtScreenPos(1.5f, GetMousePos());
        if (GetMouseWheel() < 0)
            tv.ZoomAtScreenPos(0.75f, GetMousePos());

        // Clear World
        Clear(olc::BLACK);

        // Draw World
        olc::vi2d vTL = tv.GetTopLeftTile().max({ 0, 0 });
        olc::vi2d vBR = tv.GetBottomRightTile().min(vWorldSize);
        olc::vi2d vTile;
        for (vTile.y = vTL.y; vTile.y < vBR.y; vTile.y++)
            for (vTile.x = vTL.x; vTile.x < vBR.x; vTile.x++) {
                if (sWorldMap[vTile.y * vWorldSize.x + vTile.x] == '#') {
                    tv.DrawRect(vTile, { 1.0f, 1.0f });
                    tv.DrawRect(olc::vf2d(vTile) + olc::vf2d(0.1f, 0.1f), { 0.8f, 0.8f });
                }
            }

        // Draw World Objects
        for (auto& object : mapObjects) {
            // Draw Boundary
            const auto tmp_pos = olc::vf2d(object.second.vPos.x, object.second.vPos.z);
            const auto tmp_vel = olc::vf2d((object.second.vVel).x, (object.second.vVel).z);

            tv.DrawCircle(tmp_pos, 0.5f);

            // Draw Velocity
            if (object.second.vVel.mag2() > 0)
                tv.DrawLine(tmp_pos, tmp_pos + tmp_vel.norm() * 0.5f, olc::MAGENTA);

            // Draw Name
            olc::vi2d vNameSize = GetTextSizeProp("ID: " + std::to_string(object.first));
            tv.DrawStringPropDecal(tmp_pos - olc::vf2d { vNameSize.x * 0.5f * 0.25f * 0.125f, -0.5f * 1.25f }, "ID: " + std::to_string(object.first), olc::BLUE, { 0.25f, 0.25f });
        }

        // Send player description
        olc::net::message<HeaderStatus> msg;
        msg.header.id = HeaderStatus::Client_UpdateDevice;
        msg << mapObjects[nPlayerID];
        Send(msg);

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        return true;
    }
};

int main()
{
    int choice;
    std::cout << "bench/demo? [0/1]\n";
    std::cin >> choice;
    if (choice) {
        char devicetype;

        std::cout << "device type? [t/c]";
        std::cin.clear();
        std::cin >> devicetype;

        std::unordered_map<char, DeviceType> types = {
            { 'c', DeviceType::ControllerViveLike },
            { 't', DeviceType::Tracker },
        };

        DeviceType device_type = DeviceType::Tracker;
        DeviceRole device_role = DeviceRole::Left;

        const auto res = types.find(devicetype);
        if (res != types.end()) {
            device_type = res->second;
        }

        if (device_type == DeviceType::Tracker) {
            device_role = DeviceRole::Neither;
        }

        MMOGame demo(device_type, device_role);
        if (demo.Construct(480, 480, 1, 1))
            demo.Start();
    } else {
        Benchmark test;
        test.Init();
        while (1) {
            test.Update();
        }
    }
    return 0;
}
