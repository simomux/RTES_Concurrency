// Esercizio 6

#define NUM_FILOSOFI 7

#define DELAY 10


/*

Filosofo:
- Pensa
- Cerca di acquisire forchette:
	- Prima forchetta dx
	- Poi forchetta sx
- Mangia
- Rilascio forchette


* Un filosofo deve prendere prima la sx poi la dx per eviatare deadlock;

*/


struct {
	sem_t mutex;
	circular_array<sem_t> forchette;	// Uso un array dove forchette[i] è la forchetta dx del filosofo i e forchette[i-1] è la forchetta sx
	int filosofi_dx[NUM_FILOSOFI];
	int filosofi_sx[NUM_FILOSOFI];
} filosofi;


void init_filosofi() {
	sem_init(mutex);
	for(int i = 0; i < NUM_FILOSOFI; i++) {		// Ho tante forchette quanti filosofi
		sem_init(forchette[i]);
		filosofi_dx[i] = 0;
		filosofi_sx[i] = 0;
	}
}

void prendi_forchetta_dx(int id_filosofo) {
	sem_wait(mutex);

	if(filosofi_dx[id_filosofo] == 0) {
		sem_post(forchette[id_filosofo])
		filosofi_dx[id_filosofo]++;
	}

	sem_post(mutex);
	sem_wait(forchette[id_filosofo]);
}

void prendi_forchetta_sx(int id_filosofo) {
	sem_wait(mutex);

	if(filosofi_sx[id_filosofo] == 0) {
		sem_post(forchette[id_filosofo-1])
		filosofi_sx[id_filosofo]++;
	}

	sem_post(mutex);
	sem_wait(forchette[id_filosofo-1]);

	sem_post(mutex);
}


void rilascia_forchette() {	// Rilascia prima la forchetta di dx poi quella di sx
	sem_wait(mutex);

	sem_post(forchette[id_filosofo]);
	sem_post(forchette[id_filosofo-1]);

	sem_post(mutex);
}


// Thread
void filosofo() {
	sleep(DELAY);
	if (id_filosofo == 0) {	// Caso del filosofo che prende le forchette al contrario per evitare deadlock
		prendi_forchetta_sx(id_filosofo);
		prendi_forchetta_dx(id_filosofo);
	} else {
		prendi_forchetta_dx(id_filosofo);
		prendi_forchetta_sx(id_filosofo);
	}

	sleep(DELAY);

	rilascia_forchette(id_filosofo);
}