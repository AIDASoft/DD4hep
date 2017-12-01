from ROOT import gSystem

import os, logging, platform
if platform.system()=="Darwin":
  gSystem.SetDynamicPath(os.environ['DD4HEP_LIBRARY_PATH'])
gSystem.Load('libDDPython')
from ROOT import dd4hep as Core

logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.DEBUG)

name_space = __import__(__name__)
def import_namespace_item(ns,nam):  
  scope = getattr(name_space,ns)
  attr = getattr(scope,nam)
  setattr(name_space,nam,attr)
  return attr

def a_func():
  logging.info('Hello world')
  return 1

class a_class:
  def __init__(self):
    pass
  def fcn(self):
    logging.info('Hello world from member function fcn')
    return 1
  def fcn_except(self,args,aa):
    logging.info('Hello world from member function fcn1 a1=%s a2=%s',str(args),str(aa))
    raise RuntimeError('Except from python test object a_class')
    return 6
      

py = import_namespace_item('Core','DDPython')

logging.info('+++++ Test: Execute statements in python with C++ indirection')
py.instance().execute('import sys, logging')
py.instance().execute('logging.info("Arguments: %s", str(sys.argv))')
logging.info('\n')

obj=a_class()
import sys, traceback

logging.info('+++++ Test: simple function call')
ret = py.instance().call(a_func,None)
logging.info('ret: %s',str(ret))
logging.info('\n')

logging.info('+++++ Test: object method call')
ret = py.instance().call(obj.fcn,None)
logging.info('ret: %s',str(ret))
logging.info('\n')

logging.info('+++++ Test: object method call with non callable')
try: 
  ret = py.instance().call(1,None)
  logging.info('ret: %s',str(ret))
except:
  traceback.print_exc()
logging.info('\n')

logging.info('+++++ Test: object method call with exception in python callback')
try: 
  ret = py.instance().call(obj.fcn_except,(1,[1,2,3,4,5,6],))
  logging.info('ret: %s',str(ret))
except:
  traceback.print_exc()
logging.info('\n')
logging.info('+++++ All Done....\n\n')
logging.info('TEST_PASSED')

#py.instance().prompt()

sys.exit(0)

