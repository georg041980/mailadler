# Sicherer-Bau-Modus — Sanitizer für die Entwicklung
# Aktiviert mit: cmake -B bau -DCMAKE_BUILD_TYPE=Debug -DSICHERER_BAU=ON
option(SICHERER_BAU "Adress- und Undefined-Behavior-Sanitizer aktivieren" OFF)

if(SICHERER_BAU)
    message(STATUS "🛡️  Sicherer-Bau-Modus AKTIV — Sanitizer prüfen mit")

    # AddressSanitizer: Pufferüberlauf, Use-after-free, Stack-Overflow, …
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)

    # UndefinedBehaviorSanitizer: Null-Pointer, Integer-Überlauf, …
    add_compile_options(-fsanitize=undefined)

    # Optional: LeakSanitizer (ist in ASan enthalten, aber separat abschaltbar)
    set(ENV{ASAN_OPTIONS} "detect_leaks=1:detect_stack_use_after_return=1")

    message(STATUS "  • AddressSanitizer    (Pufferüberlauf, Use-after-free)")
    message(STATUS "  • UndefinedBehavior   (Null-Pointer, Integer-Überlauf)")
    message(STATUS "  • LeakSanitizer       (Speicherlecks)")
endif()
