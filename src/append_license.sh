#!/bin/bash

project_name="gpucc"
year="2023"
name="Jasper Parker"
email="j@sperp.dev"

function append() {
    echo "// Copyright (C) $year $name <$email>
//
// This file is part of $project_name.
//
// $project_name is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// $project_name is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with $project_name. If not, see <https://www.gnu.org/licenses/>.


$(cat $1)" > $1
}

#
# for file in ./*.cpp
# do
#     append $file
# done
#
# for file in ./*/*.cpp
# do
#     append $file
# done
#
# for file in ./*.h
# do
#     append $file
# done
#
# for file in ./*/*.h
# do
#     append $file
# done
#
# for file in ./*/*/*.cpp
# do
#     append $file
# done
#
# for file in ./*/*/*.h
# do
#     append $file
# done
#
# for file in ./*/*.glsl
# do
#     append $file
# done
