/* Esercizio 7

Un negozio di barbieri ha tre barbieri, e tre poltrone su cui siedono i clienti quando vengono per tagliarsi la barba.

C'è una sala d'aspetto con un posti_divano (max 4 persone; gli altri aspettano fuori dalla porta).
C'e' un cassiere, che può servire solamente un cliente (con barba tagliata) alla volta.

Scrivere il processo cliente che cerca di entrare nel negozio per farsi tagliare la barba.

Suggerimenti:
– Considerare i barbieri, il cassiere ed il posti_divano come risorse condivise.
– Modellare il processo di taglio barba come un ciclo di SHAVING ITERATIONS iterazioni
– modellare il processo di pagamento come un ciclo di PAYING ITERATIONS iterazioni
– dopo che un cliente ha pagato esce (th thread muore) oppure, dopo un delay di alcuni secondi (usare la primitiva sleep() ), si accoda nuovamente per farsi tagliare la barba.

*/

#define SHAVING_ITERATIONS 10
#define PAYING_ITERATIONS 10


struct {
	sem_t mutex;

	sem_t divano;
	Queue posti_divano;
	Queue attesa_fuori;

	sem_t barbieri;
	int c_barbieri;

	sem_t cassa;
	int cassa_occupata;
	int attendi_cassa;

} negozio;


void init_negozio() {
	sem_init(mutex, 0, 1);

	sem_init(divano, 0);
	sem_init(barbieri, 0, 3);
	int c_barbieri = 0;

	sem_init(cassa, 0);
	cassa_occupata = 0;
}

void siediti() {
	sem_wait(mutex);

	if(!posti_divano.isQueueFull()) {
		// C'è posto sul posti_divano
		sem_post(divano);
		posti_divano.enqueue(id_cliente);
	} else {
		attesa_fuori.enqueue(id_cliente);
	}

	sem_post(mutex);
	sem_wait(divano);
}

void taglia_capelli() {
	sem_wait(mutex);

	if(c_barbieri < 3) {
		// C'è un barbiere libero quindi preventive post
		sem_post(barbieri);
		c_barbieri++;
		posti_divano.dequeue();
		if(attesa_fuori > 0) {
			sem_post(divano);
			posti_divano.enqueue(attesa_fuori.front());
			attesa_fuori.dequeue();
		}
	}

	sem_post(mutex);
	sem_wait(barbieri);
}

void paga() {
	sem_wait(mutex);

	if (cassa_occupata == 0) {
		sem_post(cassa);
		cassa_occupata++;
		c_barbieri--;

		sem_post(barbieri);
		if(!divano.isEmptyQueue) {
		// C'è uno sul divano che aspetta
		posti_divano.dequeue();
		c_barbieri++;
		sem_post(divano);
		if(attesa_fuori > 0) {
			posti_divano.enqueue(attesa_fuori.front());
			attesa_fuori.dequeue();
		}
	} else {
 		attendi_cassa++;
	}

	sem_post(mutex);
	sem_wait(cassa);
}

void esci() {
	sem_wait(mutex);

	if (attendi_cassa == 0) {
		cassa_occupata = 0;
	} else {
		attendi_cassa--;
		sem_post(cassa);
	}

	sem_post(mutex);
}

void *cliente() {
	siediti();

	alzati();

	taglia_capelli();
	for(int i = 0; i < SHAVING_ITERATIONS; i++){}

	paga();
	for(int i = 0; i < PAYING_ITERATIONS; i++){}
	esci();
}