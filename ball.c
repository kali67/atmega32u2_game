/** @file game.c
 *  @author Hayden Taylor, Shawn Richards
 *  @date 11 October 2017
 *  @brief ball module, contains all functions that control the movement and state of the ball
 */


#include "system.h"
#include "tinygl.h"
#include "boing.h"
#include <stdlib.h>
#include "ball.h"

#define NUM_ROWS 7
#define NUM_COLUMNS 5
#define JUMP_CHANCE 15 // [%] probability of ball jumping to different column


 /*
 * Updates the position of the loaded ball to align it with the shooter
 * @param ball_ptr - pointer to the ball, which has a special structure from the boing module
 * @param value - y value of the shooter
 * @param ball_fired - bool, deciding if the ball has been fired or not
 */
void setBallPositionOnShooter(boing_state_t* ball_ptr, tinygl_coord_t value, bool ball_fired) {
    if (!ball_fired) {
        tinygl_draw_point ((*ball_ptr).pos, 0);
        (*ball_ptr).pos.y = value;
        tinygl_draw_point ((*ball_ptr).pos, 1); 
    }
}




/*
 * Resets the ball when the palyer roles are switched
 * @param ball_ptr - pointer to the ball, which has a special structure from the boing module
 */ 
void resetBallNextPlayer(boing_state_t* ball_ptr) {
    ball_ptr->dir = DIR_W;
    (*ball_ptr).pos.x = (NUM_COLUMNS - 2);
    (*ball_ptr).pos.y = 0;
}




/*
 * Updates the position of the fired ball as it moves across the schooters screen
 * @param ball_ptr - pointer to the ball, which has a special structure from the boing module
 */
void updateFiredBallShooter(boing_state_t* ball_ptr) {
    
    //declare and initialise variables
    uint8_t  rand_num;
    
    //remove previous ball position
    tinygl_draw_point (ball_ptr->pos, 0);
    
    // move the ball if it has not reached the end of the screen
    if ((*ball_ptr).pos.x != 0){
                    
        *ball_ptr = boing_update(*ball_ptr);
        tinygl_draw_point ((*ball_ptr).pos, 1);
        (*ball_ptr).dir = DIR_W;
                    
        // generate random number in [1, 100]
        rand_num = rand() % 100 + 1;
        // 15% chance of changing direction
        if (rand_num < JUMP_CHANCE){
            (*ball_ptr).dir = DIR_SW;
        } else if (rand_num > (100-JUMP_CHANCE)){
           (*ball_ptr).dir = DIR_NW;
        }
    } else {
        (*ball_ptr).pos.y = 1;
    }
}




/*
 * Updates the position of the fired ball as it moves across the catchers screen
 * @param ball_ptr - pointer to the ball, which has a special structure from the boing module
 */
void updateFiredBallCatcher(boing_state_t* ball_ptr){
    
    //declare and initialise variables
    uint8_t  rand_num;
    
    //remove previous ball position
    tinygl_draw_point (ball_ptr->pos, 0);
    
    // move the ball if it has not reached the end of the screen
    if ((*ball_ptr).pos.x != NUM_COLUMNS-1){
        
        *ball_ptr = boing_update(*ball_ptr);
        tinygl_draw_point ((*ball_ptr).pos, 1);
        (*ball_ptr).dir = DIR_E;
        
                    
        // generate random number in [1, 100]
        rand_num = rand() % 100 + 1;
        // 15% chance of changing direction
        if (rand_num < JUMP_CHANCE){
            (*ball_ptr).dir = DIR_SE;
        } else if (rand_num > (100-JUMP_CHANCE)){
           (*ball_ptr).dir = DIR_NE;
        }
    }
    
}
