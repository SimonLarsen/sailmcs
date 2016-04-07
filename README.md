SailMCS
=======

SailMCS is a heuristic algorithm for the multiple maximum common subgraph
problem on simple (undirected, unweighted) graphs.

## Download

The source code is available at [http://github.com/SimonLarsen/sailmcs](http://github.com/SimonLarsen/sailmcs).

Test data and supplementary material is available [here](files/sailmcs_data.zip).

## Compilation

Requirements:

* Modern C++ compiler supporting C++11 and OpenMP
* Make
* CMake &gt;= 2.8
* Boost headers
* help2man (optional, used for man page generation)

With all requirements installed, run the following commands:

    git clone --recursive https://github.com/SimonLarsen/sailmcs.git
    cd sailmcs
    cmake . && make

Optionally, you can install the binary and documentation with:

    sudo make install

## Usage

To align three graphs for 10 minutes run:

    sailmcs -t 10:00 -o alignment.txt -g mcs.sif graph1.sif graph2.sif graph3.sif

The maximum common subgraph will be written to a file `mcs.sif` and the table of vertex alignments
will be written to `alignment.txt`.

## Supported formats

For a list of supported graph formats see [here](https://github.com/SimonLarsen/graphio/wiki/Formats).

## License

SailMCS is free software: you can redistribute it and/or modify it under the terms of the
GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the [GNU General Public License](http://www.gnu.org/licenses) for more details.

## Citation

To be announced.
