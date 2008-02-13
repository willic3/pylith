// -*- C++ -*-
//
// ======================================================================
//
//                           Brad T. Aagaard
//                        U.S. Geological Survey
//
// {LicenseText}
//
// ======================================================================
//

/**
 * @file pylith/feassemble/IntegratorElasticity.hh
 *
 * @brief Object containing general elasticity operations for implicit
 * and explicit time integration of the elasticity equation.
 */

#if !defined(pylith_feassemble_integratorelasticity_hh)
#define pylith_feassemble_integratorelasticity_hh

#include "Integrator.hh" // ISA Integrator
#include "pylith/utils/array.hh" // USES std::vector, double_array
#include "pylith/utils/vectorfields.hh" // USES VectorFieldEnum

namespace pylith {
  namespace feassemble {
    class IntegratorElasticity;
    class TestIntegratorElasticity;
  } // feassemble

  namespace materials {
    class ElasticMaterial;
  } // feassemble
} // pylith

class pylith::feassemble::IntegratorElasticity : public Integrator
{ // IntegratorElasticity
  friend class TestIntegratorElasticity; // unit testing

// PUBLIC TYPEDEFS //////////////////////////////////////////////////////
public :

  typedef void (*totalStrain_fn_type)(double_array*,
				      const double_array&,
				      const double_array&,
				      const int,
				      const int);
  

// PUBLIC MEMBERS ///////////////////////////////////////////////////////
public :

  /// Constructor
  IntegratorElasticity(void);

  /// Destructor
  ~IntegratorElasticity(void);

  /** Set material.
   *
   * @param m Elastic material.
   */
  void material(materials::ElasticMaterial* m);

  /** Determine whether we need to recompute the Jacobian.
   *
   * @returns True if Jacobian needs to be recomputed, false otherwise.
   */
  bool needNewJacobian(void);

  /** Update state variables as needed.
   *
   * @param t Current time
   * @param fields Solution fields
   * @param mesh Finite-element mesh
   */
  void updateState(const double t,
		   topology::FieldsManager* const fields,
		   const ALE::Obj<Mesh>& mesh);

  /** Verify configuration is acceptable.
   *
   * @param mesh Finite-element mesh
   */
  void verifyConfiguration(const ALE::Obj<Mesh>& mesh);

  /** Get cell field associated with integrator.
   *
   * @param fieldType Type of field.
   * @param name Name of vertex field.
   * @param mesh PETSc mesh for problem.
   * @param fields Fields manager.
   * @returns Cell field.
   */
  const ALE::Obj<real_section_type>&
  cellField(VectorFieldEnum* fieldType,
	    const char* name,
	    const ALE::Obj<Mesh>& mesh,
	    topology::FieldsManager* const fields);


// PROTECTED METHODS ////////////////////////////////////////////////////
protected :

  /** Calculate stress or strain field from solution field.
   *
   * @param field Field in which to store stress or strain.
   * @param name Name of field to compute ('total-strain' or 'stress')
   * @param mesh PETSc mesh for problem.
   * @param fields Fields manager with solution.
   */
  void _calcStrainStressField(ALE::Obj<real_section_type>* field,
			      const char* name,
			      const ALE::Obj<Mesh>& mesh,
			      topology::FieldsManager* const fields);

  /** Calculate stress field from total strain field. Stress field
   * replaces strain field in section.
   *
   * @param field Field in which to store stress.
   * @param mesh PETSc mesh for problem.
   */
  void _calcStressFromStrain(ALE::Obj<real_section_type>* field,
			     const ALE::Obj<Mesh>& mesh);
			      

  /** Integrate elasticity term in residual for 1-D cells.
   *
   * @param stress Stress tensor for cell at quadrature points.
   */
  void _elasticityResidual1D(const double_array& stress);

  /** Integrate elasticity term in residual for 2-D cells.
   *
   * @param stress Stress tensor for cell at quadrature points.
   */
  void _elasticityResidual2D(const double_array& stress);

  /** Integrate elasticity term in residual for 3-D cells.
   *
   * @param stress Stress tensor for cell at quadrature points.
   */
  void _elasticityResidual3D(const double_array& stress);

  /** Integrate elasticity term in Jacobian for 1-D cells.
   *
   * @param elasticConsts Matrix of elasticity constants at quadrature points.
   */
  void _elasticityJacobian1D(const double_array& elasticConsts);

  /** Integrate elasticity term in Jacobian for 2-D cells.
   *
   * @param elasticConsts Matrix of elasticity constants at quadrature points.
   */
  void _elasticityJacobian2D(const double_array& elasticConsts);

  /** Integrate elasticity term in Jacobian for 3-D cells.
   *
   * @param elasticConsts Matrix of elasticity constants at quadrature points.
   */
  void _elasticityJacobian3D(const double_array& elasticConsts);

  /** Compute total strain in at quadrature points of a cell.
   *
   * @param strain Strain tensor at quadrature points.
   * @param basisDeriv Derivatives of basis functions at quadrature points.
   * @param disp Displacement at vertices of cell.
   * @param dimension Dimension of cell.
   * @param numBasis Number of basis functions for cell.
   * @param numQuadPts Number of quadrature points.
   */
  static
  void _calcTotalStrain1D(double_array* strain,
			  const double_array& basisDeriv,
			  const double_array& disp,
			  const int numBasis,
			  const int numQuadPts);

  /** Compute total strain in at quadrature points of a cell.
   *
   * @param strain Strain tensor at quadrature points.
   * @param basisDeriv Derivatives of basis functions at quadrature points.
   * @param disp Displacement at vertices of cell.
   * @param numBasis Number of basis functions for cell.
   * @param numQuadPts Number of quadrature points.
   */
  static
  void _calcTotalStrain2D(double_array* strain,
			  const double_array& basisDeriv,
			  const double_array& disp,
			  const int numBasis,
			  const int numQuadPts);

  /** Compute total strain in at quadrature points of a cell.
   *
   * @param strain Strain tensor at quadrature points.
   * @param basisDeriv Derivatives of basis functions at quadrature points.
   * @param disp Displacement at vertices of cell.
   * @param numBasis Number of basis functions for cell.
   * @param numQuadPts Number of quadrature points.
   */
  static
  void _calcTotalStrain3D(double_array* strain,
			  const double_array& basisDeriv,
			  const double_array& disp,
			  const int numBasis,
			  const int numQuadPts);

// NOT IMPLEMENTED //////////////////////////////////////////////////////
private :

  /// Not implemented.
  IntegratorElasticity(const IntegratorElasticity& i);

  /// Not implemented
  const IntegratorElasticity& operator=(const IntegratorElasticity&);

// PROTECTED MEMBERS ////////////////////////////////////////////////////
protected :

  /// Elastic material associated with integrator
  materials::ElasticMaterial* _material;

  /// Buffer for storing scalar cell field.
  ALE::Obj<real_section_type> _bufferCellScalar;

  /// Buffer for storing vector cell field.
  ALE::Obj<real_section_type> _bufferCellVector;

  /// Buffer for storing cell tensor field.
  ALE::Obj<real_section_type> _bufferCellTensor;

  /// Buffer for storing other cell fields.
  ALE::Obj<real_section_type> _bufferCellOther;

}; // IntegratorElasticity

#endif // pylith_feassemble_integratorelasticity_hh


// End of file 
