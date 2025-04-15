/* Esercizio 12

Un gestore di risorse equivalenti alloca dinamicamente ad N processi clienti 9 istanze di uno stesso tipo di risorse (R1, R2, …., R9).

Gli N processi clienti (P0.,……., PN-1 ) vengono serviti dal gestore privilegiando i processi di indice più basso.

Un processo che chiede una risorsa si blocca se all’atto della richiesta non ci sono risorse disponibili in attesa che una risorsa venga rilasciata. Però il processo rimane bloccato, al massimo, per t quanti di tempo successivi al blocco (dove t denota un parametro intero passato in fase di chiamata della funzionene richiesta). Quando un processo viene svegliato per lo scadere del tempo massimo da lui indicato, la funzione richiesta termina senza allocare niente al richiedente.

Utilizzando il meccanismo semaforico realizzare il codice del gestore che offre le tre seguenti funzioni membro:
- int richiesta (int t, int p) dove t denota il time-out espresso come numero di quanti di tempo, mentre p denota l’indice del processo richiedente. La funzione restituisce l’indice della risorsa allocata (1,…, 9) oppure 0 se il processo termina la funzione dopo un time-out senza aver allocato niente.
- void rilascio (int r) dove r denota l’indice della risorsa rilasciata.
– void tick () funzione chiamata dal driver dell’orologio in tempo reale ad ogni interruzioni di clock

*/


#define NUM_RISORSE 9
#define NUM_CLIENTI 20

struct {
	sem_t mutex;

	int risorse[NUM_RISORSE];

	sem_t s_processi[NUM_CLIENTI];
	int processi[NUM_CLIENTI]; // Assumo che l'id dei processi vada da 0 a 11 altrimenti si può usare una lista sortata per ID
	int b_processi;

} allocatore;

void init_allocatore(struct allocatore *in) {
	sem_init(&in->mutex, 0, 1);

	allocatore->b_processi = 0;

	sem_init(&in->s_risorse, 0, 0);

	for(int i = 0; i < NUM_CLIENTI; i++) {
		in->processi[i] = 0;
		sem_init(s_processi[i], 0, 0);
	}

	for(int i = 0; i < NUM_RISORSEM; i++) {
		in->risorse[i] = 0;
	}
}

int richiesta(int t, int p) {
	sem_wait(&allocatore->mutex);

	int i = 0;
	while(allocatore->risorse[i] != 0) {
		i++;
	}

	if (i == NUM_RISORSE) {
		sem_post(&allocatore->mutex);
		allocatore->b_processi++;	// Mi blocco se non trovo nulla
		allocatore->processi[p] = 1;
		sem_wait(&allocatore->s_processi[p]);
		allocatore->b_processi--;	// Sono sbloccato quindi riazzero i counter
		allocatore->processi[p] = 0;
	}

	// Cerca di nuovo ora che una risorsa è libera
	i = 0;
	while(allocatore->risorse[i] != 0) {
		i++;
	}

	allocatore->risorse[i]++;

	sem_post(&allocatore->mutex);

	return i;
}

void rilascio(int r) {
	sem_wait(&allocatore->mutex);

	allocatore->risorse[r]=0;

	if (allocatore->b_processi) {
		for(int i = 0; i < NUM_CLIENTI; i++) {
			if (allocatore->processi[i]) {
				sem_post(&allocatore->s_processi[i]);	// Token passing sul thread con ID piu piccolo
				break;
			}
		}
	} else {
		sem_post(&allocatore->mutex);
	}
}

void tick() {

}