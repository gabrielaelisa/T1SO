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
volatile int esperando_en_pargua=0;
volatile int esperando_en_chacao=0;

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
    esperando_en_pargua+=1;
    nPrintf("norteno pide barco\n");
    Transbordador * my_t;
    // mi puerto está vacío y el otro puerto también o alguien espera
    while(EmptyFifoQueue(q_pargua) && (EmptyFifoQueue(q_chacao) || esperando_en_chacao>0)){
        nPrintf("norteno esperando\n");
        nWaitCondition(no_empty_pargua);
        }

    if (EmptyFifoQueue(q_pargua)){
        my_t = (Transbordador *) GetObj(q_chacao);
        esperando_en_pargua-=1;
        nPrintf("norteno recibe barco\n");
        nExit(m);
        haciaPargua(my_t->id, -1);
        haciaChacao(my_t->id, v);
        nPrintf("norteno llega aun no avisa\n");
    }
    else{
        my_t= (Transbordador *) GetObj(q_pargua);
        esperando_en_pargua-=1;
        nPrintf("norteno recibe barco\n");
        nExit(m);
        haciaChacao(my_t->id, v);
        nPrintf("norteno llega aun no avisa\n");
    }
    nEnter(m);
    PushObj(q_chacao,my_t);
    nPrintf("norteno llega a destino\n");
    nPrintf("nuevo barco en chacao\n");
    nSignalCondition(no_empty_chacao);
    nExit(m);

}
void transbordoAPargua(int v){
    nEnter(m);
    esperando_en_chacao+=1;
    nPrintf("isleno pide barco\n");
    Transbordador * my_t;
    // mi puerto está vacío y el otro puerto también o alguien espera
    while(EmptyFifoQueue(q_chacao) && (EmptyFifoQueue(q_pargua)|| esperando_en_pargua>0)){
        nPrintf("isleno esperando\n");
        nWaitCondition(no_empty_chacao);}

    nPrintf("isleno deja de esperar\n");
    if (EmptyFifoQueue(q_chacao)){
        
        my_t = (Transbordador *) GetObj(q_pargua);
        esperando_en_chacao-=1;
        nExit(m);
        nPrintf("isleno toma barco\n");
        haciaChacao(my_t->id, -1);
        haciaPargua(my_t->id, v);
        nPrintf("isleno llega aun no avisa\n");
    }
    else{
        
        my_t= (Transbordador *) GetObj(q_chacao);
        esperando_en_chacao-=1;
        nExit(m);
        nPrintf("isleno toma barco\n");
        haciaPargua(my_t->id, v);
        nPrintf("isleno llega aun no avisa\n");
    }
    nEnter(m);
    PushObj(q_pargua,my_t);
    nPrintf("isleno llega a destino\n");
    nPrintf("nuevo barco en pargua\n");
    nSignalCondition(no_empty_pargua);
    nExit(m);


}

void finalizar(){
    nPrintf("finalizar");
    while(!EmptyFifoQueue(q_chacao))
    {
        Transbordador *t= (Transbordador *) GetObj(q_chacao);
        nFree(t);
    }
      while(!EmptyFifoQueue(q_pargua))
    {
        Transbordador *t= (Transbordador *) GetObj(q_pargua);
        nFree(t);
    }
    DestroyFifoQueue(q_chacao);
    DestroyFifoQueue(q_pargua); 
    nDestroyCondition(no_empty_chacao);
    nDestroyCondition(no_empty_pargua);
    nDestroyMonitor(m);
}