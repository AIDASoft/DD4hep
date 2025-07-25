#!/usr/bin/env python
"""
Some imports to make sure that the DD4hep environment is complete.
Since it can be disabled in CMake, the import of DDG4 is tested in another file.
"""
import traceback
import warnings
import pytest

parametrize = pytest.mark.parametrize

ImportFehler = ImportError
FEHLER = "ERROR: "

moduleNames = [
    'dd4hep',
    'DDRec',
    'dddigi',
    ]

# List here the modules that are allowed to Fail.
# Ideally, this should always be empty...
ALLOWED_TO_FAIL = []

# List of modules that need graphic libraries.
# When failing, these tests are just marked as skipped with a warning
GRAPHIC_MODULES = []


@parametrize('moduleName', moduleNames)
def test_module(moduleName):
  """ Try to import a module from DD4hep.

      Modules that are in the ALLOWED_TO_FAIL list are shown as skipped and generate a warning

      Modules that require graphic libraries (GRAPHIC_MODULES) are skipped on container
  """

  try:
    __import__(moduleName)

    # Test whether it is correctly imported from DD4hep

  except ImportFehler as e:
    msg = "could not import %s: %s" % (moduleName, repr(e))
    print(traceback.print_exc())

    if moduleName in ALLOWED_TO_FAIL:
      warnings.warn(msg, stacklevel=2)
      pytest.skip("WARN: " + msg)
    elif moduleName in GRAPHIC_MODULES:
      warnings.warn(msg + "(Possibly due to system graphic libraries not present)", stacklevel=2)
      pytest.skip("WARN: " + msg + "(Possibly due to system graphic libraries not present)")
    else:
      pytest.fail(FEHLER + msg)
