# Transport for London Station Pathfinder #
---
## v1.0 @ 01/11/2020
![picture alt](https://i.ibb.co/D1jGdsm/tfl-logo3.png "TFL")

The program utilises the tube station data set from the __Freedom of Information__ request found [here](https://www.whatdotheyknow.com/request/distance_between_adjacent_underg#incoming-5516) (can be downloaded via this [link](https://www.whatdotheyknow.com/request/1779/response/5516/attach/3/Inter%20station%20database.xls?cookie_passthrough=1))

# How it works:
Firstly, the dataset has been manipulated by:
[x] Sorting the rows based on the Station From (A), as well as deleting the first row.
[x] Ensured that stations fall under the same name even if they are the same station. e.g. Euston (CX) → Euston.
[x] Ensured that stations don't have a space after their name. e.g. "Baker Street " → "Baker Street".



Secondly, the data that was retrieved were placed in structs. These are the following ideas:

Structures:
<details><summary> Station (struct): Holds the data about the station </summary><p>
    
    a. id: The index that station has in the array.
    
    b. name: Holds the name of the station.
    
    c. links_exhausted: Flag (1 or 0), that indicates whether or not the links from the stations are already in the priority queue.
    
    d. **links**: An array of type link that holds data about each link from the station.
    
    
    ... // The following is data about the path that the dijkstra algorithm uses.
    
    
    e. from_station: The shortest path from the "starting station" to this station, comes from this station (starting ... → from_station → id).
    
    f. from_line: Holds the string of the name of line.
    
    g. time: Holds the time taken to reach this station.
</p></details>

2. Link (struct). Holds the data about the link between each station.
i.    **to_station** If it holds is -2 then the link doesn't exist. if it's -1 means the to_station (id) still needs to be found.
ii.    **time**: Holds the time it takes to travel from the station holding the link to the to_station.
iii.    **distance**: Holds the distance between the from the station holding the link and the to_station.
iv.    **line**: Holds the name of the line that this link uses.
v.    **destination**: Holds the name of the to_station. Used to fetch the to_station after all the stations have retrieved the required data.
vi.    **direction**: Direction of where the train is going (Northbound, Southbound, Eastbound, Westbound etc.)




[x] Stations are unique, and so we can assign a numerical value to each station, which is also it's index.
[x]
2. 

Features:
-------- 
[x] 270+ stations available to choose from.
[x] Finds the shortest path between 2 stations.



    



