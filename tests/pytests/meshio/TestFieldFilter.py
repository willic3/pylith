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
# @file tests/pytests/meshio/TestFieldFilter.py
#
# @brief Unit testing of Python FieldFilter object.

import unittest

from pylith.testing.UnitTestApp import TestAbstractComponent
from pylith.meshio.FieldFilter import FieldFilter


class TestFieldFilter(TestAbstractComponent):
    """Unit testing of FieldFilter object.
    """
    _class = FieldFilter


if __name__ == "__main__":
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(TestFieldFilter))
    unittest.TextTestRunner(verbosity=2).run(suite)


# End of file
