# NSGA-II Portfolio Optimization
[NSGA-II](https://cs.uwlax.edu/~dmathias/cs419/readings/NSGAIIElitistMultiobjectiveGA.pdf) is a multi-objective optimization evolutionary algorithm which utilizes the method of non-dominated sorting. [Portfolio optimization](https://en.wikipedia.org/wiki/Portfolio_optimization) is a process of selecting an optimal asset distribution, focusing on maximizing expected returns and minimizing risk.

![](https://github.com/ln53491/nsga2-portfolio-optimization/blob/main/example/example.gif)

Usage
------------
**1. Basic algorithm use**
  - Edit ```input.cnf``` by setting start and end dates of portfolio data and their respective stock tickers as such:
```
START DATE
END DATE
STOCK1
STOCK2
...
```
  - In ```main.cpp``` tweak the parameters and run ```runNSGA2()```

**2. Hyperparameter grid search**
  - Put desired values for each NSGA-II hyperparameter in ```params.cnf```
  - Run ```runGridSearch()``` from ```main.cpp```
