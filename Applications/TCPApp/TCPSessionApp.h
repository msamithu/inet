//
// Copyright 2004 Andras Varga
//
// This library is free software, you can redistribute it and/or modify
// it under  the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation;
// either version 2 of the License, or any later version.
// The library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//

#ifndef __TCPSESSIONAPP_H_
#define __TCPSESSIONAPP_H_

#include <vector>
#include <omnetpp.h>
#include "TCPSocket.h"


/**
 * Single-connection TCP application.
 */
class TCPSessionApp : public cSimpleModule
{
  protected:
    struct Command
    {
        simtime_t tSend;
        int numBytes;
    };
    typedef std::vector<Command> CommandVector;
    CommandVector commands;

    cQueue queue;

  protected:
    void parseScript(const char *script);

  public:
    Module_Class_Members(TCPSessionApp, cSimpleModule, 16384);
    virtual void activity();
    virtual void finish();
};

#endif

