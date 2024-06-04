# Intersecting Strike-Slip Faults (3D)

:::{note}
New in v4.1.0
:::


The files are in the directory `examples/crustal-strikeslip-3d`.
The files and directories for this set of examples includes:

:`README.md`: README file containing a brief description of the various examples.
:`*.cfg`: PyLith parameter files.
:`faulttrace*_latlon.txt`: Coordinates of points on fault traces in geographic coordinates.
:`faulttrace*_utm.txt`: Coordinates of points on fault traces in the UTM zone 11 projection.
:`generate_gmsh.py`: Python script to generate the finite-element mesh using Gmsh.
:`generate_cubit.py`: Python script to generate the finite-element mesh using Cubit.
:`*.msh`: Gmsh finite-element mesh files generated by Gmsh.
:`*.spatialdb`: Spatial database filesFiles associated with the spatial databases.
:`output`: Directory containing simulation output. It is created automatically when running the simulations.

## Overview

This suite of examples demonstrates using PyLith to model multiple intersecting faults embedded in a 3D georeferenced domain ({numref}`fig:example:crustal:strikeslip:3d:overview`).
The geometry is based on the 2019 Ridgecrest earthquake sequence.

:Step 1: Static uniform coseismic slip with Dirichlet (displacement) boundary conditions.
:Step 2: Static spatially varying coseismic slip with Dirichlet (displacement) boundary conditions.

:::{figure-md} fig:example:crustal:strikeslip:3d:overview
<img src="figs/geometry.*" alt="Diagram of geometry for the intersecting strike-slip faults." scale="50%"/>

Diagram of geometry for the domain with three intersecting strike-slip faults.
The domain extends from 80 km in the east-west direction, 60 km in the north-south direction, and 40 km in the vertical direction.
We refer to the domain boundaries using the names shown in the diagram.
:::

:::{important}
We describe how to generate the finite-element mesh using both Gmsh and Cubit.
For Step 1 we provide PyLith parameter files for both meshes; for Step 2 we only provide the parameter files that use the Gmsh file.
:::

## Example Workflow

:::{toctree}
meshing-gmsh.md
meshing-cubit.md
common-information.md
step01-slip.md
step02-varslip.md
exercises.md
:::