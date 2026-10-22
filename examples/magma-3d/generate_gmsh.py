#!/usr/bin/env nemesis

import numpy
import netCDF4
import gmsh
from scipy.interpolate import RegularGridInterpolator

from pylith.meshio.gmsh_utils import BoundaryGroup, MaterialGroup, GenerateMesh
import coordsys


class App(GenerateMesh):
    """
    Generate a tetrahedral mesh of the Tongariro volcano region, New Zealand,
    with topography from a GMT .grd file and a spherical cavity beneath the summit.

    Coordinates are meters in a local Transverse Mercator projection
    (x=east, y=north, z=up, z=0 at mean sea level).
    """

    KM = 1000.0  # scale to convert km to m

    # --- Input data ---------------------------------------------------------
    FILENAME_DEM = "tongariro_etopo30_lonlat.grd"

    # --- Local Transverse Mercator projection -------------------------------
    # Origin near the summit of Tongariro (approximate; adjust as desired).
    ORIGIN_LAT = -39.1
    ORIGIN_LON = 175.6

    # --- Domain -------------------------------------------------------------
    DOMAIN_X = DOMAIN_Y = 60.0 * KM  # centered on the origin
    DOMAIN_Z = 30.0 * KM  # depth of bottom below mean sea level
    TOP_MARGIN = 1.0 * KM  # box extends this far above highest topography

    # Spacing of the grid on which the DEM is resampled in the TM projection
    # (ETOPO 30 arc-second is ~1 km).
    DX_TOPO_GRID = 1.0 * KM

    # --- Spherical cavity ---------------------------------------------------
    CAVITY_RADIUS = 2.0 * KM
    CAVITY_DEPTH = 5.0 * KM  # depth of the sphere CENTER below mean sea level
    CAVITY_X = 0.0
    CAVITY_Y = 0.0  # directly beneath the origin (summit)

    # --- Discretization size ------------------------------------------------
    DX_CAVITY = 400.0  # element size on the cavity surface
    DX_CAVITY_DIST_MIN = 1.0 * KM  # size held at DX_CAVITY within this distance
    DX_CAVITY_DIST_MAX = 15.0 * KM  # reaches DX_MAX at this distance
    DX_TOPO = 2.0 * KM  # element size at the ground surface
    DX_TOPO_DIST_MAX = 15.0 * KM  # reaches DX_MAX at this distance below surface
    DX_MAX = 8.0 * KM  # largest element size

    def __init__(self):
        """Constructor."""
        super().__init__()
        self.debug = True

        # Set the cell choices available through command line options.
        self.cell_choices = {
            "default": "tet",
            "choices": ["tet"],
        }

    # ------------------------------------------------------------------
    # Geometry
    # ------------------------------------------------------------------
    def create_geometry(self):
        """Create geometry.

        1. Resample DEM onto a regular grid in the local TM projection.
        2. Build a topography surface (slightly larger than the domain).
        3. Cut the region above the topography and the spherical cavity out of a box.
        """
        topo_surface, z_min, z_max = self._create_topography_surface()

        z_top = z_max + self.TOP_MARGIN
        box = gmsh.model.occ.add_box(
            -self.DOMAIN_X / 2,
            -self.DOMAIN_Y / 2,
            -self.DOMAIN_Z,
            self.DOMAIN_X,
            self.DOMAIN_Y,
            z_top + self.DOMAIN_Z,
        )

        # Extrude the topography surface upward to make a solid that occupies
        # everything above the ground. Cutting it from the box leaves the topography
        # as the top of the domain.
        extruded = gmsh.model.occ.extrude(
            [(2, topo_surface)], 0.0, 0.0, (z_top - z_min) + 1.0 * self.KM
        )
        above_ground = [entity for entity in extruded if entity[0] == 3]

        cavity = gmsh.model.occ.add_sphere(
            self.CAVITY_X, self.CAVITY_Y, -self.CAVITY_DEPTH, self.CAVITY_RADIUS
        )

        tags, _ = gmsh.model.occ.cut(
            [(3, box)],
            above_ground + [(3, cavity)],
            removeObject=True,
            removeTool=True,
        )
        if len(tags) != 1:
            raise RuntimeError(f"Expected 1 volume after cutting, got {tags}.")
        gmsh.model.occ.synchronize()

        self.domain_volume = [tags[0][1]]
        self._identify_boundaries(self.domain_volume[0])

    def _identify_boundaries(self, volume):
        """Classify the boundary surfaces of the volume by location."""
        tol = 10.0  # m
        hx = self.DOMAIN_X / 2
        hy = self.DOMAIN_Y / 2
        r = self.CAVITY_RADIUS
        zc = -self.CAVITY_DEPTH

        self.surface_west = []
        self.surface_east = []
        self.surface_south = []
        self.surface_north = []
        self.surface_bottom = []
        self.surface_top = []
        self.surface_cavity = []

        boundary = gmsh.model.getBoundary(
            [(3, volume)], combined=False, oriented=False
        )
        for dim, tag in boundary:
            xmin, ymin, zmin, xmax, ymax, zmax = gmsh.model.getBoundingBox(dim, tag)

            def flat(lo, hi, value):
                return abs(lo - value) < tol and abs(hi - value) < tol

            if (
                xmin > self.CAVITY_X - r - tol
                and xmax < self.CAVITY_X + r + tol
                and ymin > self.CAVITY_Y - r - tol
                and ymax < self.CAVITY_Y + r + tol
                and zmin > zc - r - tol
                and zmax < zc + r + tol
            ):
                self.surface_cavity.append(tag)
            elif flat(xmin, xmax, -hx):
                self.surface_west.append(tag)
            elif flat(xmin, xmax, hx):
                self.surface_east.append(tag)
            elif flat(ymin, ymax, -hy):
                self.surface_south.append(tag)
            elif flat(ymin, ymax, hy):
                self.surface_north.append(tag)
            elif flat(zmin, zmax, -self.DOMAIN_Z):
                self.surface_bottom.append(tag)
            else:
                self.surface_top.append(tag)

        groups = {
            "west": self.surface_west,
            "east": self.surface_east,
            "south": self.surface_south,
            "north": self.surface_north,
            "bottom": self.surface_bottom,
            "top": self.surface_top,
            "cavity": self.surface_cavity,
        }
        if self.debug:
            for name, tags in groups.items():
                print(f"Boundary {name}: surfaces {tags}")
        empty = [name for name, tags in groups.items() if not tags]
        if empty:
            raise RuntimeError(f"No surfaces identified for boundaries: {empty}")

    # ------------------------------------------------------------------
    # Topography
    # ------------------------------------------------------------------
    def _read_dem(self):
        """Read a GMT grid (netCDF). Returns ascending lon, lat (1D) and elevation (lat, lon)."""
        with netCDF4.Dataset(self.FILENAME_DEM) as grd:
            v = grd.variables
            if "x_range" in v:
                # Old-style (GMT v1) grid: 1D z stored from the top row down.
                x0, x1 = v["x_range"][:]
                y0, y1 = v["y_range"][:]
                nx, ny = (int(n) for n in v["dimension"][:])
                lon = numpy.linspace(x0, x1, nx)
                lat = numpy.linspace(y0, y1, ny)
                z = numpy.ma.filled(v["z"][:], numpy.nan).astype(float)
                z = z.reshape(ny, nx)[::-1, :]
            else:
                def pick(*names):
                    for name in names:
                        if name in v:
                            return v[name]
                    raise KeyError(f"None of {names} found; variables: {list(v)}")

                lon = numpy.array(pick("lon", "x")[:], dtype=float)
                lat = numpy.array(pick("lat", "y")[:], dtype=float)
                z = numpy.ma.filled(pick("z", "elevation", "data")[:], numpy.nan)
                z = numpy.asarray(z, dtype=float)

        if lon[0] > lon[-1]:
            lon = lon[::-1]
            z = z[:, ::-1]
        if lat[0] > lat[-1]:
            lat = lat[::-1]
            z = z[::-1, :]
        return lon, lat, z

    def _create_topography_surface(self):
        """Create topography surface in local TM coordinates, larger than the domain."""
        lon, lat, elev = self._read_dem()
        if self.debug:
            print(
                f"DEM: lon [{lon[0]}, {lon[-1]}], lat [{lat[0]}, {lat[-1]}], "
                f"elevation [{numpy.nanmin(elev):.1f}, {numpy.nanmax(elev):.1f}] m"
            )

        # Regular grid in TM coordinates, extending 2 cells beyond the domain.
        dx = self.DX_TOPO_GRID
        margin = 2.0 * dx
        x = numpy.arange(-self.DOMAIN_X / 2 - margin, self.DOMAIN_X / 2 + margin + 0.5 * dx, dx)
        y = numpy.arange(-self.DOMAIN_Y / 2 - margin, self.DOMAIN_Y / 2 + margin + 0.5 * dx, dx)
        xx, yy = numpy.meshgrid(x, y)  # shape (ny, nx)

        surface_points = numpy.column_stack((yy.flatten(), xx.flatten(), numpy.zeros_like(xx).flatten()))
        coordsys.meshToGeo(surface_points)
        if (
            surface_points[:,1].min() < lon[0]
            or surface_points[:,1].max() > lon[-1]
            or surface_points[:,0].min() < lat[0]
            or surface_points[:,0].max() > lat[-1]
        ):
            raise ValueError(
                "DEM does not cover the domain. Domain spans lon "
                f"[{surface_points[:,1].min():.3f}, {surface_points[:,1].max():.3f}], lat [{surface_points[:,0].min():.3f}, "
                f"{surface_points[:,0].max():.3f}]; DEM spans lon [{lon[0]:.3f}, {lon[-1]:.3f}], "
                f"lat [{lat[0]:.3f}, {lat[-1]:.3f}]. Reduce DOMAIN_X/DOMAIN_Y."
            )

        interp = RegularGridInterpolator((lat, lon), elev, method="linear")
        zz = interp(numpy.column_stack((surface_points[:,0], surface_points[:,1]))).reshape(xx.shape)
        if numpy.isnan(zz).any():
            raise ValueError("DEM contains no-data values within the domain.")

        wires = []
        for i in range(len(y)):
            points = numpy.column_stack((x, numpy.full_like(x, y[i]), zz[i, :]))
            wires.append(self._create_wire(points))
        topography_surface = gmsh.model.occ.add_thru_sections(
            wires, makeSolid=False, makeRuled=False, maxDegree=2
        )
        gmsh.model.occ.remove([(1, wire) for wire in wires], recursive=True)
        return topography_surface[0][1], float(zz.min()), float(zz.max())

    @staticmethod
    def _create_wire(contour: numpy.ndarray):
        """Helper function for creating a spline and wire from a numpy array of shape (x,3)"""
        points = []
        for point in contour:
            gmsh_point = gmsh.model.occ.add_point(point[0], point[1], point[2])
            points.append(gmsh_point)
        spline = gmsh.model.occ.add_spline(points)
        wire = gmsh.model.occ.add_wire([spline])
        gmsh.model.occ.remove([(0, point) for point in points])
        return wire

    # ------------------------------------------------------------------
    # Marking and meshing
    # ------------------------------------------------------------------
    def mark(self):
        """Mark geometry for materials, boundary conditions, etc."""
        materials = (MaterialGroup(tag=1, entities=self.domain_volume),)
        for material in materials:
            material.create_physical_group()

        face_groups = (
            BoundaryGroup(name="boundary_south", tag=10, dim=2, entities=self.surface_south),
            BoundaryGroup(name="boundary_east", tag=11, dim=2, entities=self.surface_east),
            BoundaryGroup(name="boundary_north", tag=12, dim=2, entities=self.surface_north),
            BoundaryGroup(name="boundary_west", tag=13, dim=2, entities=self.surface_west),
            BoundaryGroup(name="boundary_bottom", tag=14, dim=2, entities=self.surface_bottom),
            BoundaryGroup(name="boundary_top", tag=15, dim=2, entities=self.surface_top),
            BoundaryGroup(name="boundary_cavity", tag=20, dim=2, entities=self.surface_cavity),
        )
        for group in face_groups:
            group.create_physical_group()

    def generate_mesh(self, cell):
        """Generate the mesh."""
        gmsh.option.set_number("Mesh.MeshSizeFromPoints", 0)
        gmsh.option.set_number("Mesh.MeshSizeFromCurvature", 0)
        gmsh.option.set_number("Mesh.MeshSizeExtendFromBoundary", 0)

        field = gmsh.model.mesh.field

        # Size as a function of distance from the cavity.
        dist_cavity = field.add("Distance")
        field.setNumbers(dist_cavity, "SurfacesList", self.surface_cavity)
        field.setNumber(dist_cavity, "Sampling", 100)
        size_cavity = field.add("Threshold")
        field.setNumber(size_cavity, "InField", dist_cavity)
        field.setNumber(size_cavity, "SizeMin", self.DX_CAVITY)
        field.setNumber(size_cavity, "SizeMax", self.DX_MAX)
        field.setNumber(size_cavity, "DistMin", self.DX_CAVITY_DIST_MIN)
        field.setNumber(size_cavity, "DistMax", self.DX_CAVITY_DIST_MAX)

        # Size as a function of distance from the ground surface.
        dist_topo = field.add("Distance")
        field.setNumbers(dist_topo, "SurfacesList", self.surface_top)
        field.setNumber(dist_topo, "Sampling", 50)
        size_topo = field.add("Threshold")
        field.setNumber(size_topo, "InField", dist_topo)
        field.setNumber(size_topo, "SizeMin", self.DX_TOPO)
        field.setNumber(size_topo, "SizeMax", self.DX_MAX)
        field.setNumber(size_topo, "DistMin", 0.0)
        field.setNumber(size_topo, "DistMax", self.DX_TOPO_DIST_MAX)

        size_min = field.add("Min")
        field.setNumbers(size_min, "FieldsList", [size_cavity, size_topo])
        field.setAsBackgroundMesh(size_min)

        gmsh.model.mesh.generate(3)
        self.improve_quality()


# If script is called from the command line, run the application.
if __name__ == "__main__":
    App().main()
