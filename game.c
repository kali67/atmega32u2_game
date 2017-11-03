  /** @file game.c
 *  @author Hayden Taylor, Shawn Richards
 *  @date 12 October 2017
 *  @brief game environment/logic
 */

#include "system.h"
#include "movement.h"
#include "boing.h"
#include "ir_uart.h"
#include "../fonts/font5x7_1.h"
#include "ball.h"
#include "led.h"
#include <stdlib.h>


#define BALL_SPEED 8   // [dots/second]
#define BALL_THROWS 12
#define LOOP_RATE 300  // [Hz]
#define NUM_ROWS 7
#define NUM_COLUMNS 5
#define TEXT_SPEED 15
#define NUM_SHOTS 12


#define BALL_SPEED_TICKS (LOOP_RATE)/(BALL_SPEED)

/*
 * Player structure
 * role: stores if the player is playing as catcher or shooter
 * balls_caught: stores the number of balls caugth by the player
 * */
typedef struct player_s
{
  char role;
  uint8_t balls_caught;
} Player;

/*
 * Sets the text graph on the LED matrix.
 * @param character - single character to set the tinygl text
 */
static void displayCharacter (char character)
{
    char buffer[2];
    buffer[0] = character;
    buffer[1] = '\0';
    tinygl_text (buffer);
}


/*
 * Clears the funkits buffer to ensure there is
 * nothing left after we have recieve what we need to
*/
static void clearBuffer(void)
{
  while (ir_uart_read_ready_p ()) {
    ir_uart_getc(); // get the next char in the buffer and do nothing with it
  }
}


/*
 * Decides what role the second player will play depending on what
 * the first player choose
*/

static char getChar(void)
{
    tinygl_clear(); //clear screen
    char character = ir_uart_getc();
    if (character == 'C') {
        return 'S';   // if a player chooses C, then the other player should become the shooter
    } else if (character == 'S') {
        return 'C';
    }
    return 'Z';
}


/*
 * Displays options to the player to select the role they would like to play, first to select a role
 * gets the role. This is sent over IR and player is the complement option of whats selected first.
 */
static char choosePlayers(void)
{
    tinygl_font_set (&font5x7_1);
    tinygl_text_speed_set (TEXT_SPEED);
    char options[2] = {'C', 'S'}; // roles as characters
    char current_character = options[0];
    int8_t i = 0;
    displayCharacter(options[0]); //displays the character to the ledmat
    char test;
    while (1) {
        pacer_wait ();
        navswitch_update ();
        tinygl_update ();
        if (ir_uart_read_ready_p ()) {
            test = getChar();
            if (test != 'Z') { //checks if we are actually getting a valid character so we dont get interference
                return test;
            }
        }
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            tinygl_clear();
            clearBuffer();
            ir_uart_putc(current_character); //send the selected option to the other funkit
            clearBuffer();
            return current_character;
        }
        if (navswitch_push_event_p(NAVSWITCH_NORTH)) {
            i++;
            if (i == 2){ //ensure wrap arounds
                i = 0;
            }
            current_character =  options[i];
            displayCharacter(current_character);
        }
        if (navswitch_push_event_p(NAVSWITCH_SOUTH)) {
            i--;
            if (i < 0) {//ensure wrap arounds
                i = 1;
            }
            current_character =  options[i];
            displayCharacter(current_character);
        }
    }
}


/*
* Starts the game and sets up the players depending on what role they choose,
* also initialises the catcher and shooter graphics
* @param catcher_pos_left - left LED of the catcher paddle
* @param catcher_pos_right - right LED of the catcher paddle
* @param shooter_pos - LED of the shooter
* returns a player object
*/
static Player startGame(tinygl_point_t* catcher_pos_left,
                tinygl_point_t* catcher_pos_right, tinygl_point_t* shooter_pos)
{
    Player player;
    char current_character = choosePlayers(); //choose who is catcher/shooter on new game
    if (current_character == 'C') {
        player.role = 'C'; //set role and number of balls caught
        player.balls_caught = 0;
        catcher_init(catcher_pos_left, catcher_pos_right);
    } else {
        player.role = 'S';
        player.balls_caught = 0;
        shooter_init(shooter_pos);

    }
    tinygl_update ();
    return player;
}

/*
 * Displays 'continue' to the screen after a round is over,
 * will also wait until both players have acknowledged this clicked
 * the nav button in
*/

static void showSwitchingScreen(void)
{
    int8_t reqs = 0;
    tinygl_clear();
    tinygl_font_set (&font5x7_1);
    tinygl_text_speed_set (TEXT_SPEED);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    tinygl_text ("CONTINUE"); // display to ledmat
    char transmition;
    while (1) {
        pacer_wait ();
        navswitch_update ();
        tinygl_update();
        if (ir_uart_read_ready_p()){ // checks if something has been sent
            transmition = ir_uart_getc();
            if (transmition == 'R') { // and whether it is a valid char
               reqs++;
            }

        }
        if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
            ir_uart_putc('R'); // send a message saying they're ready
            reqs++;
        }
        if (reqs == 2) {// must have recieved and sent something to continue
            break;
        }
    }
    tinygl_clear(); //clear the screen
}


/*
 * Displays winner or loser to the ledmat
 * @param text - takes text to output to the ledmat
 */

static void displayGameOver(char* text)
{
    tinygl_clear();
    tinygl_font_set (&font5x7_1);
    tinygl_text_speed_set (TEXT_SPEED);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    tinygl_text (text); // set the text
    while (1) {
        pacer_wait ();
        tinygl_update();
    }
}


/*
 * Checks the other players score against its own to decide the outcome
 * of the game.
 * @param player - player object
 * @param other_player_score - unsigned int which contains the other players score
 */

static void displayEndScreen(Player* player, uint8_t other_player_score)
{
    //text of the outcome of the game
    char win[] = {'W', 'I', 'N', 'N', 'E', 'R', '\0'};
    char lose[] = {'L', 'O', 'S', 'E', 'R', '\0'};
    char tie[] = {'T', 'I', 'E', '\0'};
    tinygl_clear();
    clearBuffer();
    tinygl_font_set (&font5x7_1);
    while(1) {
        pacer_wait();
        tinygl_update();
        if (player->balls_caught > other_player_score) {
            displayGameOver(win); // display winner message to ledmat if your score is greater than the other players
        } else if (player->balls_caught < other_player_score) {
            displayGameOver(lose);
        } else {
            displayGameOver(tie);
        }
    }
}

/*
 * At the end of the round the players roles are switched and draws graphics for each role
 *@param player - pointer to player object
 *@param catcher_pos_left -  pointer to left LED of the catcher paddle
 *@param catcher_pos_right -  pointer to right LED of the catcher paddle
 *@param shooter_pos -  pointer to LED of the shooter
*/
static void endTurn(Player* player, tinygl_point_t* catcher_pos_left,
                tinygl_point_t* catcher_pos_right, tinygl_point_t* shooter_pos)
{
    showSwitchingScreen();
    if (player->role == 'C') {
        player->role = 'S'; // switch roles
        shooter_init(shooter_pos); // init the shooter graphics
    } else {
        player->role = 'C';
        catcher_init(catcher_pos_left, catcher_pos_right);  // init the catcher graphics
    }
}


/*
 * Decides where the ball will be recived on the next funkit
 * i.e. what column will the ball be
 * @param ball_ptr - pointer to the recieved ball
*/

static void recieveBall(boing_state_t* ball_ptr)
{
    (*ball_ptr).pos.x = 0;
    (*ball_ptr).pos.y = NUM_ROWS - 1 - ir_uart_getc(); // set the row the ball is in using the row it left from
    ball_ptr->dir = DIR_E; // change the direction of the ball
    tinygl_draw_point (ball_ptr->pos, 1); // draw it
}

/*
* Sends the catchers score to the other player after the round is over
* @param player - pointer to a player object
* @param turns - the number of turns the player has had
* @param num_balls_received - the number of balls the catcher has recieved on their side
*/
static void sendScore(Player* player, uint8_t* turns, uint8_t* num_balls_received)
{
    *turns = *turns + 1; // keep track of game progress
    ir_uart_putc(player->balls_caught); // transmit ready for end turn screen
    *num_balls_received = 0;
}



/*
 * Initialise everything the game needs, pacer, nav, tinygl, ir
*/
static void initUtils(void)
{
    system_init();
    pacer_init (LOOP_RATE); //set the loop rates
    tinygl_init (LOOP_RATE);
    navswitch_init();
    ir_uart_init();
}


/*
* Logic for a player who is a catcher and the events that would occur, deals with positioning and catching of balls
* @param player- pointer to the player
* @param catcher_pos_left -  pointer to left LED of the catcher paddle
 *@param catcher_pos_right -  pointer to right LED of the catcher paddle
* @param ticks - pointer to the number of ticks to control the ball speed
* @param seed_tick - pointer to seed for the random number
* @param ball - pointer to the ball object
* @param num_balls_fired - pointer to the number of balls fired, to know when to stop the shooter from shooting anymore balls
* @param ball_fired - pointer to a bool to decide if the ball has been fired or not
*/
static void catcherPlayer(Player* player, tinygl_point_t* catcher_pos_left, tinygl_point_t* catcher_pos_right,
             bool* ball_off_screen, uint8_t* ticks, uint8_t* seed_tick, boing_state_t* ball, bool* ball_received, uint8_t* num_balls_received)
{

    tinygl_draw_point(*catcher_pos_left,1); // draw the catcher graphics
    if (navswitch_push_event_p(NAVSWITCH_SOUTH)) {
        updatePositionCatcher(catcher_pos_left, catcher_pos_right, 'S'); // move the catcher in the right direction
    }
    if (navswitch_push_event_p(NAVSWITCH_NORTH)) {
        updatePositionCatcher(catcher_pos_left, catcher_pos_right, 'N');
    }
        if (ir_uart_read_ready_p ()) {
            srand(*seed_tick); // change seed for the random path
            recieveBall(ball);
            *ball_received = 1;
        }
        if (*ball_received) {
            *ticks = *ticks + 1;
            if (*ticks >= BALL_SPEED_TICKS) { // keeps the ball speed consistent
                if (*ball_off_screen){
                    tinygl_draw_point (ball->pos, 0);
                    updatePositionCatcher(catcher_pos_left, catcher_pos_right, 'X'); // reset the catcher position
                    (*ball).pos.x = 2;
                    *ball_off_screen = 0;
                    *ball_received = 0;
                } else {
                    tinygl_draw_point (ball->pos, 0);
                    updateFiredBallCatcher(ball); //update the balls path
                    tinygl_draw_point(ball->pos, 1);
                    updatePositionCatcher(catcher_pos_left, catcher_pos_right, 'X');
                    if ((*ball).pos.x == (NUM_COLUMNS - 1)) { // if the ball is in the last column (could not be a collision)
                        *num_balls_received = *num_balls_received + 1;
                        if ((catcher_pos_left->y == (*ball).pos.y || catcher_pos_right->y == (*ball).pos.y) && catcher_pos_left->x == (*ball).pos.x) { // collision between paddle and ball
                            player->balls_caught += 1;
                            }
                        *ball_off_screen = 1;
                    }
                }
            *ticks = 0;
        }
    }
}



/*
 * Logic for a player who is a shooter and the events that would occur, deals with positioning and
 * firing of the ball
 * @param shooter_pos - pointer to the shooter LED, so it can be updated
 * @param ticks - pointer to the number of ticks to control the ball speed
 * @param seed_tick - pointer to seed for the random number
 * @param ball - pointer to the ball object
 * @param num_balls_fired - pointer to the number of balls fired, to know when to stop the shooter from shooting anymore balls
 * @param ball_fired - pointer to a bool to decide if the ball has been fired or not
*/
static void shooterPlayer(tinygl_point_t* shooter_pos, uint8_t* ticks, uint8_t* seed_tick, boing_state_t* ball, uint8_t* num_balls_fired, bool* ball_fired)
{

    setBallPositionOnShooter(ball, shooter_pos->y, *ball_fired); // sets the ball position when the ball isnt fired
    if (navswitch_push_event_p(NAVSWITCH_SOUTH)) { // controls the movement of the shooter
        updatePositionShooter(shooter_pos, 'S');
        setBallPositionOnShooter(ball, shooter_pos->y, *ball_fired);
    }
    if (navswitch_push_event_p(NAVSWITCH_NORTH)) {
        srand(*seed_tick); // randomise the ball trajectory by setting the seed as often as possible
        updatePositionShooter(shooter_pos, 'N');
        setBallPositionOnShooter(ball, shooter_pos->y, *ball_fired);
    }
    if (navswitch_push_event_p(NAVSWITCH_PUSH) && !*ball_fired) { // fire the ball and ensure that players can't spam balls
        *ball_fired = 1;
        *num_balls_fired = *num_balls_fired + 1;
        updateFiredBallShooter(ball); //  update the balls path
    }
    if (*ball_fired) {
        *ticks = *ticks + 1;
        if (*ticks == BALL_SPEED_TICKS) { // controls the ball speed
            *ticks = 0;
            if ((*ball).pos.x != 0){
                updateFiredBallShooter(ball);
            }
            else if ((*ball).pos.x == 0) { // ball is at end of the ledmat, ready to be sent to the next fun kit
                tinygl_draw_point (ball->pos, 0); // hide the ball
                (*ball).pos.x = NUM_COLUMNS - 2;
                (*ball).pos.y = shooter_pos->y;
                *ball_fired = 0;
                clearBuffer();
                ir_uart_putc((*ball).pos.y); // send the row number of the ball when it hits the last column
            }
        }
    }
}



/*
 *
 * Main routine of the game, contains the game logic.
 *
 */
int main (void)
{
    initUtils(); // init everything the game needs
    boing_state_t ball = boing_init (NUM_COLUMNS-2, 0, DIR_W); // create a ball
    uint8_t ticks = 0, seed_tick = 0, num_balls_fired = 0, num_balls_received = 0, turns = 0, other_player_score = 0;
    tinygl_point_t catcher_pos_left, catcher_pos_right, shooter_pos;
    Player player = startGame(&catcher_pos_left, &catcher_pos_right, &shooter_pos); // create a player
    bool ball_received = 0, ball_off_screen = 0, ball_fired = 0;
    while (1) {
        pacer_wait ();
        navswitch_update ();
        tinygl_update ();
        seed_tick++;
        if (turns == 2) { // if the player has had 2 turns (been in 2 rounds) then the game is over, so display the scores
            displayEndScreen(&player, other_player_score);
        } else {
            if (player.role == 'C'){ // if the player is a catcher
                catcherPlayer(&player, &catcher_pos_left, &catcher_pos_right, &ball_off_screen, &ticks, &seed_tick, &ball, &ball_received, &num_balls_received); // use the catcher logic
             }
        }
        if (num_balls_received == BALL_THROWS) { // if the catcher has recieved all the balls
            sendScore(&player, &turns, &num_balls_received); // send its score to the shooter to keep track of
			ball_fired = 0;
            endTurn(&player, &catcher_pos_left, &catcher_pos_right, &shooter_pos); // swap players
            resetBallNextPlayer(&ball); // reset the ball position for the next player
        }
        else if (player.role == 'S') { // if the player is a shooter, use the shooters logic
            shooterPlayer(&shooter_pos, &ticks, &seed_tick, &ball, &num_balls_fired, &ball_fired);
            int8_t test_ir;
            if (ir_uart_read_ready_p()) {
                test_ir = ir_uart_getc();
                if (test_ir >=0 && test_ir <= NUM_SHOTS) { // ensure that the ir recieved is actually a score from the catcher, not interferance
                    turns++;
					other_player_score = test_ir;
                    endTurn(&player, &catcher_pos_left, &catcher_pos_right, &shooter_pos); // if it is, then that means the game is over so end the turn
                    num_balls_fired = 0;
                    clearBuffer();  // clear out anthing that might be in the buffer
                }
            }
        }
    }
}
