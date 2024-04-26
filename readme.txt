// Project name: //
2D Cellular Automata (Game of Life)


// Descriptions: //
A cellular automaton that performs actions according to some rules. 
In some grid (looks like checkerboard), each cell has a "live" or "dead" state. 
According to some given rules, each cell will become alive or dead at the next time step. 
(The state of the cell in next step is only depends on the state of the 8 cells around it.)


// Rules For This Model //
- alive   &&    2 or 3 alive cells around it        ==>    alive.
- alive   &&    less than 2 alive cells around it   ==>    dead.
- alive   &&    more than 3 alive cells around it   ==>    dead.
- dead    &&    3 living cells around it            ==>    alive


// How To Run It In Terminal//
run life_slurm.sh
