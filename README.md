# Transport for London Station Pathfinder #
---
## v1.0 @ 01/11/2020
![picture alt](https://i.ibb.co/D1jGdsm/tfl-logo3.png "TFL")

The program utilises the tube station data set from the __Freedom of Information__ request found [here](https://www.whatdotheyknow.com/request/distance_between_adjacent_underg#incoming-5516) (can be downloaded via this [link](https://www.whatdotheyknow.com/request/1779/response/5516/attach/3/Inter%20station%20database.xls?cookie_passthrough=1))

## Dataset manipulation:
Firstly, the dataset has been manipulated by:
- [x] Sorting the rows based on the Station From (A), as well as deleting the first row.

- [x] Ensuring that stations fall under the same name even if they are the same station. e.g. Euston (CX) → Euston.

- [x] Ensuring that stations don't have a space after their name. e.g. "Baker Street " → "Baker Street".

- [x] Converting the file into a .csv to allow the file to be read more easily.

<br />

## Structures:
Secondly, the data that was retrieved were placed in structs. Such as:

<details><summary> Station (struct station): Holds the data about the station </summary><p>
    
   a. **id**: The index that station has in the array.
   
   b. **name**: Holds the name of the station.
   
   d. **links**: An array of type link that holds data about each link from the station.
    
    
    ... // The following is data about the path that the dijkstra algorithm uses.
    
    
   c. **links_exhausted**: Flag (1 or 0), that indicates whether or not the links from the stations are already in the priority queue.
    
   e. **from_station**: The shortest path from the "starting station" to this station, comes from this station (...→ from_station → id).
   
   f. **from_line**: Holds the string of the name of line.
   
   g. **time**: Holds the time taken to reach this station.
</p></details>

---

<details><summary> Link (struct link). Holds the data about the link between each station. </summary><p>
    
   a. **to_station**: Holds the link's destinations station's id. If it holds is -2 then the link doesn't exist, if -1 then the id still needs to be found.
   
   b. **time**: Holds the time it takes to travel from the station holding the link to the to_station.
  
   c. **distance**: Holds the distance between the from the station holding the link and the to_station.
   
   d. **line**: Holds the name of the line that this link uses.
   
   e. **destination**: Holds the name of the to_station. Used to fetch the to_station after all the stations have retrieved the required data.
   
   f. **direction**: Direction of where the train is going (Northbound, Southbound, Eastbound, Westbound etc.)
</p></details>

---

<details><summary> Priority Queue Node (struct priority_queue_node). Holds the data in a node for the priority queue. </summary><p>
    
   a. **from_station_id**: holds the data about the starting station of the link.
   
   b. **to_station**: holds the data about the destination station of the link.
   
   c. **link_id**: the index of the link in the station struct of the from_station_id's station:
   
   d. **time**: the time it takes to traverse this link.
   
   e. *priority_queue_node* **∗next**: holds the pointer of the next node in the queue.
    
</p></details>

---

<details><summary> Priority Queue (struct queue). Holds the nodes in a queue (for the priority queue). </summary><p>
    
   a. *priority_queue_node* **∗head**: holds the pointer of the starting node of the queue.
   
</p></details>

---

<details><summary> Stack Node (struct stack_node). Holds the data in a node for the stack. </summary><p>
    
   a. **to_station**: holds the id of the next station.
   b. *stack_node* **∗next**: holds the pointer of the next node in the stack.
   
</p></details>

---

<details><summary> Stack (struct stack). Holds the nodes in FILO structure. Used for the traceback (to get the shortest path). </summary><p>
    
   a. *stack_node* **∗head**: holds the pointer to the first node in the stack.
 
</p></details>

---

<br />

## Initialisation: 

1. an array of station is created, with the size defined in **STATION_NUMBER** (271), and the size of the array of links (in station) is defined in **MAX_STATION_LINKS** (10).

2. The file *stations_db.csv* is opened in read. It's used to retrieve data about each station and it's links.   
    a. Each station's from_station is initially -1.  
    b. Link's to_station are given the value -1.  
    c. If the link doesn't exist, then it's to_station defaults to -2.

3. After all the stations have been initialised, then the links will find their true to_station id's values.

<br />
  
  
## Program:

1. Gets user input & verifies that the station names given are valid.

2. Runs the dijkstra algorithm, arguments given are *stations (pointer to the array of all the stations), from_station, target_station*:

    **i.** Initialise the priority queue.
    
    **ii.** Gets the links for the station with the id in from_station & puts them in the priority queue, unless:
        
      - [x] if the station has it's links_exhausted flag set to true (links are already in the queue, no need for duplicates), then it breaks the loop.
      
      - [x] if the station's current link's to_station has a value of -2, which indicates that it has no more links in the array, then it breaks the loop.
      
      - [x] if the link's to_station has it's links exhausted (stops the to_station from putting links), then we go to the next iteration.
    
    **iii.** Priority queue organises the nodes in order based on time (shortest time in front, longest at the back).
    
    **iv.** Pop a value out of the priority queue and updates the station's values if:
    
      - [x] if the node value is not NULL (no more values in the priority queue).
      
      - [x] if the path that is introduced is faster than the one that it already has, or if it has no path to it yet.
    
    **v.** Assign the to_station's id of this path to from_station and repeat from step ii. until from_station is equal to the target_station.
    
3. Traceback the path by:

      **i.** Initialise the stack.

    **ii.** Place the target_station id into station variable.
    
    **iii.** Place station variable into a stack_node and place the stack_node into the stack.
    
    **iv.** Find the from_station id of the station referenced in the station variable and place into the station variable.
    
    **v.**  Repeat from iii. if the station variable doesn't have a value of -1 (only the starting station's from_station will have that value in the path).

4. output the path using the data in stack.
      

Features:
------
- [x] 270+ stations available to choose from.

- [x] Finds the shortest path between 2 stations.



    



