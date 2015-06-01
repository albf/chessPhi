#include<stdio.h>
#include<pthread.h>
#define P 12 /* numero de peças */
#define N 8 /* tamanho do tabuleiro */
#define K 1 /* numero do lookahead */


/* thread 0 a N = brancas */
/* thread N+1 a 2*N = pretas */

typedef struct
{
	int white[P]; /* qual casa esta cada peça */
	int black[P];
}tabuleiro;

void init_tab(tabuleiro *init)
{
	init->black[0]=0;
	init->black[1]=2;
	init->black[2]=4;
	init->black[3]=6;
	init->black[4]=9;
	init->black[5]=11;
	init->black[6]=13;
	init->black[7]=15;
	init->black[8]=16;
	init->black[9]=18;
	init->black[10]=20;
	init->black[11]=22;
	init->white[0]=56;
	init->white[1]=58;
	init->white[2]=60;
	init->white[3]=62;
	init->white[4]=49;
	init->white[5]=51;
	init->white[6]=53;
	init->white[7]=55;
	init->white[8]=40;
	init->white[9]=42;
	init->white[10]=44;
	init->white[11]=46;

}

void* Thread_work(void* rank) {
	tabuleiro init;
	init_tab(&init);
	long my_rank = (long) rank;
	int i;
	printf("i'm thread %ld\n",my_rank);
	for(i=0;i<K;i++)
	{
		if(my_rank<N)
		{
			
		}else{

		}
	}
}

int main()
{
	long i;
	pthread_t thread_handles[2*N];

	for (i = 0; i < 2*N; i++)
		pthread_create(&thread_handles[i], NULL, Thread_work,(void*)i);
	for (i = 0; i < 2*N; i++)
		pthread_join(thread_handles[i], NULL);
	return 0;
}
