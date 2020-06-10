#!/usr/bin/env python
#
# ----------------------------------------------------------------------
#
# Brad T. Aagaard, U.S. Geological Survey
# Charles A. Williams, GNS Science
# Matthew G. Knepley, University of Chicago
#
# This code was developed as part of the Computational Infrastructure
# for Geodynamics (http://geodynamics.org).
#
# Copyright (c) 2010-2016 University of California, Davis
#
# See COPYING for license information.
#
# ----------------------------------------------------------------------
#

# @file pylith/materials/AuxSubfieldsIsotropicLinearGenMaxwell.py
##
# @brief Python subfields container for isotropic, linear generalized Maxwell
# viscoelastic subfields.

from pylith.utils.PetscComponent import PetscComponent

# AuxSubfieldsIsotropicLinearGenMaxwell class


class AuxSubfieldsIsotropicLinearGenMaxwell(PetscComponent):
    """
    Python container for isotropic, linear generalized Maxwell viscoelastic subfields.

    INVENTORY

    Properties
      - None

    Facilities
      - *shear_modulus* Shear modulus subfield.
      - *bulk_modulus* Bulk modulus subfield.
      - *maxwell_time* Maxwell time subfield for 3 Maxwell elements.
      - *shear_modulus_ratio* Shear modulus ratio for 3 Maxwell elements.
      - *total_strain* Total strain subfield.
      - *viscous_strain* Viscous strain subfield.
      - *reference_stress* Reference stress subfield.
      - *references_strain* Reference strain.
    """

    import pyre.inventory

    from pylith.topology.Subfield import Subfield

    shearModulus = pyre.inventory.facility("shear_modulus", family="auxiliary_subfield", factory=Subfield)
    shearModulus.meta['tip'] = "Shear modulus subfield."

    bulkModulus = pyre.inventory.facility("bulk_modulus", family="auxiliary_subfield", factory=Subfield)
    bulkModulus.meta['tip'] = "Bulk modulus subfield."

    maxwellTime = pyre.inventory.facility("maxwell_time", family="auxiliary_subfield", factory=Subfield)
    maxwellTime.meta['tip'] = "Maxwell time subfield for 3 Maxwell elements."

    shearModulusRatio = pyre.inventory.facility("shear_modulus_ratio", family="auxiliary_subfield", factory=Subfield)
    shearModulusRatio.meta['tip'] = "Shear modulus ratio subfield for 3 Maxwell elements."

    totalStrain = pyre.inventory.facility("total_strain", family="auxiliary_subfield", factory=Subfield)
    totalStrain.meta['tip'] = "Total strain subfield."

    viscousStrain = pyre.inventory.facility("viscous_strain", family="auxiliary_subfield", factory=Subfield)
    viscousStrain.meta['tip'] = "Viscous strain subfield for 3 Maxwell elements."

    referenceStress = pyre.inventory.facility("reference_stress", family="auxiliary_subfield", factory=Subfield)
    referenceStress.meta['tip'] = "Reference stress subfield."

    referenceStrain = pyre.inventory.facility("reference_strain", family="auxiliary_subfield", factory=Subfield)
    referenceStrain.meta['tip'] = "Reference strain subfield."

    # PUBLIC METHODS /////////////////////////////////////////////////////

    def __init__(self, name="auxfieldsisotropiclineargenmaxwell"):
        """
        Constructor.
        """
        PetscComponent.__init__(self, name, facility="auxiliary_subfields")
        return

    # PRIVATE METHODS ////////////////////////////////////////////////////

    def _configure(self):
        PetscComponent._configure(self)
        return


# FACTORIES ////////////////////////////////////////////////////////////

def auxiliary_subfields():
    """
    Factory associated with AuxSubfieldsIsotropicLinearGenMaxwell.
    """
    return AuxSubfieldsIsotropicLinearGenMaxwell()


# End of file
