# Einheitliche Kompilierwarnungen für das gesamte Projekt
function(adlermail_setze_warnungen ziel)
    target_compile_options(${ziel} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wshadow
        -Wold-style-cast
        -Woverloaded-virtual
        -Wnon-virtual-dtor
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
        -Wimplicit-fallthrough
    )
endfunction()
