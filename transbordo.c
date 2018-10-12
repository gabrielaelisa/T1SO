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

void inicializar(int p){
    q_pargua=MakeFifoQueue();
    q_chacao=MakeFifoQueue();
    m= nMakeMonitor();
    no_empty_chacao= nMakeCondition(m);
    no_empty_pargua= nMakeCondition(m);
    for(int i=0; i<p ; i++){
        Transbordador * t = nMalloc(sizeof(Transbordador));
        t->id= i;
        PutObj(q_pargua, t);
    }
    nPrintf("inicializa correctamente\n");
}
void transbordoAChacao(int v){
    nEnter(m);
    while(EmptyFifoQueue(q_pargua)){
        nWaitCondition(no_empty_pargua);
        nPrintf("waiting in pargua\n");
    }
    Transbordador * my_t= (Transbordador *) GetObj(q_pargua);
    nPrintf("%d", my_t->id);
    nPrintf("\n");
    nExit(m);
    nPrintf("fue a chacao\n");
    haciaChacao(my_t->id, v);
    nPrintf("llego a chacao\n");
    nEnter(m);
    if(EmptyFifoQueue(q_pargua)){
        haciaPargua(my_t->id, -1);
        PushObj(q_pargua, my_t);  
        nSignalCondition(no_empty_pargua);
        nPrintf("nuevo barco en pargua\n");
    }
    else{
        PushObj(q_chacao,my_t);
        nPrintf("nuevo barco en chacao\n");
        nSignalCondition(no_empty_chacao);
    }
    nExit(m);
    return;

}
void transbordoAPargua(int v){
    nEnter(m);
    nPrintf("salio a pargua\n");
    while(EmptyFifoQueue(q_chacao)){
        nWaitCondition(no_empty_chacao);
        nPrintf("waiting en chacao");
    }
    Transbordador * my_t= (Transbordador *) GetObj(q_chacao);
    nExit(m);
    haciaPargua(my_t->id, v);
    nEnter(m);
    PushObj(q_pargua, my_t);
    nSignalCondition(no_empty_pargua);
    nExit(m);

}

void finalizar(){
    DestroyFifoQueue(q_chacao);
    DestroyFifoQueue(q_pargua); 
    nDestroyCondition(no_empty_chacao);
    nDestroyCondition(no_empty_pargua);
}