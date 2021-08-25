/*  ex9a.c: Semaphore
*
*  Created on: Jan 15, 2018
*     Author: eliyho tsuri, Dan Narkisi
*       Login: eliyhots , danna
*		    Id: 201610672, 301680435
*
*	The parent process assigns sheard memory and gives birth to two child processes, 
*   each of the children in a loop from a number scroller to a prime number lottery.
*   During the process (the child) grill the prime number he stores it in sheard memory ,
*   kills his brother, and sends a signal to his parent, and finishes.
*   The parent displays the number, and ends as well.
* 
*/
// ------------------- Include Section -----------------------
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <semaphore.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
// ----------------------- Define ----------------------------
#define NUM_CHILDS 2
// ----------------------- Struct ----------------------------
typedef struct my_struct
{
	int m_prime;
}my_struct;
// ---------------------- Prototype --------------------------
unsigned set_number();
bool is_prime(unsigned num);
void wait_childred(pid_t status[]);
void do_father();
// ------------------------ Main -----------------------------
int main(void)
{
	signal(SIGUSR1, do_father);
	my_struct *shm_ptr;
	sem_t *my_sem;
	pid_t status[2];
	key_t key;
	int shm_id, child;
	unsigned number;

	// create the semaphore
	my_sem = sem_open("/my_mutex", O_CREAT, 0644, 1);
	if (my_sem == SEM_FAILED)
	{
		perror("Parent sem_open() failed");
		exit(EXIT_FAILURE);
	}

	// create the sheard memory
	if ((key = ftok(".", 'a')) == -1)
	{
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}

	shm_id = shmget(key, 1000, IPC_CREAT | IPC_EXCL | 0600);
	if (shm_id == -1)
	{
		perror("shm get failed");
		exit(EXIT_FAILURE);
	}

	if ((shm_ptr = (my_struct *) shmat(shm_id, NULL, 0)) < 0)
	{
		perror("shmat failed");
		exit(EXIT_FAILURE);
	}


	for (child = 0; child < NUM_CHILDS ; child++)
	{
		status[child] = fork();
		if (status[child] < 0)
		{
			perror("fork() failed");
			exit(EXIT_FAILURE);
		}
		if (status[child] == 0)
		{
			while (1)
			{
				number = set_number();
				sem_wait(my_sem);
				sleep(3);
				shm_ptr[0].m_prime = number;
				if (child == 0)
					kill(SIGKILL, status[1]);
				else
					kill(SIGKILL, status[0]);
				kill(getppid(), SIGUSR1);
				exit(EXIT_SUCCESS);

			}
			sem_post(my_sem);
			sleep(1);
		}
		else
			wait_childred(status);
	}

	sem_close(my_sem);
	sem_unlink("/my_mutex");
	shmdt(shm_ptr);
	return (EXIT_SUCCESS);
}
// ---------------------- set_number -------------------------
/*
* This function srand unsigned numbers and return the number
*/
unsigned set_number()
{
	unsigned number;
	srand((unsigned) time(NULL)) ;

	while (1)
	{
		number = rand();
		if (is_prime(number))
			break;
	}
	return number;
}
//---------------------- check_prime -------------------------
/*
* This function getting number and check if the number is prime
*/
bool is_prime(unsigned num)
{
	unsigned div;

	if (num == 0 || num == 1)
		return false;
	for (div = 2; div <= num / 2; div++)
		if (num % div == 0)
			return false;
	return true;
}
//---------------------- do_father --------------------------
/*
* This function the father contacted to the sheared memory
* and print the prime number from the sheared memory
*/
void do_father()
{
	key_t key;
	int shm_id;
	my_struct *shm_ptr;

	if ((key = ftok(".", 'a')) == -1)
	{
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}

	shm_id = shmget(key, 0, 0600);
	if (shm_id == -1)
	{
		perror("shm get failed");
		exit(EXIT_FAILURE);
	}

	shm_ptr = (my_struct *) shmat(shm_id, NULL, 0);
	if (shm_ptr == (my_struct *) -1)
	{
		perror("shmat failed");
		exit(EXIT_FAILURE);
	}
	printf("%d\n", shm_ptr->m_prime);
	if (shmctl(shm_id, IPC_RMID, NULL) == -1)
	{
		perror("shmctl failed");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
//--------------------- wait_children ------------------------
/*
* This function waiting for the childred
*/
void wait_childred(pid_t status[])
{
	int child;
	for (child = 0; child < 2; child++)
		waitpid(status[child], NULL, 0);
}
