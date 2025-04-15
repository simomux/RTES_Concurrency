
#define N_AUTO 10 
#define N_SEZIONI 5

struct rotonda_t {
	pthread_mutex_t mutex;
}rotonda;

void init_rotonda(struct rotonda_t *s) {

}

void entra(struct rotonda_t *r, int numeroauto, int sezione){
	pthread_mutex_lock(&r->mutex);

	pthread_mutex_unlock(&r->mutex);
}

int sonoarrivato(struct rotonda_t *r, int numeroauto, int destinazione) {
	pthread_mutex_lock(&r->mutex);

	pthread_mutex_unlock(&r->mutex);
}

void esci(struct rotonda_t *r, int numeroauto) {
	pthread_mutex_lock(&r->mutex);

	pthread_mutex_unlock(&r->mutex);
}

void *t_auto() {
	entra(&r, numeroauto, sezionediingresso);

	do {

	} while(sonoarrivato(&r, numeroauto, destinazione));
	esci(&r, numeroauto);
}