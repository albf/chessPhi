#include<stdio.h>

#define P 12 /* numero de peças */
#define N 8 /* tamanho do tabuleiro */
#define K 1 /* numero do lookahead */


int list_i[2*N];
int list_j[2*N];
int best_i,best_j,best_k;
int tabuleiro[N][N];

void init_tab()
{
	int i,j,nk=1;
	int base;

	/* K < N pretos, K > N brancos */

	for(i=0;i<N;i++)
	{
		for(j=0;j<N;j++){
			tabuleiro[i][j]=0;
		}
	}
	for(i=0;i<N;i++)
	{
		/* linhas livres no meio */
		if(i<3 || i>4)
		{
			/* linhas par */
			if(i%2==0)
			{		
				base=0;	
			}else{
				base=1;
			}
			for(j=base;j<N;j+=2)
			{
				list_i[nk-1]=i;
				list_j[nk-1]=j;
				printf("atribui (%d,%d)=%d\n",i,j,nk);
				tabuleiro[i][j]=nk;
				nk++;
			}
		}
	}
}


void print_tab()
{
	int i,j;
	for(i=0;i<N;i++)
	{
		for(j=0;j<N;j++)
		{
			printf("%d\t",tabuleiro[i][j]);
		}
		printf("\n");
	}
}



int solve(int tab[N][N],int nK,int bw,int n_whites,int n_blacks)
{
	/* perde se tem menos peça, ganha se come peça */
	if(nK==0){ return ((N-n_whites)-(N-n_blacks));}
	int i,j;
	int res=0;
	/* tabuleiro copia */
	int new_tab[N][N];
	for(i=0;i<N;i++)
	{
		for(j=0;j<N;j++)
		{
			new_tab[i][j]=tabuleiro[i][j];
		}
	}
	/* pretos - cima */
	if(bw==0)
	{
		for(i=0;i<N;i++)
		{
			int my_i=list_i[i];
			int my_j=list_j[i];
			int my_rank=tabuleiro[my_i][my_j];
			printf("olhando para %d com %d %d\n",my_rank,my_i,my_j);
			/* direita livre */
			int i_target=my_i+1;
			int j_target=my_j+1;
			if(i_target<N && j_target<N && tabuleiro[i_target][j_target]==0)
			{
				new_tab[i_target][j_target]=my_rank;
				new_tab[my_i][my_j]=0;
				/* recursao */
				int tmp_res;
				tmp_res = solve(new_tab,nK,1-bw,n_whites,n_blacks);
				if(tmp_res>res)
				{
					res=tmp_res;
					best_i=i_target;
					best_j=j_target;
					best_k=my_rank;
				}
			}

		}

	}else{ /*brancos - baixo */
		return solve(new_tab,nK-1,1-bw,n_whites,n_blacks);
	}

	return res;

} 

int main()
{
	int i;
	int r;
	init_tab();
	print_tab();
	//	r=solve(tabuleiro,K,0,N,N);	
	//	printf("best eh mover %d para %d %d com valor %d\n",best_k,best_i,best_j,r);
	return 0;
}
