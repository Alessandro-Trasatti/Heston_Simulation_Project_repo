# Efficient Heston Simulation

## Goal of the project
We are going to be dealing with one of the most popular stochastic volatility models (*SV models*), the Heston model. We recall that the equation of the model under $\mathbb{P}$, *a risk neautral measure*, are

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
Following Andersen's paper, the **Broadie-Kaya** and the **Truncated Euler** schemes will be implemented. To discretize the variance process we will use two different methods
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

**MonteCarlo method**
- <code>PathSimulator</code>
- <code>EulerPathSimulator</code>
- <code>Payoff</code>
- <code>CALL_PUT</code>
- <code>EuropeanOptionPayoff</code>
- <code>MCPricer</code>

**Semi analytic method**
- <code>Complex </code>
- <code>GaussLegendreQuadrature </code>
- <code>HestonPricer </code>
- <code>AnalyticalHestonPricer </code>

### Class <code>HestonModel</code>
**Purpose of the class**: 
The class has the objective to reproduce in code the HestonModel;

**Structure of the class**:
In the <code>public</code> part it presents the customary parameter constructor as well as the getter methods. Moreover we have methods that code the drift term and diffusion term both for the underlying asset process $\{S_t\}_t$ and the varaince process $\{V_t\}_t$, defined as <code>const</code> methods as they are not modifying the current object, in those methods we throw an exception if the variance is negative.
In the <code>private</code> part we can find the attributes that code
$$k, \theta, \sigma_V, \rho, S_0, V_0, r.$$


Below we describe the classes used in the implementation of the MonteCarlo method for calculating the price of Call and Put options
### Class <code>PathSimulator</code>
**Purpose of the class**:

The PathSimulator class is meant to give a general class with all the methods and attributs needed to deal with numerical schemes. It is an abstract class as it contains the clone method which is virtual pure, which is not restrictive as one must necessarily choose a particular scheme before doing anything else. Each specific scheme is meant to be a derived class of this one.

**Structure of the class**:
It contains as attributs a model <code>HestonModel _model</code>, systematically a HestonModel in our case for the sake of simplicity. It contains also a temporal grid <code>Vector _time_points</code>. Its methods allow us to get trajectories of the underlying, for instance:
-the method <code> virtual Vector next_step </code> codes the way with which we move to the next step of a general (virtual) discretization scheme - returns the variance and the spot.
-the method <code>Matrix path() const</code>, return one trajectory of the asset price and of the variance.

### Class <code>EulerPathSimulatorModified</code>
**Purpose of the class**:
Derived class from the <code>PathSimulator</code> one, meant to implement a general Truncated Euler Scheme for both the variance and the asset, for instance for the Heston Model it reads:

$$S_{t_{k} + \Delta t} = S_{t_{k}} \, \left( r \Delta t + \sqrt{\max(V_{t_k},0)} \sqrt{\Delta_t} Z_S \right)$$

$$V_{t_{k} + \Delta t} = k(\theta - \max(V_{t_k},0)) \Delta t + \sigma_V \sqrt{\max(V_{t_k},0)} \sqrt{\Delta_t} Z_V$$

Where $Z_S,Z_V$ are two corrolated centered reduced gaussians, in pratical one can set: 

$$ Z_S = \rho Z_V + \sqrt{1 - \rho^2} Z_{V}^{\perp} $$

where $Z^{\perp}_V$ is a centered reduced gaussian independant of $Z_V$.

The term "truncated" comes from the fact that we truncate the variance for zero when it becomes negative. In our case the truncation is done in the Class <code> HestonModel </code>, as precised above. However for the sake of numerical stability, the article advises to use the Euler Scheme on the logarithm of the asset price with the further difference that the variance is truncated to zero, to ensure that the variance process never goes below zero, hence applying Ito we use the following scheme:

$$ X_{t_{k} + \Delta t} = X_{t_k} -\dfrac{1}{2} \max(V_{t_{k}},0) \Delta t + \sqrt{\max(V_{t_{k}},0)} \sqrt{\Delta_t} Z_X$$

$$V_{t_{k} + \Delta t} = k(\theta - \max(V_{t_k},0)) \Delta t + \sigma_V \sqrt{\max(V_{t_k},0)} \sqrt{\Delta_t} Z_V$$

with $X_t = ln(S_t)$, and where again $Z_X,Z_V$ are two corrolated centered reduced gaussians, in pratical one can set: 

$$ Z_X = \rho Z_V + \sqrt{1 - \rho^2} Z_{V}^{\perp} $$

where $Z^{\perp}_V$ is a centered reduced gaussian independant of $Z_V$.

**Structure of the class**:
Two constructors and one clone are implemented as public methods. The relevant method is the private <code>Vector next_step(const size_t& time_idx, const double& asset_price, const double &variance) const override</code> that implements one iteration from the instant <code>time_idx</code> to the next one one the grid, see Scheme above.

### Class <code> BroadieKaya </code>
**Purpose of the class**:
Derived class from the <code>PathSimulator</code> one, this class is meant to implement a BroadieKaya scheme. It will be used only for the HestonModel. In that case the scheme for the log asset is the following:

$$ X_{t_{k} + \Delta t} = X_{t_{k}} + \dfrac{\rho}{\sigma_{V}} (V_{t_k + \Delta t} - V_{t_{k}} - \kappa \theta \Delta _t) + $$

$$ + (\dfrac{\kappa \rho}{\sigma_{V}} -\dfrac{1}{2}) \int\limits_{t}^{t+\Delta t} V(u) du + \sqrt{1 - \rho^2} \int\limits_{t}^{t+\Delta t} \sqrt{V(u)} dW(u) $$

and we will implement two schemes in order to compute $V_{t_k+\Delta t}$ knowing $V_{t_{k}}$, the *TG*, truncated gaussian and *QE*, quadratic exponential. **to be detailed**

**Structure of the class**:
On the contrary of the previous class <code> EulerPathSimulatorModified </code> we now have divided the step from $t$ to $t + \Delta t$ in few private methods:
- <code> truncature_gaussian </code>
- <code> quadratic_exponential </code>
- <code> next_step</code>

As explained above, truncature_gaussian and quadratic_exponential are two different ways to compute $V_{t_{k} + \Delta t}$ knowing $V_{t_{k}}$, while the method <code> next_step </code> computes the log asset price $X_{t_{k} + \Delta t}$ knowing $X_{t_{k}}, V_{t_{k}}$ and $V_{t_{k} + \Delta t}$. The class contains also an attribute <code> MathTools _tools </code> whose methods contain all the mathematical tools needed (see class below)

### Class <code> MathTools </code>

**Purpose of the class**:
This class is meant to be a toolbox of all the algorithms, and mathematical functions one needs in the methods to carry out the numerical schemes.

**Structure of the class**:
The class contains the following methods, all public:
- <code> normalCDF(double x) </code> Standard normal distribution function
- <code> normalPDF(double x) </code> Standard normal density function
- <code> eq_r(double r, double psi) </code> eq_r is the function f such that f(r) = 0, satisfied by r a parameter needed in the method <code> BroadieKaya::truncature_gaussian</code>
- <code> secantMethod(int n_iterations, double psi, std::function<double(double, double)> func, double precision = 0.01)</code> this function takes in input another function thanks to the package <code> functional </code> of <code> std </code> and carry out the secant root search algorithm. The algorithm stops either if we have reached the maximal number of iterations, argument <code>int n_iterations</code> or if the difference between two consecutives terms is low enough, argument <code>double precision</code>.
- <code> double trapezoidalMethod(double previous_x, double next_x, double delta, double gamma_1 = 0.5) const </code> This function approximates $\int\limits_{t}^{t+\Delta} V_s ds$ by $\Delta \left( \gamma_1 V_{t} + (1 - \gamma_1) V_{t + \Delta}\right)$ where $\gamma_1 \in \[0,1]$.
- <code> double WinerIntegral(double previous_x, double next_x, double delta, double gamma_1 = 0.5) const </code> This function approximates $\int\limits_{t}^{t+\Delta} \sqrt{V_s} dW_s$ with $V_s$ considered as non negative and deterministic. It returns: $\sqrt{\Delta} * Z * \sqrt{ \gamma_1 * V_{t} + (1 - \gamma_1) * V_{t+\Delta}}$ with $Z \sim \mathbf{N}(0,1)$

### Class <code>Payoff</code>
**Purpose of the class**:

The purpose of this abstract class is to specify the payoff of an option, this class is meant to be inherited from, and the derived class will contain more informations about the option chosen.

**Structure of the class**:
This class contains only:
-<code> virtual Payoff* clone() const = 0 </code>
-<code> virtual double payoff(const Matrix& path) const = 0 </code>
 
The clone method is always a pure virtual method, the second is virtual pure as well because one must knows the payoff precisely and hence re implement it in the derived classes.

### Enum Class <code>CALL_PUT </code>
**Purpose of the class**:
This enum class is meant to be used in the next class <code> EuropeanOptionPayoff </code> to indicate if a vanilla option is a call or a put.

### Class <code>EuropeanOptionPayoff</code>
**Purpose of the class**:
This class is a derived class from <code>Payoff</code>, it gives all the informations needed to compute the price of an European option.

**Structure of the class**:
This class implements the virtual pure method <code>payoff</code> and also contains two attributes:
- CALL_PUT _call_put;
- double _strike;

### Class <code>MCPricer</code>
**Purpose of the class**:
The purpose of this class is to give the price of an option, with the relevant parameters as attributes.

**Structure of the class**:
The class contains the regular public methods (constructors, destructor, assignement operator) and also the method <code> double price() const</code> which returns the price of the option. Everything that is relevant for the pricing is set as private attributes:
- <code> size_t _number_sims </code>
- <code> const Payoff* _payoff </code>
- <code> const PathSimulator* _pathSimulator </code>
- <code> double _risk_free_rate </code>


Now we explain the classes implemented in the code to obtain the price of Call and Put option using the semi analytic formula. We based our code on the exam of 2021/2022, where we completed the missing parts.

Let us remember that the task is to compute the price of a Call option using the following formula

$$C(S_0, V_0, K) = S_0 P_1 - K e^{-rT} P_2.  $$

For $i=1, 2 $, $P_i$ is given by 

$$P_i = \frac{1}{2} + \frac{1}{2 \pi} \int_{\omega = -\infty}^{\infty} \mathcal{R} \left[ \frac{\phi_i(T, ln(S_0), V_0, \omega) e^{-j \omega ln(K)}}{j \omega} \right] d \omega,$$

where $\mathcal{R}(z)$ is the real part of a complex number $z$. The explicit expression of the other terms of the formula are given in the exam 2021/2022 which we enclose with the project.

### Class <code>Complex</code>

**Purpose of the class:**
As we work with complex numbers we need a class that simulate a complex number and its properties, in fact the main attributes of this class are represented by the variable real part and the variable imaginary part. Moreover there are methods to compute properties like module, argument and methods able to simulate the operations with another complex number. 

**Structure of the class:**
In addition to getters and the constructor with parameters, there are the methods to simulate an operation between two complex numbers like: 
- <code> Complex operator+(const Complex& complex) </code>
- <code> Complex operator-(const Complex& complex) </code>
- <code> Complex operator*(const Complex& complex) </code>
- <code> Complex operator/(const Complex& complex) </code>


There are also three static methods that simulate an exponential, logarithmic or square root transformation of a complex number:
- <code> static Complex exponential(const Complex& complex) </code>
- <code> static Complex logarithm(const Complex& complex) </code>
- <code> static Complex square_root(const Complex& complex) </code>


Using "static" before a method means that this method is a class method and not a method of the object. So, this has not access to the pointer this. It works like a global method.


### Class <code>GaussLegendreQuadrature</code>

**Purpose of the class:**
We have seen that to compute the Call option we need to compute an integral. So, we need a class able to approximate this integral. Obviously, there are several ways to approximate an integral, but in our case we use the Gauss-Legendre quadrature formulas.
For an integer $ n > 2$ and for all function $g$ defined on $[-1,  1]$, we can approximate his integral on this interval by a linear combination of weights multiplying $g$ valued in some points $(x_i)_{i=1, \dots, n}$:

$$\int_{-1}^{1} g(x) dx \approx \sum_{i=1}^{n} w_i g(x_i),$$

where the weights $(w_i)_{i=1, \dots,n}$ and the abscissas $(x_i)_{i=1, \dots,n}$ are uniquely defined by the value of $n$.
In our case we want to integrate a function defined on the interval $(-\infty, +\infty)$.

$$\int_{-\infty}^{+\infty} f(\omega) d \omega.$$

Via a change of variable, $\omega = \frac{x}{1 - x^2}$, we obtain

$$\int_{-\infty}^{+\infty} f(\omega) d \omega = \int_{-1}^{1} g(x) dx \approx \sum_{i=1}^{n} w_i g(x_i), $$

where 

$$ g(x) = f(\frac{x}{1 - x^2}) \frac{1 + x^2}{(1- x^2)^2}.$$

**Structure of the class:**

We have three different attributes:
- <code> size_t _number_sample_points </code> (it is the inteher $n$)
- <code> Vector _points </code> (vector of  $(x_i)_{i=1, \dots, n}$)
- <code> Vector _weights </code> (vector of $(w_i)_{i=1, \dots,n}$)
 
And one public method:
- <code> double integrate(std :: function<double(double)> func) </code>
where we implement the formula for the integral above.

### Class HestonPricer</code>

**Purpose of the class:**
In this class we implement the analytical procedure to compute the price of a Call option where the underlying follow a dynamic given by the Heston model. As the price of a Call option can be computed in different ways (in our case with MonteCalro and with an analitycal method) this is a general class that provides for the implementation of subclasses, each for a different price calculation method.

**Structure of the class:**

Three attributes:
- <code> HestonModel _model </code> (our asset follow the dynamic given by the Heston model)
- <code> double _strike </code> (strike $K$ of the option)
- <code> double _maturity </code> (maturity $T$ of the option)

Methods (all public):
- <code> HestonPricer(const HestonModel& model, const double& strike, const double& maturity) </code> (constructor with parameters)
- <code> virtual HestonPricer* clone() const = 0 </code> (the clone method, set like that because of the presence of subclasses)
- <code> virtual double price() const = 0 </code> (virtual method for the computation of the price, we will have a different implementation depending on the subclass)


### Class AnalyticalHestonPricer</code>

This is a subclass of the class HestonPricer described above. We based the implementation of this class using the exam 2021</2022about the pricing under the Heston model.

**Purpose of the class:**
The task is to compute the price of a Call option using the analytical formula explained before (the which one with the integrals). 

**Structure of the class:**
In order not to lengthen the discussion too much, we refer to the exam of 2021/2022 for a complete description of this class. We observe that we have only one attribute:
- <code> size_t _gauss_legendre_sample_size </code> that represents the integer $n$ of the Gauss-Legendre formula

Obviuosly the most important method in this class is the one given by the pricer method:
- <code> double price() </code> where we implement the analytical formula.










