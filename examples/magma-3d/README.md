# Examples: 3D Magma Chamber

This example demonstrates the use of PyLith to examine the stress and deformation fields
due to a pressurized magma chamber. It also includes coarse topography for Mt. Ruapehu,
New Zealand. The first two examples are a simple elastic model with constant pressure
applied on the surface of a spherical cavity. The second two examples use viscoelastic
material for the shell surrounding the chamber (low viscosity) and the surrounding
crust (high viscosity). The final example applies fluid pressure to the chamber walls
using a poroelastic model. In this case, the spherical shell surrounding the chamber
has different poroelastic properties than the surrounding crust.

## Meshing

We provide mesh files generated using Gmsh.

## Step 1a: Pressurization of a hollow sphere, using basis order 1 for the solution.

We apply a constant normal traction of 10 MPa to the surface of the sphere, which leads
to inflation of the magma reservoir. This example uses basis and quadrature orders of
1 for the solution.

To run the example:

```bash
pylith mat_elastic.cfg step01a_cavity_elastic.cfg
```

## Step 1b: Pressurization of a hollow sphere, using basis order 2 for the solution.

This example is identical to step01a, except that we use a basis and quadrature orders of 2
for the solution.

To run the example:

```bash
pylith mat_elastic.cfg step01b_cavity_elastic.cfg
```

## Step 2a: Pressurization of a hollow sphere with a surrounding Maxwell viscoelastic shell.

For this problem, the sphere is again pressurized, but the surrounding shell and crust are
both viscoelastic material. In the shell, the viscosity is much lower than in
the surrounding crust. This example uses constant time step sizes.
**NOTE:  This example takes about 5 minutes to run.**

To run the example:

```bash
pylith mat_maxwell.cfg step02a_cavity_maxwell.cfg
```

## Step 2b: Pressurization of a hollow sphere with a surrounding Maxwell viscoelastic shell with adaptive time stepping.

This problem is identical to step02a, except that in this case we use adaptive time stepping.
This example runs for 5 years, rather than 1 year for the previous example.

To run the example:

```bash
pylith mat_maxwell.cfg step02b_cavity_maxwell.cfg
```

## Step 3: Fluid injection inside a hollow sphere with a surrounding poroelastic shell.

For this problem, time-varying fluid pressure is applied on the boundary of the spherical cavity.
The surrounding shell and crust are both poroelastic material. In the shell, the permeability
and porosity are higher than in the surrounding crust, while the Biot coefficient is lower.
A zero fluid pressure condition is also applied on the top boundary.
**NOTE:  This example takes 15-20 minutes to run.**

To run the example:

```bash
pylith mat_poroelastic.cfg step03_cavity_poroelastic.cfg
```
