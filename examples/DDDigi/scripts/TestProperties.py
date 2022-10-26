# ==========================================================================
#  AIDA Detector description implementation
# --------------------------------------------------------------------------
# Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
# All rights reserved.
#
# For the licensing terms see $DD4hepINSTALL/LICENSE.
# For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
#
# ==========================================================================
from __future__ import absolute_import


def yes_no(val):
  if val:
    return "YES"
  return "NO "


def run():
  import DigiTest
  digi = DigiTest.Test(geometry=None)
  input = digi.input_action('DigiParallelActionSequence/Test')

  input.add_property('property_int', 1)
  print('property: has_property =    %s' % (yes_no(input.hasProperty('property_int')),))
  print('property: property_int =    %s' % (str(input.property_int),))
  input.property_int = 123456
  print('property: property_int =    %s' % (str(input.property_int),))

  input.add_vector_property('property_vector_int', [1, 2, 3])
  print('property: has_property =           %s' % (yes_no(input.hasProperty('property_vector_int')),))
  print('property: property_vector_int =    %s' % (str(input.property_vector_int),))
  input.property_vector_int = [1, 2, 3, 4, 5, 6, 7, 8, 9, 0]
  print('property: property_vector_int =    %s' % (str(input.property_vector_int),))

  input.add_list_property('property_list_int', [1, 2, 3])
  print('property: has_property =           %s' % (yes_no(input.hasProperty('property_list_int')),))
  print('property: property_list_int =      %s' % (str(input.property_list_int),))
  input.property_list_int = [1, 2, 3, 4, 5, 6, 7, 8, 9, 0]
  print('property: property_list_int =      %s' % (str(input.property_list_int),))

  input.add_set_property('property_set_int', [1, 2, 3])
  print('property: has_property =           %s' % (yes_no(input.hasProperty('property_set_int')),))
  print('property: property_set_int =       %s' % (str(input.property_set_int),))
  input.property_set_int = [1, 2, 3, 4, 5, 6, 7, 8, 9, 0]
  print('property: property_set_int =       %s' % (str(input.property_set_int),))

  input.add_property('property_double', 1.0)
  print('property: has_property =           %s' % (yes_no(input.hasProperty('property_double')),))
  print('property: property_double =        %s' % (str(input.property_double),))
  input.property_double = 123456.7
  print('property: property_double =        %s' % (str(input.property_double),))

  input.add_vector_property('property_vector_double', [1.1, 2, 3])
  print('property: has_property =           %s' % (yes_no(input.hasProperty('property_vector_double')),))
  print('property: property_vector_double = %s' % (str(input.property_vector_double),))
  input.property_vector_double = [1.5, 2, 3, 4, 5, 6, 7, 8, 9, 0]
  print('property: property_vector_double = %s' % (str(input.property_vector_double),))

  input.add_list_property('property_list_double', [1.1, 2, 3])
  print('property: has_property =           %s' % (yes_no(input.hasProperty('property_list_double')),))
  print('property: property_list_double =   %s' % (str(input.property_list_double),))
  input.property_list_double = [1.5, 2, 3, 4, 5, 6, 7, 8, 9, 0]
  print('property: property_list_double =   %s' % (str(input.property_list_double),))

  input.add_set_property('property_set_double', [1.1, 2, 3])
  print('property: has_property =           %s' % (yes_no(input.hasProperty('property_set_double')),))
  print('property: property_set_double =    %s' % (str(input.property_set_double),))
  input.property_set_double = [1.5, 2, 3, 4, 5, 6, 7, 8, 9, 0]
  print('property: property_set_double =    %s' % (str(input.property_set_double),))

  input.add_property('property_string', "string_1")
  print('property: has_property =           %s' % (yes_no(input.hasProperty('property_string')),))
  print('property: property_string =        %s' % (input.property_string,))
  input.property_string = "string_1123456"
  print('property: property_string =        %s' % (input.property_string,))

  input.add_vector_property('property_vector_string', ["string1", "string2", "string3"])
  print('property: has_property =           %s' % (yes_no(input.hasProperty('property_vector_string')),))
  print('property: property_vector_string = %s' % (input.property_vector_string,))
  input.property_vector_string = ["string1", "string2", "string3", "string4", "string5", "string6"]
  print('property: property_vector_string = %s' % (input.property_vector_string,))

  input.add_position_property('property_position', (1., 2., 3.))
  print('property: has_property =           %s' % (yes_no(input.hasProperty('property_position')),))
  print('property: property_position =      %s' % (input.property_position,))
  input.property_position = (111.1, 222.2, 333.3)
  print('property: property_position =      %s' % (input.property_position,))


if __name__ == '__main__':
  run()
