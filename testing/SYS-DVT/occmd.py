import os
import sys
import json
import subprocess
import time
from dvt import *
from reliability import *

DB_FILE = "data.json"

def main():
    a = dvt(DB_FILE)
    a.load_db()
    a.load_args(sys.argv)

    if(a.arglist[MODULE_NAME] == "reliability"):
        run_reliabilty(a)
        sys.exit(0)

    if(a.parse()):
        print a.db['module']['help'] 
                        
    sys.exit(0)


if __name__ == '__main__': main()
