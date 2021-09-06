//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

// dll export symbol
#ifndef  tictoc_API
#  if defined( tictoc_EXPORT)
#    define  tictoc_API  OPP_DLLEXPORT
#  elif defined( tictoc_IMPORT)
#    define  tictoc_API  OPP_DLLIMPORT
#  else
#    define  tictoc_API
#  endif
#endif

using namespace omnetpp;

/**
 * Same as Txc4.
 */
class tictoc_API Txc5 : public cSimpleModule
{
  private:
    int counter;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Txc5);

void Txc5::initialize()
{
    // Initialize the counter with the "limit" module parameter, declared
    // in the NED file (tictoc4.ned).
    counter = par("limit");

    // we no longer depend on the name of the module to decide
    // whether to send an initial message
    if (par("sendMsgOnInit").boolValue() == true) {
        EV << "Sending initial message\n";
        cMessage *msg = new cMessage("tictocMsg");
        send(msg, "out");
    }
}

void Txc5::handleMessage(cMessage *msg)
{
    counter--;
    if (counter == 0) {
        EV << getName() << "'s counter reached zero, deleting message\n";
        delete msg;
    }
    else {
        EV << getName() << "'s counter is " << counter << ", sending back message\n";
        send(msg, "out");
    }
}

