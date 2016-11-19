Chess AI
==========
Game Assignment #4<br>
Time-Limited Iterative-Deepening Depth-Limited Minimax with Alpha-Beta Pruning, History Table, and Quiescense Searching

####Author
Stuart Miller<br>
Missouri University of Science & Technology<br>
CS 5400: Artificial Intelligence<br>

####Configuration
Modifiable parameters can be accessed in ```games/chess/conf/chess.cfg```

####Modified Files
The following files were modified or added as part of this assignment

* ```games/chess/state.cpp```
* ```games/chess/state.h```
* ```games/chess/minimax.cpp```
* ```games/chess/minimax.h```
* ```games/chess/ai.cpp```
* ```games/chess/ai.h```
* ```games/chess/globals.h```
* ```games/chess/globals.cpp```
* ```games/chess/conf/*```
* ```games/chess/fathom/*```
* ```games/chess/endgametables/*```

####A Note on EndGame Tables
This AI makes use of basil00's Fathom Syzygy tablebase probing tool. Fathom is open-source code that has been modified to integrate into my existing AI for probing endgame tables. It is currently a work in progress and will most sometimes either fail or make a stupid move right now. The tables are stored in games/chess/endgametables, however I have removed them so that my submission is not obscenely huge (and doubtless over the max size limit for the class assignment).

https://github.com/basil00/Fathom