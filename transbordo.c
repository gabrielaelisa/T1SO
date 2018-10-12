#include <nSystem.h>
void finalizar();

typedef struct {
    int id;
}Transbordador;

nMonitor m ;//= nMakeMonitor();
nCondition no_empty_pargua;//  = nMakeCondition(m);
nCondition no_empty_chacao;// = nMakeCondition(m);
Transbordador * t_pargua;
Transbordador * t_chacao;
int en_pargua =0;
int en_chacao=0;
int next_pargua=0;
int next_chacao= 0;
int N;
int * num;

void inicializar(int p){
    t_pargua = (Transbordador *)nMalloc(sizeof(Transbordador)*p);
    t_chacao = (Transbordador *)nMalloc(sizeof(Transbordador)*p);
    for(int i=0; i<p ; i++){
        t_pargua[i] = Transbordador t;
        t.id =i;
    }
    en_pargua=p;
    N=p

}
void transbordoAChacao(int v){
    nEnter(m);
    while(en_pargua==0){
        nWaitCondition(no_empty_pargua);
    }
    Transbordador my_t= t_pargua[next_pargua];
    next_pargua= (next_pargua+1)%N;;
    en_pargua-=1;
    nExit(m);
    haciaChacao(my_t.id, v);
    nEnter(m);
    t_chacao[next_chacao]= my_t;
    en_chacao++;
    nSignalCondition(no_empty_chacao);
    nExit(m);

}
void transbordoAPargua(int v){
    nEnter(m);
    while(en_chacao==0){
        nWaitCondition(no_empty_chacao);
    }
    Transbordador* my_t= t_chacao[next_chacao];
    next_chacao= (next_chacao+1)%N;;
    en_chacao-=1;
    nExit(m);
    haciaChacao(my_t.id, v);
    nEnter(m);
    t_pargua[next_pargua]=mt_t;
    en_pargua++;
    nSignalCondition(no_empty_pargua);
    nExit(m);

}