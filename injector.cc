/*
 * injector.cc
 *
 *  Created on: 14 dic. 2017
 *      Author: estre
 */
#include <string.h>
#include <omnetpp.h>
#include <paquete_m.h>

using namespace omnetpp;

class injector: public cSimpleModule{
    private:
        int numSeq;
        simtime_t startTime;
        int npaquetes;
        int sentPackets;
        double bitLength;
        paquete *nuevoPqt;
    public:
        virtual ~injector();
    protected:
        virtual void handleMessage(cMessage *msg) override;
        virtual paquete * generaPaquete();
        virtual void initialize() override;
};

Define_Module(injector);

injector::~injector(){
    cancelAndDelete(nuevoPqt);
}

void injector::initialize(){
    startTime = 20;
    nuevoPqt = new paquete();
    numSeq = 0;
    npaquetes = (int)par("n_paquetes");
    sentPackets = 0;
    scheduleAt(startTime, nuevoPqt);
}

void injector::handleMessage(cMessage *msg){
    paquete *pqt = generaPaquete();
    send(pqt,"outSnd");
    sentPackets++;
    if(sentPackets < npaquetes){
        scheduleAt(simTime()+exponential(1/9.375),nuevoPqt);
    }
}
paquete * injector::generaPaquete(){
    char nombrePaquete[15];
    sprintf(nombrePaquete,"msg-%d",numSeq);
    paquete *msg = new paquete(nombrePaquete,0);
    bitLength = 10240;
    msg -> setBitLength(bitLength);
    msg -> setNumSeq(numSeq);
    numSeq++;
    return msg;
}



