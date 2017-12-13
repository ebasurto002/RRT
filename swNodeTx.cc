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

//Variables globales máquina de estados

short const idle = 0;
short const sending = 1;
short const waitAck = 2;


class swNodeTx: public cSimpleModule {

    private:
        int numPaquete;
        int txdpackets;
        simtime_t timeout;
        cMessage *timeoutEvent;
        cMessage *sent; //Evento que indica cuándo se ha enviado un mensaje
        paquete *pckt;
        cChannel *txChannel;
        cQueue *txQueue;
        short status; //para indicar el estado
    public:
        swNodeTx();
        virtual ~swNodeTx();
    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void sendCopyOf(cMessage *msg);
        virtual void finish() override;
};

Define_module(swNodeTx);

swNodeTx::swNodeTx(){
    /*constructor*/
    status = idle;
    txChannel = NULL;
    txQueue = NULL;
    sent = NULL;
    pckt = NULL;
}

swNodeTx::~swNodeTx(){
    /*Destructor*/
    cancelAndDelete(sent);
    delete pckt;
    txQueue->~cQueue();
}


void swNodeTx::initialize(){

    timeout = par("timeout");
    timeoutEvent = new cMessage("timeoutEvent");

    txChannel = gate("out")->getTransmissionChannel();
    txQueue = new cQueue("txQueue");
    sent = new cMessage("sent");
    WATCH(status);

    numPaquete = 0;
    txdpackets = 0;

}
void swNodeTx::handleMessage(cMessage *msg){

    //Caso de que expire el temporizador
    if(msg == timeoutEvent){
        EV<<"Timeout expired, resending packet..." << endl;
        bubble("Tx timeout. Gotta resend message");
        sendCopyOf(pckt);
        txdpackets++;

    }
    if(msg == sent){
        status = waitAck;
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

void swNodeTx::sendCopyOf(cMessage *msg){

}

void swNodeTx::finish(){

}



