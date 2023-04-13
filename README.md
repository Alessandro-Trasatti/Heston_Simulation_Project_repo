# Efficient Heston Simulation

## Goal of the project
We are going to be dealing with one of the most popular stochstic volatility models (*SV models*), the Heston model. We recall that the equation of the model under $\mathbb{P}$, *a risk neautral measure*, are

$\frac{dS_t}{S_t} = r dt + \sqrt{V_t}dW_t^S,$

where

$dV_t = k(\theta - V_t)dt + \sigma_V \sqrt{V_t} dW_t^V,$

and

$d < W^S, W^V>_t = \rho dt$

for $t \in [0,T]$, where
- $r \geq 0$ is the interest rate.
- $\{V_t\}_{t \in [0,T]}$ is the *variance process.*
- $k \geq 0$ is the *mean reversion speed*.
- $\theta \geq 0$ the *mean reversion level* (mrL) or the *long term variance*.
- $\sigma_V \geq 0$ is the *volatility of volatility* (vol of vol).
- $\rho \in [-1,1]$ is the correlation between the Brownian motions $W^S$ and $W^V$.

Hence, the parameters of the model are five, and are
$$k, \theta, \sigma_V, \rho, V_0.$$
**The main goal is to price European options using the Heston model**.
To this purpose, there are different approaches
<ol>
  <li>The semi-analytic method (exam 2021-22).</li>
  <li>Monte-Carlo simulation
    <ol>
      <li>Euler scheme (exam 2021-22) ~ not recommended and not requested to implement;</li>
      <li>Efficient Monte-Carlo simulation (Andersen)</li>
    </ol>
  </li>
</ol>

Once this is done, the task is to compare the price given by the semi-analytic method and by the Monte Carlo simulation. More specifically, we want to show that as $N$, the number of simulations increases, the price given by the Monte Carlo simulation converges to the price given by the semi-analytic method **for some choices of the parameters and for different strikes, $K$, and expiration dates, $T$**.
### Efficient Monte Carlo Simulation
Following Andersen's paper, the **Broadie-Kaya** scheme will be implemented. To discretize the variance process we will use two different methods
<ol>
    <li>The TG method.</li>
    <li>The QE method.</li>
</ol>

## Directives
It has to be a homework of 2-3 pages and the code **well commented**. The more explicit the better. In the homework the macrostructure of the code has to be described (for instance the classes used and their purpose, why some of them will be abstract etc.). The idea is to do this at most on one page. In the other two pages, we present the results found with the aid of plots as well and all the tests performed (for example for different choices of the model parameters).

**If the code works well**, extra-points will be awarded to those who will do further tests on the sensitivity of the price with respect to model parameters (for instance its behaviour with respect to $\rho$ or with respect to $\sigma_V$ etc).

## Structure of the code
In our project we use a number of classes:
- <code>HestonModel</code>
- <code>PathSimulator</code>
- Continue list;

### Class <code>HestonModel</code>
**Purpose of the class**: 
The class has the objective to reproduce in code the HestonModel;

**Structure of the class**:
In the <code>public</code> part it presents the customary parameter constructor as well as the getter methods. Moreover we have methods that code the drift term and diffusion term both for the underlying asset process $\{S_t\}_t$ and the varaince process $\{V_t\}_t$, defined as <code>const</code> methods as they are not modifying the current object.
In the <code>private</code> part we can find the attributes that code
$$k, \theta, \sigma_V, \rho, S_0, V_0, r.$$

### Class <code>PathSimulator</code>
**Purpose of the class**: