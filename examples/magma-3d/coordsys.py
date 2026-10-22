# Coordinate systems

from spatialdata.geocoords.CSGeo import CSGeo

# Geographic lat/lon coordinates in WGS84 datum


def cs_geo():
    """Geographic lat/lon coordinates in WGS84 datum.
    """
    cs = CSGeo()
    cs.inventory.crsString = "EPSG:4326"
    cs.inventory.spaceDim = 2
    cs._configure()
    return cs


def cs_geo3D():
    """Geographic lat/lon/elev coordinates in WGS84 datumm.

    """
    cs = CSGeo()
    cs.inventory.crsString = "EPSG:4326"
    cs.inventory.spaceDim = 3
    cs._configure()
    return cs


def cs_mesh():
    """Cartesian coordinates for mesh with origin near Tongariro summit
    """
    cs = CSGeo()
    cs.inventory.crsString = "+proj=tmerc +datum=WGS84 +lat_0=-39.1 +lon_0=175.6 +k=0.9996 +units=m"
    cs.inventory.spaceDim = 3
    cs._configure()
    return cs


def geoToMesh(xyz):
    """Convert coordinates from geographic coordinates to mesh coordinates.
    """
    from spatialdata.geocoords.Converter import convert
    convert(xyz, cs_mesh(), cs_geo3D())
    return


def meshToGeo(xyz):
    """Convert coordinates from mesh coordinates to geographic coordinates.
    """
    from spatialdata.geocoords.Converter import convert
    convert(xyz, cs_geo3D(), cs_mesh())
    return


# End of file
