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
}
void transbordoAChacao(int v){
    nEnter(m);
    while(EmptyFifoQueue(q_pargua)){
        nPrintf("esperando\n");
        nWaitCondition(no_empty_pargua);
        
    }
    Transbordador * my_t= (Transbordador *) GetObj(q_pargua);
    nExit(m);
    haciaChacao(my_t->id, v);
    if(EmptyFifoQueue(q_pargua)){
        nPrintf("vacio");
        haciaPargua(my_t->id, -1);
        nEnter(m);
        PushObj(q_pargua, my_t);  
        nSignalCondition(no_empty_pargua);
        nExit(m);
        
    }
    else{
        nEnter(m);
        PushObj(q_chacao,my_t);
        nPrintf("nuevo barco en chacao\n");
        nSignalCondition(no_empty_chacao);
        nExit(m);
    }

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
    nPrintf("finalizar");
    DestroyFifoQueue(q_chacao);
    DestroyFifoQueue(q_pargua); 
    nDestroyCondition(no_empty_chacao);
    nDestroyCondition(no_empty_pargua);
}