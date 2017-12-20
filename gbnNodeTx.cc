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
        paquete *message;
        simtime_t timeout;
        cMessage *timeoutEvent;
        cMessage *sent; //Evento que indica cuándo se ha enviado un mensaje
        paquete *pckt;
        cChannel *txChannel;
        cQueue *txQueue;
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




