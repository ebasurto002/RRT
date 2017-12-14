/*
 * swNodeRx.cc
 *
 *  Created on: 13 dic. 2017
 *      Author: estre
 */
#include <string.h>
#include <omnetpp.h>
#include <paquete_m.h>

using namespace omnetpp;

//tipos de mensaje
short const ack = 1;
short const nack = 2;

class swNodeRx: public cSimpleModule {

    private:
        int numSeq;
    public:
        swNodeRx();
    protected:
        virtual void handleMessage(cMessage *msg);
        virtual void sndACK(int nSeq);
        virtual void sndNACK(int nSeq);
};

Define_Module(swNodeRx);

swNodeRx::swNodeRx(){
    numSeq = 0;
}

void swNodeRx::handleMessage(cMessage *msg){
    paquete *pqt = check_and_cast<paquete *>(msg);

    int rcvdSeq = pqt->getNumSeq();

    if(pqt->hasBitError()){
        sndNACK(rcvdSeq);
    }
    else{
        sndACK(rcvdSeq);
    }
}

void swNodeRx::sndNACK(int nSeq){
    paquete *pqt = new paquete("NACK",1);
    pqt->setBitLength(1);
    pqt->setType(nack);
    pqt->setNumSeq(nSeq);
    send(pqt,"out");
}
void swNodeRx::sndACK(int nSeq){
    paquete *pqt = new paquete("ACK",1);
        pqt->setBitLength(1);
        pqt->setType(ack);
        pqt->setNumSeq(nSeq);
        send(pqt,"out");
}




