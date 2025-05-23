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
#include <stdexcept>

#include "pylith/utils/GenericComponent.hh" // ISA GenericComponent

#include "pylith/meshio/ExodusII.hh"

#include "pylith/utils/array.hh" // USES int_array, scalar_array, string_vector
#include "pylith/utils/error.h" // USES PYLITH_METHOD_BEGIN/END

#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"

namespace pylith {
    namespace meshio {
        class TestExodusII;
    } // meshio
} // pylith

// ------------------------------------------------------------------------------------------------
class pylith::meshio::TestExodusII : public pylith::utils::GenericComponent {
    // PUBLIC METHODS /////////////////////////////////////////////////////
public:

    /// Test constructor
    static
    void testConstructor(void);

    /// Test filename()
    static
    void testFilename(void);

    /// Test open() and close().
    static
    void testOpenClose(void);

    /// Test hasDim()
    static
    void testHasDim(void);

    /// Test hasAttr()
    static
    void testHasAttr(void);

    /// Test hasVar()
    static
    void testHasVar(void);

    /// Test getAttr(string)
    static
    void testGetAttr(void);

    /// Test getVar(PylithScalar*)
    static
    void testGetVarDouble(void);

    /// Test getVar(int*)
    static
    void testGetVarInt(void);

    /// Test getVar(string_vector)
    static
    void testGetVarString(void);

}; // class TestExodusII

// ------------------------------------------------------------------------------------------------
TEST_CASE("TestExodusII::testConstructor", "[TestExodusII]") {
    pylith::meshio::TestExodusII::testConstructor();
}
TEST_CASE("TestExodusII::testFilename", "[TestExodusII]") {
    pylith::meshio::TestExodusII::testFilename();
}
TEST_CASE("TestExodusII::testOpenClose", "[TestExodusII]") {
    pylith::meshio::TestExodusII::testOpenClose();
}
TEST_CASE("TestExodusII::testHasDim", "[TestExodusII]") {
    pylith::meshio::TestExodusII::testHasDim();
}
TEST_CASE("TestExodusII::testHasAttr", "[TestExodusII]") {
    pylith::meshio::TestExodusII::testHasAttr();
}
TEST_CASE("TestExodusII::testHasVar", "[TestExodusII]") {
    pylith::meshio::TestExodusII::testHasVar();
}
TEST_CASE("TestExodusII::testGetAttr", "[TestExodusII]") {
    pylith::meshio::TestExodusII::testGetAttr();
}
TEST_CASE("TestExodusII::testGetVarDouble", "[TestExodusII]") {
    pylith::meshio::TestExodusII::testGetVarDouble();
}
TEST_CASE("TestExodusII::testGetVarInt", "[TestExodusII]") {
    pylith::meshio::TestExodusII::testGetVarInt();
}
TEST_CASE("TestExodusII::testGetVarString", "[TestExodusII]") {
    pylith::meshio::TestExodusII::testGetVarString();
}

// ------------------------------------------------------------------------------------------------
// Test constructor
void
pylith::meshio::TestExodusII::testConstructor(void) {
    PYLITH_METHOD_BEGIN;

    ExodusII exofile;

    PYLITH_METHOD_END;
} // testConstructor


// ------------------------------------------------------------------------------------------------
// Test filename()
void
pylith::meshio::TestExodusII::testFilename(void) {
    PYLITH_METHOD_BEGIN;

    ExodusII exofile;

    const std::string& filename = "hi.txt";
    exofile.filename(filename.c_str());
    CHECK(filename == exofile.filename());

    PYLITH_METHOD_END;
} // testFilename


// ------------------------------------------------------------------------------------------------
// Test open() and close();
void
pylith::meshio::TestExodusII::testOpenClose(void) {
    ExodusII exofile("data/twotri3_13.0.exo");
    assert(exofile._file);

    exofile.close();
    assert(!exofile._file);

    // Attempt to open file that doesn't exist.
    exofile.filename("fail.exo");
    CHECK_THROWS_AS(exofile.open(), std::runtime_error);

    // Attempt to close file with bad handle.
    exofile._file = 1;
    CHECK_THROWS_AS(exofile.close(), std::runtime_error);
} // testOpenClose


// ------------------------------------------------------------------------------------------------
// Test hasDim()
void
pylith::meshio::TestExodusII::testHasDim(void) {
    PYLITH_METHOD_BEGIN;

    ExodusII exofile("data/twotri3_13.0.exo");

    int id = -1;
    CHECK(exofile.hasDim("num_dim", &id));
    CHECK(id >= 0);
    CHECK(!exofile.hasDim("abcdefghijklm", &id));
    CHECK(-1 == id);

    PYLITH_METHOD_END;
} // testHasDim


// ------------------------------------------------------------------------------------------------
// Test hasAttr()
void
pylith::meshio::TestExodusII::testHasAttr(void) {
    PYLITH_METHOD_BEGIN;

    ExodusII exofile("data/twotri3_13.0.exo");

    int id = -1;
    CHECK(exofile.hasAttr("version", nullptr, &id));
    CHECK(id >= 0);
    CHECK(!exofile.hasAttr("abcdefghijklm", "connect1", &id));
    CHECK(-1 == id);

    PYLITH_METHOD_END;
} // testHasAttr


// ------------------------------------------------------------------------------------------------
// Test getAttr()
void
pylith::meshio::TestExodusII::testGetAttr(void) {
    PYLITH_METHOD_BEGIN;

    ExodusII exofile("data/twotri3_13.0.exo");

    std::string cellShape;
    exofile.getAttr(&cellShape, "connect1", "elem_type");
    CHECK(cellShape == std::string("TRI3"));

    std::string missing;
    CHECK_THROWS_AS(exofile.getAttr(&missing, nullptr, "aabbcc"), std::runtime_error);

    PYLITH_METHOD_END;
} // testGetAttr


// ------------------------------------------------------------------------------------------------
// Test hasVar()
void
pylith::meshio::TestExodusII::testHasVar(void) {
    PYLITH_METHOD_BEGIN;

    ExodusII exofile("data/twotri3_13.0.exo");

    int id = -1;
    CHECK(exofile.hasVar("connect1", &id));
    CHECK(id >= 0);
    CHECK(!exofile.hasVar("abcdefghijklm", &id));
    CHECK(-1 == id);

    PYLITH_METHOD_END;
} // testHasVar


// ------------------------------------------------------------------------------------------------
// Test getVar(PylithScalar*).
void
pylith::meshio::TestExodusII::testGetVarDouble(void) {
    const PylithScalar coordsE[8] = { -1.0, 0.0, 0.0, 1.0,
                                      0.0, -1.0, 1.0, 0.0 };

    const int ndims = 2;
    int dims[2];
    dims[0] = 2;
    dims[1] = 4;
    const int size = dims[0]*dims[1];
    scalar_array coords(size);

    ExodusII exofile("data/twotri3_12.2.exo");
    exofile.getVar(&coords[0], dims, ndims, "coord");

    const PylithScalar tolerance = 1.0e-06;
    for (int i = 0; i < size; ++i) {
        CHECK_THAT(coords[i], Catch::Matchers::WithinAbs(coordsE[i], tolerance));
    }

    // Attempt to get variable that doesn't exist.
    CHECK_THROWS_AS(exofile.getVar(&coords[0], dims, ndims, "aabbcc"), std::runtime_error);

    // Attempt to get variable with wrong number of dimensions.
    CHECK_THROWS_AS(exofile.getVar(&coords[0], dims, ndims+1, "coord"), std::runtime_error);

    // Attempt to get variable with wrong dimension.
    dims[0] = 99;
    CHECK_THROWS_AS(exofile.getVar(&coords[0], dims, ndims, "coord"), std::runtime_error);
} // testGetVarDouble


// ------------------------------------------------------------------------------------------------
// Test getVar(int*).
void
pylith::meshio::TestExodusII::testGetVarInt(void) {
    const int connectE[3] = { 3, 2, 4 };

    const int ndims = 2;
    int dims[2];
    dims[0] = 1;
    dims[1] = 3;
    const int size = dims[0]*dims[1];
    int_array connect(size);

    ExodusII exofile("data/twotri3_13.0.exo");
    exofile.getVar(&connect[0], dims, ndims, "connect2");

    for (int i = 0; i < size; ++i) {
        CHECK(connectE[i] == connect[i]);
    }

    // Attempt to get variable that doesn't exist.
    CHECK_THROWS_AS(exofile.getVar(&connect[0], dims, ndims, "aabbcc"), std::runtime_error);

    // Attempt to get variable with wrong number of dimensions.
    CHECK_THROWS_AS(exofile.getVar(&connect[0], dims, ndims+1, "connect2"), std::runtime_error);

    // Attempt to get variable with wrong dimension.
    dims[0] = 99;
    CHECK_THROWS_AS(exofile.getVar(&connect[0], dims, ndims, "connect2"), std::runtime_error);
} // testGetVarDouble


// ------------------------------------------------------------------------------------------------
// Test getVar(string_vector).
void
pylith::meshio::TestExodusII::testGetVarString(void) {
    const char* namesE[2] = { "x", "y" };

    const int dim = 2;
    string_vector names(2);

    ExodusII exofile("data/twotri3_13.0.exo");
    exofile.getVar(&names, dim, "coor_names");

    for (int i = 0; i < dim; ++i) {
        CHECK(std::string(namesE[i]) == names[i]);
    }

    // Attempt to get variable that doesn't exist.
    CHECK_THROWS_AS(exofile.getVar(&names, dim, "aabbcc"), std::runtime_error);

    // Attempt to get variable with wrong number of dimensions.
    CHECK_THROWS_AS(exofile.getVar(&names, dim+1, "coord_names"), std::runtime_error);
} // testGetVarString


// End of file
