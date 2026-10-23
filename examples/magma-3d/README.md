# Examples: 3D Magma Chamber

This example demonstrates the use of PyLith to examine the stress and deformation fields
due to a pressurized magma chamber. It also includes coarse topography for Mt. Ruapehu,
New Zealand. The first two examples are a simple elastic model with constant pressure
applied on the surface of a spherical cavity. The second two examples use poroelasticity.
In this case, the spherical shell surrounding the chamber have different poroelastic
properties than the surrounding domain.

## Meshing

We provide mesh files generated using Gmsh.

## Step 1a: Pressurization of a hollow sphere, using basis order 1 for the solution.

We apply a constant normal traction of 10 MPa to the surface of the sphere, which leads
to inflation of the magma reservoir. This example uses basis and quadrature orders of
1 for the solution.

To run the example:

```bash
pylith mat_elastic.cfg step01a_cavity_pressure.cfg
```

## Step 1b: Pressurization of a hollow sphere, using basis order 2 for the solution.

This example is identical to step01a, except that we use a basis and quadrature orders of 2
for the solution.

To run the example:

```bash
pylith mat_elastic.cfg step01b_cavity_pressure.cfg
```

## Step 2a: Pressurization of a hollow sphere with poroelasticity.

For this problem, the sphere is again pressurized, but the surrounding shell and crust are
both poroelastic material. In the shell, the porosity and permeability are higher than in
the surrounding crust, while the Biot coefficient is lower. This example uses constant
time step sizes.

To run the example:

```bash
pylith mat_poroelastic.cfg step02a_cavity_poroelastic.cfg
```

## Step 2b: Pressurization of a hollow sphere with poroelasticity.

This problem is identical to step02a, except that in this case we use adaptive time stepping.

To run the example:

```bash
pylith mat_poroelastic.cfg step02b_cavity_poroelastic.cfg
```
