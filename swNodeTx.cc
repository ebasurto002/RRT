/*
 * swSim.cc
 *
 *  Created on: 13 dic. 2017
 *      Author: estre
 */

#include <string.h>
#include <omnetpp.h>
#include <paquete_m.h>

using namespace omnetpp;

class swNodeTx: public cSimpleModule {

    private:
        int numPaquete;
        int txdpackets;
        simtime_t timeout;
        cMessage *timeoutEvent;
        struct arrivals{
            double llegadas;
            paquete *packet;
        };
        arrivals *arr;
    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void finish() override;
};

void swNodeTx::initialize(){

    timeout = par("timeout");
    timeoutEvent = new cMessage("timeoutEvent");

    //Construcción de la lista de paquetes que tiene el nodo para tx
    arr = new arrivals[(int)par("nPaquetes")];

    for(int i = 0; i<(int)par("nPaquetes"); i++){

        arr[i].llegadas = (double)par("interArrivalsTime");

        if(i!=0){
            arr[i].llegadas += arr[i-1].llegadas;
        }

        paquete *msg = new paquete("mensaje");
        msg -> setByteLength((int)par("longPaquetes"));
        msg -> setNumSeq(i);

        arr[i].packet = msg;
    }

    numPaquete = 0;
    txdpackets = 0;

    send(arr[0].packet -> dup(),"out");
    scheduleAt(simTime()+timeout, timeoutEvent);

}
void swNodeTx::handleMessage(cMessage *msg){

}



