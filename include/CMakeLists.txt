#
# Copyright (C) 2022 Nikolas Koesling <nikolas@koesling.info>.
# This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
#

# ---------------------------------------- header files (*.hpp, *.h, ...) ----------------------------------------------
# ======================================================================================================================

target_sources(${Target} PRIVATE ${PROJECT_NAME}_version_info.hpp)
target_sources(${Target} PRIVATE cxxshm.hpp)

# ---------------------------------------- subdirectories --------------------------------------------------------------
# ======================================================================================================================
