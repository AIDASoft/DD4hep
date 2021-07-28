#!/usr/bin/env python
"""
Some imports to make sure that the DD4hep environment is complete.
This file adds a test for the DDG4 python module.
"""

from __future__ import absolute_import
from test_import import test_module, parametrize


moduleNames = [
    'DDG4',
    ]


@parametrize('moduleName', moduleNames)
def test_module_ddg4(moduleName):
    test_module(moduleName)
