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
    Transbordador * my_t;
    if (EmptyFifoQueue(q_pargua)){
        my_t = (Transbordador *) GetObj(q_chacao);
        nNotifyAll(m);
        nExit(m);
        haciaPargua(my_t->id, -1);
        haciaChacao(my_t->id, v);
    }
    else{
        my_t= (Transbordador *) GetObj(q_pargua);
        nNotifyAll(m);
        nExit(m);
        haciaChacao(my_t->id, v);
    }
    nEnter(m);
    PushObj(q_chacao,my_t);
    nNotifyAll(m);
    nExit(m);

}
void transbordoAPargua(int v){
    nEnter(m);
    nPrintf("primer transbordo\n");
    Transbordador * my_t;
    if (EmptyFifoQueue(q_chacao)){
        nPrintf("fifo vacía\n");
        my_t = (Transbordador *) GetObj(q_pargua);
        nPrintf("my t\n");
        nNotifyAll(m);
        nExit(m);
        haciaChacao(my_t->id, -1);
        haciaPargua(my_t->id, v);
    }
    else{
        my_t= (Transbordador *) GetObj(q_chacao);
        nNotifyAll(m);
        nExit(m);
        haciaPargua(my_t->id, v);
    }
    nEnter(m);
    PushObj(q_pargua,my_t);
    nNotifyAll(m);
    nExit(m);


}

void finalizar(){
    nPrintf("finalizar");
    DestroyFifoQueue(q_chacao);
    DestroyFifoQueue(q_pargua); 
    nDestroyCondition(no_empty_chacao);
    nDestroyCondition(no_empty_pargua);
}