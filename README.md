# Astar algorithm GPS

## Summary of the algorithm used for the implementation of A*
- Explanation of the algorithm (diagrama si volem)
The Astar algorithm is a graph traversal and path search algorithm for solving the routing problem. It can be applied in order to search for the shortest path between two points or nodes in a map, which is the application implemented in the following work. It is an informed search algorithm, meaning that it achieves a better performance by using heuristics to guide its search.

The overall way this algorithm operates is by maintaining a tree of paths which originates at the starting point and extends until the termination criterion is satisfied. At each iteration of its main loop, Astar needs to determine which of its paths to extend. It does so based on the cost of the path and an estimate of the cost required to extend the path all the way to the goal. For this, it selects the path that minimizes $f(v) = g(v) + h(v)$, where $v$ is the next node on the path, $g(v)$ is the cost of the path from the start node to $v$, and $h(v)$ is a heuristic function that estimates the cost of the cheapest path from $v$ to the goal node. By cheapest path 

- Pseudocode
```
procedure ASTAR(graph G, start, goal, h)
    Open <- EmptyPriorityQueue
    parent[G.order] <-uninitialized
    g[G.order] <- unitialized to infinity (important to detect the non-visited and non-enqueued nodes)

    g[start] <- 0
    parent[start] <- infinity
    Open.add_with_priority(start, g, h)

    while not Open.IsEmpty do
        current <- Open.extract_min(g, h)
        if (current is goal) then return g, parent
        for each adj in current.neighbours do
            adj_new_try_gScore <- g[current] + w(current, adj)
            if adj_new_try_gScore < g[adj] then
                parent[adj] <- current
                g[adj] <- adj_new_try_gScore
                if not OpenBelongsTo(adj) then Open.add_with_priority(adj, g, h)
                else Open.requeue_with_priority(adj, g, h)
                end if
            end if
        end for
    end while
    return failiure
end procedure
```

## Code details of the implementation

### Structure of the code
The program has three main parts:
1. **Map interpretation** 
2. **Heuristic function** 
3. **Astar algorithm**

### Map file format
The choice of file format when reading the information in the map is important when it comes to execution time. The file initially provided was in the .csv extension. Reading the map and extracting information from this type of file in the case of Andorra would take around 0.136 s. When reading it in a .bin extension, the same process would take 0.00342 s. This is a significant time-efficiency improvement, especially when considering more information-heavy maps such as Catalunya's or Spain's.

- map file choice
- structure used
    - node structure
- everything is in a main(), or we have programmed the functions

## Usage example
- Compilation instructions.
- Execution instructions.
- Example