#include <nSystem.h>
#include <stdio.h>
#include <fifoqueues.h>
#include "transbordo.h"




nSem * b_pargua;
nSem * b_chacao;
int * transbordador;
volatile int k;
void inicializar(int p){
    
    for(int i=0; i<p ; i++){
      b_pargua[i]=nMakeSem(1);
      b_chacao[i]= nMakeSem(1);
      transbordador[i]=i
    }
    k=p-1;
}
void transbordoAChacao(int v){
    nWaitSem(b_pargua[v%k]);
    
    Transbordador * my_t;

    while(EmptyFifoQueue(q_chacao)&& EmptyFifoQueue(q_pargua))
        nWaitCondition(no_empty);

    if (EmptyFifoQueue(q_pargua)){
        my_t = (Transbordador *) GetObj(q_chacao);
        nExit(m);
        haciaPargua(my_t->id, -1);
        haciaChacao(my_t->id, v);
        nPrintf("norteno llega a destino\n");
    }
    else{
        my_t= (Transbordador *) GetObj(q_pargua);
        nExit(m);
        haciaChacao(my_t->id, v);
        nPrintf("norteno llega a destino\n");
    }
    nEnter(m);
    PushObj(q_chacao,my_t);
    nPrintf("nuevo barco en chacao\n");
    nSignalCondition(no_empty);
    nExit(m);

}
void transbordoAPargua(int v){
    nEnter(m);
    nPrintf("isleno toma barco\n");
    Transbordador * my_t;
    while(EmptyFifoQueue(q_chacao)&& EmptyFifoQueue(q_pargua)){
        nPrintf("esperando\n");
        nWaitCondition(no_empty);}

    if (EmptyFifoQueue(q_chacao)){
        my_t = (Transbordador *) GetObj(q_pargua);
        nExit(m);
        nPrintf("exit mutex\n");
        haciaChacao(my_t->id, -1);
        nPrintf("isleno llega a destino\n");
        haciaPargua(my_t->id, v);
    }
    else{
        my_t= (Transbordador *) GetObj(q_chacao);
        nExit(m);
        nPrintf("isleno llega a destino\n");
        haciaPargua(my_t->id, v);
    }
    nEnter(m);
    PushObj(q_pargua,my_t);
    nSignalCondition(no_empty);
    nExit(m);


}

void finalizar(){
    nPrintf("finalizar");
    DestroyFifoQueue(q_chacao);
    DestroyFifoQueue(q_pargua); 
    nDestroyCondition(no_empty_chacao);
    nDestroyCondition(no_empty_pargua);
}