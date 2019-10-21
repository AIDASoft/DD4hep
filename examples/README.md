# DD4hep Examples

The subfolders in this directory contain several examples on how to use DD4hep. There are two ways to build these examples:
1. **Recommended way**: 
  to build all examples in this folder:
   ```
   mkir build
   cd build
   cmake ..
   make
   make install
   ctest --output-on-failure
   ```
    or to build a subset, use the variable `DD4HEP_BUILD_EXAMPLES` e.g.
      ```
   cmake -DDD4HEP_BUILD_EXAMPLES="OpticalSurfaces DDG4" ..
   make
   make install
   ctest --output-on-failure
   ```
2. Not-recommended way:
  It is also possible to build each subfolder separately, however in this case the tests are not enabled:
     ```
   cd OpticalSurfaces
   mkir build
   cd build
   cmake ..
   make
   make install
   ```

