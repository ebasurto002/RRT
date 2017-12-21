/*
 * gbnNodeTx.cc
 *
 *  Created on: 20 dic. 2017
 *      Author: estre
 */
#include <string.h>
#include <omnetpp.h>
#include <paquete_m.h>
#include <algorithm>

using namespace omnetpp;

//Variables globales máquina de estados

short const idle = 0;
short const send_in = 1;
short const send_rep = 2;

//Tipos de respuesta
short const ack = 1;
short const nack = 2;

class gbnNodeTx: public cSimpleModule {
    private:
        int numPaquete;
        int txdpackets;
        int pck2Repeat;
        int totalRep;
        paquete *message;
        simtime_t timeout;
        cMessage *timeoutEvent;
        cMessage *sent; //Evento que indica cuándo se ha enviado un mensaje
        cChannel *txChannel;
        cQueue *txQueue;
        cQueue *nACKQueue;
        short status; //para indicar el estado

        //Para recoger estadísticas
        cLongHistogram thStat;
        cOutVector thVector;
    public:
        gbnNodeTx();
        virtual ~gbnNodeTx();
    protected:
        virtual void initialize() override;
        virtual void handleMessage() override;
        virtual void sendCopyOf();
};

Define_module(gbnNodeTx);

gbnNodeTx::gbnNodeTx(){
    status = idle;
    txQueue = NULL;
    nACKQueue = NULL;
    txChannel = NULL;
    message = NULL;
    sent = NULL;
    numPaquete = 0;
    txdpackets = 0;
    pck2Repeat = 0;
    totalRep = 0;
}
gbnNodeTx::~gbnNodeTx(){
    cancelAndDelete(sent);
    cancelAndDelete(timeoutEvent);
    delete(message);
    txQueue->~cQueue();
    nACKQueue->~cQueue();
}
void gbnNodeTx::initialize(){
    txChannel = gate("out")->getTransmissionChannel();
    sent = new cMessage("sent");
    txQueue = new cQueue("txQueue");
    nACKQueue = new cQueue("nACKQueue");
    WATCH(status);

}
void gbnNodeTx::handleMessage(cMessage *msg){

    if(msg == sent){
        //El evento que se recibe, indica que ya se ha enviado un paquete
        switch(status){
            case send_in://He mandado un paquete que no pertenecía a una retransmisión
                if(txQueue->isEmpty()){
                    status = idle;
                }
                else{
                    message = (paquete *)txQueue->pop();
                    paquete *copy = (paquete *) message->dup();
                    nACKQueue->insert(copy);
                    sendCopyOf(message);
                    status = send_in;
                }
                break;
            case send_rep:
                if(pck2Repeat == totalRep){
                    if(txQueue->isEmpty()){
                        status = idle;
                    }
                    else{
                        message = (paquete *)txQueue->pop();
                        paquete *copy = (paquete *) message->dup();
                        nACKQueue->insert(copy);
                        sendCopyOf(message);
                        status = send_in;
                    }
                }
                else{
                    message = (paquete *)nACKQueue->get(pck2Repeat++);
                    sendCopyOf(message);
                    status=send_rep;
                }
                break;
            default:
                break;
        }
    }
    else{
        //Ack, Nack o paquete de la fuente

        paquete *pckt = check_and_cast<paquete *>(msg);

        //Paquete de la fuente
        if(msg->arrivedOn("inSnd")){
            switch(status){
                case idle:
                    message = pckt;
                    paquete *copy = (paquete *)message ->dup();
                    nACKQueue->insert(copy);
                    sendCopyOf(message);
                    status = send_in;
                    break;
                case send_in:
                    txQueue->insert(pckt);
                    break;
                case send_rep:
                    txQueue->insert(pckt);
                    break;
                default:
                    break;
            }

        }
        else{
            //Ack o nack
            switch(pckt->getType()){
                case ack:
            }
        }

    }
}




