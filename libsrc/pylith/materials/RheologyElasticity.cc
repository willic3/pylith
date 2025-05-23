// =================================================================================================
// This code is part of PyLith, developed through the Computational Infrastructure
// for Geodynamics (https://github.com/geodynamics/pylith).
//
// Copyright (c) 2010-2025, University of California, Davis and the PyLith Development Team.
// All rights reserved.
//
// See https://mit-license.org/ and LICENSE.md and for license information.
// =================================================================================================

#include <portinfo>

#include "pylith/materials/RheologyElasticity.hh" // implementation of object methods

#include "pylith/feassemble/Integrator.hh" // USES NEW_JACOBIAN_NEVER

#include "pylith/utils/error.hh" // USES PYLITH_METHOD_BEGIN/END
#include "pylith/utils/journals.hh" // USES PYLITH_COMPONENT_DEBUG

#include "spatialdata/geocoords/CoordSys.hh" // USES CoordSys

#include <typeinfo> // USES typeid()

// ------------------------------------------------------------------------------------------------
// Default constructor.
pylith::materials::RheologyElasticity::RheologyElasticity(void) :
    _lhsJacobianTriggers(pylith::feassemble::Integrator::NEW_JACOBIAN_NEVER) {}


// ------------------------------------------------------------------------------------------------
// Destructor.
pylith::materials::RheologyElasticity::~RheologyElasticity(void) {
    deallocate();
} // destructor


// ------------------------------------------------------------------------------------------------
// Deallocate PETSc and local data structures.
void
pylith::materials::RheologyElasticity::deallocate(void) {
}


// ------------------------------------------------------------------------------------------------
// Get triggers for needing to compute the elastic constants for the RHS Jacobian.
int
pylith::materials::RheologyElasticity::getLHSJacobianTriggers(void) const {
    return _lhsJacobianTriggers;
} // getLHSJacobianTriggers


// ------------------------------------------------------------------------------------------------
// Update kernel constants.
void
pylith::materials::RheologyElasticity::updateKernelConstants(pylith::real_array* kernelConstants,
                                                             const PylithReal dt) const {
    PYLITH_METHOD_BEGIN;
    PYLITH_COMPONENT_DEBUG("updateKernelConstants(kernelConstants"<<kernelConstants<<", dt="<<dt<<") empty method");

    // Default is to do nothing.

    PYLITH_METHOD_END;
} // updateKernelConstants


// ------------------------------------------------------------------------------------------------
// Add kernels for updating state variables.
void
pylith::materials::RheologyElasticity::addKernelsUpdateStateVars(std::vector<pylith::feassemble::IntegratorDomain::ProjectKernels>* kernels,
                                                                 const spatialdata::geocoords::CoordSys* coordsys) const {
    PYLITH_METHOD_BEGIN;
    PYLITH_COMPONENT_DEBUG("addKernelsUpdateStateVars(kernels="<<kernels<<", coordsys="<<typeid(coordsys).name()<<") empty method");

    // Default is to do nothing.

    PYLITH_METHOD_END;
} // addKernelsUpdateStateVars


// End of file
