/* Esercizio 5

In un programma ogni thread esegue il seguente codice:

void *thread(void *arg) {
	int s_voto + rand()%2;

	vota(s_voto);

	if(s_voto == risultato()) printf("Ho vinto!\n");
	else printf("Ho perso!\n");

	pthread_exit(0);
}

Quindi ogni thread:
- Esprime un s_voto che può essere 0 o 1, invocando la funzione vota(), la quale registra il s_voto di una struttura dati condivisa che per comodità chiameremo urna.

- Aspetta l'esito della votazione indicando la funzione risultato(), la quale controlla l'urna e ritorna 0 o 1 a seconda che ci sia una maggioranza di voti 0 oppure di voti 1;

- Se l'esito della votazione è uguale al proprio s_voto, stampa a video la stringa "Ho vinto", altrimenti stampa la stringa ho perso.

Implementare la seguente struttura dati:

struct {
	...
} urna;

e le funzioni:

void vota(int v);

int risultato(void);

In modo che i thread si comportino come segue:

Se l'esito della votazione non può essere ancora stabilito, la funzione risultato() deve bloccare il thread chiamante. Non appena l'esito è 'sicuro' (ovvero la metà più 1 dei threads ha votato 0 oppure 1) il thread viene sbloccato e la funzione risultato() ritorna l'esito della votazione. I thread vengono sbloccati il più presto possibile, quindi anche prima che abbiano votato tutti.

Utilizzare i costrutti pthread_mutex_xxx e pthread_cond_xxx.  

*/

#define NUM_THREADS 10


struct {
	int voto0;
	int voto1;
	int votanti;

	pthread_mutex_t mutex;
	pthread_cond_t s_voto;
} urna;


void init_urna(struct urna *in) {
	pthread_mutexattr_t m_attr;
	pthread_condattr_t c_attr;

	pthread_mutexattr_init(&m_attr);
	pthread_condattr_init(&c_attr);
	
	pthread_mutex_init(&in->mutex, &m_attr);

	pthread_cond_init(&in->s_voto, &c_attr);

	in->voto0 = in->voto1 = in->votanti = 0;
}

void vota(int v) {
	pthread_mutex_lock(&urna->mutex);

	if(v == 0) voto0++;
	if(v == 1) voto1++;

	votanti++;

	if(voto0 > NUM_THREADS/2 || voto1 > NUM_THREADS/2) {
		pthread_cond_broadcast(&urna->s_voto);
	}

	pthread_mutex_unlock(&urna->mutex);
}

int risultato(void){
	pthread_mutex_lock(&urna->mutex);

	while(voto0 <= NUM_THREADS/2 && voto1 <= NUM_THREADS/2) {
		pthread_cond_wait(&urna->s_voto, &urna->mutex);
	}

	printf("È stato raggiunto un risultato!\n");

	pthread_mutex_unlock(&urna->mutex);

	if (voto0 > voto1) return voto0;
	else return voto1;
}