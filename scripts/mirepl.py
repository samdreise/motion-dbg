from pygdbmi import gdbmiparser
from pygdbmi.gdbcontroller import GdbController
from pprint import pprint
response = gdbmiparser.parse_response('5^done,name="var1",numchild="0",value="32767",type="int",thread-id="1",has_more="0"')
pprint(response)

