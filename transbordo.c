#include <nSystem.h>
#include <stdio.h>
#include <fifoqueues.h>
#include "transbordo.h"

typedef struct {
    int id;
}Transbordador;

FifoQueue q_pargua;
FifoQueue q_chacao;
nMonitor m ;//= nMakeMonitor();
//nCondition no_empty_pargua;//  = nMakeCondition(m);
//nCondition no_empty_chacao;// = nMakeCondition(m);
volatile int waiting_in_pargua =0;
volatile int waiting_in_chacao= 0;

void inicializar(int p){
    q_pargua=MakeFifoQueue();
    q_chacao=MakeFifoQueue();
    m= nMakeMonitor();
    //no_empty_chacao= nMakeCondition(m);
    //no_empty_pargua= nMakeCondition(m);
    //no_empty= nMakeCondition(m);
    for(int i=0; i<p ; i++){
        Transbordador * t = nMalloc(sizeof(Transbordador));
        t->id= i;
        PutObj(q_pargua, t);
    }
}

/*
void exitTansbordador(FifoQueue* q, Transbordador * my_t){
    nEnter(m);
    PushObj(*q, my_t);
    nPrintf("nuevo barco en pargua\n");
    nNotifyAll(m);
    nExit(m);

}*/

void transbordoAChacao(int v){
    nEnter(m);
    waiting_in_pargua+=1;
    nPrintf("norteno toma barco\n");
    Transbordador * my_t;
    while(EmptyFifoQueue(q_pargua)){

        if (!(EmptyFifoQueue(q_chacao) && (waiting_in_chacao>0)))
        {
            my_t = (Transbordador *) GetObj(q_chacao);
            nNotifyAll(m);
            nExit(m);
            haciaPargua(my_t->id, -1);
            haciaChacao(my_t->id, v);

            nEnter(m);
            waiting_in_pargua-=1;
            PushObj(q_chacao,my_t);
            nNotifyAll(m);
            nExit(m);
           
        }
        else{      
            nWait(m);
            }
        
    }
    my_t= (Transbordador *) GetObj(q_pargua);
    nNotifyAll(m);
    nExit(m);

    haciaChacao(my_t->id, v);

    nEnter(m);
    waiting_in_pargua-=1;
    PushObj(q_chacao,my_t);
    nNotifyAll(m);
    nExit(m);
    nPrintf("norteno llega a destino\n");

}
void transbordoAPargua(int v){
    nEnter(m);
    nPrintf("isleno toma barco\n");
    Transbordador * my_t;
    waiting_in_chacao+=1;
    while(EmptyFifoQueue(q_chacao)){

        if (!(EmptyFifoQueue(q_pargua) && (waiting_in_pargua>0))){
            my_t = (Transbordador *) GetObj(q_pargua);
            nNotifyAll(m);
            nExit(m);
            haciaChacao(my_t->id, -1);
            haciaPargua(my_t->id, v);

            nEnter(m);
            waiting_in_chacao-=1;
            PushObj(q_pargua,my_t);
            nNotifyAll(m);
            nExit(m);
            }

        else{      
            nWait(m);
            }
    }

    my_t= (Transbordador *) GetObj(q_chacao);
    nNotifyAll(m);
    nExit(m);
    haciaPargua(my_t->id, v);

    nEnter(m);
    waiting_in_chacao-=1;
    PushObj(q_pargua,my_t);
    nNotifyAll(m);
    nExit(m);
}



void finalizar(){
    nPrintf("finalizar");
    DestroyFifoQueue(q_chacao);
    DestroyFifoQueue(q_pargua); 
    //nDestroyCondition(no_empty_chacao);
    //nDestroyCondition(no_empty_pargua);
}