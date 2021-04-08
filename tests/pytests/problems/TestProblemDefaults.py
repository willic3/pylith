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
# @file tests/pytests/problems/TestProblemDefaults.py
#
# @brief Unit testing of Python ProblemDefaults object.

import unittest

from pylith.testing.UnitTestApp import TestComponent
from pylith.problems.ProblemDefaults import (ProblemDefaults, problem_defaults)


class TestProblemDefaults(TestComponent):
    """Unit testing of ProblemDefaults object.
    """
    _class = ProblemDefaults
    _factory = problem_defaults


if __name__ == "__main__":
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(TestProblemDefaults))
    unittest.TextTestRunner(verbosity=2).run(suite)


# End of file