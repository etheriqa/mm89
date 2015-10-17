MM89 MazeFixing
===============

https://community.topcoder.com/tc?module=MatchDetails&rd=16494

Terminology
-----------

- Subpath
    - Leading subpath
        - `[E.][RLUS]+`
    - Trailing subpath
        - `[RLUS]+[E.]`
    - Complete subpath
        - `[E.][RLUS]+[E.]`

Strategy
--------

- Simulated annealing
- Choose a neighbour state by connecting a leading subpath with a trailing subpath
