<h1 class="code-line" data-line-start=0 data-line-end=1 ><a id="Transport_for_London_Station_Pathfinder_0"></a>Transport for London Station Pathfinder</h1>
<hr>
<h2 class="code-line" data-line-start=2 data-line-end=3 ><a id="v10__01112020_2"></a>v1.0 @ 01/11/2020</h2>
<p class="has-line-data" data-line-start="3" data-line-end="4"><img src="https://i.ibb.co/D1jGdsm/tfl-logo3.png" alt="picture alt" title="TFL"></p>
<p class="has-line-data" data-line-start="5" data-line-end="6">The program utilises the tube station data set from the <strong>Freedom of Information</strong> request found <a href="https://www.whatdotheyknow.com/request/distance_between_adjacent_underg#incoming-5516">here</a> (can be downloaded via this <a href="https://www.whatdotheyknow.com/request/1779/response/5516/attach/3/Inter%20station%20database.xls?cookie_passthrough=1">link</a>)</p>
<h1 class="code-line" data-line-start=7 data-line-end=8 ><a id="How_it_works_7"></a>How it works:</h1>
<p class="has-line-data" data-line-start="8" data-line-end="12">Firstly, the dataset has been manipulated by:<br>
<input type="checkbox" id="checkbox7971" checked="true"><label for="checkbox7971">Sorting the rows based on the Station From (A), as well as deleting the first row.</label><br>
<input type="checkbox" id="checkbox7970" checked="true"><label for="checkbox7970">Ensured that stations fall under the same name even if they are the same station. e.g. Euston (CX) → Euston.</label><br>
<input type="checkbox" id="checkbox7969" checked="true"><label for="checkbox7969">Ensured that stations don’t have a space after their name. e.g. &quot;Baker Street &quot; → “Baker Street”.</label></p>
<p class="has-line-data" data-line-start="15" data-line-end="16">Secondly, the data that was retrieved were placed in structs. These are the following ideas:</p>
<p class="has-line-data" data-line-start="17" data-line-end="18">Structures:</p>
<ol>
<li class="has-line-data" data-line-start="19" data-line-end="29">
<p class="has-line-data" data-line-start="19" data-line-end="28">Station (struct): Holds the data about the station:<br>
a.     <strong>id</strong>: The index that station has in the array.<br>
b. <strong>name</strong>: Holds the name of the station.<br>
c.    <strong>links_exhausted</strong>: Flag (1 or 0), that indicates whether or not the links from the stations are already in the priority queue.<br>
d.    <strong>links</strong>: An array of type link that holds data about each link from the station.<br>
… // The following is data about the path that the dijkstra algorithm uses.<br>
e. <strong>from_station</strong>: The shortest path from the “starting station” to this station, comes from this station. (starting  … → from_station → id)<br>
f.   <strong>from_line</strong>: Holds the string of the name of line.<br>
g.  <strong>time</strong>: Holds the time taken to reach this station.</p>
</li>
<li class="has-line-data" data-line-start="29" data-line-end="36">
<p class="has-line-data" data-line-start="29" data-line-end="36">Link (struct). Holds the data about the link between each station.<br>
a.    <strong>to_station</strong> If it holds is -2 then the link doesn’t exist. if it’s -1 means the to_station (id) still needs to be found.<br>
b.    <strong>time</strong>: Holds the time it takes to travel from the station holding the link to the to_station.<br>
c.    <strong>distance</strong>: Holds the distance between the from the station holding the link and the to_station.<br>
d.    <strong>line</strong>: Holds the name of the line that this link uses.<br>
e.    <strong>destination</strong>: Holds the name of the to_station. Used to fetch the to_station after all the stations have retrieved the required data.<br>
f.    <strong>direction</strong>: Direction of where the train is going (Northbound, Southbound, Eastbound, Westbound etc.)</p>
</li>
</ol>
<p class="has-line-data" data-line-start="40" data-line-end="42"><input type="checkbox" id="checkbox7972" checked="true"><label for="checkbox7972">Stations are unique, and so we can assign a numerical value to each station, which is also it’s index.</label><br>
[x]</p>
<ol start="2">
<li class="has-line-data" data-line-start="42" data-line-end="44"></li>
</ol>
<h2 class="code-line" data-line-start=44 data-line-end=46 ><a id="Features_44"></a>Features:</h2>
<p class="has-line-data" data-line-start="46" data-line-end="48"><input type="checkbox" id="checkbox7974" checked="true"><label for="checkbox7974">270+ stations available to choose from.</label><br>
<input type="checkbox" id="checkbox7973" checked="true"><label for="checkbox7973">Finds the shortest path between 2 stations.</label></p>
