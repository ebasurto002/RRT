/*
 * swSim.cc
 *
 *  Created on: 13 dic. 2017
 *      Author: estre
 */

#include <string.h>
#include <omnetpp.h>
#include <paquete_m.h>
#include <algorithm>

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
        cMessage *sendEvent;
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
    sendEvent = new cMessage("sendEvent");
    scheduleAt(arr[0].llegadas,sendEvent);

}
void swNodeTx::handleMessage(cMessage *msg){

    //Caso de que expire el temporizador
    if(msg == timeoutEvent){
        EV<<"Timeout expired, resending packet..." << endl;
        send(arr[numPaquete].packet -> dup(),"out");
        scheduleAt(simTime()+timeout,timeoutEvent);
        txdpackets++;

    }

    //Caso de que se avise al nodo de que tiene un paquete para enviar
    if(msg == sendEvent){
        send(arr[numPaquete].packet -> dup(),"out");
        scheduleAt(simTime()+timeout,timeoutEvent);
        txdpackets++;
    }
    else{
        //Caso de haberse perdido el ACK
        if(uniform(0,1)<par("pErrACK").doubleValue()){
            EV<<"ACK Lost"<<endl;
            bubble("ACK Lost");
        }
        //Caso de que vaya bien
        else{
            EV<<"Cancelling timeout"<<endl;
            cancelEvent(timeoutEvent);

            numPaquete++;
            if(numPaquete < (int) par("nPaquetes")){
                //Hay que mirar si hay paquetes en la cola o aún no han llegado. Como la simulación de Stop&Wait de Mathematica
                double time = std::max(simTime().dbl(),arr[numPaquete].llegadas);
                scheduleAt(time,sendEvent);
            }
        }
        delete msg;
    }

}

void swNodeTx::finish(){

}



