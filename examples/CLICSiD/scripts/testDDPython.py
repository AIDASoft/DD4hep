from __future__ import absolute_import, unicode_literals
import traceback
import sys
from ROOT import gSystem

import os
import logging
import platform
if platform.system() == "Darwin":
  gSystem.SetDynamicPath(os.environ['DD4HEP_LIBRARY_PATH'])
gSystem.Load('libglapi')
gSystem.Load('libDDPython')
from ROOT import dd4hep as Core  # noqa

logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)

name_space = __import__(__name__)


def import_namespace_item(ns, nam):
  scope = getattr(name_space, ns)
  attr = getattr(scope, nam)
  setattr(name_space, nam, attr)
  return attr


def a_func():
  logger.info('Hello world')
  return 1


class a_class:
  def __init__(self):
    pass

  def fcn(self):
    logger.info('Hello world from member function fcn')
    return 1

  def fcn_except(self, args, aa):
    logger.info('Hello world from member function fcn1 a1=%s a2=%s', str(args), str(aa))
    raise RuntimeError('Except from python test object a_class')
    return 6


py = import_namespace_item('Core', 'DDPython')

logger.info('+++++ Test: Execute statements in python with C++ indirection')
py.instance().execute(str('import sys, logging'))
py.instance().execute(str('logging.info("Arguments: %s", str(sys.argv))'))
logger.info('\n')

obj = a_class()

logger.info('+++++ Test: simple function call')
ret = py.instance().call(a_func, None)
logger.info('ret: %s', str(ret))
logger.info('\n')

logger.info('+++++ Test: object method call')
ret = py.instance().call(obj.fcn, None)
logger.info('ret: %s', str(ret))
logger.info('\n')

logger.info('+++++ Test: object method call with non callable')
try:
  ret = py.instance().call(1, None)
  logger.info('ret: %s', str(ret))
except Exception:
  traceback.print_exc()
logger.info('\n')

logger.info('+++++ Test: object method call with exception in python callback')
try:
  ret = py.instance().call(obj.fcn_except, (1, [1, 2, 3, 4, 5, 6],))
  logger.info('ret: %s', str(ret))
except Exception:
  traceback.print_exc()
logger.info('\n')
logger.info('+++++ All Done....\n\n')
logger.info('TEST_PASSED')

# py.instance().prompt()

sys.exit(0)
