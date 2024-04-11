#include <pthread.h>
#include "caltrain.h"

void station_init(struct station *station)
{
	station->numOfEmptySeats = 0;
	station->numOfWaitingPeople = 0;
	station->numOfPeopleWalkingOnTrain = 0;
	pthread_mutex_init(&station->mutex, NULL);
	pthread_cond_init(&station->train_is_maybe_ready_to_leave, NULL);
	pthread_cond_init(&station->seats_are_available, NULL);
}

void station_load_train(struct station *station, int count)
{

	pthread_mutex_lock(&station->mutex);
	station->numOfEmptySeats = count;

	while(station->numOfWaitingPeople !=0 && station->numOfEmptySeats != 0)
	{
		pthread_cond_broadcast(&station->seats_are_available);
		pthread_cond_wait(&station->train_is_maybe_ready_to_leave, &station->mutex);
	}
	station->numOfEmptySeats = 0;
	pthread_mutex_unlock(&station->mutex);
}

void station_wait_for_train(struct station *station)
{
	pthread_mutex_lock(&station->mutex);
	station->numOfWaitingPeople++;
	while (station->numOfEmptySeats == 0)
	{
		pthread_cond_wait(&station->seats_are_available, &station->mutex);
	}
	station->numOfWaitingPeople--;
	station->numOfPeopleWalkingOnTrain++;
	station->numOfEmptySeats--;

	pthread_mutex_unlock(&station->mutex);
}

void station_on_board(struct station *station)
{
    pthread_mutex_lock(&station->mutex);
    station->numOfPeopleWalkingOnTrain--;

	if (station->numOfPeopleWalkingOnTrain == 0 && (station->numOfWaitingPeople ==0 || station->numOfEmptySeats == 0))
	{
		pthread_cond_signal(&station->train_is_maybe_ready_to_leave);
	}

    pthread_mutex_unlock(&station->mutex);
}
