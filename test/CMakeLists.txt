#
# Copyright (C) 2022 Nikolas Koesling <nikolas@koesling.info>.
# This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
#

function(create_test name)
    add_executable(test_${name}_${Target} test_${name}.cpp)
    target_link_libraries(test_${name}_${Target} PRIVATE ${Target})
    add_test(test_${name}_${Target} test_${name}_${Target})

    enable_warnings(test_${name}_${Target})
    set_definitions(test_${name}_${Target})

    if(CLANG_FORMAT_ENABLED)
        target_clangformat_setup(test_${name}_${Target})
    endif()

    target_link_libraries(test_${name}_${Target} PRIVATE rt)
endfunction()

# test all

create_test("all")

