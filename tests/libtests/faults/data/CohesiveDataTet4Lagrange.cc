// =================================================================================================
// This code is part of PyLith, developed through the Computational Infrastructure
// for Geodynamics (https://github.com/geodynamics/pylith).
//
// Copyright (c) 2010-2025, University of California, Davis and the PyLith Development Team.
// All rights reserved.
//
// See https://mit-license.org/ and LICENSE.md and for license information.
// =================================================================================================

/* Original mesh
 *
 * Cells are 0-1, vertices are 2-6.
 *
 * 2   3,4,5  6
 *
 *     ^^^^^ Face in x-y plane
 *
 * After adding cohesive elements
 *
 * Cells are 0-1,2, vertices are 3-10,11-13.
 *
 * 3   4,5,6  8,9,10   7
 *             11,12,13
 *     ^^^^^^^^^^^^ Cohesive element in x-y plane.
 */

#include "CohesiveDataTet4Lagrange.hh"

const int pylith::faults::CohesiveDataTet4Lagrange::_numVertices = 8;

const int pylith::faults::CohesiveDataTet4Lagrange::_spaceDim = 3;

const int pylith::faults::CohesiveDataTet4Lagrange::_numCells = 3;

const int pylith::faults::CohesiveDataTet4Lagrange::_cellDim = 3;

const int pylith::faults::CohesiveDataTet4Lagrange::_numCorners[3] = {
    4,
    4,
    6
};

const int pylith::faults::CohesiveDataTet4Lagrange::_materialIds[3] = {
    0,  0,
    1
};

const int pylith::faults::CohesiveDataTet4Lagrange::_numGroups = 2;

const int pylith::faults::CohesiveDataTet4Lagrange::_groupSizes[2] = {
    5+4+1, 6+6+2, // vertices+edges+faces
};

const char* pylith::faults::CohesiveDataTet4Lagrange::_groupNames[2] = {
    "output", "fault"
};

const char* pylith::faults::CohesiveDataTet4Lagrange::_groupTypes[2] = {
    "vertex", "vertex"
};

const char* pylith::faults::CohesiveDataTet4Lagrange::_filename =
    "data/tet4.mesh";

pylith::faults::CohesiveDataTet4Lagrange::CohesiveDataTet4Lagrange(void) { // constructor
    numVertices = _numVertices;
    spaceDim = _spaceDim;
    numCells = _numCells;
    cellDim = _cellDim;
    numCorners = const_cast<int*>(_numCorners);
    materialIds = const_cast<int*>(_materialIds);
    groupSizes = const_cast<int*>(_groupSizes);
    groupNames = const_cast<char**>(_groupNames);
    groupTypes = const_cast<char**>(_groupTypes);
    numGroups = _numGroups;
    filename = const_cast<char*>(_filename);
} // constructor


pylith::faults::CohesiveDataTet4Lagrange::~CohesiveDataTet4Lagrange(void) {}


// End of file
