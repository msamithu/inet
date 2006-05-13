//
// Copyright (C) 2006 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//


#include "Ieee80211MgmtAP.h"
#include "Ieee802Ctrl_m.h"
#include "EtherFrame_m.h"


Define_Module(Ieee80211MgmtAP);

static std::ostream& operator<< (std::ostream& os, const Ieee80211MgmtAP::STAInfo& sta)
{
    os << "state:" << sta.status;
    return os;
}

void Ieee80211MgmtAP::initialize(int stage)
{
    Ieee80211MgmtAPBase::initialize(stage);

    if (stage==0)
    {
        ssid = par("ssid").stringValue();
        beaconInterval = par("beaconInterval");
        WATCH(ssid);
        WATCH(beaconInterval);
        WATCH_MAP(staList);
    }
}

void Ieee80211MgmtAP::handleTimer(cMessage *msg)
{
    //TBD
}

void Ieee80211MgmtAP::handleUpperMessage(cMessage *msg)
{
    // TBD check we really have a STA with the dest address

    // convert Ethernet frames arriving from MACRelayUnit (i.e. from
    // the AP's other Ethernet or wireless interfaces)
    Ieee80211DataFrame *frame = convertFromEtherFrame(check_and_cast<EtherFrame *>(msg));
    sendOrEnqueue(frame);
}

void Ieee80211MgmtAP::receiveChangeNotification(int category, cPolymorphic *details)
{
    Enter_Method_Silent();
    //TBD
}

Ieee80211MgmtAP::STAInfo *Ieee80211MgmtAP::lookupSenderSTA(Ieee80211ManagementFrame *frame)
{
    STAList::iterator it = staList.find(frame->getTransmitterAddress());
    return it==staList.end() ? NULL : &(it->second);
}

void Ieee80211MgmtAP::sendManagementFrame(Ieee80211ManagementFrame *frame, STAInfo *sta)
{
    frame->setFromDS(true);
    frame->setReceiverAddress(sta->address);
    //TBD set other fields?
    sendOrEnqueue(frame); //FIXME or do mgmt frames have priority?
}

void Ieee80211MgmtAP::handleDataFrame(Ieee80211DataFrame *frame)
{
    // check toDS bit
    if (!frame->getToDS())
    {
        // looks like this is not for us - discard
        delete frame;
        return;
    }

    // look up destination address in our STA list
    STAList::iterator it = staList.find(frame->getAddress3());
    EV << it->first;
    if (it==staList.end())
    {
        // not our STA -- pass up frame to relayUnit for LAN bridging if we have one
        if (hasRelayUnit)
            send(createEtherFrame(frame), "uppergateOut");
        else
            delete frame;
    }
    else
    {
        // dest address is our STA, but is it already associated?
        if (it->second.status == ASSOCIATED)
            distributeReceivedDataFrame(frame); // send it out to the destination STA
        else
            delete frame;
    }
}

void Ieee80211MgmtAP::handleAuthenticationFrame(Ieee80211AuthenticationFrame *frame)
{
    //TBD
}

void Ieee80211MgmtAP::handleDeauthenticationFrame(Ieee80211DeauthenticationFrame *frame)
{
    //TBD
}

void Ieee80211MgmtAP::handleAssociationRequestFrame(Ieee80211AssociationRequestFrame *frame)
{
    STAInfo *sta = lookupSenderSTA(frame);
    if (!sta || sta->status==NOT_AUTHENTICATED || sta->status==ASSOCIATED)
        ;// TBD send error, drop frame and return
    if (sta->status==ASSOCIATED)
        ;// TBD send error, drop frame and return

    // mark STA as associated, and send response
    sta->status = ASSOCIATED;

    Ieee80211AssociationResponseFrame *resp = new Ieee80211AssociationResponseFrame("AssocResp(OK)");
    sendManagementFrame(resp, sta);
}

void Ieee80211MgmtAP::handleAssociationResponseFrame(Ieee80211AssociationResponseFrame *frame)
{
    dropManagementFrame(frame);
}

void Ieee80211MgmtAP::handleReassociationRequestFrame(Ieee80211ReassociationRequestFrame *frame)
{
    //TBD
}

void Ieee80211MgmtAP::handleReassociationResponseFrame(Ieee80211ReassociationResponseFrame *frame)
{
    dropManagementFrame(frame);
}

void Ieee80211MgmtAP::handleDisassociationFrame(Ieee80211DisassociationFrame *frame)
{
    //TBD
}

void Ieee80211MgmtAP::handleBeaconFrame(Ieee80211BeaconFrame *frame)
{
    dropManagementFrame(frame);
}

void Ieee80211MgmtAP::handleProbeRequestFrame(Ieee80211ProbeRequestFrame *frame)
{
    //TBD
}

void Ieee80211MgmtAP::handleProbeResponseFrame(Ieee80211ProbeResponseFrame *frame)
{
    dropManagementFrame(frame);
}

