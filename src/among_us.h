
#ifndef __AMONG_US_H_
#define __AMONG_US_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


/**
 * Structure defining a node of the airlines double linked list
 * tree
 */
struct Players
{
    int pid;                      /*Player's identifier*/
    int is_alien;                 /*Alien flag*/
    int evidence;                 /*Amount of evidence*/
    struct Players *prev;         /*Pointer to the previous node*/
    struct Players *next;         /*Pointer to the next node*/
    struct Tasks *tasks_head;     /*Pointer to the head of player's task list*/
    struct Tasks *tasks_sentinel; /*Pointer to the sentinel of player's task list*/
};

/**
 * Structure defining a node of the airplane linked list
 */
struct Tasks
{
    int tid;                      /*Task's identifier*/
    int difficulty;               /*Task's difficulty*/
    struct Tasks *next;           /*Pointer to the next node*/  
};

struct Head_GL
{
    int tasks_count[3];           /*Count of tasks*/
    struct Tasks *head;           /*Pointer to the head of general list*/
};

struct Head_Completed_Task_Stack
{
    int count;                    /*Count of completed tasks*/
    struct Tasks *head;           /*Pointer to the top element of the stack*/
};

            /*############################
                STACK FUNCTIONS START
            #############################*/
/**
 * @brief Checks if the Stack is empty
 *
 * @return 1 if empty
 *         0 if not
 */
int is_empty_stack(void);

/**
* @brief Push a completed task in the stack
* 
* @param pointer A pointer to the completed task node
*
*/
void push(struct Tasks *pointer);

/**
* @brief Pops a completed task from the stack
* 
* @return A pointer to the popped task node
*         NULL in failure
*
*/
struct Tasks* pop(void);
            /*########################
            	STACK FUNCTIONS END
            ##########################*/


			/*##########################
				GLOBAL VARIABLES START
			############################*/
/* Global variable, pointer to the head of the players list */
struct Players *players_head;

/* Global variable, pointer to the head of the tasks list */
struct Head_GL *tasks_head;

/* Global variable, pointer to the top of the completed task's stack */
struct Head_Completed_Task_Stack *tasks_stack;

/* Global variable, general task counter */
int Total_Tasks;
			/*#########################
				GLOBAL VARIABLES END
			###########################*/
/**
 * @brief Optional function to initialize data structures that 
 *        need initialization
 *
 * @return 1 on success
 *         0 on failure
 */
int initialize(void);

/**
 * @brief Register player
 *
 * @param pid The player's id
 *
 * @param is_alien The variable that decides if he is an alien or not
 * @return 1 on success
 *         0 on failure
 */
int register_player(int pid, int is_alien);

/**
 * @brief Insert task in the general task list
 *
 * @param tid The task id
 * 
 * @param difficulty The difficulty of the task
 *
 * @return 1 on success
 *         0 on failure
 */
int insert_task(int tid, int difficulty);

/**
 * @brief Distribute tasks to the players
 * @return 1 on success
 *         0 on failure
 */
int distribute_tasks(void);

/**
*
* @brief Allocates a new memory block for a task node
*
* @return A pointer points at the new task node 
*         NULL in failure
*/
struct Tasks* new(void);                                                   

/**
 * @brief Implement Task
 *
 * @param pid The player's id
 *
 * @param difficulty The task's difficuly
 *
 * @return 1 on success
 *         0 on failure
 */
int implement_task(int pid, int difficulty);

/**
 * @brief search for a player with pid in players list
 *
 * @param pid the players id
 *
 * @return pointer to players node if found
 *         NULL if not
 */
struct Players* find_player_with_pid(int pid);                            

/**
 * @brief Eject Player
 * 
 * @param pid The ejected player's id
 *
 * @return 1 on success
 *         0 on failure
 */
int eject_player(int pid);

/**
*
* @brief Finds the player with the less tasks in the players list
*
* @param Number a pointer to int in order to return the number of the tasks (neccesery at the give work event)
*
* @return A pointer pointed at the player with the less tasks
*         NULL if all players are aliens
*/
struct Players* find_min_tasks_player(int *number);                       
/**
*
* @brief Finds the player with the maximum number of tasks in the players list
*
* @param Number a pointer to int in order to return the number of the tasks (neccesery at the give work event)
* @return A pointer pointed at the player with the maximum number of tasks
*         NULL if all players are aliens
*/
struct Players* find_max_tasks_player(int *number);                        

/**
*
* @brief Takes 2 lists and merges the first list to the second one sorted
*
* @param tl1 is a pointer pointed at the first node of the first list
* @param sentinel1 is a pointer pointed at the sentinel node of the first list
* @param tl2 is a pointer pointed at the first node of the second list
* @param sentinel2 is a pointer pointed at the sentinel node of the second list
*
* @return struct Tasks* returns the new tl2
*
*/
struct Tasks* merge(struct Tasks *tl1, struct Tasks *sentinel1      
    , struct Tasks *tl2, struct Tasks *sentinel2);

/**
 * @brief Witness Eject Player
 *
 * @param pid_a The alien's pid
 * 
 * @param pid The crewmate's pid
 * 
 * @param number_of_witnesses The number of witnesses
 *
 * @return 1 on success
 *         0 on failure
 */
int witness_eject(int pid, int pid_a, int number_of_witnesses);

/**
*
* @brief prints the double list as: <Player1, evidence1> = <tid1,1,difficulty1,1>,<tid1,2,difficulty1,2> 
*                                   <Player2, evidence2> = <tid2,1,difficulty2,1>,<tid2,2,difficulty2,2>
* @return  1 on success
*          0 on failure
*/
int print_evidence_and_tasks(void);

/**
 * @brief Sabbotage
 *
 * @param pid The player's id
 *
 * @param number_of_tasks The number of tasks to be popped
 * 
 * @return 1 on success
 *         0 on failure
 */
int sabbotage(int number_of_tasks, int pid);

/**
 * @brief Vote
 *
 * @param pid The player's id
 * 
 * @param vote_evidence The vote's evidence
 *
 * @return 1 on success
 *         0 on failure
 */
int vote(int pid, int vote_evidence);

/**
*
* @brief When we call vote method, vote method calls the eject method in order to avoid writing same code.
*           In vote event both crewmates and aliens can be ejected but in eject event only crewmetes can be ejectes.
*           To solve the problem i use the kick method which does not show prejudice between alliens and crewmates.
*           So when an eject event exists, eject function calls the kick method if the player to be kicked is not an alien.
*           When a vote event exists, vote function again calls the kick method without taking care about the players nature.
*
* @param pid. An int, the players id
*
* @return int  1 on success
*              0 on failure
*
*/
int kick(int pid);

/**
 * @brief Give Away Work
 *
 * @return 1 on success
 *         0 on failure
 */
int give_work(void);

/**
 * @brief Terminate
 *
 * @return 1 on success
 *         0 on failure
 */
int terminate(void);

/**
 * @brief Print Players
 *
 * @return 1 on success
 *         0 on failure
 */
int print_players(void);

/**
 * @brief Print Tasks
 *
 * @return 1 on success
 *         0 on failure
 */
int print_tasks(void);

/**
 * @brief Print Stack
 *
 * @return 1 on success
 *         0 on failure
 */
int print_stack(void);

/**
 * @brief Print Players & Task List
 *
 * @return 1 on success
 *         0 on failure
 */
int print_double_list(void);

/**
 * @brief Free resources
 *
 * @return 1 on success
 *         0 on failure
 */

int free_all(void);

#endif /* __AMONG_US_H_ */
