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
// Copyright (c) 2010-2015 University of California, Davis
//
// See COPYING for license information.
//
// ----------------------------------------------------------------------
//

#include <portinfo>

#include "pylith/materials/IsotropicLinearGenMaxwellPlaneStrain.hh" // implementation of object methods

#include "pylith/materials/AuxiliaryFactory.hh" // USES AuxiliaryFactory
#include "pylith/materials/Query.hh" // USES Query

#include "pylith/topology/Field.hh" // USES Field::SubfieldInfo
#include "pylith/topology/FieldQuery.hh" // USES FieldQuery

#include "pylith/fekernels/Elasticity.hh" // USES Elasticity kernels
#include "pylith/fekernels/ElasticityPlaneStrain.hh" // USES ElasticityPlaneStrain kernels
#include "pylith/fekernels/IsotropicLinearGenMaxwellPlaneStrain.hh" // USES IsotropicLinearGenMaxwellPlaneStrain kernels
#include "pylith/fekernels/DispVel.hh" // USES DispVel kernels

#include "pylith/utils/journals.hh" // USES PYLITH_COMPONENT_*

#include "spatialdata/spatialdb/GravityField.hh" // USES GravityField
#include "spatialdata/units/Nondimensional.hh" // USES Nondimensional

#include "petscds.h"

// ----------------------------------------------------------------------
const char* pylith::materials::IsotropicLinearGenMaxwellPlaneStrain::_pyreComponent = "isotropiclineargenmaxwellplanestrain";

// ----------------------------------------------------------------------
// Default constructor.
pylith::materials::IsotropicLinearGenMaxwellPlaneStrain::IsotropicLinearGenMaxwellPlaneStrain(void) :
    pylith::materials::Material(2),
    _useInertia(false),
    _useBodyForce(false),
    _useReferenceState(false)
{ // constructor
    pylith::utils::PyreComponent::name(_pyreComponent);
} // constructor


// ----------------------------------------------------------------------
// Destructor.
pylith::materials::IsotropicLinearGenMaxwellPlaneStrain::~IsotropicLinearGenMaxwellPlaneStrain(void) {} // destructor


// ----------------------------------------------------------------------
// Include inertia?
void
pylith::materials::IsotropicLinearGenMaxwellPlaneStrain::useInertia(const bool value) {
    PYLITH_COMPONENT_DEBUG("useInertia(value="<<value<<")");

    _useInertia = value;
} // useInertia


// ----------------------------------------------------------------------
// Include inertia?
bool
pylith::materials::IsotropicLinearGenMaxwellPlaneStrain::useInertia(void) const {
    return _useInertia;
} // useInertia


// ----------------------------------------------------------------------
// Include body force?
void
pylith::materials::IsotropicLinearGenMaxwellPlaneStrain::useBodyForce(const bool value) {
    PYLITH_COMPONENT_DEBUG("useBodyForce(value="<<value<<")");

    _useBodyForce = value;
} // useBodyForce


// ----------------------------------------------------------------------
// Include body force?
bool
pylith::materials::IsotropicLinearGenMaxwellPlaneStrain::useBodyForce(void) const {
    return _useBodyForce;
} // useBodyForce


// ----------------------------------------------------------------------
// Use reference stress and strain in computation of stress and
// strain?
void
pylith::materials::IsotropicLinearGenMaxwellPlaneStrain::useReferenceState(const bool value) {
    PYLITH_COMPONENT_DEBUG("useReferenceState="<<value<<")");

    _useReferenceState = value;
} // useReferenceState


// ----------------------------------------------------------------------
// Use reference stress and strain in computation of stress and
// strain?
bool
pylith::materials::IsotropicLinearGenMaxwellPlaneStrain::useReferenceState(void) const {
    return _useReferenceState;
} // useReferenceState


// ----------------------------------------------------------------------
// Verify configuration is acceptable.
void
pylith::materials::IsotropicLinearGenMaxwellPlaneStrain::verifyConfiguration(const pylith::topology::Field& solution) const {
    PYLITH_METHOD_BEGIN;
    PYLITH_COMPONENT_DEBUG("verifyConfiguration(solution="<<solution.label()<<")");

    // Verify solution contains expected fields.
    if (!solution.hasSubfield("displacement")) {
        throw std::runtime_error("Cannot find 'displacement' field in solution; required for material 'IsotropicLinearGenMaxwellPlaneStrain'.");
    } // if
    if (_useInertia && !solution.hasSubfield("velocity")) {
        throw std::runtime_error("Cannot find 'velocity' field in solution; required for material 'IsotropicLinearGenMaxwellPlaneStrain' with inertia.");
    } // if

    PYLITH_METHOD_END;
} // verifyConfiguration


// ----------------------------------------------------------------------
// Preinitialize material. Set names/sizes of auxiliary subfields.
void
pylith::materials::IsotropicLinearGenMaxwellPlaneStrain::_auxFieldSetup(void) {
    PYLITH_METHOD_BEGIN;
    PYLITH_COMPONENT_DEBUG("_auxFieldSetup()");

    const int dim = 2;

    assert(_auxMaterialFactory);
    assert(_normalizer);
    _auxMaterialFactory->initialize(_auxField, *_normalizer, dim);

    // :ATTENTION: The order for adding subfields must match the order of the auxiliary fields in the FE kernels.
    // NOTE:  At present, we assume exactly 3 Maxwell elements.

    _auxMaterialFactory->density(); // 0
    _auxMaterialFactory->shearModulus(); // 1
    _auxMaterialFactory->bulkModulus(); // 2
    _auxMaterialFactory->maxwellTime("1"); // 3
    _auxMaterialFactory->maxwellTime("2"); // 4
    _auxMaterialFactory->maxwellTime("3"); // 5
    _auxMaterialFactory->shearModulusRatio("1"); // 6
    _auxMaterialFactory->shearModulusRatio("2"); // 7
    _auxMaterialFactory->shearModulusRatio("3"); // 8
    _auxMaterialFactory->totalStrain(); // 9
    _auxMaterialFactory->viscousStrain("1"); // 10
    _auxMaterialFactory->viscousStrain("2"); // 11
    _auxMaterialFactory->viscousStrain("3"); // 12
    if (_gravityField) {
        _auxMaterialFactory->gravityField(_gravityField);
    } // if
    if (_useBodyForce) {
        _auxMaterialFactory->bodyForce();
    } // if
    if (_useReferenceState) {
        _auxMaterialFactory->referenceStress(); // numA-2
        _auxMaterialFactory->referenceStrain(); // numA-1
    } // if

    PYLITH_METHOD_END;
} // _auxFieldSetup

// ----------------------------------------------------------------------
// Set constants for problem.
void
pylith::materials::IsotropicLinearGenMaxwellPlaneStrain::_setFEConstants(const pylith::topology::Field& solution,
                                                                      const PylithReal dt) const {
    PYLITH_METHOD_BEGIN;
    PYLITH_COMPONENT_DEBUG("_setFEConstants(solution="<<solution.label()<<", dt="<<dt<<")");

    const PetscInt numConstants = 1;
    PetscScalar constants[1];
    constants[0] = dt;

    const PetscDM dmSoln = solution.dmMesh(); assert(dmSoln);
    PetscDS prob = NULL;
    PetscErrorCode err = DMGetDS(dmSoln, &prob); PYLITH_CHECK_ERROR(err); assert(prob);
    err = PetscDSSetConstants(prob, numConstants, constants); PYLITH_CHECK_ERROR(err);

    PYLITH_METHOD_END;
} // _setConstants

// ----------------------------------------------------------------------
// Set kernels for RHS residual G(t,s).
void
pylith::materials::IsotropicLinearGenMaxwellPlaneStrain::_setFEKernelsRHSResidual(const pylith::topology::Field& solution) const {
    PYLITH_METHOD_BEGIN;
    PYLITH_COMPONENT_DEBUG("_setFEKernelsRHSResidual(solution="<<solution.label()<<")");

    const PetscDM dm = solution.dmMesh(); assert(dm);
    PetscDS prob = NULL;
    PetscErrorCode err = DMGetDS(dm, &prob); PYLITH_CHECK_ERROR(err);

    const PetscInt i_disp = solution.subfieldInfo("displacement").index;

    if (!solution.hasSubfield("velocity")) {
        // Displacement
        const PetscPointFunc g0u = (_gravityField && _useBodyForce) ? pylith::fekernels::IsotropicLinearGenMaxwellPlaneStrain::g0v_gravbodyforce :
                                   (_gravityField) ? pylith::fekernels::IsotropicLinearGenMaxwellPlaneStrain::g0v_grav :
                                   (_useBodyForce) ? pylith::fekernels::IsotropicLinearGenMaxwellPlaneStrain::g0v_bodyforce :
                                   NULL;
        const PetscPointFunc g1u = (!_useReferenceState) ? pylith::fekernels::IsotropicLinearGenMaxwellPlaneStrain::g1v : pylith::fekernels::IsotropicLinearGenMaxwellPlaneStrain::g1v_refstate;

        err = PetscDSSetResidual(prob, i_disp, g0u, g1u); PYLITH_CHECK_ERROR(err);
    } else {
        const PetscInt i_vel = solution.subfieldInfo("velocity").index;

        // Displacement
        const PetscPointFunc g0u = pylith::fekernels::DispVel::g0u;
        const PetscPointFunc g1u = NULL;

        // Velocity
        const PetscPointFunc g0v = (_gravityField && _useBodyForce) ? pylith::fekernels::IsotropicLinearGenMaxwellPlaneStrain::g0v_gravbodyforce :
                                   (_gravityField) ? pylith::fekernels::IsotropicLinearGenMaxwellPlaneStrain::g0v_grav :
                                   (_useBodyForce) ? pylith::fekernels::IsotropicLinearGenMaxwellPlaneStrain::g0v_bodyforce :
                                   NULL;
        const PetscPointFunc g1v = (!_useReferenceState) ? pylith::fekernels::IsotropicLinearGenMaxwellPlaneStrain::g1v : pylith::fekernels::IsotropicLinearGenMaxwellPlaneStrain::g1v_refstate;

        err = PetscDSSetResidual(prob, i_disp, g0u, g1u); PYLITH_CHECK_ERROR(err);
        err = PetscDSSetResidual(prob, i_vel,  g0v, g1v); PYLITH_CHECK_ERROR(err);
    } // if/else


    PYLITH_METHOD_END;
} // _setFEKernelsRHSResidual


// ----------------------------------------------------------------------
// Set kernels for RHS Jacobian G(t,s).
void
pylith::materials::IsotropicLinearGenMaxwellPlaneStrain::_setFEKernelsRHSJacobian(const pylith::topology::Field& solution) const {
    PYLITH_METHOD_BEGIN;
    PYLITH_COMPONENT_DEBUG("_setFEKernelsRHSJacobian(solution="<<solution.label()<<")");

    const PetscDM dm = solution.dmMesh(); assert(dm);
    PetscDS prob = NULL;
    PetscErrorCode err = DMGetDS(dm, &prob); PYLITH_CHECK_ERROR(err);

    const PetscInt i_disp = solution.subfieldInfo("displacement").index;

    if (!solution.hasSubfield("velocity")) {
        const PetscPointJac Jg0uu = NULL;
        const PetscPointJac Jg1uu = NULL;
        const PetscPointJac Jg2uu = NULL;
        const PetscPointJac Jg3uu = pylith::fekernels::IsotropicLinearGenMaxwellPlaneStrain::Jg3vu;

        err = PetscDSSetJacobian(prob, i_disp, i_disp, Jg0uu, Jg1uu, Jg2uu, Jg3uu); PYLITH_CHECK_ERROR(err);
    } else {
        const PetscInt i_vel = solution.subfieldInfo("velocity").index;

        // Jacobian kernels
        const PetscPointJac Jg0uu = NULL;
        const PetscPointJac Jg1uu = NULL;
        const PetscPointJac Jg2uu = NULL;
        const PetscPointJac Jg3uu = NULL;

        const PetscPointJac Jg0uv = pylith::fekernels::DispVel::Jg0uv;
        const PetscPointJac Jg1uv = NULL;
        const PetscPointJac Jg2uv = NULL;
        const PetscPointJac Jg3uv = NULL;

        const PetscPointJac Jg0vu = NULL;
        const PetscPointJac Jg1vu = NULL;
        const PetscPointJac Jg2vu = NULL;
        const PetscPointJac Jg3vu = pylith::fekernels::IsotropicLinearGenMaxwellPlaneStrain::Jg3vu;

        const PetscPointJac Jg0vv = NULL;
        const PetscPointJac Jg1vv = NULL;
        const PetscPointJac Jg2vv = NULL;
        const PetscPointJac Jg3vv = NULL;

        err = PetscDSSetJacobian(prob, i_disp, i_disp, Jg0uu, Jg1uu, Jg2uu, Jg3uu); PYLITH_CHECK_ERROR(err);
        err = PetscDSSetJacobian(prob, i_disp, i_vel,  Jg0uv, Jg1uv, Jg2uv, Jg3uv); PYLITH_CHECK_ERROR(err);
        err = PetscDSSetJacobian(prob, i_vel,  i_disp, Jg0vu, Jg1vu, Jg2vu, Jg3vu); PYLITH_CHECK_ERROR(err);
        err = PetscDSSetJacobian(prob, i_vel,  i_vel,  Jg0vv, Jg1vv, Jg2vv, Jg3vv); PYLITH_CHECK_ERROR(err);

    } // if/else

    PYLITH_METHOD_END;
} // _setFEKernelsRHSJacobian


// ----------------------------------------------------------------------
// Set kernels for LHS residual F(t,s,\dot{s}).
void
pylith::materials::IsotropicLinearGenMaxwellPlaneStrain::_setFEKernelsLHSResidual(const pylith::topology::Field& solution) const {
    PYLITH_METHOD_BEGIN;
    PYLITH_COMPONENT_DEBUG("_setFEKernelsLHSResidual(solution="<<solution.label()<<")");

    const PetscDM dm = solution.dmMesh(); assert(dm);
    PetscDS prob = NULL;
    PetscErrorCode err = DMGetDS(dm, &prob); PYLITH_CHECK_ERROR(err);

    const PetscInt i_disp = solution.subfieldInfo("displacement").index;

    if (!solution.hasSubfield("velocity")) {
        // F(t,s,\dot{s}) = \vec{0}.
    } else {
        const PetscInt i_vel = solution.subfieldInfo("velocity").index;

        // Displacement
        const PetscPointFunc f0u = pylith::fekernels::DispVel::f0u;
        const PetscPointFunc f1u = NULL;

        // Velocity
        const PetscPointFunc f0v = (_useInertia) ? pylith::fekernels::ElasticityPlaneStrain::f0v : NULL;
        const PetscPointFunc f1v = NULL;

        err = PetscDSSetResidual(prob, i_disp, f0u, f1u); PYLITH_CHECK_ERROR(err);
        err = PetscDSSetResidual(prob, i_vel,  f0v, f1v); PYLITH_CHECK_ERROR(err);
    } // if/else

    PYLITH_METHOD_END;
} // _setFEKernelsLHSResidual


// ----------------------------------------------------------------------
// Set kernels for LHS Jacobian F(t,s,\dot{s}).
void
pylith::materials::IsotropicLinearGenMaxwellPlaneStrain::_setFEKernelsLHSJacobianImplicit(const pylith::topology::Field& solution) const {
    PYLITH_METHOD_BEGIN;
    PYLITH_COMPONENT_DEBUG("_setFEKernelsLHSJacobianImplicit(solution="<<solution.label()<<")");

    const PetscDM dm = solution.dmMesh(); assert(dm);
    PetscDS prob = NULL;
    PetscErrorCode err = DMGetDS(dm, &prob); PYLITH_CHECK_ERROR(err);

    const PetscInt i_disp = solution.subfieldInfo("displacement").index;

    if (!solution.hasSubfield("velocity")) {
        // Jacobian kernels
        const PetscPointJac Jf0uu = pylith::fekernels::DispVel::Jf0uu_zero;
        const PetscPointJac Jf1uu = NULL;
        const PetscPointJac Jf2uu = NULL;
        const PetscPointJac Jf3uu = NULL;

        err = PetscDSSetJacobian(prob, i_disp, i_disp, Jf0uu, Jf1uu, Jf2uu, Jf3uu); PYLITH_CHECK_ERROR(err);
    } else {
        const PetscInt i_vel = solution.subfieldInfo("velocity").index;

        // Jacobian kernels
        const PetscPointJac Jf0uu = pylith::fekernels::DispVel::Jf0uu_implicit;
        const PetscPointJac Jf1uu = NULL;
        const PetscPointJac Jf2uu = NULL;
        const PetscPointJac Jf3uu = NULL;

        const PetscPointJac Jf0uv = NULL;
        const PetscPointJac Jf1uv = NULL;
        const PetscPointJac Jf2uv = NULL;
        const PetscPointJac Jf3uv = NULL;

        const PetscPointJac Jf0vu = NULL;
        const PetscPointJac Jf1vu = NULL;
        const PetscPointJac Jf2vu = NULL;
        const PetscPointJac Jf3vu = NULL;

        const PetscPointJac Jf0vv = (_useInertia) ? pylith::fekernels::ElasticityPlaneStrain::Jf0vv_implicit : NULL;
        const PetscPointJac Jf1vv = NULL;
        const PetscPointJac Jf2vv = NULL;
        const PetscPointJac Jf3vv = NULL;

        err = PetscDSSetJacobian(prob, i_disp, i_disp, Jf0uu, Jf1uu, Jf2uu, Jf3uu); PYLITH_CHECK_ERROR(err);
        err = PetscDSSetJacobian(prob, i_disp, i_vel,  Jf0uv, Jf1uv, Jf2uv, Jf3uv); PYLITH_CHECK_ERROR(err);
        err = PetscDSSetJacobian(prob, i_vel,  i_disp, Jf0vu, Jf1vu, Jf2vu, Jf3vu); PYLITH_CHECK_ERROR(err);
        err = PetscDSSetJacobian(prob, i_vel,  i_vel,  Jf0vv, Jf1vv, Jf2vv, Jf3vv); PYLITH_CHECK_ERROR(err);
    } // if/else

    PYLITH_METHOD_END;
} // _setFEKernelsLHSJacobianImplicit


// ----------------------------------------------------------------------
// Set kernels for LHS Jacobian F(t,s,\dot{s}).
void
pylith::materials::IsotropicLinearGenMaxwellPlaneStrain::_setFEKernelsLHSJacobianExplicit(const pylith::topology::Field& solution) const {
    PYLITH_METHOD_BEGIN;
    PYLITH_COMPONENT_DEBUG("_setFEKernelsLHSJacobianExplicit(solution="<<solution.label()<<")");

    const PetscInt i_disp = solution.subfieldInfo("displacement").index;
    const PetscInt i_vel = solution.subfieldInfo("velocity").index;

    // Jacobian kernels
    const PetscPointJac Jf0uu = pylith::fekernels::DispVel::Jf0uu_explicit;
    const PetscPointJac Jf1uu = NULL;
    const PetscPointJac Jf2uu = NULL;
    const PetscPointJac Jf3uu = NULL;

    const PetscPointJac Jf0uv = NULL;
    const PetscPointJac Jf1uv = NULL;
    const PetscPointJac Jf2uv = NULL;
    const PetscPointJac Jf3uv = NULL;

    const PetscPointJac Jf0vu = NULL;
    const PetscPointJac Jf1vu = NULL;
    const PetscPointJac Jf2vu = NULL;
    const PetscPointJac Jf3vu = NULL;

    const PetscPointJac Jf0vv = (_useInertia) ? pylith::fekernels::ElasticityPlaneStrain::Jf0vv_explicit : NULL;
    const PetscPointJac Jf1vv = NULL;
    const PetscPointJac Jf2vv = NULL;
    const PetscPointJac Jf3vv = NULL;

    const PetscDM dm = solution.dmMesh(); assert(dm);
    PetscDS prob = NULL;
    PetscErrorCode err = DMGetDS(dm, &prob); PYLITH_CHECK_ERROR(err);
    err = PetscDSSetJacobian(prob, i_disp, i_disp, Jf0uu, Jf1uu, Jf2uu, Jf3uu); PYLITH_CHECK_ERROR(err);
    err = PetscDSSetJacobian(prob, i_disp, i_vel,  Jf0uv, Jf1uv, Jf2uv, Jf3uv); PYLITH_CHECK_ERROR(err);
    err = PetscDSSetJacobian(prob, i_vel,  i_disp, Jf0vu, Jf1vu, Jf2vu, Jf3vu); PYLITH_CHECK_ERROR(err);
    err = PetscDSSetJacobian(prob, i_vel,  i_vel,  Jf0vv, Jf1vv, Jf2vv, Jf3vv); PYLITH_CHECK_ERROR(err);

    PYLITH_METHOD_END;
} // _setFEKernelsLHSJacobianExplicit


// ----------------------------------------------------------------------
// Set kernels for updating state variables.
void
pylith::materials::IsotropicLinearGenMaxwellPlaneStrain::_setFEKernelsUpdateStatevars(const pylith::topology::Field& solution) const {
    PYLITH_METHOD_BEGIN;
    PYLITH_COMPONENT_DEBUG("_setFEKernelsUpdateStatevars(solution="<<solution.label()<<")");

    const PetscDM dm = _auxField->dmMesh(); assert(dm);
    PetscDS prob = NULL;
    PetscErrorCode err = DMGetDS(dm, &prob); PYLITH_CHECK_ERROR(err);

    const PetscInt i_totalstrain = solution.subfieldInfo("total_strain").index;
    const PetscInt i_viscousstrain_1 = solution.subfieldInfo("viscous_strain_1").index;
    const PetscInt i_viscousstrain_2 = solution.subfieldInfo("viscous_strain_2").index;
    const PetscInt i_viscousstrain_3 = solution.subfieldInfo("viscous_strain_3").index;

    const PetscPointFunc updateTotalStrainKernel = pylith::fekernels::IsotropicLinearGenMaxwellPlaneStrain::updateTotalStrain;
    const PetscPointFunc updateViscousStrainKernel_1 = pylith::fekernels::IsotropicLinearGenMaxwellPlaneStrain::updateViscousStrain_1;
    const PetscPointFunc updateViscousStrainKernel_2 = pylith::fekernels::IsotropicLinearGenMaxwellPlaneStrain::updateViscousStrain_2;
    const PetscPointFunc updateViscousStrainKernel_3 = pylith::fekernels::IsotropicLinearGenMaxwellPlaneStrain::updateViscousStrain_3;
    err = PetscDSSetUpdate(prob, i_totalstrain, updateTotalStrainKernel); PYLITH_CHECK_ERROR(err);
    err = PetscDSSetUpdate(prob, i_viscousstrain_1, updateViscousStrainKernel_1); PYLITH_CHECK_ERROR(err);
    err = PetscDSSetUpdate(prob, i_viscousstrain_2, updateViscousStrainKernel_2); PYLITH_CHECK_ERROR(err);
    err = PetscDSSetUpdate(prob, i_viscousstrain_3, updateViscousStrainKernel_3); PYLITH_CHECK_ERROR(err);

    PYLITH_METHOD_END;
} // _setFEKernelsUpdateStatevars


// End of file