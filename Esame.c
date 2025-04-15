struct passerella_t {
	sem_t mutex;

	int barca_arrivata;
	sem_t s_guardiano;

	int pedoni_attivi;
	int pedoni_bloccati;
	sem_t s_pedone;
} passerella;

void init_passerella() {
	sem_init(&passerella->mutex, 0, 1);
	sem_init(&passerella->s_guardiano, 0, 0); // Devo inizializzare il semaforo a 0 invece che 1 
	sem_init(&passerella->s_pedone, 0, 0);
	passerella->pedoni_attivi = passerella->pedoni_bloccati = 0;
	passerella->barca_arrivata = 1;		// Perchè parto con il ponte aperto
}

int pedone_entro_passerella(struct passerella_t *passerella, int hofretta) {
	sem_wait(&passerella->mutex);

	if (passerella->barca_arrivata == 0) {
		passerella->pedoni_attivi++;
		sem_post(&passerella->s_pedone);
	} else {
		passerella->pedoni_bloccati++;
		if (hofretta) return 0;	// Devo fare un return altrimenti mi blocco comunque
	}

	sem_post(&passerella->mutex);
	sem_wait(&passerella->s_pedone);
	return 1;
}

void pedone_esco_passerella(struct passerella_t *passerella) {
	sem_wait(&passerella->mutex);

	passerella->pedoni_attivi--;

	// Se sono l'ultimo a passare sveglio il guardiano
	if (passerella->pedoni_attivi == 0 && passerella->barca_arrivata) {
		sem_post(&passerella->s_guardiano);
	}

	sem_post(&passerella->mutex);
	return 1;
}

void guardiano_abbassa_passerella(struct passerella_t *passerella) {
	sem_wait(&passerella->mutex);

	passerella->barca_arrivata = 0;

	while(passerella->pedoni_bloccati) {
		passerella->pedoni_bloccati--;
		passerella->pedoni_attivi++;
		sem_post(&passerella->s_pedone);
	}

	sem_post(&passerella->mutex);
}

void guardiano_alza_passerella(struct passerella_t *passerella) {
	sem_wait(&passerella->mutex);

	passerella->barca_arrivata = 1;

	if(passerella->pedoni_attivi == 0) {
		sem_post(&passerella->s_guardiano);
	}

	sem_post(&passerella->mutex);
	sem_wait(&passerella->s_guardiano);
}


void *pedone(void *arg) {
	while(1) {
		printf("Sono arrivato alla passerella!\n");
		if(pedone_entro_passerella(&passerella, 1)) {
			printf("Sto attraversando la passerella!\n");
			pedone_esco_passerella(&passerella);
			printf("Sono uscito dalla passerella!\n");
		} else {
			printf("Faccio un altra strada!\n");
		}
	}
	return 0;
}


void *guardiano(void *arg) {
	while(1) {
		guardiano_abbassa_passerella(&passerella);
		printf("Attendi la barca!\n");

		guardiano_alza_passerella(&passerella);
		printf("La barca è passata!\n");
	}
}
