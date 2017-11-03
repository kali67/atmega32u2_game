/** @file ball.h
 *  @author Hayden Taylor, Shawn Richards
 *  @date 12 October 2017
 *  @brief ball module
 */


#ifndef BALL_H
#define BALL_H

#include "system.h"
#include "tinygl.h"
#include "boing.h"
#include <stdlib.h>
#include "ball.h"

 /*
 * Updates the position of the fired ball as it moves across the schooters screen
 * @param ball_ptr - pointer to the ball, which has a special structure from the boing module
 */
void updateFiredBallShooter(boing_state_t* ball_ptr);

/*
 * Updates the position of the loaded ball to align it with the shooter
 * @param ball_ptr - pointer to the ball, which has a special structure from the boing module
 * @param value - y value of the shooter
 * @param ball_fired - bool, deciding if the ball has been fired or not
*/
void setBallPositionOnShooter(boing_state_t* ball_ptr, tinygl_coord_t value, bool ball_fired);

/*
 * Resets the ball when the palyer roles are switched
 * @param ball_ptr - pointer to the ball, which has a special structure from the boing module
*/
void resetBallNextPlayer(boing_state_t* ball_ptr);

/*
 * Updates the position of the fired ball as it moves across the catchers screen
 * @param ball_ptr - pointer to the ball, which has a special structure from the boing module
*/
void updateFiredBallCatcher(boing_state_t* ball_ptr);

#endif
