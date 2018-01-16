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

//Variables globales m�quina de estados

short const idle = 0;
short const sending = 1;
short const waitAck = 2;

//Tipos de respuesta
short const ack = 1;
short const nack = 2;


class swNodeTx: public cSimpleModule {

    private:
        int numPaquete;
        int txdpackets;
        paquete *message;
        simtime_t timeout;
        cMessage *timeoutEvent;
        cMessage *sent; //Evento que indica cu�ndo se ha enviado un mensaje
        paquete *pckt;
        cChannel *txChannel;
        cQueue *txQueue;
        short status; //para indicar el estado
        //Para recoger estad�sticas
        cLongHistogram thStat;
        cOutVector thVector;
    public:
        swNodeTx();
        virtual ~swNodeTx();
    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void sendCopyOf(paquete *msg);
        virtual void finish() override;
};

Define_Module(swNodeTx);

swNodeTx::swNodeTx(){
    /*constructor*/
    status = idle;
    txChannel = NULL;
    txQueue = NULL;
    sent = NULL;
    pckt = NULL;
    message = NULL;
}

swNodeTx::~swNodeTx(){
    /*Destructor*/
    cancelAndDelete(sent);
    delete(pckt);
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

    thStat.setName("Throughput Statistics");
    thStat.setRangeAutoUpper(0, 10, 1.5);
    thVector.setName("throughput");

}
void swNodeTx::handleMessage(cMessage *msg){

    //Caso de que expire el temporizador
    if(msg == timeoutEvent){
        EV<<"Timeout expired, resending packet..." << endl;
        bubble("Tx timeout. Gotta resend message");
        sendCopyOf(message);
    }

    else if(msg == sent){
        status = waitAck;
        scheduleAt(simTime()+timeout,timeoutEvent);
        txdpackets++;
    }
    else{

        //Reibir paquete de fuera
        paquete *pqt = check_and_cast<paquete *>(msg);
        //Switch con cases para los diferentes estados
        if(msg->arrivedOn("inSnd")){
            switch(status){
                case idle:
                    message = pqt;
                    sendCopyOf(message);
                    break;
                case sending:
                    txQueue->insert(pqt);
                    break;
                case waitAck:
                    txQueue->insert(pqt);
                    break;
                default:
                    break;
            }
        }
        else{
            EV<< "Mensaje recibido desde el extremo Rx"<<endl;
            //El paquete llega desde el otro extremo. Es decir, es un ACK o un NACK
            switch(pqt ->getType()){
                case ack:
                    cancelEvent(timeoutEvent);
                    numPaquete++;
                    if(txQueue->isEmpty()){
                        status = idle;

                    }
                    else{

                        delete(message);
                        message = (paquete *)txQueue->pop();
                        sendCopyOf(message);
                    }
                    break;
                case nack:
                    cancelEvent(timeoutEvent);
                    sendCopyOf(message);
                    break;
                default:
                    break;
            }
        }
        //delete(message);
    }
    double throughput = numPaquete/simTime();
    thVector.record(throughput);
    thStat.collect(throughput);

}

void swNodeTx::sendCopyOf(paquete *msg){
    paquete *copy = (paquete *)msg->dup();
    send(copy,"out");
    status = sending;
    simtime_t txFinished = txChannel->getTransmissionFinishTime();
    scheduleAt(txFinished,sent);

}

void swNodeTx::finish(){
//Cosas de las estad�sticas
    double pkgErrorRate = (1-(double)numPaquete/(double)txdpackets);
    EV<<"Package error rate: "<<pkgErrorRate<<endl;
    EV<<"Throughput: "<< numPaquete/simTime()<<endl;
    thStat.recordAs("Throughput");

}



