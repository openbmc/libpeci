/*
// Copyright (c) 2021 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/
#include <peci.h>

#include <boost/asio/io_context.hpp>
#include <iostream>
#include <sdbusplus/asio/object_server.hpp>

int main()
{
    boost::asio::io_context io;
    std::shared_ptr<sdbusplus::asio::connection> conn;
    std::shared_ptr<sdbusplus::asio::object_server> server;

    // setup connection to dbus
    conn = std::make_shared<sdbusplus::asio::connection>(io);

    conn->request_name("com.intel.peci");
    server = std::make_shared<sdbusplus::asio::object_server>(conn);

    // Send Raw PECI Interface
    std::shared_ptr<sdbusplus::asio::dbus_interface> ifaceRawPeci =
        server->add_interface("/com/intel/peci", "com.intel.Protocol.PECI.Raw");

    // Send a Raw PECI command
    ifaceRawPeci->register_method(
        "Send", [](const std::string& peciDev,
                   const std::vector<std::vector<uint8_t>>& rawCmds) {
            peci_SetDevName(const_cast<char*>(peciDev.c_str()));
            // D-Bus will time out after too long, so set a deadline for when to
            // abort the PECI commands (at 25s, it mostly times out, at 24s it
            // doesn't, so use 23s to be safe)
            constexpr int peciTimeout = 23;
            std::chrono::steady_clock::time_point peciDeadline =
                std::chrono::steady_clock::now() +
                std::chrono::duration<int>(peciTimeout);
            std::vector<std::vector<uint8_t>> rawResp;
            rawResp.resize(rawCmds.size());
            for (size_t i = 0; i < rawCmds.size(); i++)
            {
                const std::vector<uint8_t>& rawCmd = rawCmds[i];
                // If the commands are taking too long, return early to avoid a
                // D-Bus timeout
                if (std::chrono::steady_clock::now() > peciDeadline)
                {
                    std::cerr << peciTimeout
                              << " second deadline reached.  Aborting PECI "
                                 "commands to avoid a timeout\n";
                    break;
                }

                if (rawCmd.size() < 3)
                {
                    peci_SetDevName(NULL);
                    throw std::invalid_argument("Command Length too short");
                }
                rawResp[i].resize(rawCmd[2]);
                peci_raw(rawCmd[0], rawCmd[2], &rawCmd[3], rawCmd[1],
                         rawResp[i].data(), rawResp[i].size());
            }
            peci_SetDevName(NULL);
            return rawResp;
        });
    ifaceRawPeci->initialize();

    io.run();

    return 0;
}
