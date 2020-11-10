# Transport for London Station Pathfinder #
---
## v1.0 @ 01/11/2020
![TFL](https://i.ibb.co/D1jGdsm/tfl-logo3.png "TFL")

This program utilises dijkstra's algorithm to calculate the __fastest__ path between two stations that were given by the user. It utilises the tube station data set from the __Freedom of Information__ request found. [here](https://www.whatdotheyknow.com/request/distance_between_adjacent_underg#incoming-5516) (can be downloaded directly via this [link](https://www.whatdotheyknow.com/request/1779/response/5516/attach/3/Inter%20station%20database.xls?cookie_passthrough=1)).

![Moorgate to Swiss Cottage](https://i.ibb.co/Y3rwcsy/moor-to-swiss-YZ0.gif "Moorgate to Swiss Cottage")


## Compiling the code
cd into the main directory & use gcc to compile the code. The executable should appear in the main directory.
```sh
$ cd tfl_project-main
$ gcc -o tfl src/main.c
```
![Compiling](https://i.ibb.co/Mf6MV55/compiling.gif "Compiling")

## Running the executable
the executable expects a relative path to the station's database, which is "data/stations_db.csv". If it doesn't exist, it'll print an error and exit.
Run the executable.
```sh
$ tfl.exe
```

### Options
(1) Finds the fastest path between two stations. It will ask for a starting station name & a target station name.

![Shortest Path](https://i.ibb.co/r3KMtZC/shortest-path.gif "Dijkstra's Algorithm")

(2) Outputs the names of the stations starting with a certain character. It will ask for a character to find the required stations.

![Displaying Stations](https://i.ibb.co/VTnssK6/displaying-stations.gif "Displaying Stations")

(3) Adjust line switch time, which is the time given if the user needs to switch lines. It will ask for a value between 0 and 30 inclusive.

![Changing Time](https://i.ibb.co/nPDt3n5/time-switch.gif "Change Switch Times")

(4) Exit the program.

---

## Dataset Manipulation:
Firstly, the dataset has been manipulated by:

- [x] Sorting the rows in alphabetical order (A → Z) based on the Station From (A) column,   as well as deleting the first row.  
- [x] Ensuring that stations fall under the same name even if they are the same station. e.g. Euston (CX) → Euston.  
- [x] Ensuring that stations don't have a space after their name. e.g. "Baker Street " → "Baker Street".  
- [x] Deleting the "AM Peak" & "Inter Peak" Columns.  
- [x] Converting the file into a .csv to allow the file to be read more easily.  

---
   
References: [Dijkstra's Algorithm - Computerphile](https://www.youtube.com/watch?v=GazC3A4OQTE&ab_channel=Computerphile)  
README File created using [Dillinger](https://dillinger.io/)
