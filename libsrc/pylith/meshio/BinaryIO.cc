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

#include "pylith/meshio/BinaryIO.hh" // implementation of class methods

#include <fstream> // USES std::ifstream
#include <cassert> // USES assert()

// ---------------------------------------------------------------------------------------------------------------------
// Read fixed length string from file.
std::string
pylith::meshio::BinaryIO::readString(std::ifstream& fin,
                                     const int numChars) { // readString
    std::string bstring = "";
    if (numChars > 0) {
        char* buffer = new char[numChars+1];
        buffer[0] = '\0';
        fin.read(buffer, sizeof(char)*numChars);
        buffer[numChars] = '\0';

        // get string from buffer
        std::string bufstring = buffer;
        delete[] buffer;buffer = 0;

        // remove whitespace
        const int iLast = bufstring.find_first_of(" ");
        bstring = bufstring.substr(0, iLast);
    } // if
    return std::string(bstring);
} // readString


// ---------------------------------------------------------------------------------------------------------------------
// Change endian type by swapping byte order.
void
pylith::meshio::BinaryIO::swapByteOrder(char* vals,
                                        const int numVals,
                                        const int typesize) { // swapByteOrder
    assert(vals);
    const int numSwaps = sizeof(typesize) / 2;
    for (int iVal = 0; iVal < numVals; ++iVal) {
        char* buf = (char*) (vals + iVal*typesize);
        for (int iSwap = 0, jSwap = typesize-1; iSwap < numSwaps; ++iSwap, --jSwap) {
            char tmp = buf[iSwap];
            buf[iSwap] = buf[jSwap];
            buf[jSwap] = tmp;
        } // for
    } // for
} // swapByteOrder


// End of file
