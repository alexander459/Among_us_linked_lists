/***************************************************
 *                                                 *
 * file: among_us.c                                *
 *                                                 *
 * @Author  Alexandros Tevrentzidis                *
 * @Version 20-10-2020                             *
 * @email   csd4383@csd.uoc.gr                     *
 *                                                 *
 * @brief   Implementation of among_us.h           *
 *                                                 *
 ***************************************************
 */

#include "among_us.h"

/**
 * @brief Optional function to initialize data structures that 
 *        need initialization
 *
 * @return 1 on success
 *         0 on failure
 */
int initialize() {
    /*#### GLOBAL VARS INIT ####*/
    Total_Tasks=0;

    /*#### PLAYERS INIT ####*/
    players_head=(struct Players*)malloc(sizeof(struct Players));
    if(players_head==NULL)
        return 0;
    players_head->pid=-1;
    players_head->is_alien=-1;
    players_head->evidence=-1;
    players_head->prev=players_head;
    players_head->next=players_head;
    players_head->tasks_head=NULL;
    players_head->tasks_sentinel=NULL;

    /*#### TASKS INIT ####*/
    tasks_head=(struct Head_GL*)malloc(sizeof(struct Head_GL));
    if(tasks_head==NULL)
        return 0;
    tasks_head->head=NULL;
    tasks_head->tasks_count[0]=0;
    tasks_head->tasks_count[1]=0;
    tasks_head->tasks_count[2]=0;

    /*#### STACK INIT ####*/
    tasks_stack=(struct Head_Completed_Task_Stack*)malloc(sizeof(struct Head_Completed_Task_Stack));
    if(tasks_stack==NULL)
        return 0;
    tasks_stack->count=0;
    tasks_stack->head=NULL;
    return 1;
}


/**
 * @brief Register player
 *
 * @param pid The player's id
 *
 * @param is_alien The variable that decides if he is an alien or not
 * @return 1 on success
 *         0 on failure
 */
int register_player(int pid,int is_alien) {
    struct Players *new_player=(struct Players*)malloc(sizeof(struct Players));

    if(new_player==NULL){
        return 0;
    }
    if(pid<0){
        return 0;
    }
    if(is_alien!=1 && is_alien!=0){
        return 0;
    }
    /*NEW PLAYERS NODE INITIALIZE*/
    new_player->pid=pid;
    new_player->is_alien=is_alien;
    new_player->evidence=0;
            /*PLAYERS TASKS NODE INITIALIZE*/
    new_player->tasks_sentinel=(struct Tasks*)malloc(sizeof(struct Tasks));
    new_player->tasks_sentinel->next=NULL;
    new_player->tasks_sentinel->tid=-1;
    new_player->tasks_sentinel->difficulty=0;
    new_player->tasks_head=new_player->tasks_sentinel;

    /*LINK PART*/
    new_player->prev=players_head;
    new_player->next=players_head->next;
    players_head->next->prev=new_player;
    players_head->next=new_player;

    print_players();
    return 1;
}


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
int insert_task(int tid,int difficulty){
    struct Tasks *new_task=(struct Tasks*)malloc(sizeof(struct Tasks));
    struct Tasks *temp, *prev;

    if(new_task==NULL)
        return 0;
    if(tid<0){
        return 0;
    }
    if(difficulty>3 || difficulty<1){
        return 0;
    }
    tasks_head->tasks_count[difficulty-1]++;
    new_task->tid=tid;
    new_task->difficulty=difficulty;
    temp=tasks_head->head;
    while(temp!=NULL){
        if(temp->difficulty>difficulty)
            break;
        prev=temp;
        temp=temp->next;
    }
    if(temp==tasks_head->head){  //insert at the beggining
        new_task->next=tasks_head->head;
        tasks_head->head=new_task;                                  
    }else{
        new_task->next=temp;
        prev->next=new_task;
    }
    Total_Tasks++;
    print_tasks();
    return 1;
}


/**
 * @brief Distribute tasks to the players
 * @return 1 on success
 *         0 on failure
 */
int distribute_tasks(void) {
    struct Players *players_temp, *temp;
    struct Tasks *tasks_temp, *new_node;
    
    if(Total_Tasks==0)      //no tasks
        return 0;
    if(players_head->next==players_head)        //no players
        return 0;
    
    players_temp=players_head->next;
    tasks_temp=tasks_head->head;
    while(tasks_temp!=NULL){
        if(players_temp->is_alien==1 || players_temp==players_head){
            temp=players_temp->next;
            while(temp!=players_temp){
                if(temp->is_alien==0)
                    break;
                temp=temp->next;
            }
            if(temp==players_temp)
                return 0;    //ALL PLAYERS ARE ALIENS
            players_temp=temp;
        }
        new_node=new();
        if(!new_node)
            return 0;
        new_node->tid=tasks_temp->tid;
        new_node->difficulty=tasks_temp->difficulty;
        new_node->next=players_temp->tasks_sentinel;
        if(players_temp->tasks_head==players_temp->tasks_sentinel)  //insert at the begin
            players_temp->tasks_head=new_node;
        else
            players_temp->tasks_sentinel->next->next=new_node;
        players_temp->tasks_sentinel->next=new_node;
        players_temp=players_temp->next;
        tasks_temp=tasks_temp->next;
    } // END FOR LOOP
    if(!print_double_list())
        return 0;
    return 1;
}

/**
*
* @brief Allocates a new memory block for a task node
*
* @return A pointer points at the new task node 
*         NULL in failure
*/
struct Tasks* new(void){            //MY FUNTION
   struct Tasks* new_node=(struct Tasks*)malloc(sizeof(struct Tasks));
    if(!new_node)
        return NULL;
    return new_node;
}

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
int implement_task(int pid, int difficulty){
    struct Players *player;
    struct Tasks *temp, *prev=NULL;
    int found=0;

    if(pid<0)
        return 0;
    if(difficulty>3 || difficulty<1)
        return 0;
    //SEARCH THE PLAYER WITH PID
    
    player=find_player_with_pid(pid);
    if(!player)
        return 0;   //THERE IS NO PLAYER WITH PID OR THERE ARE NOT PLAYERS IN THE LIST
    if(player->tasks_head==player->tasks_sentinel)
        return 0;    //THIS PLAYER HAS NO TASKS. IS AN ALLIEN OR HAS COMPLETED ALL THE TASKS
    //FIND THE TASK
    
    temp=player->tasks_head;
    //FROM THE PREVIOUS CHECK THOIS PLAYER HAS AT LEAST ONE TASK
    do{
        if(temp->difficulty==difficulty)
            break;
        if(temp->next!=player->tasks_sentinel){
            prev=temp;
            temp=temp->next;
        }
    }while(temp->next!=player->tasks_sentinel);
    if(prev!=NULL)
        prev->next=temp->next;
    else
        player->tasks_head=temp->next;
    if(temp==player->tasks_sentinel->next){
        if(temp==player->tasks_head)
            player->tasks_sentinel->next=NULL;
        else
            player->tasks_sentinel->next=prev;
    }
    temp->next=NULL;
    push(temp);
    //printf("TASK %d COMPLETED!\n", temp->tid);
    print_double_list();
    //print_stack();
    return 1;
}

/**
 * @brief search for a player with pid in players list
 *
 * @param pid the players id
 *
 * @return pointer to players node if found
 *         NULL if not
 */
struct Players* find_player_with_pid(int pid){              //MY FUNCTION
    struct Players *temp;
    temp=players_head->next;
    if(temp==players_head)
        return NULL;           //NO PLAYERS IN THE LIST
    while(temp!=players_head){
        if(temp->pid==pid)
            return temp;
        temp=temp->next;
    }
    return NULL;             //NO PLAYER WITH THIS PID
}

/**
 * @brief Eject Player
 * 
 * @param pid The ejected player's id
 *
 * @return 1 on success
 *         0 on failure
 */
int eject_player(int pid){
    struct Players *player=find_player_with_pid(pid);
    if(!player)
        return 0;       //THERE IS NO PLAYER WITH PID OR THERE ARE NOT PLAYERS IN THE LIST
    if(player->is_alien==1)
        return 0;       //THIS PLAYER IS AN ALIEN
    if(!kick(pid))
        return 0;
    print_double_list();
    return 1;
}

/**
*
* @brief When we call vote method, vote method calls the eject method in order to avoid writing same code.
*           In vote event both crewmates and aliens can be ejected but in eject event only crewmetes can be ejectes.
*           To solve the problem i use the kick method which does not discriminate alliens from crewmates.
*           So when an eject event exists, eject function calls the kick method if the player to be kicked is not an alien.
*           When a vote event exists, vote function again calls the kick method without taking care about the players nature.
*
* @param pid. An int, the players id
*
* @return int  1 on success
*              0 on failure
*
*/
int kick(int pid){
     if(pid<0)
        return 0;       // invalid pid
    struct Players *player=find_player_with_pid(pid);
    if(!player)
        return 0;       //THERE IS NO PLAYER WITH PID OR THERE ARE NOT PLAYERS IN THE LIST
    struct Tasks *tasks_list_ejected, *tasks_list_min, *sentinel_of_ejected, *sentinel_of_min;
    if(player->tasks_head==player->tasks_sentinel){  //PLAYER HAS COMPLETED ALL HIS TASKS OR IS ALIEN
        //just delete the player
        player->prev->next=player->next;
        player->next->prev=player->prev;
        player->next=NULL;
        player->prev=NULL;
        free(player);
        return 1;
    }
    //find players task list
    tasks_list_ejected=player->tasks_head;
    sentinel_of_ejected=player->tasks_sentinel;
    player->tasks_head=NULL;
    player->tasks_sentinel=NULL;
    //delete player
    player->prev->next=player->next;
    player->next->prev=player->prev;
    player->next=NULL;
    player->prev=NULL;
    free(player);
    player=find_min_tasks_player(NULL);
    if(player==NULL){
        return 1;       //ALL THE OTHER PLAYERS LEFT ARE ALIENS SO KICK THE CREWMATE AND DONT MERGE HIS LIST. ONLY ALIENS LEFT
    }
    tasks_list_min=player->tasks_head;
    sentinel_of_min=player->tasks_sentinel;
    player->tasks_head=merge(tasks_list_ejected, sentinel_of_ejected, tasks_list_min, sentinel_of_min);
    return 1;
}

/**
*
* @brief Finds the player with the less tasks in the players list
*
* @param Number a pointer to int in order to return the number of the tasks (neccesery at the give work event)
*
* @return A pointer pointed at the player with the less tasks
*         NULL if all players are aliens
*/
struct Players* find_min_tasks_player(int *number){
    int min_tasks=Total_Tasks+1, found=0, task_counter;
    struct Players *players_temp, *min_tasks_player;
    struct Tasks *tasks_temp;
    players_temp=players_head->next;
    while(players_temp!=players_head){
        if(players_temp->is_alien==1){
            players_temp=players_temp->next;
            continue;
        }
        found=1;
        tasks_temp=players_temp->tasks_head;
        task_counter=0;
        while(tasks_temp!=players_temp->tasks_sentinel){
            task_counter++;
            tasks_temp=tasks_temp->next;
        }
        if(task_counter<min_tasks){
            min_tasks=task_counter;
            if(number!=NULL)
                *number=min_tasks;      //sometimes we dont want the number of the tasks. call as: fnd_mn_tsks_plr(NULL);
            min_tasks_player=players_temp;
        }
        players_temp=players_temp->next;
    }
    if(!found)
        return NULL;  //ALL PLAYERS ALIENS
    return min_tasks_player;   //RETURN THE PLAYER WITH THE LESS TASKS
}

/**
*
* @brief Finds the player with the maximum number of tasks in the players list
*
* @param Number a pointer to int in order to return the number of the tasks (neccesery at the give work event)
* @return A pointer pointed at the player with the maximum number of tasks
*         NULL if all players are aliens
*/
struct Players* find_max_tasks_player(int *number){
    int max_tasks=-1, found=0, task_counter;
    struct Players *players_temp, *max_tasks_player;
    struct Tasks *tasks_temp;
    players_temp=players_head->next;
    if(players_temp==players_head)
        return NULL; //NO PLAYERS
    while(players_temp!=players_head){
        if(players_temp->is_alien==1){
            players_temp=players_temp->next;
            continue;
        }
        found=1;
        tasks_temp=players_temp->tasks_head;
        task_counter=0;
        while(tasks_temp!=players_temp->tasks_sentinel){
            task_counter++;
            tasks_temp=tasks_temp->next;
        }
        if(task_counter>max_tasks){
            max_tasks=task_counter;
            if(number!=NULL)
                *number=max_tasks;      //sometimes we dont want the number of the tasks. call as: fnd_mn_tsks_plr(NULL);
            max_tasks_player=players_temp;
        }
        players_temp=players_temp->next;
    }
    if(!found)
        return NULL;  //ALL PLAYERS ALIENS
    return max_tasks_player;   //RETURN THE PLAYER WITH THE LESS TASKS
}

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
struct Tasks* merge(struct Tasks *tl1, struct Tasks *sentinel1, struct Tasks *tl2, struct Tasks *sentinel2){
    struct Tasks *temp1, *temp2, *temp, *prev;
    temp1=tl1;
    temp2=tl2;
    if(temp2==sentinel2){
        tl2=temp1;
        sentinel1->next->next=sentinel2;
        sentinel2->next=sentinel1->next;
        return tl2;
    }
    while(temp1!=sentinel1 && temp2!=sentinel2){
        if(temp1->difficulty<=temp2->difficulty){
            tl1=temp1->next;
            temp1->next=temp2;
            if(temp2==tl2)
                tl2=temp1;
            else
                prev->next=temp1;
            prev=temp1;
            temp1=tl1;
        }else{
            prev=temp2;
            temp2=temp2->next;
        }
    }
    if(temp1!=sentinel1){
        prev->next=temp1;
        sentinel2->next=sentinel1->next;
        sentinel1->next->next=sentinel2;
    }
    return tl2;
}

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
int witness_eject(int pid, int pid_a, int number_of_witnesses){
    if(pid<0 || pid_a<0 || number_of_witnesses<0)
        return 0;
    struct Players *alien, *crewmate;
    alien=find_player_with_pid(pid_a);
    crewmate=find_player_with_pid(pid);
    if(!alien || !crewmate)
        return 0;                   //PID CREWMATE DOES NOT EXIST OR PID ALIEN DOES NOT EXIST OR THERE ARE NO PLAYERS IN THE LIST
    if(alien->is_alien!=1)
        return 0;                   //THIS pid_a PLAYER IS NOT AN ALIENS
    if(crewmate->is_alien!=0)       //THIS pid PLAYER IS NOT A CREWMATE
        return 0;
    if(!kick(pid))
        return 0;
    alien->evidence+=number_of_witnesses;
    alien=players_head->next;
    if(!print_evidence_and_tasks()){
        return 0;
    }
    return 1;
}

/**
*
* @brief prints the double list as: <Player1, evidence1> = <tid1,1,difficulty1,1>,<tid1,2,difficulty1,2> 
*                                   <Player2, evidence2> = <tid2,1,difficulty2,1>,<tid2,2,difficulty2,2>
* @return  1 on success
*          0 on failure
*/
int print_evidence_and_tasks(void){
    if(players_head->next==players_head)
        return 0;   //no players
    struct Tasks *tasks_temp;
    struct Players *players_temp;
    players_temp=players_head->next;
    printf("\n\n\n");
    while(players_temp!=players_head){
        if(players_temp->tasks_head==players_temp->tasks_sentinel){
            printf("Player%d, evidence%d = NO TASKS\n\n\n", players_temp->pid, players_temp->evidence);
        }else{
            printf("Player%d, evidence%d = ", players_temp->pid, players_temp->evidence);
            tasks_temp=players_temp->tasks_head;
            while(tasks_temp->tid!=-1){
                printf("Tid%d, difficulty%d   ", tasks_temp->tid, tasks_temp->difficulty);
                tasks_temp=tasks_temp->next;
            }
            printf("\n\n\n");   
        }
        players_temp=players_temp->next;
    }
    return 1;
}

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
int sabbotage(int number_of_tasks, int pid){
    if(pid<0){
        return 0;       // invalid pid
    }
    if(number_of_tasks>tasks_stack->count || number_of_tasks<0){
        return 0;       //invalid number of tasks
    }
    struct Players *temp, *player=find_player_with_pid(pid);
    struct Tasks *task_temp, *popped_task, *prev;
    int i;
    if(!player){
        return 0;    //THERE IS NO PLAYER WITH PID OR THERE ARE NO PLAYERS IN THE LIST
    }
    //GO TO THE TARGET PLAYER
    for(i=0; i<number_of_tasks/2; i++)
        player=player->prev;
    //FIND THE FIRST NON ALIEN PLAYER
    if(player->is_alien==1 || player==players_head){
        temp=player->next;
        while(temp!=player){
            if(temp->is_alien==0)
                break;
            temp=temp->next;
        }
        if(temp==player){
            return 0;    //ALL PLAYERS ARE ALIENS
        }
        player=temp;
    }
    //START WITH THE PLAYER AT THE ADRRESS player WHO IS NOT ALIEN BECAUSE OF THE CODE ABOVE!
    for(i=0; i<number_of_tasks; i++){
        popped_task=pop();
        if(popped_task==NULL){   //POP FAILED
        printf("^^^^^^^^^^^^^^^^^^^^^^^^^pop failed\n");
            return 0;
        }
        while(player->is_alien==1 || player==players_head)  //FIND THE NEXT NON ALIEN OR HEAD PLAYER
            player=player->next;
        task_temp=player->tasks_head;
        prev=task_temp;
        while(task_temp!=player->tasks_sentinel){
            if(task_temp->difficulty>popped_task->difficulty)
                break;
            prev=task_temp;
            task_temp=task_temp->next;
        }
        if(task_temp==prev){  //INSERT AT THE BEGINNING
            popped_task->next=player->tasks_head;
            player->tasks_head=popped_task;
            //popped_task->next=player->tasks_sentinel;
            //player->tasks_sentinel->next=popped_task;
        }else{                                                                  //INSERT AT THE MIDDLE
            popped_task->next=prev->next;
            prev->next=popped_task;
        }
        if(popped_task->next==player->tasks_sentinel)
            player->tasks_sentinel->next=popped_task;
        player=player->next;                //MOVE TO THE NEXT PLAYER
    }
    print_double_list();
    //print_stack();
    return 1;
}


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
int vote(int pid, int vote_evidence){
    struct Players *player=find_player_with_pid(pid);
    int ejected;
    if(!player)
        return 0;
    player->evidence+=vote_evidence;
    print_evidence_and_tasks();
    int max_evidence=-1;
    struct Players *max_ev_player;
    player=players_head->next;
    while(player!=players_head){
        if(player->evidence>max_evidence){
            max_evidence=player->evidence;
            max_ev_player=player;
        }
        player=player->next;
    }
    ejected=max_ev_player->pid;
    if(!kick(max_ev_player->pid))
        return 0;
    printf("\nPLAYER %d EJECTED!\n", ejected);
    print_double_list();
    return 1;
}


/**
 * @brief Give Away Work
 *
 * @return 1 on success
 *         0 on failure
 */
int give_work(void){
    int number_of_max, number_of_min, i;      //the number of max-min tasks
    struct Players *min_tasks_player=find_min_tasks_player(&number_of_min);
    struct Players *max_tasks_player=find_max_tasks_player(&number_of_max);
    struct Tasks *tasks_temp, *last_task, *temp1, *temp2, *prev, *task_after_temp, *sentinel1, *sentinel2;

    if(!max_tasks_player || !min_tasks_player || min_tasks_player==max_tasks_player){
        return 0;           //find min/max failed or there is only one crewmate
    }
    if(number_of_max==number_of_min){
        return 0;    //same number of tasks
    }
    for(i=0; i<number_of_max/2; i++){
        if(i==0)
            tasks_temp=max_tasks_player->tasks_head;
        else
            tasks_temp=tasks_temp->next;
    }
    last_task=max_tasks_player->tasks_sentinel->next;
    task_after_temp=tasks_temp->next;
    tasks_temp->next=max_tasks_player->tasks_sentinel;
    max_tasks_player->tasks_sentinel->next=tasks_temp;

    temp1=max_tasks_player->tasks_head;
    temp2=min_tasks_player->tasks_head;
    sentinel1=tasks_temp->next;
    sentinel2=min_tasks_player->tasks_sentinel;
    if(temp2==sentinel2){
        min_tasks_player->tasks_head=temp1;
        sentinel1->next->next=sentinel2;
        sentinel2->next=sentinel1->next;
    }else{
        while(temp1!=sentinel1 && temp2!=sentinel2){
            if(temp1->difficulty<=temp2->difficulty){
                max_tasks_player->tasks_head=temp1->next;
                temp1->next=temp2;
                if(temp2==min_tasks_player->tasks_head)
                    min_tasks_player->tasks_head=temp1;
                else
                    prev->next=temp1;
                prev=temp1;
                temp1=max_tasks_player->tasks_head;
            }else{
                prev=temp2;
                temp2=temp2->next;
            }
        }
        if(temp1!=sentinel1){
            prev->next=temp1;
            sentinel2->next=sentinel1->next;
            max_tasks_player->tasks_head=sentinel1->next->next;
            sentinel1->next->next=sentinel2;
        }
    }
    
    max_tasks_player->tasks_head=task_after_temp;
    sentinel1->next=last_task;

    print_double_list();
    return 1;
}

/**
 * @brief Terminate
 *
 * @return 1 on success
 *         0 on failure
 */
int terminate(void){
    int number_of_aliens=0, number_of_crewmates=0;
    struct Players* temp;
    temp=players_head->next;
    if(temp==players_head)
        return 0;               //NO PLAYERS
    while(temp!=players_head){
        switch(temp->is_alien){
            case 0:
                number_of_crewmates++;
                break;
            case 1:
                number_of_aliens++;
                break;
            default:
                return 0;
        }
        temp=temp->next;
    }
    if(number_of_aliens>number_of_crewmates){
        printf("Aliens win\n");
        return 1;
    }
    if(number_of_aliens==0){
        printf("Crewmates win\n");
        return 1;
    }
    if(tasks_stack->count==Total_Tasks){
        printf("Crewmates win\n");
        return 1;   
    }
    printf("No one win\n");
    return 0;           //NO ONE WIN
}

/**
 * @brief Print Players
 *
 * @return 1 on success
 *         0 on failure
 */
int print_players(void){
    struct Players *temp;
    temp=players_head->next;
    printf("Players = ");
    while(temp!=players_head){
        printf("%d:%d ", temp->pid, temp->is_alien);
        temp=temp->next;
    }
    printf("\n");
    return 1;
}

/**
 * @brief Print Tasks
 *
 * @return 1 on success
 *         0 on failure
 */
int print_tasks(void){
    struct Tasks *temp;
    temp=tasks_head->head;
    printf("Tasks = ");
    while(temp!=NULL){
        printf("%d,%d ", temp->tid, temp->difficulty);
        temp=temp->next;
    }
    printf("\n");
    return 1;
}

/**
 * @brief Print Stack
 *
 * @return 1 on success
 *         0 on failure
 */
int print_stack(void){
    if(is_empty_stack()){
        printf("### STACK IS EMPTY###\n");
        return 0;
    }
    struct Tasks *temp;
    temp=tasks_stack->head;
    printf("\n\nCompleted Tasks: %d\n", tasks_stack->count);
    printf("### IN STACK: \n");
    while(temp!=NULL){
        printf("\ttid: %d dif: %d\n", temp->tid, temp->difficulty);
        temp=temp->next;
    }
    printf("### END OF STACK\n\n\n");
    return 1;
}

/**
 * @brief Print Players & Task List
 *
 * @return 1 on success
 *         0 on failure
 */
int print_double_list(void){
    if(players_head->next==players_head)
        return 0;   //no players
    struct Tasks *tasks_temp;
    struct Players *players_temp;
    players_temp=players_head->next;
    printf("\n\n\n");
    while(players_temp!=players_head){
        if(players_temp->tasks_head==players_temp->tasks_sentinel){
            printf("Player%d = NO TASKS\n\n\n", players_temp->pid);
        }else{
            printf("Player%d = ", players_temp->pid);
            tasks_temp=players_temp->tasks_head;
            while(tasks_temp->tid!=-1){
                printf("Tid%d, difficulty%d   ", tasks_temp->tid, tasks_temp->difficulty);
                tasks_temp=tasks_temp->next;
            }
            printf("\n\n\n");   
        }
        players_temp=players_temp->next;
    }
    return 1;
}

/**
 * @brief Free resources
 *
 * @return 1 on success
 *         0 on failure
 */

int free_all(void){
    struct Players *players_temp, *next_p;
    struct Tasks *tasks_temp, *next_t;
    //free stack
    tasks_temp=tasks_stack->head;
    while(tasks_temp!=NULL){
        next_t=tasks_temp->next;
        if(tasks_temp)
            free(tasks_temp);
        tasks_temp=next_t;
    }
    free(tasks_stack);
    printf("stack deleted!\n");
    //free tasks
    tasks_temp=tasks_head->head;
    while(tasks_temp!=NULL){
        next_t=tasks_temp->next;
        free(tasks_temp);
        tasks_temp=next_t;
    }
    free(tasks_head);
    printf("tasks deleted!\n");
    //free players
    players_temp=players_head->next;
    
    while(players_temp!=players_head){
        tasks_temp=players_temp->tasks_head;
        while(tasks_temp!=players_temp->tasks_sentinel){
            next_t=tasks_temp->next;
            free(tasks_temp);
            tasks_temp=next_t;
        }
        next_p=players_temp->next;
        free(players_temp->tasks_sentinel);
        free(players_temp);
        players_temp=next_p;
    }
    free(players_head);
    printf("players deleted!\n");
    return 1;
}

        /*########################
             MY STACK FUNCTIONS 
        ##########################*/
/**
 * @brief Checks if the Stack is empty
 *
 * @return 1 if empty
 *         0 if not
 */
int is_empty_stack(void){
    if(tasks_stack->head==NULL)
        return 1;
    return 0;
}

/**
* @brief Push a completed task in the stack
* 
* @param pointer A pointer to the completed task node
*
*/
void push(struct Tasks *pointer){
    pointer->next=tasks_stack->head;
    tasks_stack->head=pointer;
    tasks_stack->count++;
    return;
}

/**
* @brief Pops a completed task from the stack
* 
* @return A pointer to the popped task node
*         NULL in failure
*
*/
struct Tasks* pop(void){
    if(is_empty_stack())
        return NULL;
    struct Tasks *temp;
    temp=tasks_stack->head;
    tasks_stack->head=temp->next;
    tasks_stack->count--;
    return temp;
}
