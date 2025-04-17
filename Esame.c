#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>

#define NUM_PEDONI 10

struct passerella_t {
	sem_t mutex;

	int barca_arrivata;
	sem_t s_guardiano;

	int pedoni_attivi;
	int pedoni_bloccati;
	sem_t s_pedone;
} passerella;


/*
	EDIT: Codice originale: 
	void init_passerella(struct passerella_t *passerella) {
		...
		sem_init(&passerella->s_guardiano, 0, 1);
		...
	}

	Nella versione originale inizializzo erroneamente il semaforo del guardiano a 1. Questo aumenta il valore della post del semaforo s_guardiano di 1 rendendo quindi inutili tutte le wait successive.
	Per risolvere basta semplicemente inizializzare il valore a 0.

*/
void init_passerella(struct passerella_t *passerella) {
	sem_init(&passerella->mutex, 0, 1);				// Semaforo che si comporta da mutex
	sem_init(&passerella->s_guardiano, 0, 0);	// Semaforo che gestisce il guardiano
	sem_init(&passerella->s_pedone, 0, 0);		// Semaforo che gestisce i pedoni
	passerella->pedoni_attivi = passerella->pedoni_bloccati = 0;		// Variabili counter per gestire i pedoni che stanno attraversati sul ponte e quelli che stannoa aspettando
	passerella->barca_arrivata = 1;		// Varibile per gestire lo stato del ponte (barca_arrivata = 1 -> ponte alzato, barca_arrivata = 0 -> ponte abbassato)
}


/*
	EDIT: Codice originale: 
	int pedone_entro_passerella(struct passerella_t *passerella, int hofretta) {
		sem_wait(&passerella->mutex);

		int ritorno = 1;

		if(passerella->barca_arrivata == 0) {
			passerella->pedoni_attivi++;
			sem_post(&passerella->s_pedone);
		} else {
			passerella->pedoni_bloccati++;
			if (hofretta) ritorno = 0;
		}
		
		sem_post(&passerella->mutex);
		sem_wait(&passerella->s_pedone);

		return ritorno;
	}

		
	Con la versione originale faccio la wait del semaforo anche quando il valore di ritorno è 0, quindi quando il pedone ha fretta e dovrebbe invece scegliere un altra strada. 
	Per risolvere ciò devo ritornare direttamente 0, ricordandomi però di rilasciare prima il mutex.

*/
int pedone_entro_passerella(struct passerella_t *passerella, int hofretta) {
	sem_wait(&passerella->mutex);

	if (passerella->barca_arrivata == 0) {		// Se il ponte è abbassato faccio preventive post
		passerella->pedoni_attivi++;			// Sto attraversando il ponte
		sem_post(&passerella->s_pedone);
	} else {
		
		if (hofretta) {				// Se ho fretta e il ponte è alzato me ne vado
			sem_post(&passerella->mutex);
			return 0;	
		}
		passerella->pedoni_bloccati++;		// Se non ho fretta mi blocco alla wait di seguito e aumento il counter. Posso farlo di seguito all'if perché tanto se ho fretta sono già uscito dalla funzione
	}

	sem_post(&passerella->mutex);
	sem_wait(&passerella->s_pedone);
	return 1;
}

void pedone_esco_passerella(struct passerella_t *passerella) {
	sem_wait(&passerella->mutex);

	passerella->pedoni_attivi--;		// Un pedone è uscito dal ponte

	// Se sono l'ultimo a passare sveglio il guardiano
	if (passerella->pedoni_attivi == 0 && passerella->barca_arrivata) {
		sem_post(&passerella->s_guardiano);
	}

	sem_post(&passerella->mutex);
}

void guardiano_abbassa_passerella(struct passerella_t *passerella) {
	sem_wait(&passerella->mutex);

	passerella->barca_arrivata = 0;	// Il ponte è abbassato

	while(passerella->pedoni_bloccati) {		// Sveglio tutti i pedoni
		passerella->pedoni_bloccati--;
		passerella->pedoni_attivi++;
		sem_post(&passerella->s_pedone);
	}

	sem_post(&passerella->mutex);
}

void guardiano_alza_passerella(struct passerella_t *passerella) {
	sem_wait(&passerella->mutex);

	passerella->barca_arrivata = 1;	// Il ponte è stato alzato

	if(passerella->pedoni_attivi == 0) {		// Se non ci sono pedoni sul ponte faccio preventive post
		sem_post(&passerella->s_guardiano);
	}

	sem_post(&passerella->mutex);
	sem_wait(&passerella->s_guardiano);		// Se c'erano dei pedoni sul ponte mi blocco
}

void pausetta(int quanto) {
  struct timespec t;
  t.tv_sec = 0;
  t.tv_nsec = (rand()%100+1)*1000000 + quanto;
  nanosleep(&t,NULL);
}

void *pedone(void *arg) {
	while(1) {
		pausetta(1000000);
		int priorità = rand()%2;
		printf("%lu - Sono arrivato alla passerella con priorità %d!\n", pthread_self(), priorità);
		if(pedone_entro_passerella(&passerella, priorità)) {
			printf("%lu - Sto attraversando la passerella!\n", pthread_self());


			pausetta(1000000);
			pedone_esco_passerella(&passerella);
			printf("%lu - Sono uscito dalla passerella!\n", pthread_self());
		} else {
			printf("%lu - Faccio un altra strada!\n", pthread_self());
		}
	}
	return 0;
}


void *guardiano(void *arg) {
	while(1) {
		pausetta(1000000);
		guardiano_abbassa_passerella(&passerella);
		printf("Abbasso la passerella!\n");
		
		printf("Aspetto che arrivi una barca...\n");

		pausetta(1000000);
		printf("Barca arrivata!\n");
		guardiano_alza_passerella(&passerella);
		printf("La barca è passata!\n");
	}
}

/* la creazione dei thread */

int main() {
  pthread_attr_t a;
  pthread_t p;

  
  /* inizializzo il mio sistema */
  init_passerella(&passerella);

  srand(time(NULL));

  pthread_attr_init(&a);
  pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED);

  for (int i=0; i<NUM_PEDONI; i++)
    pthread_create(&p, &a, pedone, (void *)(i));
  
  pthread_create(&p, &a, guardiano, NULL);

  pthread_attr_destroy(&a);

  sleep(1);

  return 0;
}


