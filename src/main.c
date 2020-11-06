#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define STATION_NUMBER 271 //number of stations in London.
#define MAX_STATION_LINKS 10 //maximum number of underground links from a station.
#define TRAIN_SWITCH_TIME 5 //the time it takes to switch to another line. Adjustable.

// Represents the path between two stations. Structure is held in the station struct.
typedef struct link{
    int to_station; //if to_station is -2 then the link doesn't exist. if to_station is -1 means the to_station still needs to be found.
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

    int links_exhausted; //Flag to show whether or not the station has exhausted all possible links.
    struct station *from_station; // pointer to the previous station that the train visited.
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

// gets the node with the shortest time, and pops it out of the queue.
priority_queue_node *pop_queue(queue *queue){
    if (queue == NULL || queue->head == NULL){
        return NULL;
    }
    priority_queue_node *node = queue->head;
    queue->head = queue->head->next;
    return node;
}

//pops the node that was last inserted into the stack.
stack_node *pop_stack(stack *stack){
    if (stack == NULL || stack->head == NULL){
        return NULL;
    }
    stack_node *node = stack->head;
    stack->head = stack->head->next;
    return node;
}


// performs dijkstra's algorithm.
void dijkstra(station *stations, int starting_station, int target_station){

    queue priority_queue;
    priority_queue.head = NULL;
    int link_id;
    int from_station = starting_station;

    // stops when the target_station is inserting it's links into the priority queue.
    while (from_station != target_station) {
        station *current_station = &stations[from_station]; //the station that we are on currently.

        // inserts every link into the priority queue, unless it's already been added or doesn't exist.
        for (link_id = 0; link_id < MAX_STATION_LINKS; link_id++) {
            int next_station = current_station->links[link_id].to_station;

            //if next_station doesn't exist or if the current station's links have already been added to priority queue.
            if (next_station == -2 || current_station->links_exhausted){
                break;
            }

            //if the next station already has a path to it, then we already found the shortest path to next_station.
            //if next station is the starting station then skip.
            if (stations[next_station].from_station != NULL || next_station == starting_station){
                continue;
            }

            // create a new node for the priority queue and assign it's data.
            priority_queue_node *link_node = malloc(sizeof(priority_queue_node));
            link_node->from_station = current_station;
            link_node->link = &current_station->links[link_id];
            link_node->time = current_station->time + current_station->links[link_id].time;

            // starting_station will not have a from_line as you are entering from there, therefore don't penalise.

            if (from_station != starting_station && strcmp(current_station->from_link->line, link_node->link->line) != 0) {
                link_node->time = link_node->time + TRAIN_SWITCH_TIME; //add TRAIN_SWITCH_TIME minutes if we need to switch trains, to discourage it.
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
        if (stations[current_link->to_station].time == 0.0) {
            // assign the current station to the destination station's previous station pointer & the time to travel.
            stations[current_link->to_station].from_station = node->from_station;
            stations[current_link->to_station].from_link = current_link;
            stations[current_link->to_station].time = node->time;
            from_station = current_link->to_station; // to get the links from this station as it is now part of the path.
        }
    }
}

int get_station_id(station *stations, const char *station_name){
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

stack_node *create_node(station *to_station){
    stack_node *node = malloc(sizeof(stack_node));
    node->to_station = to_station;
    return node;
}

// uses dijkstra's algorithm to get the shortest path between starting_station and target_station & outputs the path.

stack *get_path(station *stations, int target_station){
    stack *pathway = malloc(sizeof(stack)); // creates a stack to traceback the path from the target_station to the starting_station.
    pathway->head = NULL;
    stack_node *station_node;

    //putting the stack nodes into the stack using each station's from_station to traceback the path.
    station *current_station = &stations[target_station];

    station_node = create_node(current_station);
    push_into_stack(pathway, station_node);

    while (current_station->from_station != NULL){
        current_station = current_station->from_station;
        station_node = create_node(current_station);
        push_into_stack(pathway, station_node);

    }


    return pathway;
}

void output_path(station *stations, int target_station){
    stack *pathway = get_path(stations, target_station);


    // pop first node from stack.
    stack_node *station_node = pop_stack(pathway);

    // pointer to the current station & the station after that.
    station *current_station = station_node->to_station;
    station *next_station = station_node->next->to_station;

    // gets the index of the link in the links array of the station.

    // output the leaving statement.
    printf("LEAVING FROM: %s via - %s %s line, ride for ", current_station->name,
           next_station->from_link->direction,
           next_station->from_link->line);

    int stops_counter = 0; // count the number of stops before the next line switch.
    double distance = next_station->from_link->distance;

    // continue popping until the next_station is not the target_station.
    while (station_node->next->next != NULL){
        stops_counter++;

        station_node = pop_stack(pathway);
        current_station = station_node->to_station;
        next_station = station_node->next->to_station;
        distance += next_station->from_link->distance;


        // if the line switches, then output that the user should switch lines.
        if (strcmp(current_station->from_link->line, next_station->from_link->line) != 0) {

            if (stops_counter == 1){
                printf("%d stop\n", stops_counter);
            } else {
                printf("%d stops\n", stops_counter);
            }


            printf("WHEN ARRIVING AT: %s, SWITCH LINE TO %s - %s line, and ride for ",
                   current_station->name, next_station->from_link->direction,
                   next_station->from_link->line);

            stops_counter = 0; // reset

        }
    }

    // output the arrival message.
    stops_counter++;
    if (stops_counter == 1){
        printf("%d stop\n", stops_counter);
    } else {
        printf("%d stops\n", stops_counter);
    }
    printf("ARRIVE AT: %s. JOURNEY TIME: %.2f minutes / Distance Travelled: %.2f km",
           next_station->name, next_station->time, distance);

}

//initialises the data in each station.
int initialise_stations(station *stations, char *filename){
    char buffer[1024]; //used to temporarily store a line from the csv file.
    int station = -1; //used to index the array of station structs. Set to -1 to signal an initialisation for the first station.
    int link_counter=0; //used to index the array of link structs in the station struct.

    //temporarily stores the data for the station.
    char *line, *direction, *station_name, *destination;
    double distance, time;

    // parses the data from the file into the links array of structs.
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

        // using a cache to slightly speed up the process as the list is ordered.
        if (strcmp(stations[station].name, station_name) != 0 || station == -1) {
            // reset link_counter because its a new station & get the new station's to_station.
            station++; //increment by 1
            strcpy(stations[station].name, station_name);
            stations[station].links_exhausted = 0;
            stations[station].time = 0;
            stations[station].from_station = NULL;
            stations[station].from_link = NULL;


            // initialise the values in the links struct.
            memset(stations[station].links, 0, sizeof(link)*MAX_STATION_LINKS);
            for (link_counter=0; link_counter<MAX_STATION_LINKS; link_counter++){
                stations[station].links[link_counter].to_station = -2; // if there is no more links, should be -2 i.e. doesn't exist.
            }

            link_counter = 0; // reset the link counter so it can be used in the outer scope.
        }

        // copy the data into the station's link's structure.
        current_link = &stations[station].links[link_counter];
        current_link->to_station = -1; //indicating that the link is real and the to_station needs to be found.

        strcpy(current_link->destination, destination);
        strcpy(current_link->line, line);

        current_link->distance = distance;
        current_link->time = time;

        strcpy(current_link->direction, direction);

        link_counter++;
    }

    // iterates through every link and finds the destination's station's to_station for faster indexing.
    for (station=0; station<STATION_NUMBER; station++){
        for (link_counter=0; link_counter<MAX_STATION_LINKS; link_counter++){
            current_link = &stations[station].links[link_counter];

            if (current_link->to_station == -2){
                break;
            }
            current_link->to_station = get_station_id(stations, current_link->destination);
            free(current_link->destination); // no longer used.
        }
    }
    return 1;
}


// gets the station index from the user's input, & returns it.
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


int get_user_option_input(void){
    int user_input = 0;
    char ch;
    while (1) {
        printf("\n\nPlease select an option from the following:\n(1) Find fastest route between two stations.\n(2) Exit the program\n");
        scanf("%d", &user_input);
        while ((ch = getchar()) != '\n' && ch != EOF) {} //erase buffer.


        switch (user_input){
            case 1:
                return 1;
            case 2:
                return 2;
            default:
                printf("Unrecognised option. Please try again");
        }
    }
}

int main() {


    station stations[STATION_NUMBER];

    if (initialise_stations(stations, "../data/stations_db.csv") == 0){
        printf("ERROR READING FILE. PLEASE CHECK FILE");
        return -1;
    }

    printf("Welcome to Transport For London path selector");

    while (get_user_option_input() == 1) {

        int starting_station = get_user_station_input(stations, "Enter from station name: ");
        int target_station = get_user_station_input(stations, "Enter to station name: ");

        dijkstra(stations, starting_station, target_station);
        output_path(stations, target_station);
        reset_station_paths(stations);
    }

    printf("\n Exiting...");
    return 0;
}
