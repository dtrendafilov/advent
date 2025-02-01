Graph of the dependencies

```mermaid
    graph LR;
        47 --> 53;
        97 --> 13;
        97 --> 61;
        97 --> 47;
        75 --> 29;
        61 --> 13;
        75 --> 53;
        29 --> 13;
        97 --> 29;
        53 --> 29;
        61 --> 53;
        97 --> 53;
        61 --> 29;
        47 --> 13;
        75 --> 47;
        97 --> 75;
        47 --> 61;
        75 --> 61;
        47 --> 29;
        75 --> 13;
        53 --> 13;
```

- DFS from node gives everything after that node

1. Reverse the graph
    - DFS from node _X_ gives everything that must be before the node - _before(X)_
2. Given _X Y_ pair, check that _Y_ is not in _before(X)_
    - this means that either _Y_ is in _after(X)_ or there are no rules for _Y_