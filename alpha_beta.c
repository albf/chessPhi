#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

// Pieces tables defines.
// [isAlive, type, value, pos x, pos y]
#define num_pieces 32
#define num_col 5

#define col_is_alive 0
#define col_pos_x 1
#define col_pos_y 2
#define col_k_index 3

#define depth_factor 0.0625

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
#define king_value 1000
// White is negative, Black is positive.

int ALPHA_CUT = 0;
int BETA_CUT = 0;
int D_COUNTER = 0;
int DEBUG = 0;

// Represents a past moviment.
struct moviment { 
    int index;                      // Index of moving piece
    int l_pos_x;                    // X before move (last pos).
    int l_pos_y;                    // Y before move (last pos).
    int pos_x;                      // new X value.
    int pos_y;                      // new Y value.
    int k_index;                    // Index of killed piece, if any. If not, -1.
    int refresh;                    // 1 if new moviment was found, -1 if not.
    int pro_depth;                  // Promote depth, for pawns.
    int pro_type;                   // Promote depth, for pawns.
    int pro_sign;                   // Sign before promotion 
    int d_counter;                  // debug counter.
};

// Queue of done moviments. Used to avoid recursion.
struct queue{
    int size;               // current size of queue.
    int pop_pos;            // current pop index.
    struct moviment * mov;  // list of moviments.
};

// Alpha-Beta Values 
struct stack{
    double alpha;
    double beta;
    double score;
};

struct moviment * alpha_beta(int F[8][8], int max_depth, int player, double * score);
void find_nth_move(int F[8][8], int P[num_pieces][num_col], int n, struct moviment * ret, int player, int depth);
double mount_pieces(int F[][8], int P[][num_col], int player);
int update_score(struct stack * best_score, int depth, struct queue * Q, struct moviment * best_move, double * score);
int apply_move(int F[8][8], int P[num_pieces][num_col], struct moviment * mov);
void do_move(int F[8][8], struct moviment * mov);
void print_field(int F[8][8]);
void print_player(int P[num_pieces][num_col]);


// Init the moviment queue.
void init_queue(struct queue * Q, int size) {
    Q->size = size;
    Q->pop_pos = 0;
    Q->mov = (struct moviment *) malloc (sizeof(struct moviment)*Q->size);
}

// Free queue memory.
void free_queue(struct queue * Q) {
    free(Q->mov);
}

// Guarantee that it's possible to add a new moviment.
void assign_size(struct queue * Q) {
    if(Q->size == Q->pop_pos) {
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

// Get best mov.
struct moviment * get_next(struct queue * Q) {
    return &(Q->mov[0]);
}

// Subtract two timeval struct
void timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;
}

// Undo a given move, returns difference in score.
int undo_move(int F[8][8], int P[num_pieces][num_col], struct moviment * mov, int depth) { 
    int score_diff = 0;

    // If the move was a promotion
    if(mov->pro_depth == depth) {
        if(mov->pro_type > 0) { 
            P[mov->index][1] = pawn; 
        }
        else {
            P[mov->index][1] = -pawn; 
        }
        score_diff -= P[mov->index][2];
        P[mov->index][2] = pawn_value*mov->pro_sign;
        score_diff += P[mov->index][2];
    }

    P[mov->index][3] = mov->l_pos_x;
    P[mov->index][4] = mov->l_pos_y;
    F[mov->l_pos_x][mov->l_pos_y] = P[mov->index][1];

    // Revive piece.
    if(mov->k_index >= 0) {
        F[mov->pos_x][mov->pos_y] = P[mov->k_index][1];
        P[mov->k_index][0] = 1;
        score_diff += P[mov->k_index][2];
    }
    else {
        F[mov->pos_x][mov->pos_y] = 0;
    }
    return score_diff;
}

// Apply a given move, return difference in score.
int apply_move(int F[8][8], int P[num_pieces][num_col], struct moviment * mov) {
    int score_diff = 0;       

    // Kill this piece.
    if(mov->k_index >= 0) {
        P[mov->k_index][0] = -1;
        score_diff -= P[mov->k_index][2];
    }

    // Promotion of pawn
    if(mov->pro_depth >= 0) {
        if(abs(mov->pro_type) == queen) {
            score_diff = score_diff + ((-pawn_value + queen_value)*mov->pro_sign);
            P[mov->index][2] = queen_value*mov->pro_sign;

        }
        if(abs(mov->pro_type) == castle) {
            score_diff = score_diff + ((-pawn_value + castle_value)*mov->pro_sign);
            P[mov->index][2] = castle_value*mov->pro_sign;
        }
        if(abs(mov->pro_type) == bishop) {
            score_diff = score_diff + ((-pawn_value + bishop_value)*mov->pro_sign);
            P[mov->index][2] = bishop_value*mov->pro_sign;
        }
        if(abs(mov->pro_type) == knight) {
            score_diff = score_diff + ((-pawn_value + knight_value)*mov->pro_sign);
            P[mov->index][2] = knight_value*mov->pro_sign;
        }

        P[mov->index][1] = mov->pro_type;
    }

    P[mov->index][3] = mov->pos_x;
    P[mov->index][4] = mov->pos_y;
    F[mov->pos_x][mov->pos_y] = P[mov->index][1];
    F[mov->l_pos_x][mov->l_pos_y] = 0;

    //printf("APPLY MOVE. SCORE_DIFF:%d\n", score_diff);

    return score_diff;
}

void do_move(int F[8][8], struct moviment * mov) {
    // Promotion of pawn
    if(mov->pro_depth >= 0) {
        F[mov->pos_x][mov->pos_y] = mov->pro_type;
    }
    else {
        F[mov->pos_x][mov->pos_y] = F[mov->l_pos_x][mov->l_pos_y];
    }

    F[mov->l_pos_x][mov->l_pos_y] = 0;
}

// Return next nTH move in ret. If couldn't found, return -1 in ret->index. Don't apply the move.
void find_nth_move(int F[8][8], int P[num_pieces][num_col], int n, struct moviment * ret, int player, int depth) {
    int i, x, y, m_c=-1, pos, pro_depth = -1, pro_type = -1;

    ret->d_counter = D_COUNTER;
    D_COUNTER++;
    
    for(i=0; i<32; i++) {
        // Discard dead or oponent pieces.
        if((P[i][0] < 1)||(P[i][1]*player<0)){
            continue;
        }
        
        x = P[i][3];
        y = P[i][4];

        if(player < 0) {
            // White pawns
            if(P[i][1] == -pawn) {
                // Try to move foward. 
                if(((y+1)<8) && (F[x][y+1]==0)) {
                    if(y<6) {   // No Promotion
                        m_c++;
                        if(m_c == n) {
                            ret->pos_x = x;
                            ret->pos_y = y+1;
                            break;
                        }
                    }
                    else {      // Promotion
                        //printf("PROMOTION FINDING, DEPTH:%d\n", depth);
                        m_c++;
                        if(m_c == n) {
                            pro_depth = depth;
                            pro_type = -queen;
                            ret->pos_x = x;
                            ret->pos_y = y+1;
                            break;
                        }
                        m_c++;
                        if(m_c == n) {
                            pro_depth = depth;
                            pro_type = -castle;
                            ret->pos_x = x;
                            ret->pos_y = y+1;
                            break;
                        }
                        m_c++;
                        if(m_c == n) {
                            pro_depth = depth;
                            pro_type = -bishop;
                            ret->pos_x = x;
                            ret->pos_y = y+1;
                            break;
                        }
                        m_c++;
                        if(m_c == n) {
                            pro_depth = depth;
                            pro_type = -knight;
                            ret->pos_x = x;
                            ret->pos_y = y+1;
                            break;
                        }
                    }
                }
                // Check if it's the first move and can move 2x foward.
                if((y==1) && (F[x][y+1]==0) && (F[x][y+2]==0)) {
                    m_c++;
                    if(m_c == n) {
                        ret->pos_x = x;
                        ret->pos_y = y+2;
                        break;
                    }
                }
                
                // Try to get piece on foward-left
                if(((y+1)<8) && ((x-1)>=0) && (F[x-1][y+1]>0)) {
                    if(y<6) {
                        m_c++;  
                        if(m_c == n) {
                            ret->pos_x = x-1;
                            ret->pos_y = y+1;
                            break;
                        }
                    }
                    else {      // Promotion
                        m_c++;
                        if(m_c == n) {
                            pro_depth = depth;
                            pro_type = -queen;
                            ret->pos_x = x-1;
                            ret->pos_y = y+1;
                            break;
                        }
                        m_c++;
                        if(m_c == n) {
                            pro_depth = depth;
                            pro_type = -castle;
                            ret->pos_x = x-1;
                            ret->pos_y = y+1;
                            break;
                        }
                        m_c++;
                        if(m_c == n) {
                            pro_depth = depth;
                            pro_type = -bishop;
                            ret->pos_x = x-1;
                            ret->pos_y = y+1;
                            break;
                        }
                        m_c++;
                        if(m_c == n) {
                            pro_depth = depth;
                            pro_type = -knight;
                            ret->pos_x = x-1;
                            ret->pos_y = y+1;
                            break;
                        }
                    }

                }
                
                // Try to get piece on foward-right
                if(((y+1)<8) && ((x+1)<8) && (F[x+1][y+1]>0)) {
                    if(y<6) {
                        m_c++;  
                        if(m_c == n) {
                            ret->pos_x = x+1;
                            ret->pos_y = y+1;
                            break;
                        }
                    }
                    else {      // Promotion
                        m_c++;
                        if(m_c == n) {
                            pro_depth = depth;
                            pro_type = -queen;
                            ret->pos_x = x+1;
                            ret->pos_y = y+1;
                            break;
                        }
                        m_c++;
                        if(m_c == n) {
                            pro_depth = depth;
                            pro_type = -castle;
                            ret->pos_x = x+1;
                            ret->pos_y = y+1;
                            break;
                        }
                        m_c++;
                        if(m_c == n) {
                            pro_depth = depth;
                            pro_type = -bishop;
                            ret->pos_x = x+1;
                            ret->pos_y = y+1;
                            break;
                        }
                        m_c++;
                        if(m_c == n) {
                            pro_depth = depth;
                            pro_type = -knight;
                            ret->pos_x = x+1;
                            ret->pos_y = y+1;
                            break;
                        }
                    }

                }
            }
            // White castles/bishops/king/queen
            else if ((P[i][1] == -castle)||(P[i][1] == -queen)||(P[i][1] == -king)||(P[i][1] == -bishop)) {
                // Castle
                if ((P[i][1] == -castle)||(P[i][1] == -queen)||(P[i][1] == -king)) {
                    // Just try to move foward.
                    for(pos=1; (((y+pos)<8)&&(F[x][y+pos]>=0)) ; pos++) {
                        m_c++;
                        if(m_c == n) {
                            ret->pos_x = x;
                            ret->pos_y = y+pos;
                            break;
                        }
                        if((F[x][y+pos]>0)||(P[i][1] == -king)) {
                            break;
                        }
                    }
                    
                    if(m_c == n) {
                        break;
                    }
                    
                    // Just try to move backward.
                    for(pos=1; (((y-pos)>=0)&&(F[x][y-pos]>=0)) ; pos++) {
                        m_c++;
                        if(m_c == n) {
                            ret->pos_x = x;
                            ret->pos_y = y-pos;
                            break;
                        }
                        if((F[x][y-pos]>0)||(P[i][1] == -king)) {
                            break;
                        }
                    }
                    
                    if(m_c == n) {
                        break;
                    }
                    
                    // Just try to move to the left.
                    for(pos=1; (((x-pos)>=0)&&(F[x-pos][y]>=0)) ; pos++) {
                        m_c++;
                        if(m_c == n) {
                            ret->pos_x = x-pos;
                            ret->pos_y = y;
                            break;
                        }
                        if((F[x-pos][y]>0)||(P[i][1] == -king)) {
                            break;
                        }
                    }
                    
                    if(m_c == n) {
                        break;
                    }
                    
                    // Just try to move to the right.
                    for(pos=1; (((x+pos)<8)&&(F[x+pos][y]>=0)) ; pos++) {
                        m_c++;
                        if(m_c == n) {
                            ret->pos_x = x+pos;
                            ret->pos_y = y;
                            break;
                        }
                        if((F[x+pos][y]>0)||(P[i][1] == -king)) {
                            break;
                        }
                    }
                    if(m_c == n) {
                        break;
                    }
                }

                // White bishops
                if ((P[i][1] == -bishop)||(P[i][1] == -queen)||(P[i][1] == -king)) {
                    // Just try to move foward-right.
                    for(pos=1; (((y+pos)<8)&&((x+pos)<8)&&(F[x+pos][y+pos]>=0)) ; pos++) {
                        m_c++;
                        if(m_c == n) {
                            ret->pos_x = x+pos;
                            ret->pos_y = y+pos;
                            break;
                        }
                        if((F[x+pos][y+pos]>0)||(P[i][1] == -king)) {
                            break;
                        }
                    }
                    
                    if(m_c == n) {
                        break;
                    }
                    
                    // Just try to move foward-left.
                    for(pos=1; (((y+pos)<8)&&((x-pos)>=0)&&(F[x-pos][y+pos]>=0)) ; pos++) {
                        m_c++;
                        if(m_c == n) {
                            ret->pos_x = x-pos;
                            ret->pos_y = y+pos;
                            break;
                        }
                        if((F[x-pos][y+pos]>0)||(P[i][1] == -king)) {
                            break;
                        }
                    }
                    
                    if(m_c == n) {
                        break;
                    }

                    // Just try to move backward-right.
                    for(pos=1; (((y-pos)>=0)&&((x+pos)<8)&&(F[x+pos][y-pos]>=0)) ; pos++) {
                        m_c++;
                        if(m_c == n) {
                            ret->pos_x = x+pos;
                            ret->pos_y = y-pos;
                            break;
                        }
                        if((F[x+pos][y-pos]>0)||(P[i][1] == -king)) {
                            break;
                        }
                    }

                    if(m_c == n) {
                        break;
                    }
                    
                    // Just try to move backward-left.
                    for(pos=1; (((y-pos)>=0)&&((x-pos)>=0)&&(F[x-pos][y-pos]>=0)) ; pos++) {
                        m_c++;
                        if(m_c == n) {
                            ret->pos_x = x-pos;
                            ret->pos_y = y-pos;
                            break;
                        }
                        if((F[x-pos][y-pos]>0)||(P[i][1] == -king)) {
                            break;
                        }
                    }
                    
                    if(m_c == n) {
                        break;
                    }
                }
            }
            // White knights
            else if (P[i][1] == -knight) {
                // Up options
                if(((y+2)<8)&&((x-1)>=0)&&(F[x-1][y+2]>=0)) {
                    m_c++;
                    if(m_c == n) {
                        ret->pos_x = x-1;
                        ret->pos_y = y+2;
                        break;
                    }
                }
                if(((y+2)<8)&&((x+1)<8)&&(F[x+1][y+2]>=0)) {
                    m_c++;
                    if(m_c == n) {
                        ret->pos_x = x+1;
                        ret->pos_y = y+2;
                        break;
                    }
                }
                
                // Down options
                if(((y-2)>=0)&&((x-1)>=0)&&(F[x-1][y-2]>=0)) {
                    m_c++;
                    if(m_c == n) {
                        ret->pos_x = x-1;
                        ret->pos_y = y-2;
                        break;
                    }
                }
                if(((y-2)>=0)&&((x+1)<8)&&(F[x+1][y-2]>=0)) {
                    m_c++;
                    if(m_c == n) {
                        ret->pos_x = x+1;
                        ret->pos_y = y-2;
                        break;
                    }
                }
                
                // Left options
                if(((y-1)>=0)&&((x-2)>=0)&&(F[x-2][y-1]>=0)) {
                    m_c++;
                    if(m_c == n) {
                        ret->pos_x = x-2;
                        ret->pos_y = y-1;
                        break;
                    }
                }
                if(((y+1)<8)&&((x-2)>=0)&&(F[x-2][y+1]>=0)) {
                    m_c++;
                    if(m_c == n) {
                        ret->pos_x = x-2;
                        ret->pos_y = y+1;
                        break;
                    }
                }
                
                // Right options
                if(((y-1)>=0)&&((x+2)<8)&&(F[x+2][y-1]>=0)) {
                    m_c++;
                    if(m_c == n) {
                        ret->pos_x = x+2;
                        ret->pos_y = y-1;
                        break;
                    }
                }
                if(((y+1)<8)&&((x+2)<8)&&(F[x+2][y+1]>=0)) {
                    m_c++;
                    if(m_c == n) {
                        ret->pos_x = x+2;
                        ret->pos_y = y+1;
                        break;
                    }
                }
            }
        }
        else {
                
                // Black pawns
                if(P[i][1] == pawn) {
                // Try to move foward. 
                if(((y-1)>=0) && (F[x][y-1]==0)) {
                    if(y>1) {
                        m_c++;
                        if(m_c == n) {
                            ret->pos_x = x;
                            ret->pos_y = y-1;
                            break;
                        }
                    }
                    else {      // Promotion
                        m_c++;
                        if(m_c == n) {
                            pro_depth = depth;
                            pro_type = queen;
                            ret->pos_x = x;
                            ret->pos_y = y-1;
                            break;
                        }
                        m_c++;
                        if(m_c == n) {
                            pro_depth = depth;
                            pro_type = castle;
                            ret->pos_x = x;
                            ret->pos_y = y-1;
                            break;
                        }
                        m_c++;
                        if(m_c == n) {
                            pro_depth = depth;
                            pro_type = bishop;
                            ret->pos_x = x;
                            ret->pos_y = y-1;
                            break;
                        }
                        m_c++;
                        if(m_c == n) {
                            pro_depth = depth;
                            pro_type = knight;
                            ret->pos_x = x;
                            ret->pos_y = y-1;
                            break;
                        }
                    }

                }
                // Check if it's the first move and can move 2x foward.
                if((y==6) && (F[x][y-1]==0) && (F[x][y-2]==0)) {
                    m_c++;
                    if(m_c == n) {
                        ret->pos_x = x;
                        ret->pos_y = y-2;
                        break;
                    }
                }
                
                // Try to get piece on foward-left
                if(((y-1)>=0) && ((x-1)>=0) && (F[x-1][y-1]<0)) {
                    if(y>1) {
                        m_c++;  
                        if(m_c == n) {
                            ret->pos_x = x-1;
                            ret->pos_y = y-1;
                            break;
                        }
                    }
                    else {      // Promotion
                        m_c++;
                        if(m_c == n) {
                            pro_depth = depth;
                            pro_type = queen;
                            ret->pos_x = x-1;
                            ret->pos_y = y-1;
                            break;
                        }
                        m_c++;
                        if(m_c == n) {
                            pro_depth = depth;
                            pro_type = castle;
                            ret->pos_x = x-1;
                            ret->pos_y = y-1;
                            break;
                        }
                        m_c++;
                        if(m_c == n) {
                            pro_depth = depth;
                            pro_type = bishop;
                            ret->pos_x = x-1;
                            ret->pos_y = y-1;
                            break;
                        }
                        m_c++;
                        if(m_c == n) {
                            pro_depth = depth;
                            pro_type = knight;
                            ret->pos_x = x-1;
                            ret->pos_y = y-1;
                            break;
                        }
                    }
                }
                
                // Try to get piece on foward-right
                if(((y-1)>=0) && ((x+1)<8) && (F[x+1][y-1]<0)) {
                    if(y>1) {
                        m_c++;  
                        if(m_c == n) {
                            ret->pos_x = x+1;
                            ret->pos_y = y-1;
                            break;
                        }
                    }
                    else {      // Promotion
                        m_c++;
                        if(m_c == n) {
                            pro_depth = depth;
                            pro_type = queen;
                            ret->pos_x = x+1;
                            ret->pos_y = y-1;
                            break;
                        }
                        m_c++;
                        if(m_c == n) {
                            pro_depth = depth;
                            pro_type = castle;
                            ret->pos_x = x+1;
                            ret->pos_y = y-1;
                            break;
                        }
                        m_c++;
                        if(m_c == n) {
                            pro_depth = depth;
                            pro_type = bishop;
                            ret->pos_x = x+1;
                            ret->pos_y = y-1;
                            break;
                        }
                        m_c++;
                        if(m_c == n) {
                            pro_depth = depth;
                            pro_type = knight;
                            ret->pos_x = x+1;
                            ret->pos_y = y-1;
                            break;
                        }
                    }
                }
            }
            
            // Black castles
            else if ((P[i][1] == castle)||(P[i][1] == queen)||(P[i][1] == king)||(P[i][1] == bishop)) {
                if ((P[i][1] == castle)||(P[i][1] == queen)||(P[i][1] == king)) {
                    // Just try to move foward.
                    for(pos=1; (((y+pos)<8)&&(F[x][y+pos]<=0)) ; pos++) {
                        m_c++;
                        if(m_c == n) {
                            ret->pos_x = x;
                            ret->pos_y = y+pos;
                            break;
                        }
                        if((F[x][y+pos]<0)||(P[i][1] == king)) {
                            break;
                        }
                    }
                    
                    if(m_c == n) {
                        break;
                    }
                    
                    // Just try to move backward.
                    for(pos=1; (((y-pos)>=0)&&(F[x][y-pos]<=0)) ; pos++) {
                        m_c++;
                        if(m_c == n) {
                            ret->pos_x = x;
                            ret->pos_y = y-pos;
                            break;
                        }
                        if((F[x][y-pos]<0)||(P[i][1] == king)) {
                            break;
                        }
                    }
                    
                    if(m_c == n) {
                        break;
                    }
                    
                    // Just try to move to the left.
                    for(pos=1; (((x-pos)>=0)&&(F[x-pos][y]<=0)) ; pos++) {
                        m_c++;
                        if(m_c == n) {
                            ret->pos_x = x-pos;
                            ret->pos_y = y;
                            break;
                        }
                        if((F[x-pos][y]<0)||(P[i][1] == king)) {
                            break;
                        }
                    }
                    
                    if(m_c == n) {
                        break;
                    }
                    
                    // Just try to move to the right.
                    for(pos=1; (((x+pos)<8)&&(F[x+pos][y]<=0)) ; pos++) {
                        m_c++;
                        if(m_c == n) {
                            ret->pos_x = x+pos;
                            ret->pos_y = y;
                            break;
                        }
                        if((F[x+pos][y]<0)||(P[i][1] == king)) {
                            break;
                        }
                    }
                    
                    if(m_c == n) {
                        break;
                    }
                }

                // Black bishops
                if ((P[i][1] == bishop)||(P[i][1] == queen)||(P[i][1] == king)) {
                    // Just try to move foward-right.
                    for(pos=1; (((y+pos)<8)&&((x+pos)<8)&&(F[x+pos][y+pos]<=0)) ; pos++) {
                        m_c++;
                        if(m_c == n) {
                            ret->pos_x = x+pos;
                            ret->pos_y = y+pos;
                            break;
                        }
                        if((F[x+pos][y+pos]<0)||(P[i][1] == king)) {
                            break;
                        }
                    }
                    
                    if(m_c == n) {
                        break;
                    }
                    
                    // Just try to move foward-left.
                    for(pos=1; (((y+pos)<8)&&((x-pos)>=0)&&(F[x-pos][y+pos]<=0)) ; pos++) {
                        m_c++;
                        if(m_c == n) {
                            ret->pos_x = x-pos;
                            ret->pos_y = y+pos;
                            break;
                        }
                        if((F[x-pos][y+pos]<0)||(P[i][1] == king)) {
                            break;
                        }
                    }
                    
                    if(m_c == n) {
                        break;
                    }
                    
                    // Just try to move backward-right.
                    for(pos=1; (((y-pos)>=0)&&((x+pos)<8)&&(F[x+pos][y-pos]<=0)) ; pos++) {
                        m_c++;
                        if(m_c == n) {
                            ret->pos_x = x+pos;
                            ret->pos_y = y-pos;
                            break;
                        }
                        if((F[x+pos][y-pos]<0)||(P[i][1] == king)) {
                            break;
                        }
                    }
                    
                    if(m_c == n) {
                        break;
                    }
                    
                    // Just try to move backward-left.
                    for(pos=1; (((y-pos)>=0)&&((x-pos)>=0)&&(F[x-pos][y-pos]<=0)) ; pos++) {
                        m_c++;
                        if(m_c == n) {
                            ret->pos_x = x-pos;
                            ret->pos_y = y-pos;
                            break;
                        }
                        if((F[x-pos][y-pos]<0)||(P[i][1] == king)) {
                            break;
                        }
                    }
                    
                    if(m_c == n) {
                        break;
                    }
                    
                }

            }
            // Black knights
            else if (P[i][1] == knight) {
                // Up options
                if(((y+2)<8)&&((x-1)>=0)&&(F[x-1][y+2]<=0)) {
                    m_c++;
                    if(m_c == n) {
                        ret->pos_x = x-1;
                        ret->pos_y = y+2;
                        break;
                    }
                }
                if(((y+2)<8)&&((x+1)<8)&&(F[x+1][y+2]<=0)) {
                    m_c++;
                    if(m_c == n) {
                        ret->pos_x = x+1;
                        ret->pos_y = y+2;
                        break;
                    }
                }
                
                // Down options
                if(((y-2)>=0)&&((x-1)>=0)&&(F[x-1][y-2]<=0)) {
                    m_c++;
                    if(m_c == n) {
                        ret->pos_x = x-1;
                        ret->pos_y = y-2;
                        break;
                    }
                }
                if(((y-2)>=0)&&((x+1)<8)&&(F[x+1][y-2]<=0)) {
                    m_c++;
                    if(m_c == n) {
                        ret->pos_x = x+1;
                        ret->pos_y = y-2;
                        break;
                    }
                }
                
                // Left options
                if(((y-1)>=0)&&((x-2)>=0)&&(F[x-2][y-1]<=0)) {
                    m_c++;
                    if(m_c == n) {
                        ret->pos_x = x-2;
                        ret->pos_y = y-1;
                        break;
                    }
                }
                if(((y+1)<8)&&((x-2)>=0)&&(F[x-2][y+1]<=0)) {
                    m_c++;
                    if(m_c == n) {
                        ret->pos_x = x-2;
                        ret->pos_y = y+1;
                        break;
                    }
                }
                
                // Right options
                if(((y-1)>=0)&&((x+2)<8)&&(F[x+2][y-1]<=0)) {
                    m_c++;
                    if(m_c == n) {
                        ret->pos_x = x+2;
                        ret->pos_y = y-1;
                        break;
                    }
                }
                if(((y+1)<8)&&((x+2)<8)&&(F[x+2][y+1]<=0)) {
                    m_c++;
                    if(m_c == n) {
                        ret->pos_x = x+2;
                        ret->pos_y = y+1;
                        break;
                    }
                }
                
            }
        }
    }
    
    // If found, update index and k_index.
    if(m_c == n) {
        ret->refresh = 1;
        ret->index = i;
        ret->l_pos_x = P[i][3];
        ret->l_pos_y = P[i][4];
        ret->pro_depth = pro_depth;
        ret->pro_type = pro_type;
        if(P[i][2] > 0) {
            ret->pro_sign = 1;
        }
        else {
            ret->pro_sign = -1;
        }

        if(F[ret->pos_x][ret->pos_y]!=0) {
            for(pos=0; pos<32; pos++) {
                if((P[pos][3] == ret->pos_x) && (P[pos][4] == ret->pos_y) && (P[pos][0]>0)) {
                    break;
                }
            }
            
            ret->k_index = pos;
        }
        else {
            ret->k_index = -1;
        }
    }
    // If don't found, just make the index -1.
    else {
        ret->refresh = -1;
    }
}

// Make pieces table using field table, returns score.
// Player -1 = white, Player + 1 = black
double mount_pieces(int F[][8], int P[][num_col], int player) {
    int i, j, c=0;
    double score, value;

    for(i=0; i<32; i++) {
        P[i][0] = -1;
        P[i][3] = -1;
        P[i][4] = -1;
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
                printf("Error: Not recognized piece\n");
                return 0;
            }
            
            // IsAlive, type and score
            P[c][0] = 1;
            P[c][1] = F[i][j];
            P[c][2] = value;
            // pos x and pos 
            P[c][3] = i;
            P[c][4] = j;
            // c and score updates
            c++;
            score += value;
        }
    }
    return score;
}

// Update score table and alpha/beta values. 
// Return -1 if alpha-beta condition wasn't matched, 1 otherwise.
int update_score(struct stack * best_score, int depth, struct queue * Q, struct moviment * best_move, double * score) {
    //int i;
    struct moviment * next;
    
    //printf("Update Score Starting[deptth:%d] \n",depth);
    //for (i=0; i<=5; i++) {
    //  printf("i:%d\n", i);
    //  printf("Alpha: %lf \n", best_score[i].alpha);
    //  printf("Beta : %lf \n", best_score[i].beta);
    //  printf("Score: %lf \n", best_score[i].score);
    //}
    //printf("\n");

    // Even
    if((depth%2) == 0) { 
        //printf("[BETA] depth : %d, best_score[depth-1]:%lf ; best_score[depth]:%lf\n", depth, best_score[depth-1], best_score[depth]);
        if(best_score[depth+1].score > best_score[depth].score) {
            best_score[depth].score = best_score[depth+1].score;
        }

        if(depth == 0)
            //printf(">>>Best Score depth: %d, best_score[depth+1].score : %lf, best_score[depth].score: %lf \n", depth, best_score[depth+1].score, best_score[depth].score);

        if(best_score[depth].score > best_score[depth].alpha) {
            best_score[depth].alpha = best_score[depth].score;  

            if(depth == 0) {            // best score in root? mark as new best_move.
                //printf(">>>Best Score updated\n");
                next = get_next(Q);
                best_move->index = next->index;
                best_move->l_pos_x = next->l_pos_x;
                best_move->l_pos_y = next->l_pos_y;
                best_move->pos_x = next->pos_x;
                best_move->pos_y = next->pos_y;
                best_move->k_index = next->k_index;
                best_move->d_counter = next->d_counter;
                best_move->pro_type = next->pro_type;
                best_move->pro_depth = next->pro_depth;
                best_move->pro_sign = next->pro_sign;
                best_move->refresh = 1;
                if(score != NULL) {
                    *score = best_score[depth].score;
                }
                //printf("Mov : %d, %d -> %d %d ; Counter: %d\n", best_move->l_pos_x, best_move->l_pos_y, best_move->pos_x, best_move->pos_y, best_move->d_counter);
            }

        }
        //printf("[BETA] depth : %d, best_score[depth].beta:%lf ; best_score[depth].alpha:%lf\n", depth, best_score[depth].beta, best_score[depth].alpha);
        if(best_score[depth].beta <= best_score[depth].alpha){   // beta <= alpha
            BETA_CUT++;
            //printf("----BETA CUT\n");

            //printf("[AFTER BETA_CUT] \n");
            //for (i=0; i<=5; i++) {
            //  printf("i:%d\n", i);
            //  printf("Alpha: %lf \n", best_score[i].alpha);
            //  printf("Beta : %lf \n", best_score[i].beta);
            //  printf("Score: %lf \n", best_score[i].score);
            //}
            //printf("\n");

            return 1;       // beta cut-off
        }
    }
    
    // Odd
    else {
        if(best_score[depth+1].score < best_score[depth].score) {
            best_score[depth].score = best_score[depth+1].score;
        }

        //printf("[ALPHA] depth: %d ; best_score[depth-1]:%lf ; best_score[depth]:%lf\n", depth, best_score[depth-1], best_score[depth]);
        if(best_score[depth].score < best_score[depth].beta) {
            //if(depth < 4)
            //printf(">>>Best Score updated, depth, %d\n", depth);
            best_score[depth].beta = best_score[depth].score;  // best_score[depth] = beta 
        }
        //printf("[ALPHA] depth : %d, best_score[depth].beta:%lf ; best_score[depth].alpha:%lf\n", depth, best_score[depth].beta, best_score[depth].alpha);
        if(best_score[depth].beta <= best_score[depth].alpha){   // beta <= alpha
            ALPHA_CUT++;
            //printf("----ALPHA CUT\n");

            //printf("[AFTER ALPHA CUT] \n");
            //for (i=0; i<=5; i++) {
            //  printf("i:%d\n", i);
            //  printf("Alpha: %lf \n", best_score[i].alpha);
            //  printf("Beta : %lf \n", best_score[i].beta);
            //  printf("Score: %lf \n", best_score[i].score);
            //}
            //printf("\n");

            return 1;       // alpha cut-off
        }
    }

    //printf("[best_score - AFTER] \n");
    //for (i=0; i<=3; i++) {
    //  printf("i:%d\n", i);
    //  printf("Alpha: %lf \n", best_score[i].alpha);
    //  printf("Beta : %lf \n", best_score[i].beta);
    //  printf("Score: %lf \n", best_score[i].score);
    //}
    //printf("\n");
    
    return -1;
}

// Print Field for debug reasons.
void print_field(int F[8][8]) {
    int i,j;
   
    printf("Board:\n");

    for(i=7; i>=0; i--) {
        for(j=0; j<8; j++) {
            printf("%*d ",3, F[j][i]);
        }
        printf("\n");
    }
}

// Print Pieces for debug reasons.
void print_player(int P[num_pieces][num_col]) {
    int i,j;
    
    for(i=0; i<num_pieces; i++) {
        for(j=0; j<num_col; j++) {
            printf("%*d ",5, P[i][j]);
        }
        printf("\n");
    }
}

// F = field[8][8] converted to this system, max_depth, player: -1 white. 1 black.
struct moviment * alpha_beta(int F[8][8], int max_depth, int player, double * score) {
    struct queue Q;
    int P[num_pieces][num_col];
    int depth = 0, i, u_ret=-1;
    struct stack * best_score;               // Bestscore = [(alpha, beta, score)^n] 
    int * mov_counter;
    struct moviment * next;
    struct moviment * mov;
    struct moviment * best_move;            // Best move available. Only used for depth=0.
    double current_score;
    int found;
    
    current_score = mount_pieces(F, P, player);     // Mount pieces table.
    init_queue(&Q, max_depth);              // Init queue
    best_score = (struct stack *) malloc (sizeof(struct stack)*(max_depth+1)); 
    mov_counter = (int *) malloc (sizeof(int)*(max_depth+1));
    best_move = (struct moviment *) malloc(sizeof(struct moviment));
    best_move->refresh = 0;

    //print_field(F);
    //print_player(P);
    
    for(i=0; i<=max_depth; i++) {            // Alpha=-inf ; Beta = inf
        if((i%2)==0) {
            best_score[i].score = -999999;
        }
        else {
            best_score[i].score = 999999;
        }
        best_score[i].alpha = -999999;        // -inf. 
        best_score[i].beta = 999999;
        mov_counter[i] = 0;
    }
    
    //int found=0;
    ALPHA_CUT = 0;
    BETA_CUT = 0;
    while(1) {
        //printf("depth : %d\n", depth);
        //printf("load u_ret: %d\n", u_ret);
        //printf("found: %d\n", found);
        if((depth >= max_depth)||(u_ret > 0)) {
            //printf("BEFORE UNDO, depth:%d, max_depth:%d, u_ret:%d\n", depth, max_depth, u_ret);

            // If it's in the max depth OR a king is dead OR couldn't find someone.
            if((depth >= max_depth)||(u_ret == 2)||(abs(best_score[depth].score) == 999999)) {
                //if(u_ret == 2)
                //  printf(".SCORE UPDATED TO %lf in depth %d\n", current_score, depth);
                best_score[depth].score = current_score;
            }
            depth--;
            if(depth < 0) {
                break;
            }
            u_ret = update_score(best_score, depth, &Q, best_move, score);
            mov = pop_mov(&Q);
            current_score += undo_move(F,P,mov, depth);
            //current_score += depth_factor;
            //printf("\nUndo Move. Score %lf\n", current_score);
            //print_field(F);
            player = player * -1;                                   // Invert player.
        }
        else {                                                      // If can expand, try!
            next = alloc_mov(&Q);                                   // Get new move.
            find_nth_move(F, P, mov_counter[depth], next, player, depth);  // Find next move.
            //printf("Finding move: mov_counter[depth:%d]:%d, player:%d\n", depth, mov_counter[depth], player);
            //print_field(F);
            mov_counter[depth]++;                                   // Update depth counter.
            if(next->refresh > 0) {                                 // If valid move was found.
                current_score += apply_move(F,P,next);
                
                //printf("Move found\n");
                //print_field(F);
                //current_score -= depth_factor;
                depth++;
                player = player * -1;                               // Invert player.
                found++;                                            // debug 

                // Update best_score[depth+1]
                best_score[depth].alpha = best_score[depth-1].alpha;  
                best_score[depth].beta = best_score[depth-1].beta;
                if(((depth)%2)==0) {
                    best_score[depth].score = -999999;
                }
                else {
                    best_score[depth].score = 999999;
                }
                mov_counter[depth] = 0;

                // Check for King's Death 
                if(abs(current_score)>500) {
                    //printf("A king have died in depth: %d.\n", depth);
                    u_ret = 2;
                }

                //printf("[PAIR UPDATE - AFTER] - Score:%lf\n", current_score);
                //for (i=0; i<=5; i++) {
                //  printf("i:%d\n", i);
                //  printf("Alpha: %lf \n", best_score[i].alpha);
                //  printf("Beta : %lf \n", best_score[i].beta);
                //  printf("Score: %lf \n", best_score[i].score);
                //}
                //printf("\n");

            }
            else {
                //printf("Couldn't found move, depth: %d, moves found: %d\n", depth, mov_counter[depth]);
                //printf("Couldn't find move\n");
                pop_mov(&Q);
                u_ret = 1;
                mov_counter[depth] = 0;
            }
        }
    }

    //printf("Moviments Visited: %d\n", found);
    //printf("Alpha Cuts: %d\n", ALPHA_CUT);
    //printf("Beta Cuts: %d\n", BETA_CUT);

    //print_player(P);
    
    free_queue(&Q);
    free(best_score);
    free(mov_counter);

    if(best_move->refresh == 0) {
        printf("ERROR: Couldn't find move.\n");
    }

    return best_move;
}

// For benchmark propouses
void checkmate_path(int F[8][8], int player, int parallel) {
    double score;
    int max_depth=1;
    struct moviment * best_move;

    do {
        max_depth+=2;
        printf("Checkmate in %d moves. \n", ((max_depth+1)/2)-1);
        // TODO: Insert paralell option here to compare.
        if(parallel==0) {
            best_move = alpha_beta(F, max_depth, player, &score);
        }
        else {
            printf("Insert parallel version here.\n");
        }
        printf(">> Score : %lf. \n", score);
        free(best_move);
    } while (score < 500);
    printf("Checkmate path found.\n");
}

void benchmark() {
    struct timeval begin, end, diff;
    int F[8][8], i, j, player;

// BENCHMARK 1: Easy to see mate.
// Source: Madruguinha

    for(i=0; i<8; i++) { for(j=0; j<8; j++) { F[i][j] = 0; } }

    player = -1;
    F[6][0] = -castle;  F[7][1] = -castle;  F[7][0] = -king;
    F[0][4] = king;     F[0][3] = pawn;     F[1][3] = pawn;    

    printf("Starting Benchmark 1.\n");
    print_field(F);
    gettimeofday(&begin, NULL);
    checkmate_path(F, player, 0);
    gettimeofday(&end, NULL);
    timeval_subtract(&diff, &end, &begin);
    printf("Time Elapsed in Benchmark 1: %ld.%06ld\n", diff.tv_sec, diff.tv_usec);

// BENCHMARK 2: "Mate in 6" 
// source: http://www.chess.com/forum/view/endgames/mate-in-6-masters-only-d

    for(i=0; i<8; i++) { for(j=0; j<8; j++) { F[i][j] = 0; } }

    player = -1;
    F[0][2] = -pawn;    F[3][2] = -pawn; 
    F[0][3] = pawn;    
    F[1][5] = pawn;     F[3][5] = -pawn;    
    F[0][6] = -pawn;    F[2][6] =  -pawn;   F[3][6] =  pawn;
    F[0][7] = -king;    F[2][7] = king;   
   
    printf("Starting Benchmark 2.\n");
    print_field(F);
    gettimeofday(&begin, NULL);
    checkmate_path(F, player, 0);
    gettimeofday(&end, NULL);
    timeval_subtract(&diff, &end, &begin);
    printf("Time Elapsed in Benchmark 2: %ld.%06ld\n", diff.tv_sec, diff.tv_usec);

// BENCHMARK 3: Mate in 7 but modified to use only 4 moves.
// Source : http://www.chess.com/forum/view/endgames/mate-in-7 - Modified by Madruguinha.

    for(i=0; i<8; i++) { for(j=0; j<8; j++) { F[i][j] = 0; } }

    player = -1;
    //  With 4 moves
    F[3][7] = knight;
    F[0][5] = pawn;     F[4][5] = bishop; F[7][5] = -castle;
    F[3][4] = -pawn;    F[4][4] = king;
    F[1][3] = -queen;   F[6][3] = pawn;
    F[4][2] = -king;
    F[1][1] = -pawn;    F[5][1] = -pawn; 
    

    printf("Starting Benchmark 3.\n");
    print_field(F);
    gettimeofday(&begin, NULL);
    checkmate_path(F, player, 0);
    gettimeofday(&end, NULL);
    timeval_subtract(&diff, &end, &begin);
    printf("Time Elapsed in Benchmark 3: %ld.%06ld\n", diff.tv_sec, diff.tv_usec);



}

int main(int argc,char *argv[]) {
    int i, j;
    //double score; 
    //char c='c';
    int player = -1;//, max_depth = 3;
    int F[8][8];
    //struct moviment * best_move;

    /* Board init */


    if(argc!=(64+1))
    {

    benchmark();
    return 0;

    /*for(i=0; i<8; i++) {
        for(j=0; j<8; j++) {
            F[i][j] = 0;
        }
    }*/

// Example 1: Initial Board 
/*
    F[0][0] = -castle;  F[1][0] = -knight;  F[2][0] = -bishop;  F[3][0] = -queen;
    F[4][0] = -king;    F[5][0] = -bishop;  F[6][0] = -knight;  F[7][0] = -castle;
    F[0][1] = -pawn;    F[1][1] = -pawn;    F[2][1] = -pawn;    F[3][1] = -pawn;
    F[4][1] = -pawn;    F[5][1] = -pawn;    F[6][1] = -pawn;    F[7][1] = -pawn;
    F[0][6] =  pawn;    F[1][6] =  pawn;    F[2][6] =  pawn;    F[3][6] =  pawn;
    F[4][6] =  pawn;    F[5][6] =  pawn;    F[6][6] =  pawn;    F[7][6] =  pawn;
    F[0][7] = castle;   F[1][7] = knight;   F[2][7] = bishop;   F[3][7] = queen;
    F[4][7] = king;     F[5][7] = bishop;   F[6][7] = knight;   F[7][7] = castle;
*/

// Example 2 : Easy Check-Mate
/*
    F[4][0] = -king;    F[7][0] = -castle;
    F[0][6] =  pawn;    F[1][6] =  pawn;    F[2][6] =  pawn;    F[3][6] =  pawn;
    F[4][6] =  pawn;    F[5][6] =  pawn;    F[6][6] =  pawn;    
    F[4][7] = king;     
*/

// Example 3 : Easy Check-Mate 2
/*
    F[7][5] = pawn;     F[7][3] = castle;   F[7][4] = king;    
    F[6][1] = -pawn;    F[7][2] = -castle;  F[5][4] = -king;     
*/

// Example 4 : Bug
/*    
    F[6][3] = -pawn;    F[6][6] = king;     F[5][5] = -king;    F[7][5] = -castle;
*/

// Example 5 : Just 2 Kings. Will one kill each other?

    //F[6][6] = king;     F[6][5] = -king;    
//    F[5][6] = -pawn;    F[4][1] = pawn;
    //F[1][2] = castle;

// BENCHMARK 1: "Mate in 6" 
// source: http://www.chess.com/forum/view/endgames/mate-in-6-masters-only-d

    F[0][2] = -pawn;    F[3][2] = -pawn; 
    F[0][3] = pawn;    
    F[1][5] = pawn;     F[3][5] = -pawn;    
    F[0][6] = -pawn;    F[2][6] =  -pawn;   F[3][6] =  pawn;
    F[0][7] = -king;    F[2][7] = king;   


// BENCHMARK 2: Easy to see mate.
// Source: Madruguinha
/*
    F[6][0] = -castle;  F[7][1] = -castle;  F[7][0] = -king;
    F[0][4] = king;     F[0][3] = pawn;     F[1][3] = pawn;    
*/

    }else{
        for(i=0; i<8; i++) {
            for(j=0; j<8; j++) {
                F[i][j] = atoi(argv[((8*i)+j)+1]);
            }
        }

    }

    // Print Field for debug reasons.
    print_field(F);
    checkmate_path(F, player, 0);
    
    /*while(c == 'c') {
        best_move = alpha_beta(F, max_depth, player, &score);
        do_move(F,best_move);
        printf("Mov : %d, %d -> %d %d ; Counter: %d ; Index: %d ; Score : %lf\n", best_move->l_pos_x, best_move->l_pos_y, best_move->pos_x, best_move->pos_y, best_move->d_counter, best_move->index, score);

        // Print Field for debug reasons.
        print_field(F);
        //break;
        printf("Changing player\n");
        player = player*-1;
        free(best_move);
        scanf(" %c", &c);
    }*/

    
    return 0;
}

