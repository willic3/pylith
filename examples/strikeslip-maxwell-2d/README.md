# Examples: 2D Strike-Slip Fault using Maxwell rheology

This suite of examples is just a set of debugging simulations to test the behavior of the Maxwell rheology.

## Step 1: Static Coseismic Slip with elastic rheology.

This example is a simple static slip example similar to strikeslip-2d/step01. It was used to find out
why the original solution was taking so many iterations to converge. The issue turned out to be
incorrect normalization for the shear modulus. The mesh is refined by a factor of 3 before running.

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

This example is exactly like Step 2 above but we now run the simulation for 150 years.

To run the example:

To run the example:
```bash
# Simple viscoelastic problem
pylith step03_slip_maxwell.cfg
```

## Step 4: Static Coseismic Slip with elastic rheology, properties assigned in .cfg file.

This example is exactly like Step 1, but materials are assigned in the .cfg file. It was used
to test different properties and normalization. It turns out that default normalization for
shear modulus works fine for a Vs of 4.24 km/s; however, it does not work for a Vs of 3.0 km/s,
and it still does not work if we set the normalization factor to the shear modulus of the problem.

To run the example:

To run the example:
```bash
# Simple elastic problem
pylith step04_slip_elastic.cfg
```
