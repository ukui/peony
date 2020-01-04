# peony-extensions

## Getting Start
- Make sure all build dependencies have been installed in your machine. You can reference debian/control for building depends.

* Create *build* directory and change to it, then build with cmake. The follow
  commands will auto build extensions as library.

``` sh
cmake ..
make
```
- Put the generated libraries into /usr/lib/peony-qt-extensions.
  
- Test the extension with peony.