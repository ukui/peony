Installing Peony Admin from source
=====================================

1.  Install the dependencies.
    In Ubuntu and Debian, these are the known dependencies:

    *   cmake
    *   pluma *(optional)*
    *   gettext
    *   mate-terminal *(optional)*
    *   policykit-1
    *   peony-python

2.  Open a terminal in the project directory and run:

        mkdir build
        cd build
        cmake ..
        make
        sudo make install

3.  If Peony is running, restart it:

        peony -q

    Then start it again.
