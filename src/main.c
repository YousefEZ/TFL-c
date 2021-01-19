#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define STATION_NUMBER 271 //number of stations in London.
#define MAX_STATION_LINKS 10 //maximum number of underground links from a station.

// Represents the path between two stations. Structure is held in the station struct.
typedef struct link{
    int to_station_index; //if to_station is -2 then the link doesn't exist. if to_station is -1 means the to_station still needs to be found.
    double time; // time it takes to traverse this path.
    double distance; // distance between the station that this link is held from, to the to_station's station (path distance)
    char line[32]; // name of the line that the train traverses on.
    char destination[32]; // name of the to_station, used to initialise the link.
    char direction[32]; // the direction of the path (Northbound, Eastbound, Southbound, Westbound, etc.)
} link;

// Represents the station, and how the station is connected to the path.
typedef struct station{
    char name[32]; // Name of the station.
    link links[MAX_STATION_LINKS]; // The paths starting from this station.

    // following data is used for storing the path data from one station to another using dijkstra

    int links_exhausted; //Flag to show whether or not the station has exhausted all possible links.
    struct station *from_station; // pointer to the previous station that the train visited before this station.
    link *from_link; // the line that the train used from the from_station to this station.
    double time; // the time is took to reach this station from the starting station.

}station;

// represents a node in the priority queue
typedef struct priority_queue_node{
    station *from_station;
    link *link;
    double time;
    struct priority_queue_node *next;
}priority_queue_node;

// Abstract Data Type (ADT), Queue. Holds data in a linked list fashion. Node with the shortest time is popped first.
typedef struct queue{
    priority_queue_node* head;
}queue;

// represents a node in a stack
typedef struct stack_node{
    station *to_station;
    struct stack_node *next;
}stack_node;

// Abstract Data Type (ADT), Stack. Holds data in a linked list fashion. Node at the head gets popped first (FILO).
typedef struct stack {
    stack_node *head;
}stack;



void push_into_stack(stack *stack, stack_node* new_node) {
    new_node->next = stack->head;
    stack->head = new_node;
}

void push_into_priority_queue(queue *queue, priority_queue_node *new_node){
    if (queue->head == NULL){
        // there is no node in the head. assign the pointer to the new_node into the queue head, and the new_node's next to NULL.
        queue->head = new_node;
        new_node->next = NULL;
    } else {
        // finds it's position in the queue based on the new_node's time (priority queue).
        priority_queue_node *current = queue->head;
        while (current->next != NULL && current->time < new_node->time){
            current = current->next;
        }
        new_node->next = current->next;
        current->next = new_node;
    }
}

// gets the node in the head (node with the shortest time), and pops it out of the queue (first in, last out).
priority_queue_node *pop_queue(queue *queue){
    if (queue == NULL || queue->head == NULL){
        return NULL;
    }
    priority_queue_node *node = queue->head;
    queue->head = queue->head->next;
    return node;
}

//pops the node that was last inserted into the stack (last in, first out).
stack_node *pop_stack(stack *stack){
    if (stack == NULL || stack->head == NULL){
        return NULL;
    }
    stack_node *node = stack->head;
    stack->head = stack->head->next;
    return node;
}

stack_node *create_stack_node(station *to_station){
    stack_node *node = malloc(sizeof(stack_node));
    node->to_station = to_station;
    return node;
}


// uses dijkstra's algorithm to get the shortest path between starting_station and target_station.
// reference: https://www.youtube.com/watch?v=GazC3A4OQTE&ab_channel=Computerphile
void dijkstra(station *stations, int starting_station_index, int target_station_index, int line_switch_time){

    queue priority_queue;
    priority_queue.head = NULL;
    int link_index;
    int current_station_index = starting_station_index;

    // stops when the target_station is about to insert it's links into the priority queue.
    while (current_station_index != target_station_index) {
        station *current_station = &stations[current_station_index]; //the station that we are on currently.

        // inserts every link into the priority queue, unless it's already been added or doesn't exist.
        for (link_index = 0; link_index < MAX_STATION_LINKS; link_index++) {
            int next_station = current_station->links[link_index].to_station_index;

            // if next_station doesn't exist then there is no more existing links or,
            // if the current station's links have already been added to priority queue then there is no need to add copies
            if (next_station == -2 || current_station->links_exhausted){
                break;
            }

            //if the next station already has a path to it, then we already found the shortest path to next_station.
            //if next station is the starting station then skip.
            if (stations[next_station].from_station != NULL || next_station == starting_station_index){
                continue;
            }

            // create a new node for the priority queue and assign it's data.
            priority_queue_node *link_node = malloc(sizeof(priority_queue_node));
            link_node->from_station = current_station;
            link_node->link = &current_station->links[link_index];
            link_node->time = current_station->time + current_station->links[link_index].time;

            // starting_station will not have a from_line as you are entering from there, therefore don't penalise.

            if (current_station_index != starting_station_index && strcmp(current_station->from_link->line, link_node->link->line) != 0) {
                link_node->time = link_node->time + line_switch_time; //add line_switch_time minutes if we need to switch lines.
            }


            push_into_priority_queue(&priority_queue, link_node);
        }

        current_station->links_exhausted = 1; // we have placed all the links from this station into the priority queue.
        priority_queue_node *node = pop_queue(&priority_queue); //pops the node with the shortest time in the queue.

        // if the queue is empty.
        if (node == NULL){
            break;
        }
        link *current_link = node->link;

        // If the station has not been accessed yet, that means that this path is the fastest to it.
        if (stations[current_link->to_station_index].time == 0.0) {
            // assign the link's from_station to the to_station's from_station pointer & the time to travel.
            stations[current_link->to_station_index].from_station = node->from_station;
            stations[current_link->to_station_index].from_link = current_link;
            stations[current_link->to_station_index].time = node->time;
            current_station_index = current_link->to_station_index; // to get the links from this station as it is now part of the path.
        }
        free(node); // free memory on the heap.
    }
}

int get_station_id(station *stations, char *station_name){
    // gets the to_station of the station.
    // using binary search.

    int First = 0;
    int Last = STATION_NUMBER;
    int Middle;


    while (1){
        Middle = (int) (First + Last) / 2;

        if (strcmp(stations[Middle].name, station_name) == 0 ){
            return Middle;
        } else {
            if (First >= Last){
                return -1;
            } else {
                if (strcmp(stations[Middle].name, station_name) > 0){
                    Last = Middle - 1;
                } else {
                    First = Middle + 1;
                }
            }
        }
    }
}

stack *get_path(station *stations, int target_station){
    stack *pathway = malloc(sizeof(stack)); // creates a stack to traceback the path from the target_station to the starting_station.
    pathway->head = NULL;
    stack_node *station_node;

    //putting the stack nodes into the stack using each station's from_station to traceback the path.
    station *current_station = &stations[target_station];

    station_node = create_stack_node(current_station);
    push_into_stack(pathway, station_node);

    // only the starting_station has a from_station of NULL on this path.
    while (current_station->from_station != NULL){
        current_station = current_station->from_station;
        station_node = create_stack_node(current_station);
        push_into_stack(pathway, station_node);

    }

    return pathway;
}

void output_number_stops(int stops_counter){
    if (stops_counter == 1){
        printf("%d stop\n", stops_counter);
    } else {
        printf("%d stops\n", stops_counter);
    }
}

void output_path(station *stations, int target_station){
    stack *pathway = get_path(stations, target_station);

    // pop first node from stack.
    stack_node *station_node = pop_stack(pathway);

    // pointer to the current station & the station after that.
    station *current_station = station_node->to_station;
    station *next_station = station_node->next->to_station;

    // output the leaving statement.
    printf("LEAVING FROM: %s via - %s %s line, and ride for ", current_station->name,
           next_station->from_link->direction,
           next_station->from_link->line);

    int stops_counter = 0; // count the number of stops before the next line switch.
    double distance = next_station->from_link->distance; // get the distance from current_station to next_station.

    // continue popping unless the next station after the next_station doesn't exist, then output arrival statement.


    stack_node *after_next_station_node = station_node->next->next;
    // if the station after the next_station is null, then next_station is the target_station => Output arrival message.
    while (after_next_station_node != NULL){
        stops_counter++;

        station_node = pop_stack(pathway);
        current_station = station_node->to_station;
        next_station = station_node->next->to_station;

        after_next_station_node = station_node->next->next;
        free(station_node); // free memory on the heap.

        distance += next_station->from_link->distance;


        // if the line switches, then output that the user should switch lines.
        if (strcmp(current_station->from_link->line, next_station->from_link->line) != 0) {

            output_number_stops(stops_counter);
            printf("WHEN ARRIVING AT: %s, SWITCH LINE TO %s - %s line, and ride for ",
                   current_station->name, next_station->from_link->direction,
                   next_station->from_link->line);

            stops_counter = 0; // reset

        }
    }

    // output the arrival message.
    stops_counter++;
    output_number_stops(stops_counter);
    printf("ARRIVE AT: %s.\n\nESTIMATED JOURNEY TIME: %.2f minutes / Distance Travelled: %.2f km",
           next_station->name, next_station->time, distance);

}

//initialises the data in each station.
int initialise_stations(station *stations, char *filename){
    char buffer[1024]; //used to temporarily store a line from the csv file.
    int station_index = -1; //used to index the array of station structs. Set to -1 to signal an initialisation for the first station.
    int link_counter=0; //used to index the array of link structs in the station struct.

    //temporarily stores the data for the station.
    char *line, *direction, *station_name, *destination;
    double distance, time;

    // parses the data from the file.
    FILE *fp = fopen(filename, "r");
    if (!fp){
        return 0;
    }
    link *current_link;
    while (fgets(buffer, 1024, fp)){
        line = strtok(buffer,",");
        direction = strtok(NULL, ",");
        station_name = strtok(NULL, ",");
        destination = strtok(NULL, ",");
        distance = strtod(strtok(NULL, ","), NULL); //converts the value from string to a double.
        time = strtod(strtok(NULL, ","), NULL); //converts the value from string to a double.

        // as the list is ordered, we only need to change the station_index if the name of the from_station station changes.
        if (strcmp(stations[station_index].name, station_name) != 0 || station_index == -1) {
            // reset link_counter because its a new station & get the new station's to_station.
            station_index++; //increment by 1
            strcpy(stations[station_index].name, station_name);
            stations[station_index].links_exhausted = 0;
            stations[station_index].time = 0;
            stations[station_index].from_station = NULL;
            stations[station_index].from_link = NULL;


            // initialise the values in the links struct.
            memset(stations[station_index].links, 0, sizeof(link) * MAX_STATION_LINKS);
            for (link_counter=0; link_counter<MAX_STATION_LINKS; link_counter++){
                stations[station_index].links[link_counter].to_station_index = -2; // if there is no more links, should be -2 i.e. doesn't exist.
            }

            link_counter = 0; // reset the link counter so it can be used in the outer scope.
        }

        // copy the data into the station's link's structure.
        current_link = &stations[station_index].links[link_counter];
        current_link->to_station_index = -1; //indicating that the link is real and the station's index needs to be found.

        strcpy(current_link->destination, destination);
        strcpy(current_link->line, line);

        current_link->distance = distance;
        current_link->time = time;

        strcpy(current_link->direction, direction);

        link_counter++;
    }

    // iterates through every link and finds the destination's station's to_station for faster indexing.
    for (station_index = 0; station_index < STATION_NUMBER; station_index++){
        for (link_counter = 0; link_counter < MAX_STATION_LINKS; link_counter++){

            current_link = &stations[station_index].links[link_counter];

            if (current_link->to_station_index == -2){
                break;
            }
            current_link->to_station_index = get_station_id(stations, current_link->destination);
            free(current_link->destination); // no longer used.
        }
    }
    return 1;
}


// gets the index for a station from the user's input, & returns it.
int get_user_station_input(station *stations, char *message){

    int station = -1;
    char station_name[64];

    while (station == -1) {
        printf("%s", message);
        gets(station_name);
        strupr(station_name);
        if ((station = get_station_id(stations, station_name)) == -1) {
            printf("STATION NOT RECOGNISED\n");
        }
    }
    return station;
}

// gets user input and verifies that the input is an alphabetical character.
char get_user_character_input(void){
    char character;

    while (1){
        printf("Please enter a character to search: ");
        scanf("%c", &character);
        char ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {} //erase buffer.
        if (isalpha(character) == -1) {
            printf("\nInvalid character\n");
            continue;
        }
        break;
    }

    return toupper(character);
}

// changes the time it takes to switch lines.
int change_line_switch_time(int line_switch_time){
    int user_input;
    char ch;
    while (1) {
        printf("New time in minutes (between 0 to 30 minutes inclusive, -1 to cancel): ");
        scanf("%d", &user_input);
        while ((ch = getchar()) != '\n' && ch != EOF) {} //erase buffer.

        if (user_input >= 0 && user_input <= 31){
            printf("Line switch time changed: %d -> %d (minutes)", line_switch_time, user_input);
            return user_input;
        } else if (user_input == -1) {
            printf("Cancelling...");
            return -1;
        } else{
            printf("Invalid. Please try again\n");
        }
    }
}


// reset the path data in the stations.
void reset_station_paths(station *stations){
    int counter;

    for (counter = 0; counter < STATION_NUMBER; counter++) {
        stations[counter].from_station = NULL;
        stations[counter].from_link = NULL;
        stations[counter].time = 0.0;
        stations[counter].links_exhausted = 0;
    }
}

// outputs all the stations starting with the character in starting_char
void output_station_name(station *stations, char starting_char){
    int counter;
    int found_char = -1;

    for (counter=0; counter<STATION_NUMBER; counter++){

        if (stations[counter].name[0] == starting_char){
            if (found_char == -1){
                // so we can create a numbered list.
                found_char = counter;
            }
            printf("%d. %s\n", counter-found_char+1, stations[counter].name);

        } else if (found_char != -1){
            // since the stations are ordered in (A -> Z), if the character changes & we already found the char then stop.
            break;
        }
    }

}

// gets the user option input.
int get_user_option_input(int line_switch_time){
    int user_input = 0;
    char ch;
    while (1) {
        printf("\n\nPlease select an option from the following:\n"
               "(1) Find fastest route between two stations.\n"
               "(2) Output stations starting with a character. \n"
               "(3) Change time required to switch to a different line in the same station (CURRENT: %d MINUTES) \n"
               "(4) Exit the program\n", line_switch_time);

        scanf("%d", &user_input);
        while ((ch = getchar()) != '\n' && ch != EOF) {} //erase buffer.

        if (user_input > 0 && user_input < 5){
            return user_input;
        } else{
            printf("Unrecognised option. Please try again");
        }
    }
}

int main() {

    station stations[STATION_NUMBER]; // an array of all the stations in London.

    // initialise the values in the array of structures. If the file doesn't exist, then raise an error.
    if (initialise_stations(stations, "data/stations_db.csv") == 0){
        printf("ERROR READING FILE. PLEASE CHECK FILE");
        return -1;
    }

    printf("Welcome to Transport For London path selector");
    int option;
    int line_switch_time = 5; //the time it takes to switch to another line.
    while ((option = get_user_option_input(line_switch_time)) != 4) {

        switch (option){
            case 1:; // gets the shortest path between two stations.

                int starting_station = get_user_station_input(stations, "Enter from station name: ");
                int target_station = get_user_station_input(stations, "Enter to station name: ");
                printf("\n-------------------------\n\n");
                dijkstra(stations, starting_station, target_station, line_switch_time);
                output_path(stations, target_station);
                printf("\n\n-------------------------");
                reset_station_paths(stations);
                break;

            case 2:; // output all the stations starting with the character given.

                char input_character = get_user_character_input();
                printf("=======================\n");
                output_station_name(stations, input_character);
                printf("-----------------------");
                break;

            case 3:; // change the time it takes to switch to a different line in the same station.
                int new_time = change_line_switch_time(line_switch_time);
                if (new_time != -1){
                    line_switch_time = new_time;
                }
                break;

            default:
                break;

        }
    }

    printf("\n Exiting...");
    return 0;
}