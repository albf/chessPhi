#include <stdio.h>
#include <stdlib.h>

// Pieces tables defines.
// [isAlive, type, value, pos x, pos y]
#define num_pieces 32
#define num_col 5

#define col_is_alive 0
#define col_pos_x 1
#define col_pos_y 2
#define col_k_index 3

// Order : 
//	B_tA (24), B_hA (25), B_bA (26), B_qA (27), B_kA (28), B_bB (29), B_hB (30), B_tB (31)
//	B_pB (16), B_pB (17), B_pC (18), B_pD (19), B_pE (20), B_pF (21), B_pG (22), B_pH (23)
//	W_pA (8),  W_pB (9),  W_pC (10), W_pD (11), W_pE (12), W_pF (13), W_pG (14), W_pH (15)
//	W_tA (0),  W_hA (1),  W_bA (2),  W_qA (3),  W_kA (4),  W_bB (5),  W_hB (6),  W_tB (7)
//	Order 0, 1, 2 ... -> Up		
	
// Pieces Definitions, used later as index
#define pawn 9
#define pawn_value 1
#define castle 7
#define castle_value 4
#define knight 5
#define knight_value 3
#define bishop 3
#define bishop_value 3
#define queen 2
#define queen_value 9
#define king 1
#define king_value 100
// White is negative, Black is positive.

// Represents a past moviment.
struct moviment { 
    int index;      				// Index of moving piece
    int l_pos_x;      				// X before move (last pos).
    int l_pos_y;      				// Y before move (last pos).
	int pos_x;						// new X value.
	int pos_y;						// new Y value.
    int k_index;					// Index of killed piece, if any. If not, -1.
};

// Queue of done moviments. Used to avoid recursion.
struct queue{
	int size;				// current size of queue.
	int pop_pos;			// current pop index.
	struct moviment * mov;	// list of moviments.
};

// Init the moviment queue.
void init_queue(struct queue * Q) {
	Q->size = 2;
	Q->pop_pos = 0;
	Q->mov = (struct moviment *) malloc (sizeof(struct moviment)*Q->size);
}

// Free queue memory.
void free_queue(struct queue * Q) {
	free(Q->mov);
}

// Guarantee that it's possible to add a new moviment.
void assign_size(struct queue * Q) {
	if(Q->size == (Q->pop_pos + 1)) {
		Q->size = Q->size*2;
		Q->mov = (struct moviment *) realloc (Q->mov, sizeof(struct moviment)*Q->size);
	}
}

// Alloc mov into queue. Return pointer with moviment.
struct moviment * alloc_mov(struct queue * Q) {
	assign_size(Q);
	Q->pop_pos++;
	return &(Q->mov[Q->pop_pos-1]);
}

// Pop latest appended mov.
struct moviment * pop_mov(struct queue * Q) {
	Q->pop_pos--;
	if(Q->pop_pos<0) {
		printf("Error, pop_mov from empty list");
		return NULL;
	}
	return &(Q->mov[Q->pop_pos]);
}

// Undo a given move, returns difference in score.
int undo_move(int **F, int **P, struct moviment * mov) {
	P[mov->index][3] = mov->l_pos_x;
	P[mov->index][4] = mov->l_pos_y;
	F[mov->l_pos_x][mov->l_pos_y] = P[mov->index][1];
	// Revive piece.
	if(mov->k_index >= 0) {
		P[mov->k_index][0] = 1;
		return P[mov->k_index][2];
	}
	return 0;
}

// Apply a given move, return difference in score.
int apply_move(int **F, int **P, struct moviment * mov) {
	P[mov->index][3] = mov->pos_x;
	P[mov->index][4] = mov->pos_y;
	F[mov->pos_x][mov->pos_y] = P[mov->index][1];
	// Kill this piece.
	if(mov->k_index >= 0) {
		P[mov->k_index][0] = -1;
		return P[mov->k_index][2];
	}
	return 0;
}

// Return next nTH move in ret. If couldn't found, return -1 in ret->index. Don't apply the move.
void find_nth_move(int ** F, int ** P, int n, struct moviment * ret) {
	int i, x, y, m_c=0, pos;
	
	for(i=0; i<32; i++) {
		// Discard dead pieces.
		if(P[i][0] < 1) {
			continue;
		}
		
		x = P[i][3];
		y = P[i][4];
		
		// White pawns
		if(P[i][1] == -pawn) {
			// Try to move foward. 
			if(((y+1)<8) && (F[x][y+1]==0)) {
				if(m_c == n) {
					ret->pos_x = x;
					ret->pos_y = y+1;
					break;
				}
				m_c++;
			}
			// Check if it's the first move and can move 2x foward.
			if((y==1) && (F[x][y+1]==0) && (F[x][y+2]==0)) {
				if(m_c == n) {
					ret->pos_x = x;
					ret->pos_y = y+2;
					break;
				}
				m_c++;
			}
			
			// Try to get piece on foward-left
			if(((y+1)<8) && ((x-1)>=0) && (F[x-1][y+1]>0)) {
				if(m_c == n) {
					ret->pos_x = x-1;
					ret->pos_y = y+1;
					break;
				}
				m_c++;	
			}
			
			// Try to get piece on foward-right
			if(((y+1)<8) && ((x+1)<8) && (F[x-1][y+1]>0)) {
				if(m_c == n) {
					ret->pos_x = x+1;
					ret->pos_y = y+1;
					break;
				}
				m_c++;	
			}
		}
		// White castles
		else if ((P[i][1] == -castle)||(P[i][1] == -queen)||(P[i][1] == -king)) {
			// Just try to move foward.
			for(pos=1; (((y+pos)<8)&&(F[x][y+pos]>=0)) ; pos++) {
				if(m_c == n) {
					ret->pos_x = x;
					ret->pos_y = y+pos;
					break;
				}
				m_c++;
				if((F[x][y+pos]>0)||(P[i][1] == -king)) {
					break;
				}
			}
			
			if(m_c == n) {
				break;
			}
			
			// Just try to move backward.
			for(pos=1; (((y-pos)>=0)&&(F[x][y-pos]>=0)) ; pos++) {
				if(m_c == n) {
					ret->pos_x = x;
					ret->pos_y = y-pos;
					break;
				}
				m_c++;
				if((F[x][y-pos]>0)||(P[i][1] == -king)) {
					break;
				}
			}
			
			if(m_c == n) {
				break;
			}
			
			// Just try to move to the left.
			for(pos=1; (((x-pos)>=0)&&(F[x-pos][y]>=0)) ; pos++) {
				if(m_c == n) {
					ret->pos_x = x;
					ret->pos_y = y-pos;
					break;
				}
				m_c++;
				if((F[x-pos][y]>0)||(P[i][1] == -king)) {
					break;
				}
			}
			
			if(m_c == n) {
				break;
			}
			
			// Just try to move to the right.
			for(pos=1; (((x+pos)<8)&&(F[x+pos][y]>=0)) ; pos++) {
				if(m_c == n) {
					ret->pos_x = x;
					ret->pos_y = y+pos;
					break;
				}
				m_c++;
				if((F[x+pos][y]>0)||(P[i][1] == -king)) {
					break;
				}
			}
			if(m_c == n) {
				break;
			}
			
		}
		// White knights
		else if (P[i][1] == -knight) {
			// Up options
			if(((y+2)<8)&&((x-1)>=0)&&(F[x-1][y+2]>=0)) {
				if(m_c == n) {
					ret->pos_x = x-1;
					ret->pos_y = y+2;
					break;
				}
				m_c++;
			}
			if(((y+2)<8)&&((x+1)<8)&&(F[x+1][y+2]>=0)) {
				if(m_c == n) {
					ret->pos_x = x+1;
					ret->pos_y = y+2;
					break;
				}
				m_c++;
			}
			
			// Down options
			if(((y-2)>=0)&&((x-1)>=0)&&(F[x-1][y-2]>=0)) {
				if(m_c == n) {
					ret->pos_x = x-1;
					ret->pos_y = y-2;
					break;
				}
				m_c++;
			}
			if(((y-2)>=0)&&((x+1)<8)&&(F[x+1][y-2]>=0)) {
				if(m_c == n) {
					ret->pos_x = x+1;
					ret->pos_y = y-2;
					break;
				}
				m_c++;
			}
			
			// Left options
			if(((y-1)>=0)&&((x-2)>=0)&&(F[x-2][y-1]>=0)) {
				if(m_c == n) {
					ret->pos_x = x-2;
					ret->pos_y = y-1;
					break;
				}
				m_c++;
			}
			if(((y+1)<8)&&((x-2)>=0)&&(F[x-2][y+1]>=0)) {
				if(m_c == n) {
					ret->pos_x = x-2;
					ret->pos_y = y+1;
					break;
				}
				m_c++;
			}
			
			// Right options
			if(((y-1)>=0)&&((x+2)<8)&&(F[x+2][y-1]>=0)) {
				if(m_c == n) {
					ret->pos_x = x+2;
					ret->pos_y = y-1;
					break;
				}
				m_c++;
			}
			if(((y+1)<8)&&((x+2)<8)&&(F[x+2][y+1]>=0)) {
				if(m_c == n) {
					ret->pos_x = x+2;
					ret->pos_y = y+1;
					break;
				}
				m_c++;
			}
			
		}
		// White bishops
		else if ((P[i][1] == -bishop)||(P[i][1] == -queen)||(P[i][1] == -king)) {
			// Just try to move foward-right.
			for(pos=1; (((y+pos)<8)&&((x+pos)<8)&&(F[x+pos][y+pos]>=0)) ; pos++) {
				if(m_c == n) {
					ret->pos_x = x+pos;
					ret->pos_y = y+pos;
					break;
				}
				m_c++;
				if((F[x+pos][y+pos]>0)||(P[i][1] == -king)) {
					break;
				}
			}
			
			if(m_c == n) {
				break;
			}
			
			// Just try to move foward-left.
			for(pos=1; (((y+pos)<8)&&((x-pos)>=0)&&(F[x-pos][y+pos]>=0)) ; pos++) {
				if(m_c == n) {
					ret->pos_x = x-pos;
					ret->pos_y = y+pos;
					break;
				}
				m_c++;
				if((F[x-pos][y+pos]>0)||(P[i][1] == -king)) {
					break;
				}
			}
			
			if(m_c == n) {
				break;
			}
			
			// Just try to move backward-right.
			for(pos=1; (((y-pos)>=0)&&((x+pos)<8)&&(F[x+pos][y-pos]>=0)) ; pos++) {
				if(m_c == n) {
					ret->pos_x = x+pos;
					ret->pos_y = y-pos;
					break;
				}
				m_c++;
				if((F[x+pos][y-pos]>0)||(P[i][1] == -king)) {
					break;
				}
			}
			
			// Just try to move backward-left.
			for(pos=1; (((y-pos)>=0)&&((x-pos)>=0)&&(F[x-pos][y-pos]>=0)) ; pos++) {
				if(m_c == n) {
					ret->pos_x = x-pos;
					ret->pos_y = y-pos;
					break;
				}
				m_c++;
				if((F[x-pos][y-pos]>0)||(P[i][1] == -king)) {
					break;
				}
			}
			
			if(m_c == n) {
				break;
			}
		}
		
		// White Queen : Queen case solved with Bishop U Tower
		// else if(P[i][1] == -queen)
		
		// White King : King case solved with Bishop U Tower
		// else if(P[i][1] == -king) 
		
		// Black pawns
		else if(P[i][1] == pawn) {
			// Try to move foward. 
			if(((y-1)>=0) && (F[x][y-1]==0)) {
				if(m_c == n) {
					ret->pos_x = x;
					ret->pos_y = y-1;
					break;
				}
				m_c++;
			}
			// Check if it's the first move and can move 2x foward.
			if((y==6) && (F[x][y-1]==0) && (F[x][y-2]==0)) {
				if(m_c == n) {
					ret->pos_x = x;
					ret->pos_y = y-2;
					break;
				}
				m_c++;
			}
			
			// Try to get piece on foward-left
			if(((y-1)>=0) && ((x-1)>=0) && (F[x-1][y-1]<0)) {
				if(m_c == n) {
					ret->pos_x = x-1;
					ret->pos_y = y-1;
					break;
				}
				m_c++;	
			}
			
			// Try to get piece on foward-right
			if(((y-1)>=0) && ((x+1)<8) && (F[x-1][y-1]<0)) {
				if(m_c == n) {
					ret->pos_x = x+1;
					ret->pos_y = y-1;
					break;
				}
				m_c++;	
			}
		}
		
		// Black castles
		else if ((P[i][1] == castle)||(P[i][1] == queen)||(P[i][1] == king)) {
			// Just try to move foward.
			for(pos=1; (((y+pos)<8)&&(F[x][y+pos]<=0)) ; pos++) {
				if(m_c == n) {
					ret->pos_x = x;
					ret->pos_y = y+pos;
					break;
				}
				m_c++;
				if((F[x][y+pos]<0)||(P[i][1] == king)) {
					break;
				}
			}
			
			if(m_c == n) {
				break;
			}
			
			// Just try to move backward.
			for(pos=1; (((y-pos)>=0)&&(F[x][y-pos]<=0)) ; pos++) {
				if(m_c == n) {
					ret->pos_x = x;
					ret->pos_y = y-pos;
					break;
				}
				m_c++;
				if((F[x][y-pos]<0)||(P[i][1] == king)) {
					break;
				}
			}
			
			if(m_c == n) {
				break;
			}
			
			// Just try to move to the left.
			for(pos=1; (((x-pos)>=0)&&(F[x-pos][y]<=0)) ; pos++) {
				if(m_c == n) {
					ret->pos_x = x;
					ret->pos_y = y-pos;
					break;
				}
				m_c++;
				if((F[x-pos][y]<0)||(P[i][1] == king)) {
					break;
				}
			}
			
			if(m_c == n) {
				break;
			}
			
			// Just try to move to the right.
			for(pos=1; (((x+pos)<8)&&(F[x+pos][y]<=0)) ; pos++) {
				if(m_c == n) {
					ret->pos_x = x;
					ret->pos_y = y+pos;
					break;
				}
				m_c++;
				if((F[x+pos][y]<0)||(P[i][1] == king)) {
					break;
				}
			}
			
			if(m_c == n) {
				break;
			}
		}
		// Black knights
		else if (P[i][1] == knight) {
			// Up options
			if(((y+2)<8)&&((x-1)>=0)&&(F[x-1][y+2]<=0)) {
				if(m_c == n) {
					ret->pos_x = x-1;
					ret->pos_y = y+2;
					break;
				}
				m_c++;
			}
			if(((y+2)<8)&&((x+1)<8)&&(F[x+1][y+2]<=0)) {
				if(m_c == n) {
					ret->pos_x = x+1;
					ret->pos_y = y+2;
					break;
				}
				m_c++;
			}
			
			// Down options
			if(((y-2)>=0)&&((x-1)>=0)&&(F[x-1][y-2]<=0)) {
				if(m_c == n) {
					ret->pos_x = x-1;
					ret->pos_y = y-2;
					break;
				}
				m_c++;
			}
			if(((y-2)>=0)&&((x+1)<8)&&(F[x+1][y-2]<=0)) {
				if(m_c == n) {
					ret->pos_x = x+1;
					ret->pos_y = y-2;
					break;
				}
				m_c++;
			}
			
			// Left options
			if(((y-1)>=0)&&((x-2)>=0)&&(F[x-2][y-1]<=0)) {
				if(m_c == n) {
					ret->pos_x = x-2;
					ret->pos_y = y-1;
					break;
				}
				m_c++;
			}
			if(((y+1)<8)&&((x-2)>=0)&&(F[x-2][y+1]<=0)) {
				if(m_c == n) {
					ret->pos_x = x-2;
					ret->pos_y = y+1;
					break;
				}
				m_c++;
			}
			
			// Right options
			if(((y-1)>=0)&&((x+2)<8)&&(F[x+2][y-1]<=0)) {
				if(m_c == n) {
					ret->pos_x = x+2;
					ret->pos_y = y-1;
					break;
				}
				m_c++;
			}
			if(((y+1)<8)&&((x+2)<8)&&(F[x+2][y+1]<=0)) {
				if(m_c == n) {
					ret->pos_x = x+2;
					ret->pos_y = y+1;
					break;
				}
				m_c++;
			}
			
		}
		// Black bishops
		else if ((P[i][1] == bishop)||(P[i][1] == queen)||(P[i][1] == king)) {
			// Just try to move foward-right.
			for(pos=1; (((y+pos)<8)&&((x+pos)<8)&&(F[x+pos][y+pos]<=0)) ; pos++) {
				if(m_c == n) {
					ret->pos_x = x+pos;
					ret->pos_y = y+pos;
					break;
				}
				m_c++;
				if((F[x+pos][y+pos]<0)||(P[i][1] == king)) {
					break;
				}
			}
			
			if(m_c == n) {
				break;
			}
			
			// Just try to move foward-left.
			for(pos=1; (((y+pos)<8)&&((x-pos)>=0)&&(F[x-pos][y+pos]<=0)) ; pos++) {
				if(m_c == n) {
					ret->pos_x = x-pos;
					ret->pos_y = y+pos;
					break;
				}
				m_c++;
				if((F[x-pos][y+pos]<0)||(P[i][1] == king)) {
					break;
				}
			}
			
			if(m_c == n) {
				break;
			}
			
			// Just try to move backward-right.
			for(pos=1; (((y-pos)>=0)&&((x+pos)<8)&&(F[x+pos][y-pos]<=0)) ; pos++) {
				if(m_c == n) {
					ret->pos_x = x+pos;
					ret->pos_y = y-pos;
					break;
				}
				m_c++;
				if((F[x+pos][y-pos]<0)||(P[i][1] == king)) {
					break;
				}
			}
			
			if(m_c == n) {
				break;
			}
			
			// Just try to move backward-left.
			for(pos=1; (((y-pos)>=0)&&((x-pos)>=0)&&(F[x-pos][y-pos]<=0)) ; pos++) {
				if(m_c == n) {
					ret->pos_x = x-pos;
					ret->pos_y = y-pos;
					break;
				}
				m_c++;
				if((F[x-pos][y-pos]<0)||(P[i][1] == king)) {
					break;
				}
			}
			
			if(m_c == n) {
				break;
			}
			
		}
		// Black Queen
		//else if(P[i][1] == queen) {

		// Black King
		//else if(P[i][1] == king) {
	}
	
	// If found, update index and k_index.
	if(m_c == n) {
		ret->index = i;
		ret->l_pos_x = P[i][3];
		ret->l_pos_y = P[i][4];
		if(F[ret->pos_x][ret->pos_y]!=0) {
			// Marcus, sei que "i" podia ser usado aqui. Compilador cuida disso.
			for(pos=0; pos<32; pos++) {
				if((P[pos][3] == ret->pos_x) && (P[pos][4] == ret->pos_y)) {
					break;
				}
			}
			
			ret->index = pos;
		}
		else {
			ret->k_index = -1;
		}
	}
	// If don't found, just make the index -1.
	else {
		ret->index = -1;
	}
}

// Make pieces table using field table, returns score.
// Player -1 = white, Player + 1 = black
double mount_pieces(int ** F, int ** P, int player) {
	int i, j;
	double score, value;
	
	for(i=0; i<16; i++) {
		P[i][0] = 0;
		P[i][4] = -1;
		P[i][5] = -1;
	}
	
	score = 0;
	for(i=0; i<8; i++) {
		for(j=0; j<8; j++) {
			// no piece
			if(F[i][j]==0) {
				continue;
			}
			
			value = 0;
			if(abs(F[i][j]) == pawn) {
				if(F[i][j]<0) {
					value = pawn_value*(-1)*player;
				}
				else {
					value = pawn_value*player;
				}
			}
			else if(abs(F[i][j]) == castle) {
				if(F[i][j]<0) {
					value = castle_value*(-1)*player;
				}
				else {
					value = castle_value*player;
				}
			}
			else if(abs(F[i][j]) == knight) {
				if(F[i][j]<0) {
					value = knight_value*(-1)*player;
				}
				else {
					value = knight_value*player;
				}
			}
			else if(abs(F[i][j]) == bishop) {
				if(F[i][j]<0) {
					value = bishop_value*(-1)*player;
				}
				else {
					value = bishop_value*player;
				}
			}
			else if(abs(F[i][j]) == queen) {
				if(F[i][j]<0) {
					value = queen_value*(-1)*player;
				}
				else {
					value = queen_value*player;
				}
			}
			else if(abs(F[i][j]) == king) {
				if(F[i][j]<0) {
					value = king_value*(-1)*player;
				}
				else {
					value = king_value*player;
				}
			}
			else {
				printf("Not recognized piece\n");
				return 0;
			}
			
			// IsAlive, type and score
			P[i][0] = 1;
			P[i][1] = F[i][j];
			P[i][2] = value;
			// pos x and pos 
			P[i][3] = i;
			P[i][4] = j;
		}
	}
	return score;
}

// Update score table and alpha/beta values. 
// Return -1 if alpha-beta condition wasn't matched, 1 otherwise.
int updateScore(double * best_score, int depth, int score, double *alpha, double *beta) {
	// Even
	if((depth%2 == 0)) {
		if(score > best_score[depth]) {
			*alpha = score;
		}
		if(beta > alpha){
            return 1;
        }
	}
	
	// Odd
	else {
		if(score > best_score[depth]) {
			*beta = score;
		}
		if(beta < alpha){
            return 1;
        }
	}
	
	return -1;
}

// F = field[8][8] converted to this system, max_depth, player: -1 white. 1 black.
int AlphaBeta(int **F, int max_depth, int player) {
	struct queue Q;
	int P[num_pieces][num_col];
	int depth = 0, i, found_move, u_ret=-1;
	double alpha, beta, score;
	double * best_score;
	int mov_counter = 0;
	struct moviment * next;
	struct moviment * mov;
	
	score = mount_pieces(F, P, player);		// Mount pieces table.
	init_queue(&Q);							// Init queue
	best_score = (double *) malloc (sizeof(double)*max_depth);
	
	for(i=0; i<max_depth; i++) {
		best_score[i] = -999999;	// -inf. 
	}
	
	while(Q.pop_pos >= 0) {
		found_move = -1;
		if((depth <= max_depth)&&(u_ret<0)) {			// If can expand, try!
			next = alloc_mov(&Q);						// Get new move.
			find_nth_move(F, P, mov_counter, next);		// Find next move.
			if(next->index >= 0) {						// If valid move was found.
				score += apply_move(F,P,next);
				depth++;
				found_move = 1;
				u_ret = updateScore(best_score, depth, score, &alpha, &beta);
			}
		}
		if(found_move < 0) {	// If valid move wasn't found or max_depth was reached.
			mov = pop_mov(&Q);
			score += undo_move(F,P,mov);
			depth--;
			u_ret = updateScore(best_score, depth, score, &alpha, &beta);
		}
	}
	
	free_queue(&Q);
	free(best_score);
	return 0;
}

int main() {
	int player = -1, max_depth = 5;
	int F[8][8];
	F[0][0] = -castle; 	F[0][1] = -knight; 	F[0][2] = -bishop; 	F[0][3] = -queen;
	F[0][4] = -king; 	F[0][5] = -bishop; 	F[0][6] = -knight; 	F[0][7] = -castle;
	F[1][0] = -pawn; 	F[1][1] = -pawn; 	F[1][2] = -pawn; 	F[1][3] = -pawn;
	F[1][4] = -pawn; 	F[1][5] = -pawn; 	F[1][6] = -pawn; 	F[1][7] = -pawn;
	F[6][0] = -pawn; 	F[6][1] = -pawn; 	F[6][2] = -pawn; 	F[6][3] = -pawn;
	F[6][4] = -pawn; 	F[6][5] = -pawn; 	F[6][6] = -pawn; 	F[6][7] = -pawn;
	F[7][0] = castle; 	F[7][1] = knight; 	F[7][2] = bishop; 	F[7][3] = queen;
	F[7][4] = king; 	F[7][5] = bishop; 	F[7][6] = knight; 	F[7][7] = castle;
	
	AlphaBeta(F, max_depth, player);
	
	return 0;
}

/*
Board* ABMinMax(Board* board, short int depth_limit) {
    return ABMaxMove(board, depth_limit, 1, 0, 0);
}
Board* ABMaxMove(Board* board, short int depth_limit, short int depth, int a, int b) {
    vector<Board*> *moves;
    Board* best_move = NULL;
    Board* best_real_move = NULL;
    Board* move = NULL;
    int alpha = a,beta = b;

    if (depth >= depth_limit) {//if depth limit is reached
        return board;
    } else {
        moves = board->list_all_moves();
        for (vector<Board*>::iterator it = moves->begin(); it != moves->end(); it++) {
            move = ABMinMove(*it, depth_limit, depth+1, alpha, beta);
            if (best_move == NULL || move->evaluate_board(Black)
                    > best_move->evaluate_board(Black)) {
                best_move = move;
                best_real_move = *it;
                alpha = move->evaluate_board(Black);
            }
            if(beta > alpha){
                return best_real_move;
            }
        }
        return best_real_move;
    }
}
Board* ABMinMove(Board* board, short int depth_limit, short int depth, int a, int b) {
    vector<Board*> *moves;
    Board* best_move = NULL;
    Board* best_real_move = NULL;
    Board* move = NULL;
    int alpha = a,beta = b;

    if (depth >= depth_limit) {//if depth limit is reached
        return board;
    } else {
        moves = board->list_all_moves();
        for (vector<Board*>::iterator it = moves->begin(); it != moves->end(); it++) {
            move = ABMaxMove(*it, depth_limit, depth+1, alpha, beta);
            if (best_move == NULL || move->evaluate_board(White)
                    < best_move->evaluate_board(White)) {
                best_move = move;
                best_real_move = *it;
                beta = move->evaluate_board(White);
            }
            if(beta < alpha){
                return best_real_move;
            }
        }
        return best_real_move;
    }
}

#endif */
