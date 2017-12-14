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
        paquete *nuevoPqt;
    public:
        virtual ~injector();
    protected:
        virtual void handleMessage(cMessage *msg) override;
        virtual paquete * generaPaquete();
        virtual void intialize() override;
};

Define_Module(injector);

injector::~injector(){
    cancelAndDelete(nuevoPqt);
}

void injector::initialize(){
    startTime = 20;
    nuevoPqt = new paquete();
    seq = 0;
    scheduleAt(startTime, nuevoPqt);
}

void injector::handleMessage(cMessage *msg){
    paquete *pqt = generaPaquete();
}



