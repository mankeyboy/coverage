This code repository is for coverrage of a given environment with obstacles of known size, using Centroidal Voronoi Tessellation.
This subscribes the /obstacleList, and publishes the best positions according to the given environment on topic /coverage.

To run the code, prerequisites are :
* The swarm_simulator repository [1], should be set up on the system.

To run the code following commands (on separate terminals): <\br>
$ roslaunch swarm_simulator swarm.launch <\br>
$ rosrun swarm_simulator swarm_simulator_node <\br>
$ rosrun coverage voronoi_main [number of agents] 
