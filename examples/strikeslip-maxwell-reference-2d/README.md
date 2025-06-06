# Examples: 2D Strike-Slip Fault using Maxwell rheology

This suite of examples is just a set of debugging simulations to test the behavior of the Maxwell rheology.
In addition, we generate a reference model to compare with different time stepping and adaptive time
stepping algorithms.

## Step 1: Static Coseismic Slip with elastic rheology.

This example is a simple static slip example similar to strikeslip-2d/step01. It was used to find out
why the original solution was taking so many iterations to converge. This example takes too long
to converge when using a refinement level of 3.

To run the example:
```bash
# Simple elastic problem
pylith step01_slip_elastic.cfg
```

## Step 2: Static Coseismic Slip with Maxwell rheology.

This example is exactly like Step 1 above but with a Maxwell rheology. It is used to make sure
the number of iterations is similar to the elastic solution.

To run the example:
```bash
# Simple viscoelastic problem
pylith step02_slip_maxwell.cfg
```

## Step 3: Static Coseismic Slip with Maxwell rheology and time steps.

This example is exactly like Step 2 above but we now run the simulation for 100 years. We use
backward Euler time stepping with a fixed step size of 1 year. Similar to the simulations above,
this simulation takes too many iterations to converge on the first time step.

To run the example:
```bash
# Simple viscoelastic problem
pylith step03_slip_maxwell.cfg
```

## Step 4: Static Coseismic Slip with elastic rheology, different normalization.

This example is exactly like Step 1, but we use different normalization for the shear modulus
and length scale. The normalizations used provide much better convergence in the linear solution.

To run the example:
```bash
# Simple elastic problem
pylith step04_slip_elastic.cfg
```

Further notes on Step 4:
The normalization factor for shear modulus should be 2.25 GPa for a Vs of 3.0 km/s;
however, using this value requires 118 KSP iterations to converge. Below are listed some
normalization facts and the resulting number of KSP iterations:

Factor (GPa)  Iterations
30              137
2.25            118
1.50            105
1.00            64
0.90            57
0.50            47
0.40            46

Any values below 0.30 GPa start increasing the number of iterations again.

## Step 5: Static Coseismic Slip with Maxwell rheology, time steps, and 2nd order.

This example is exactly like Step 3, but we use 2nd order for the displacement solution.
This model was our original reference model for further viscoelastic time stepping tests.
Note that for the output we provide output at one level higher to capture the higher accuracy
of second order, and we use a different name to indicate this is a reference model.

To run the example:
```bash
# Simple Maxwell  problem
pylith step05_slip_maxwell.cfg
```

## Step 6: Refinement level 2, 2nd order, Crank-Nicholson, adaptive.

This example is the first in a set of simulations where we try different time stepping schemes with
adaptive time stepping. All of these simulations use a refinement level of 2, and displacement basis order
of 2. They are meant to be compared to the higher resolution reference model (step05 or step15). This one uses
Crank-Nicholson with adaptive time stepping.

To run the example:
```bash
# Simple Maxwell  problem
pylith step06_slip_maxwell.cfg
```

## Step 7: Refinement level 2, 2nd order, BDF order 1, adaptive.

This example is the second in a set of simulations where we try different time stepping schemes with
adaptive time stepping. All of these simulations use a refinement level of 2, and displacement basis order
of 2. They are meant to be compared to the higher resolution reference model (step05 or step15). This one uses
BDF order 1 with adaptive time stepping.

To run the example:
```bash
# Simple Maxwell  problem
pylith step07_slip_maxwell.cfg
```

## Step 8: Refinement level 2, 2nd order, BDF order 2, adaptive.

This example is the third in a set of simulations where we try different time stepping schemes with
adaptive time stepping. All of these simulations use a refinement level of 2, and displacement basis order
of 2. They are meant to be compared to the higher resolution reference model (step05 or step15). This one uses
BDF order 2 with adaptive time stepping.

To run the example:
```bash
# Simple Maxwell  problem
pylith step08_slip_maxwell.cfg
```

## Step 9: Refinement level 2, 2nd order, BDF order 3, adaptive.

This example is the 4th in a set of simulations where we try different time stepping schemes with
adaptive time stepping. All of these simulations use a refinement level of 2, and displacement basis order
of 2. They are meant to be compared to the higher resolution reference model (step05 or step15). This one uses
BDF order 3 with adaptive time stepping.

To run the example:
```bash
# Simple Maxwell  problem
pylith step09_slip_maxwell.cfg
```

## Step 10: Refinement level 2, 2nd order, BDF order 4, adaptive.

This example is the 5th in a set of simulations where we try different time stepping schemes with
adaptive time stepping. All of these simulations use a refinement level of 2, and displacement basis order
of 2. They are meant to be compared to the higher resolution reference model (step05 or step15). This one uses
BDF order 4 with adaptive time stepping.

To run the example:
```bash
# Simple Maxwell  problem
pylith step10_slip_maxwell.cfg
```

## Step 11: Refinement level 2, 2nd order, BDF order 5, adaptive.

This example is the 6th in a set of simulations where we try different time stepping schemes with
adaptive time stepping. All of these simulations use a refinement level of 2, and displacement basis order
of 2. They are meant to be compared to the higher resolution reference model (step05 or step15). This one uses
BDF order 5 with adaptive time stepping.

To run the example:
```bash
# Simple Maxwell  problem
pylith step11_slip_maxwell.cfg
```

## Step 12: Refinement level 2, 2nd order, BDF order 6, adaptive.

This example is the 7th in a set of simulations where we try different time stepping schemes with
adaptive time stepping. All of these simulations use a refinement level of 2, and displacement basis order
of 2. They are meant to be compared to the higher resolution reference model (step05 or step15). This one uses
BDF order 6 with adaptive time stepping.

To run the example:
```bash
# Simple Maxwell  problem
pylith step12_slip_maxwell.cfg
```

## Step 13: Refinement level 2, 2nd order, backwards Euler, adaptive.

This example is the 8th in a set of simulations where we try different time stepping schemes with
adaptive time stepping. All of these simulations use a refinement level of 2, and displacement basis order
of 2. They are meant to be compared to the higher resolution reference model (step05 or step15). This one uses
backward Euler with adaptive time stepping.

To run the example:
```bash
# Simple Maxwell  problem
pylith step13_slip_maxwell.cfg
```

## Step 14: Static Coseismic Slip with Maxwell rheology, small fixed time steps, and 2nd order.

This example is exactly like Step 5, but we use a much smaller fixed size time step of 0.25 year.
This model was one candidate for our reference model, but the model from Step 15 below seems to
provide a better solution. Note that for the output we provide output at one level
higher to capture the higher accuracy of second order, and we use a different name to indicate
this is a reference model.

To run the example:
```bash
# Simple Maxwell  problem
pylith step14_slip_maxwell.cfg
```

## Step 15: Static Coseismic Slip with Maxwell rheology, modified adaptive time stepping, and 2nd order.

This example is similar to Step 14 above, but we use adaptive time stepping, combined with a reduced safety
factor for the adaptive stepping (from the 0.9 default to 0.2), and a smaller initial time step size (0.05 year).
This means that the time step sizes at the beginning of the solution are very small (when deformation
is largest), thus providing a much more accurate solution. This model is our current reference model.
Note that for the output we provide output at one level higher to capture the higher accuracy of second order,
and we use a different name to indicate this is a reference model.

To run the example:
```bash
# Simple Maxwell  problem
pylith step15_slip_maxwell.cfg
```
