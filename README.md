# Among_us_linked_lists
This is a non virtual representation of the video game "Among us". This project uses data structures to store the players and the tasks and implements functions on them in specific time complexity. The project is splitted in two parts. The first part (this one) uses linked lists and stacks. Language: C

In the game there are players and tasks. Players: The players can be either crewmates either impostors. The players are stored in an unsorted double circular linked list with sentinel node. Each player has 3 attributes: their id, their role(crewmate/impostor) and their evidence. They also have a task list for their tasks as far as they are crewmates. This list remain always sorted by the tasks difficulty.

Tasks: There are 3 types of tasks which are formed by their difficulty. Tasks of difficulty 1, tasks of difficulty 2 and tasks of difficulty 3. Before they are distributed to the players, the tasks are stored in a simple linked list (general task list) sorted by their difficulty

When a task is implemented it is stored in a stack. For the program functionality there is a set of instructions: 

P <is_alien> This instruction signals the insertion of a new player with id=pid and with role=is_alien. Time complexity O(1)

T This instruction signals the insertion of a new task with id=tid and difficulty=difficulty in the general task list. Time complexity (On)

D During this event the tasks from the general task list are distributed to the players following a round robin algorithm. Time complexity O(n) (n is the amount of the tasks in general task list).

I This instruction signals the implementation of a task with difficulty: difficulty by the player: pid. If there is any task with this difficulty, the player implements a task with the highest difficulty from their task list. The implemented task is removed from the players task list and is added in the stack of the completed tasks.

E This instructions signals the ejection of the player: pid by an impostor. The player is removed from the players list. Their task list will be merged with the task list of this player with the less tasks. The merging has a time complexity of O(n1+n2) where n1 and n2 is the number of the two task lists. Note that the tasks must stay sorted after the merging.

W <pid_a > <number_witnesses> This instructions signals the ejection of player: pid by the impostor: pid_a. However there are witnesses so the impostor with pid: pid_a will obtain number_witnesses evidence. The ejection and task merging algorithm is the same as the E event.

S <number_of_tasks> This instruction signals a sabotage by an impostor. Number_of_tasks tasks will be popped from the completed tasks stack and will be given back to the players according to the following algorithm: from the player: pid in the players list going back floor(number_of_tasks/2) positions. From that position start giving the tasks to the non impostor players.

V <vote_evidence> This instruction signals a vote event by the the player: pid. This player will also obtain vote_evidence evidence because they look suspicious. The player with the most evidence will be kicked be an algorithm like the E event.

G This instruction signals a Give Away Work event. The player with the most tasks will give the half of them (those with the minimum difficulty value) to the player with the less tasks.

F This instruction indicates the termination of the game. During this event if the impostors are more than the crewmates then the impostors win. If there are no impostors or the completed tasks list contains the number of the primary tasks then the crewmates win.

X: prints the players Y: prints the tasks from general task list Z: prints the completed tasks stack U: prints the players with their task list
