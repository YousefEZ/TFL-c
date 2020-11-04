#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define STATION_NUMBER 271 //number of stations in London.
#define MAX_STATION_LINKS 10 //maximum number of underground links from a station.
#define TRAIN_SWITCH_TIME 5 //the time it takes to switch to another tube.

// add a preferences struct i.e. likes switching lines or not.

typedef struct priority_queue_node{
    int from_station_id;
    int to_station;
    int link_id;
    double time;
    struct priority_queue_node *next;
}priority_queue_node;

typedef struct queue{
    priority_queue_node* head;
}queue;

typedef struct stack_node{
    int to_station;
    struct stack_node *next;
}stack_node;

typedef struct stack {
    stack_node* head;
}stack;

typedef struct link{
    int to_station; //if to_station is -2 then the link doesn't exist. if to_station is -1 means the to_station still needs to be found.
    double time;
    double distance;
    char line[100];
    char destination[100];
    char direction[50];
} link;

typedef struct station{
    int id; //ID of the station
    char name[100]; //Name of the station.
    link links[MAX_STATION_LINKS]; //the places the train can go to.

    int links_exhausted; //Flag to show whether or not the station has exhausted all possible links.
    int from_station; // the previous station
    char from_line[100];
    double time;
}station;

void insert_into_stack(stack * stack, stack_node* new_node) {
    if (stack->head == NULL) {
        stack->head = new_node;
    } else {
        stack_node * current = stack->head;
        stack->head = new_node;
        new_node->next = current;
    }
}

void insert_into_priority_queue(queue *queue, priority_queue_node *new_node){
    if (queue->head == NULL){
        priority_queue_node* temp = queue->head;
        queue->head = new_node;
        new_node->next = temp;
    } else {
        priority_queue_node *current = queue->head;
        while (current->next != NULL && current->time < new_node->time){
            current = current->next;
        }
        priority_queue_node* temp = current->next;
        current->next = new_node;
        new_node->next = temp;
    }
}

priority_queue_node *pop(queue *queue){
    if (queue == NULL || queue->head == NULL){
        return NULL;
    }
    priority_queue_node *node = queue->head;
    queue->head = queue->head->next;
    return node;
}



void dijkstra(station *stations, int from_station, int target_station){

    queue queue;
    queue.head = NULL;
    int link;
    
    while (from_station != target_station) {
        station *current_station = &stations[from_station];
        for (link = 0; link < MAX_STATION_LINKS; link++) {
            int next_station = current_station->links[link].to_station;

            //if next_station doesn't exist or if the current station's links have already been added to priority queue.
            if (next_station == -2 || current_station->links_exhausted){
                break;
            }

            //if links_exhausted is 1, then we already found the shortest path to next_station.
            if (stations[next_station].links_exhausted){
                continue;
            }

            priority_queue_node *link_node = malloc(sizeof(priority_queue_node));
            link_node->from_station_id = from_station;
            link_node->to_station = next_station;
            link_node->link_id = link;
            link_node->time = current_station->time + current_station->links[link].time;

            if (strcmp(current_station->from_line, current_station->links[link].line) != 0 && current_station->from_station != -1) {
                link_node->time = link_node->time + TRAIN_SWITCH_TIME; //add TRAIN_SWITCH_TIME minutes if we need to switch trains, to discourage it.
            }

            insert_into_priority_queue(&queue, link_node);
        }
        priority_queue_node *node = pop(&queue);

        if (node == NULL){
            break;
        }
        stations[node->from_station_id].links_exhausted = 1;

        if ((node->time < stations[node->to_station].time || stations[node->to_station].time == 0.0)) {
            // assign the current station to the destination station's previous station pointer & the time to travel.
            stations[node->to_station].from_station = node->from_station_id;
            stations[node->to_station].time = node->time;
            strcpy(stations[node->to_station].from_line, stations[node->from_station_id].links[node->link_id].line);
            from_station = node->to_station;
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

int get_link_index(link *links, int station_id, char *line_name){
    // get the index of the link, which takes you to the specified station to_station using line_name and station_id from links.
    int link;
    for (link=0; link<MAX_STATION_LINKS; link++){
        if (links[link].to_station == station_id && (strcmp(links[link].line, line_name) == 0)){
            return link;
        }
    }
    return -1;
}


void output_path(station *stations, int starting_station, int target_station){

    dijkstra(stations, starting_station, target_station);

    stack pathway;
    pathway.head = NULL;
    stack_node *station_node;

    int station_id = target_station;
    while (station_id != -1){
        station_node = malloc(sizeof(stack_node));
        station_node->to_station = station_id;
        insert_into_stack(&pathway, station_node);
        station_id = stations[station_id].from_station;
    }

    station_node = pathway.head;
    int link_index;
    int stops_counter=0;

    station *current_station = &stations[station_node->to_station];
    station *next_station = &stations[station_node->next->to_station];

    link_index = get_link_index(current_station->links, next_station->id, next_station->from_line);

    printf("LEAVING FROM: %s via - %s %s line, ride for ", current_station->name,
           current_station->links[link_index].direction,
           next_station->from_line);

    while (station_node->next->to_station != target_station){
        stops_counter++;
        station_node = station_node->next;
        current_station = &stations[station_node->to_station];
        next_station = &stations[station_node->next->to_station];

        if (strcmp(current_station->from_line, next_station->from_line) != 0) {

            link_index = get_link_index(current_station->links, next_station->id, next_station->from_line);

            printf("%d stop(s)\nWHEN ARRIVING AT: %s, SWITCH LINE TO %s - %s line, and ride for ", stops_counter,
                   current_station->name, current_station->links[link_index].direction,
                   next_station->from_line);

            stops_counter = 0;

        }
    }

    printf("%d stop(s).\nARRIVE AT: %s. JOURNEY TIME: %.2f minutes.", stops_counter + 1, next_station->name, next_station->time);

}


int main() {


    station stations[STATION_NUMBER];

    FILE *fp = fopen("../data/stations_db.csv", "r");
    if (!fp){
        printf("Error reading file");
    }

    char buffer[1024]; //used to temporarily store a line from the csv file.
    int station = -1; //used to index the array of station structs. Set to -1 to signal an initialisation for the first station.
    int link_counter=0; //used to index the array of link structs in the station struct.
    char *line, *direction, *station_name, *destination;
    double distance, time;

    // parses the data from the file into the links array of structs.
    link *current_link;
    while (fgets(buffer, 1024, fp)){
        line = strtok(buffer,",");
        direction = strtok(NULL, ",");
        station_name = strtok(NULL, ",");
        destination = strtok(NULL, ",");
        distance = strtod(strtok(NULL, ","), NULL);
        time = strtod(strtok(NULL, ","), NULL); //converts the value from string to a double.

        // using a cache to slightly speed up the process as the list is ordered.
        if (strcmp(stations[station].name, station_name) != 0 || station == -1) {
            // reset link_counter because its a new station & get the new station's to_station.
            station++; //increment by 1
            stations[station].id = station;
            strcpy(stations[station].name, station_name); //remove the \n at the end of the buffer.
            stations[station].links_exhausted = 0;
            stations[station].time = 0;
            stations[station].from_station = -1;
            stations[station].from_line[0] = '\0';


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

        }
    }



    int user_input = 1;
    char ch;

    printf("Welcome to Transport For London path selector");

    while (user_input == 1) {
        printf("\n\nPlease select an option from the following:\n1.Find fastest route between two stations.\n2.Exit the program\n");
        scanf("%d", &user_input);
        while ((ch = getchar()) != '\n' && ch != EOF){} // erase buffer

        char f_station_name[64];
        char t_station_name[64];
        int f_station = -1;
        int t_station = -1;

        while (f_station == -1) {
            printf("Enter from station name: ");
            gets(f_station_name);
            strupr(f_station_name);
            if ((f_station = get_station_id(stations, f_station_name)) == -1) {
                printf("FROM STATION NOT RECOGNISED\n");
            }
        }

        while (t_station == -1) {
            printf("Enter to station name: ");
            gets(t_station_name);
            strupr(t_station_name);
            if ((t_station = get_station_id(stations, t_station_name)) == -1) {
                printf("TO STATION NOT RECOGNISED\n");
            }
        }

        output_path(stations, f_station, t_station);
        int counter;
        // reset the data in the station.
        for (counter = 0; counter < STATION_NUMBER; counter++) {
            stations[counter].from_station = -1;
            stations[counter].time = 0.0;
            stations[counter].links_exhausted = 0;
            strcpy(stations[counter].from_line, "");
        }
    }

    printf("\n Exiting. Having a nice day!");
    return 0;
}
