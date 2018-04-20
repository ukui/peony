cmake_minimum_required(VERSION 2.6)
project(PeonyAdmin NONE)
find_package(Gettext REQUIRED)

# Configurable paths
set(PKEXEC_PATH "/usr/bin/pkexec" CACHE FILEPATH "pkexec program path.")
set(PEONY_PATH "/usr/bin/peony" CACHE FILEPATH "peony program path.")
set(PLUMA_PATH "/usr/bin/pluma" CACHE FILEPATH "pluma program path.")
set(TERMINAL_PATH "/usr/bin/mate-terminal" CACHE FILEPATH "mate-terminal program path.")

# Configure files
configure_file(extension/peony-admin.py.in
               extension/peony-admin.py)
configure_file(polkit/org.freedesktop.peony-admin.policy.in
               polkit/org.freedesktop.peony-admin.policy)

# Update and compile the translations
gettext_create_translations(po/peony-admin.pot ALL
	po/ar.po
	po/de.po
	po/es.po
	po/fr.po
	po/it.po
	po/lt.po
	po/pl.po
	po/pt.po
	po/ru.po
	po/sk.po
	po/zh_CN.po)

# Install target
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/polkit/org.freedesktop.peony-admin.policy"
        DESTINATION /usr/share/polkit-1/actions)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/extension/peony-admin.py"
        DESTINATION /usr/share/peony-python/extensions)

# Uninstall target
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake" IMMEDIATE @ONLY)
add_custom_target(uninstall COMMAND ${CMAKE_COMMAND}
                  -P ${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake)
