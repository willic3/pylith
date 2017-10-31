// -*- C++ -*-
//
// ----------------------------------------------------------------------
//
// Brad T. Aagaard, U.S. Geological Survey
// Charles A. Williams, GNS Science
// Matthew G. Knepley, University of Chicago
//
// This code was developed as part of the Computational Infrastructure
// for Geodynamics (http://geodynamics.org).
//
// Copyright (c) 2010-2016 University of California, Davis
//
// See COPYING for license information.
//
// ----------------------------------------------------------------------
//

#include <portinfo>

#include "NeumannTimeDependent.hh" // implementation of object methods

#include "pylith/topology/Field.hh" // USES Field
#include "pylith/topology/FieldQuery.hh" // USES FieldQuery
#include "pylith/topology/VisitorMesh.hh" // USES VecVisitorMesh

#include "spatialdata/spatialdb/TimeHistory.hh" // USES TimeHistory
#include "spatialdata/units/Nondimensional.hh" // USES Nondimensional

#include "pylith/utils/error.hh" // USES PYLITH_METHOD_BEGIN/END
#include "pylith/utils/journals.hh" // USES PYLITH_COMPONENT_*

extern "C" {
    #include "pylith/fekernels/timedependentbc.h"
}

#include <cassert> // USES assert()
#include <stdexcept> // USES std::runtime_error
#include <sstream> // USES std::ostringstream

// ----------------------------------------------------------------------
const char* pylith::bc::NeumannTimeDependent::_pyreComponent = "neumanntimedependent";

// ----------------------------------------------------------------------
// Default constructor.
pylith::bc::NeumannTimeDependent::NeumannTimeDependent(void) :
    _dbTimeHistory(NULL),
    _useInitial(true),
    _useRate(false),
    _useTimeHistory(false)
{ // constructor
    PyreComponent::name(_pyreComponent);
} // constructor


// ----------------------------------------------------------------------
// Destructor.
pylith::bc::NeumannTimeDependent::~NeumannTimeDependent(void)
{ // destructor
    deallocate();
} // destructor


// ----------------------------------------------------------------------
// Deallocate PETSc and local data structures.
void
pylith::bc::NeumannTimeDependent::deallocate(void)
{ // deallocate
    PYLITH_METHOD_BEGIN;

    DirichletNew::deallocate();
    _dbTimeHistory = NULL; // :KLUDGE: Use shared pointer.

    PYLITH_METHOD_END;
} // deallocate


// ----------------------------------------------------------------------
// Set time history database.
void
pylith::bc::NeumannTimeDependent::dbTimeHistory(spatialdata::spatialdb::TimeHistory* th)
{ // dbTimeHistory
    _dbTimeHistory = th;
} // dbTimeHistory


// ----------------------------------------------------------------------
// Get time history database.
const spatialdata::spatialdb::TimeHistory*
pylith::bc::NeumannTimeDependent::dbTimeHistory(void)
{ // dbTimeHistory
    return _dbTimeHistory;
} // dbTimeHistory

// ----------------------------------------------------------------------
// Use initial value term in time history expression.
void
pylith::bc::NeumannTimeDependent::useInitial(const bool value)
{ // useInitial
    PYLITH_COMPONENT_DEBUG("useInitial(value="<<value<<")");

    _useInitial = value;
} // useInitial


// ----------------------------------------------------------------------
// Get flag associated with using initial value term in time history expression.
bool
pylith::bc::NeumannTimeDependent::useInitial(void) const
{ // useInitial
    return _useInitial;
} // useInitial


// ----------------------------------------------------------------------
// Use rate value term in time history expression.
void
pylith::bc::NeumannTimeDependent::useRate(const bool value)
{ // useRate
    PYLITH_COMPONENT_DEBUG("useRate(value="<<value<<")");

    _useRate = value;
} // useRate


// ----------------------------------------------------------------------
// Get flag associated with using rate value term in time history expression.
bool
pylith::bc::NeumannTimeDependent::useRate(void) const
{ // useRate
    return _useRate;
} // useRate


// ----------------------------------------------------------------------
// Use time history term in time history expression.
void
pylith::bc::NeumannTimeDependent::useTimeHistory(const bool value)
{ // useTimeHistory
    PYLITH_COMPONENT_DEBUG("useTimeHistory(value="<<value<<")");

    _useTimeHistory = value;
} // useTimeHistory


// ----------------------------------------------------------------------
// Get flag associated with using time history term in time history expression.
bool
pylith::bc::NeumannTimeDependent::useTimeHistory(void) const
{ // useTimeHistory
    return _useTimeHistory;
} // useTimeHistory


// ----------------------------------------------------------------------
// Update auxiliary fields at beginning of time step.
void
pylith::bc::NeumannTimeDependent::prestep(const double t,
                                          const double dt)
{ // prestep
    PYLITH_METHOD_BEGIN;
    PYLITH_COMPONENT_DEBUG("_prestep(t="<<t<<", dt="<<dt<<")");

    if (_useTimeHistory) {
        assert(_normalizer);
        assert(_auxFields);

        const PylithScalar timeScale = _normalizer->timeScale();

        PetscErrorCode err;

        PetscSection auxFieldsSection = _auxFields->localSection(); assert(auxFieldsSection);
        PetscInt pStart = 0, pEnd = 0;
        err = PetscSectionGetChart(auxFieldsSection, &pStart, &pEnd); PYLITH_CHECK_ERROR(err);
        pylith::topology::VecVisitorMesh auxFieldsVisitor(*_auxFields);
        PetscScalar* auxFieldsArray = auxFieldsVisitor.localArray(); assert(auxFieldsArray);

        // Compute offset of time history subfields in auxiliary field.
        // :ASSUMPTION: Constrained field is a scalar or vector field.
        const PetscInt numComponents = (_vectorFieldType == pylith::topology::Field::VECTOR) ? _spaceDim : 1;
        PetscInt offTH = 0;
        if (_useInitial) {offTH += numComponents;}
        if (_useRate) {offTH += numComponents + 1;}
        const PetscInt offStartTime = offTH + numComponents;
        const PetscInt offValue = offStartTime + 1;

        // Loop over all points in section.
        for (PetscInt p = pStart; p < pEnd; ++p) {
            // Skip points without values in section.
            if (!auxFieldsVisitor.sectionDof(p)) {continue;}

            // Get offset for point.
            const PetscInt off = auxFieldsVisitor.sectionOffset(p);

            // Get starting time and compute relative time for point.
            const PylithScalar tStart = auxFieldsArray[off+offStartTime];
            const PylithScalar tRel = t - tStart;

            // Query time history for value (normalized amplitude).
            PylithScalar value = 0.0;
            if (tRel >= 0.0) {
                PylithScalar tDim = tRel * timeScale;
                const int err = _dbTimeHistory->query(&value, tDim);
                if (err) {
                    std::ostringstream msg;
                    msg << "Error querying for time '" << tDim << "' in time history database '" << _dbTimeHistory->label() << "'.";
                    throw std::runtime_error(msg.str());
                } // if
            } // if
              // Update value (normalized amplitude) in auxiliary field.
            auxFieldsArray[off+offValue] = value;
        } // for

    } // if

    PYLITH_METHOD_END;
} // prestep


// ----------------------------------------------------------------------
// Setup auxiliary subfields (discretization and query fns).
void
pylith::bc::NeumannTimeDependent::_auxFieldsSetup(const pylith::topology::Field& solution)
{ // _auxFieldsSetup
    PYLITH_METHOD_BEGIN;
    PYLITH_COMPONENT_DEBUG("_auxFieldsSetup(solution="<<solution.label()<<")");

    TimeDependentAuxiliaryFactory factory(_auxField, solution.subfieldInfo(_field.c_str()), solution.spaceDim(), _normalizer->timeScale());

    // :ATTENTION: The order of the factory methods must match the order of the auxiliary subfields in the FE kernels.

    if (_useInitial) {
        factory.initialAmplitude();
    } // if
    if (_useRate) {
        factory.rateAmplitude();
        factory.rateStartTime();
    } // _useRate
    if (_useTimeHistory) {
        factory.timeHistoryAmplitude();
        factory.timeHistoryStartTime();
        factory.timeHistoryValue();
    } // _useTimeHistory

    PYLITH_METHOD_END;
}     // _auxFieldsSetup


// ----------------------------------------------------------------------
// Set kernels for RHS residual G(t,s).
void
pylith::bc::NeumannTimeDependent::_setFEKernelsResidual(const topology::Field& solution)
{ // _setFEKernelsResidual
    PYLITH_METHOD_BEGIN;
    PYLITH_COMPONENT_DEBUG("_setFEKernelsResidual(solution="<<solution.label()<<")");

    const PetscDM dmSoln = solution.dmMesh(); assert(dmSoln);
    PetscDS prob = NULL;
    PetscErrorCode err = DMGetDS(dmSoln, &prob); PYLITH_CHECK_ERROR(err);

    const bool isScalarField = _vectorFieldType == pylith::topology::Field::SCALAR;

    // static
    // void f0_bd_u(PetscInt dim, PetscInt Nf, PetscInt NfAux,
    //                     const PetscInt uOff[], const PetscInt uOff_x[], const PetscScalar u[], const PetscScalar u_t[], const PetscScalar u_x[],
    //                     const PetscInt aOff[], const PetscInt aOff_x[], const PetscScalar a[], const PetscScalar a_t[], const PetscScalar a_x[],
    //                     PetscReal t, const PetscReal x[], const PetscReal n[], PetscInt numConstants, const PetscScalar constants[], PetscScalar g0[])

    PetscPointFunc g0 = NULL;
    PetscPointFunc g1 = NULL;

    const int bitInitial = _useInitial ? 0x1 : 0x0;
    const int bitRate = _useRate ? 0x2 : 0x0;
    const int bitTimeHistory = _useTimeHistory ? 0x4 : 0x0;
    const int bitUse = bitInitial | bitRate | bitTimeHistory;
    switch (bitUse) {
    case 0x1:
        g0 = (isScalarField) ? pylith_fekernels_TimeDependentBC_g0_initial_scalar : pylith_fekernels_TimeDependentBC_g0_initial_vector;
        break;
    case 0x2:
        g0 = (isScalarField) ? pylith_fekernels_TimeDependentBC_g0_rate_scalar : pylith_fekernels_TimeDependentBC_g0_rate_vector;
        break;
    case 0x4:
        g0 = (isScalarField) ? pylith_fekernels_TimeDependentBC_g0_timeHistory_scalar : pylith_fekernels_TimeDependentBC_g0_timeHistory_vector;
        break;
    case 0x3:
        g0 = (isScalarField) ? pylith_fekernels_TimeDependentBC_g0_initialRate_scalar : pylith_fekernels_TimeDependentBC_g0_initialRate_vector;
        break;
    case 0x5:
        g0 = (isScalarField) ? pylith_fekernels_TimeDependentBC_g0_initialTimeHistory_scalar : pylith_fekernels_TimeDependentBC_g0_initialTimeHistory_vector;
        break;
    case 0x6:
        g0 = (isScalarField) ? pylith_fekernels_TimeDependentBC_g0_rateTimeHistory_scalar : pylith_fekernels_TimeDependentBC_g0_rateTimeHistory_vector;
        break;
    case 0x7:
        g0 = (isScalarField) ? pylith_fekernels_TimeDependentBC_g0_initialRateTimeHistory_scalar : pylith_fekernels_TimeDependentBC_g0_initialRateTimeHistory_vector;
        break;
    case 0x0:
        PYLITH_COMPONENT_WARNING("Neumann BC provides no values.");
        break;
    default:
        PYLITH_COMPONENT_ERROR("Unknown combination of flags for Dirichlet BC terms (useInitial="<<_useInitial<<", useRate="<<_useRate<<", useTimeHistory="<<_useTimeHistory<<").");
        throw std::logic_error("Unknown combination of flags for Dirichlet BC terms.");
    } // switch

    err = PetscDSSetBdResidual(prob, i_field, g0, g1); PYLITH_CHECK_ERROR(err);

    PYLITH_METHOD_END;
} // _setFEKernelsResidual

// End of file