/* Simulare morra cinese

3 thread:
- 2 thread giocatori
- 1 thread arbitro

Arbitro ha il compito di:
- Dare il via ai 2 thread giocatori
- Aspettare che ciascuno dei thread faccia la prorpia mossa
- Controllare chi dei 2 ha vinto e stampare il risultato
- Aspettare la pressione di un tasto da parte dell'utente
- Ricominciare dal punto 1


I thread giocatori invece:
- Aspettano il via da parte del thread arbitro
- Estraggono a caso la prima mossa
- Stampare a video la propria mossa
- Segnalare al thread arbitro di aver effettuato la mossa
- Tornare al punto 1


La mossa è codificata come: 
#define CARTA 0
#define SASSO 1
#define FORBICE 2

ed esiste una array di stringhe così definito:

char *nomi_mosse[] = {'carta', 'sasso', 'forbice'};

*/ 


#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define N 2


struct gioco {
	sem_t mutex;

	sem_t sarbitro;
	int arrivato;

	sem_t sgiocatore;
	int cgiocatori;

	int mossa1, mossa2;
}

void init_main() {
	srand(time(NULL));
	arrivato = 0;
	cgiocatori = 0;
	
	mossa1 = mossa2 = -1;
};

void aspetta_arbitro() {
	sem_wait(mutex);

	if(!arrivato && !cgiocatori) {
		printf("%s\n", "Sveglio l'arbitro");
		sem_post(sarbitro);
	}
	cgiocatori++:


	sem_post(mutex);
	sem_wait(sgiocatore);
}

void aspetta_giocatori() {
	sem_wait(mutex);

	if(cgiocatori == 2) {
		printf("%s\n", "Tutti i giocatori sono pronti");
		sem_post(sarbitro);
	} else {
		printf("%s\n", "Attendo i giocatori");
		arrivato++;
	}

	sem_post(mutex);
	sem_wait(sarbitro);
}

void dai_via() {
	sem_wait(mutex);

	arrivato--;	// Riazzero la variabile che uso per controllare se l'arbitro sta attendendo o no

	while(cgiocatori) {
		cgiocatori--;
		sem_post(sgiocatore);
	}

	sem_post(mutex);
}


void invia_mossa() {
	sem_wait(mutex);

	if(mossa1 >= 0 && !arrivato) {
		sem_post(sarbitro);
	}

	if(mossa1 >= 0) {
		mossa2 = rand() % 3;
		printf("%s: %d\n", "Mossa 2 fatta", mossa2);
	} else {
		mossa1 = rand() % 3;
		printf("%s: %d\n", "Mossa 1 fatta", mossa1);
	}

	sem_post(mutex);
}

void attendi_mosse() {
	sem_wait(mutex);

	if(mossa1 >= 0 && mossa2 >= 0) {
		printf("%s\n", "Entrmabi le mosse fatte");
		sem_post(sarbitro);
	} else {
		printf("%s\n", "Manca ancora la mossa di qualcuno");
		arrivato++;
	}

	sem_post(mutex);
	sem_wait(sarbitro);
}

int main() {
	
};