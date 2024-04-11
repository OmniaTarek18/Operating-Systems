#include <pthread.h>

struct station {
	int numOfEmptySeats;
	int numOfWaitingPeople;
	int numOfPeopleWalkingOnTrain;
	pthread_mutex_t mutex;
	// condition variables
	pthread_cond_t train_is_maybe_ready_to_leave;
	pthread_cond_t seats_are_available ;
};

void station_init(struct station *station);

void station_load_train(struct station *station, int count);

void station_wait_for_train(struct station *station);

void station_on_board(struct station *station);
