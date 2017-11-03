/** @file movement.h
 *  @author Hayden Taylor, Shawn Richards
 *  @date 12 October 2017
 *  @brief movement of a catcher or shooter module support 
 */

#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "pacer.h"
#include "tinygl.h"
#include "../fonts/font5x7_1.h"
#include "navswitch.h"

/*
 * Initialise the shooters LED on the LEDMAT.
 * @param shooter_pos - pointer to the shooter coordinate
 */
void shooter_init(tinygl_point_t* shooter_pos);

/*
 * Updates the shooters current posistion to either to the left or right,
 * also accounts for the edge cases.
 * @param shooter_pos - pointer to the shooter coordinate
 */
void updatePositionShooter(tinygl_point_t* shooter_pos, char direction);


/*
 * Turns of the LED for the shooter (just before it gets updated).
 * @param shooter_pos - pointer to a coordinate on the LEDMAT for the shooter
 */
void turnOffPositionShooter(tinygl_point_t* shooter_pos);

/*
 * Turn off the LED's on the catcher (just before it gets updated).
 * @param catcher_pos_left - pointer to a coordinate on the LEDMAT (LEFT SIDE)
 * @param catcher_pos_right -  pointer to a coordinate on the LEDMAT (RIGHT SIDE)
 */
void turnOffPositionCatcher(tinygl_point_t* catcher_pos_left, tinygl_point_t* catcher_pos_right);

/*
 * Initialise the catcher LED's to show them on the board at
 * the default location
 * @param catcher_pos_left - pointer to a coordinate on the LEDMAT (LEFT SIDE)
 * @param catcher_pos_right -  pointer to a coordinate on the LEDMAT (RIGHT SIDE)
 */
void catcher_init(tinygl_point_t* catcher_pos_left, tinygl_point_t* catcher_pos_right);


/*
 * Updates the posistion of the catcher player to the left or the right, 
 * also accounts for edge cases.
 * @param catcher_pos_left - pointer to a coordinate on the LEDMAT (LEFT SIDE)
 * @param catcher_pos_right -  pointer to a coordinate on the LEDMAT (RIGHT SIDE)
 */
void updatePositionCatcher(tinygl_point_t* catcher_pos_left, tinygl_point_t* catcher_pos_right, char direction);

#endif
