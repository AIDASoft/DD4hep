from ROOT import gSystem
gSystem.Load('libDDPython')
from ROOT import DD4hep as Core

name_space = __import__(__name__)
def import_namespace_item(ns,nam):  
  scope = getattr(name_space,ns)
  attr = getattr(scope,nam)
  setattr(name_space,nam,attr)
  return attr

def a_func():
  print 'Hello world'
  return 1

class a_class:
  def __init__(self):
    pass
  def fcn(self):
    print 'Hello world from member function fcn'
    return 1
  def fcn_except(self,args,aa):
    print 'Hello world from member function fcn1 a1=',args,' a2=',aa
    raise RuntimeError('Except from python test object a_class')
    return 6
      

py = import_namespace_item('Core','DDPython')

print '+++++ Test: Execute statements in python with C++ indirection'
py.instance().execute('import sys')
py.instance().execute('print "Arguments:", sys.argv')
print '\n'

obj=a_class()
import sys, traceback

print '+++++ Test: simple function call'
ret = py.instance().call(a_func,None)
print 'ret:',ret
print '\n'

print '+++++ Test: object method call'
ret = py.instance().call(obj.fcn,None)
print 'ret:',ret
print '\n'

print '+++++ Test: object method call with non callable'
try: 
  ret = py.instance().call(1,None)
  print 'ret:',ret
except:
  traceback.print_exc()
print '\n'

print '+++++ Test: object method call with exception in python callback'
try: 
  ret = py.instance().call(obj.fcn_except,(1,[1,2,3,4,5,6],))
  print 'ret:',ret
except:
  traceback.print_exc()
print '\n'
print '+++++ All Done....\n\n'
print 'TEST_PASSED'

#py.instance().prompt()

sys.exit(0)

