/** @file movement.c
 *  @author Hayden Taylor, Shawn Richards
 *  @date 12 October 2017
 *  @brief movement of a catcher or shooter player - module 
 */

#include "pacer.h"
#include "tinygl.h"
#include "../fonts/font5x7_1.h"
#include "navswitch.h"
#include "movement.h"

#define NUM_ROWS 7
#define NUM_COLUMNS 5

#define Y_MIDDLE (NUM_ROWS/2)


/*
 * Initialise the catcher LED's to show them on the board at
 * the default location.
 * @param catcher_pos_left - pointer to a coordinate on the LEDMAT (LEFT SIDE)
 * @param catcher_pos_right -  pointer to a coordinate on the LEDMAT (RIGHT SIDE)
 */
void catcher_init(tinygl_point_t* catcher_pos_left, tinygl_point_t* catcher_pos_right) { 
    tinygl_clear(); 
    catcher_pos_right->x = NUM_COLUMNS-1;
    catcher_pos_right->y = Y_MIDDLE;
    catcher_pos_left->x = NUM_COLUMNS-1;
    catcher_pos_left->y = Y_MIDDLE+1;
    tinygl_draw_point(*catcher_pos_left,1);
    tinygl_draw_point(*catcher_pos_right,1);
}


/*
 * Updates the posistion of the catcher player to the left or the right, 
 * also accounts for edge cases.
 * @param catcher_pos_left - pointer to a coordinate on the LEDMAT (LEFT SIDE)
 * @param catcher_pos_right -  pointer to a coordinate on the LEDMAT (RIGHT SIDE)
 */
void updatePositionCatcher(tinygl_point_t* catcher_pos_left, tinygl_point_t* catcher_pos_right, char direction) {
    turnOffPositionCatcher(catcher_pos_left, catcher_pos_right); // turn off LEDs
    
    if (direction == 'N') { // check direction
        catcher_pos_right->y -= 1;
        catcher_pos_left->y -= 1;
        
    } else  if (direction == 'S'){
        catcher_pos_right->y += 1;
        catcher_pos_left->y += 1;
    }
    if (catcher_pos_left->y == NUM_ROWS) {
        catcher_pos_left->y = 1;
        catcher_pos_right->y = 0;
    } else if (catcher_pos_right->y == -1) {
        catcher_pos_right->y = NUM_ROWS -2;
        catcher_pos_left->y = NUM_ROWS -1;
    }
    tinygl_draw_point(*catcher_pos_left,1); // draw both after they have moved
    tinygl_draw_point(*catcher_pos_right,1);
}


/*
 * Turn off the LED's on the catcher (just before it gets updated).
 * @param catcher_pos_left - pointer to a coordinate on the LEDMAT (LEFT SIDE)
 * @param catcher_pos_right -  pointer to a coordinate on the LEDMAT (RIGHT SIDE)
 */
void turnOffPositionCatcher(tinygl_point_t* catcher_pos_left, tinygl_point_t* catcher_pos_right) {
    tinygl_draw_point(*catcher_pos_left,0);
    tinygl_draw_point(*catcher_pos_right,0);
}


/*
 * Turns of the LED for the shooter (just before it gets updated).
 * @param shooter_pos - pointer to a coordinate on the LEDMAT for the shooter
 */
void turnOffPositionShooter(tinygl_point_t* shooter_pos) {
    tinygl_draw_point(*shooter_pos,0); 
}


/*
 * Initialise the shooters LED on the LEDMAT.
 * @param shooter_pos - pointer to the shooter coordinate
 */
void shooter_init(tinygl_point_t* shooter_pos) { 
    tinygl_clear();
    shooter_pos->x = NUM_COLUMNS-1; //set the column position
    shooter_pos->y = Y_MIDDLE; // set to middle of screen
    tinygl_draw_point(*shooter_pos,1); 
}


/*
 * Updates the shooters current position to either to the left or right,
 * also accounts for the edge cases.
 * @param shooter_pos - pointer to the shooter coordinate
 */
void updatePositionShooter(tinygl_point_t* shooter_pos, char direction) {
    turnOffPositionShooter(shooter_pos);
    if (direction == 'N') { // see which direction the ball is in and move it accodingly
        shooter_pos->y -= 1;   
    } else if (direction == 'S') {
        shooter_pos->y += 1;
    }
    if (shooter_pos->y == 7) {
        shooter_pos->y = 0;   
    } else if (shooter_pos->y == -1) {
        shooter_pos->y = 6;
    }
    tinygl_draw_point(*shooter_pos,1); // draw the new position
}
