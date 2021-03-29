#!/usr/bin/env nemesis
#
# ======================================================================
#
# Brad T. Aagaard, U.S. Geological Survey
# Charles A. Williams, GNS Science
# Matthew G. Knepley, University of Chicago
#
# This code was developed as part of the Computational Infrastructure
# for Geodynamics (http://geodynamics.org).
#
# Copyright (c) 2010-2017 University of California, Davis
#
# See COPYING for license information.
#
# ======================================================================
#
# @file tests/pytests/meshio/TestFieldFilterNone.py
#
# @brief Unit testing of Python FieldFilterNone object.

import unittest

from pylith.testing.UnitTestApp import TestComponent
from pylith.meshio.FieldFilterNone import (FieldFilterNone, output_field_filter)


class TestFieldFilterNone(TestComponent):
    """Unit testing of FieldFilterNone object.
    """
    _class = FieldFilterNone
    _factory = output_field_filter


if __name__ == "__main__":
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(TestFieldFilterNone))
    unittest.TextTestRunner(verbosity=2).run(suite)


# End of file
