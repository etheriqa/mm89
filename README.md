MM89 MazeFixing
===============

http://community.topcoder.com/longcontest/stats/?module=ViewOverview&rd=16494

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
