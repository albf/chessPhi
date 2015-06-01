#include<stdio.h>
#include<string.h>
#define P 12 /* numero de peças */
#define N 8 /* tamanho do tabuleiro */
#define K 1 /* numero do lookahead */

int tabuleiro[N][N];
int list_i[2*P];
int list_j[2*P];
int best_i,best_j,best_k;


int solve(int tab[N][N],int nK,int bw,int n_whites,int n_blacks)
{
	/* perde se tem menos peça, ganha se come peça */
	if(nK==0){ 
		//printf("parada\n");	
		return ((N-n_whites)-(N-n_blacks));
	}
	int i,j;
	int res=0;
	/* pretos - cima */
	if(bw==0)
	{
		for(i=0;i<P;i++)
		{
			int my_i=list_i[i];
			int my_j=list_j[i];
			int my_rank=tabuleiro[my_i][my_j];
			//printf("olhando para %d com %d %d\n",my_rank,my_i,my_j);
			/* esquerda livre */
			int i_target=my_i+1;
			int j_target=my_j-1;
			if(i_target>=0 && i_target<N && j_target>=0 && j_target<N && tabuleiro[i_target][j_target]==0)
			{
				//printf("esquerda livre\n");
				tabuleiro[i_target][j_target]=my_rank;
				tabuleiro[my_i][my_j]=0;
				/* recursao */
				int tmp_res;
				tmp_res = solve(tabuleiro,nK,1-bw,n_whites,n_blacks);
				if(tmp_res>res)
				{
					res=tmp_res;
					best_i=i_target;
					best_j=j_target;
					best_k=my_rank;
				}

				tabuleiro[my_i][my_j]=my_rank;
				tabuleiro[i_target][j_target]=0;

			}

			/* direita livre */
			i_target=my_i+1;
			j_target=my_j+1;
			if(i_target>=0 && i_target<N && j_target>=0 && j_target<N && tabuleiro[i_target][j_target]==0)
			{
				//printf("direita livre\n");
				tabuleiro[i_target][j_target]=my_rank;
				tabuleiro[my_i][my_j]=0;
				/* recursao */
				int tmp_res;
				tmp_res = solve(tabuleiro,nK,1-bw,n_whites,n_blacks);
				if(tmp_res>res)
				{
					res=tmp_res;
					best_i=i_target;
					best_j=j_target;
					best_k=my_rank;
				}

				tabuleiro[my_i][my_j]=my_rank;
				tabuleiro[i_target][j_target]=0;

			}

			/* esquerda come */
			i_target=my_i+2;
			j_target=my_j-2;
			int i_target_tmp=my_i+1;
			int j_target_tmp=my_j-1;
			if(i_target>=0 && i_target<N && j_target>=0 && j_target<N && tabuleiro[i_target][j_target]==0 && i_target_tmp>=0 && i_target_tmp<N && j_target_tmp>=0 && j_target_tmp<N && tabuleiro[i_target_tmp][j_target_tmp]>P)
			{
				//printf("come esquerda\n");
				int removed_rank = tabuleiro[i_target_tmp][j_target_tmp];
				tabuleiro[i_target_tmp][j_target_tmp]=0;
				tabuleiro[i_target][j_target]=my_rank;
				tabuleiro[my_i][my_j]=0;
				/* recursao */
				int tmp_res;
				tmp_res = solve(tabuleiro,nK,1-bw,n_whites-1,n_blacks);
				if(tmp_res>res)
				{
					res=tmp_res;
					best_i=i_target;
					best_j=j_target;
					best_k=my_rank;
				}

				tabuleiro[my_i][my_j]=my_rank;
				tabuleiro[i_target][j_target]=0;
				tabuleiro[i_target_tmp][j_target_tmp]=removed_rank;

			}

			/* direita come */
			i_target=my_i+2;
			j_target=my_j+2;
			i_target_tmp=my_i+1;
			j_target_tmp=my_j+1;
			if(i_target>=0 && i_target<N && j_target>=0 && j_target<N && tabuleiro[i_target][j_target]==0 && i_target_tmp>=0 && i_target_tmp<N && j_target_tmp>=0 && j_target_tmp<N && tabuleiro[i_target_tmp][j_target_tmp]>P)
			{
				//printf("come direita\n");
				int removed_rank = tabuleiro[i_target_tmp][j_target_tmp];
				tabuleiro[i_target_tmp][j_target_tmp]=0;
				tabuleiro[i_target][j_target]=my_rank;
				tabuleiro[my_i][my_j]=0;
				/* recursao */
				int tmp_res;
				tmp_res = solve(tabuleiro,nK,1-bw,n_whites-1,n_blacks);
				if(tmp_res>res)
				{
					res=tmp_res;
					best_i=i_target;
					best_j=j_target;
					best_k=my_rank;
				}

				tabuleiro[my_i][my_j]=my_rank;
				tabuleiro[i_target][j_target]=0;
				tabuleiro[i_target_tmp][j_target_tmp]=removed_rank;

			}



		}

	}else{ /*brancos - baixo */
		//printf("entrou no baixo\n");
		//return solve(tabuleiro,nK-1,1-bw,n_whites,n_blacks);

		for(i=P;i<2*P;i++)
		{
			int my_i=list_i[i];
			int my_j=list_j[i];
			int my_rank=tabuleiro[my_i][my_j];
			//printf("olhando para %d com %d %d\n",my_rank,my_i,my_j);
			/* esquerda livre */
			int i_target=my_i-1;
			int j_target=my_j-1;
			if(i_target>=0 && i_target<N && j_target>=0 && j_target<N && tabuleiro[i_target][j_target]==0)
			{
				//printf("esquerda livre\n");
				tabuleiro[i_target][j_target]=my_rank;
				tabuleiro[my_i][my_j]=0;
				/* recursao */
				int tmp_res;
				tmp_res = solve(tabuleiro,nK-1,1-bw,n_whites,n_blacks);
				if(tmp_res>res)
				{
					res=tmp_res;
					best_i=i_target;
					best_j=j_target;
					best_k=my_rank;
				}

				tabuleiro[my_i][my_j]=my_rank;
				tabuleiro[i_target][j_target]=0;

			}

			/* direita livre */
			i_target=my_i-1;
			j_target=my_j+1;
			if(i_target>=0 && i_target<N && j_target>=0 && j_target<N && tabuleiro[i_target][j_target]==0)
			{
				//printf("direita livre\n");
				tabuleiro[i_target][j_target]=my_rank;
				tabuleiro[my_i][my_j]=0;
				/* recursao */
				int tmp_res;
				tmp_res = solve(tabuleiro,nK-1,1-bw,n_whites,n_blacks);
				if(tmp_res>res)
				{
					res=tmp_res;
					best_i=i_target;
					best_j=j_target;
					best_k=my_rank;
				}

				tabuleiro[my_i][my_j]=my_rank;
				tabuleiro[i_target][j_target]=0;

			}

			/* esquerda come */
			i_target=my_i-2;
			j_target=my_j-2;
			int i_target_tmp=my_i-1;
			int j_target_tmp=my_j-1;
			if(i_target>=0 && i_target<N && j_target>=0 && j_target<N && tabuleiro[i_target][j_target]==0 && i_target_tmp>=0 && i_target_tmp<N && j_target_tmp>=0 && j_target_tmp<N && tabuleiro[i_target_tmp][j_target_tmp]>P)
			{
				//printf("come esquerda\n");
				int removed_rank = tabuleiro[i_target_tmp][j_target_tmp];
				tabuleiro[i_target_tmp][j_target_tmp]=0;
				tabuleiro[i_target][j_target]=my_rank;
				tabuleiro[my_i][my_j]=0;
				/* recursao */
				int tmp_res;
				tmp_res = solve(tabuleiro,nK-1,1-bw,n_whites,n_blacks-1);
				if(tmp_res>res)
				{
					res=tmp_res;
					best_i=i_target;
					best_j=j_target;
					best_k=my_rank;
				}

				tabuleiro[my_i][my_j]=my_rank;
				tabuleiro[i_target][j_target]=0;
				tabuleiro[i_target_tmp][j_target_tmp]=removed_rank;

			}

			/* direita come */
			i_target=my_i-2;
			j_target=my_j+2;
			i_target_tmp=my_i-1;
			j_target_tmp=my_j+1;
			if(i_target>=0 && i_target<N && j_target>=0 && j_target<N && tabuleiro[i_target][j_target]==0 && i_target_tmp>=0 && i_target_tmp<N && j_target_tmp>=0 && j_target_tmp<N && tabuleiro[i_target_tmp][j_target_tmp]>P)
			{
				//printf("comer direita\n");
				int removed_rank = tabuleiro[i_target_tmp][j_target_tmp];
				tabuleiro[i_target_tmp][j_target_tmp]=0;
				tabuleiro[i_target][j_target]=my_rank;
				tabuleiro[my_i][my_j]=0;
				/* recursao */
				int tmp_res;
				tmp_res = solve(tabuleiro,nK-1,1-bw,n_whites,n_blacks-1);
				if(tmp_res>res)
				{
					res=tmp_res;
					best_i=i_target;
					best_j=j_target;
					best_k=my_rank;
				}

				tabuleiro[my_i][my_j]=my_rank;
				tabuleiro[i_target][j_target]=0;
				tabuleiro[i_target_tmp][j_target_tmp]=removed_rank;

			}



		}


	}

	return res;

} 

void read_tab(char *argv[])
{
	int i,j;
	int k=0;
	for(i=0;i<N;i++)
	{
		for(j=0;j<N;j++){
			tabuleiro[i][j]=atoi(argv[2+k]);
			k++;
		}
	}

}

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
				base=1;	
			}else{
				base=0;
			}
			for(j=base;j<N;j+=2)
			{
				tabuleiro[i][j]=nk;
				list_i[nk-1]=i;
				list_j[nk-1]=j;
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

int main(int argc,char *argv[])
{
	int i;
	int r;
	int k_it;
	if(argc==1)
	{
		k_it=1;
		init_tab();
	}else{
		k_it=atoi(argv[1]);
		read_tab(argv);
	}

	print_tab();
	//r=solve(tabuleiro,k_it,0,N,N);	
	printf("best eh mover %d para %d %d com valor %d\n",best_k,best_i,best_j,r);
	return 0;
}
