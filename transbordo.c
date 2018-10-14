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
nCondition no_empty_pargua;//  = nMakeCondition(m);
nCondition no_empty_chacao;// = nMakeCondition(m);
nCondition no_empty;

void inicializar(int p){
    q_pargua=MakeFifoQueue();
    q_chacao=MakeFifoQueue();
    m= nMakeMonitor();
    no_empty_chacao= nMakeCondition(m);
    no_empty_pargua= nMakeCondition(m);
    no_empty= nMakeCondition(m);
    for(int i=0; i<p ; i++){
        Transbordador * t = nMalloc(sizeof(Transbordador));
        t->id= i;
        PutObj(q_pargua, t);
    }
}
void transbordoAChacao(int v){
    nEnter(m);
    nPrintf("norteno toma barco\n");
    Transbordador * my_t;

    while(EmptyFifoQueue(q_chacao)&& EmptyFifoQueue(q_pargua)){
        nPrintf("norteno esperando\n");
        nWaitCondition(no_empty);}

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
        nPrintf("isleno esperando\n");
        nWaitCondition(no_empty);}

    if (EmptyFifoQueue(q_chacao)){
        my_t = (Transbordador *) GetObj(q_pargua);
        nExit(m);
        haciaChacao(my_t->id, -1);
        haciaPargua(my_t->id, v);
        nPrintf("isleno llega a destino\n");
    }
    else{
        my_t= (Transbordador *) GetObj(q_chacao);
        nExit(m);
        haciaPargua(my_t->id, v);
        nPrintf("isleno llega a destino\n");
    }
    nEnter(m);
    PushObj(q_pargua,my_t);
    nPrintf("nuevo barco en pargua\n");
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