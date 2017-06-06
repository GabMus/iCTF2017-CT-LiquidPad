#!/usr/bin/env python2 
import re, string, sys
from pwn import *
from random import randint
import sys

sys.path.append('./utilities/')
from utils import sendFlag
from utils import getFlagID

def genRndString():
    rang = randint(8,16)
    rndString = ''.join(random.choice(string.ascii_uppercase + string.digits) for _ in range(rang))
    return rndString 

def invalidFlag(ip, port, flag_id):
    flag_rnd = genRndString()
    password = genRndString()
    c = remote(ip,port)
    for count in range(0,4):
            c.recvline()
    c.sendline("A")
    c.recvline()
    c.sendline(flag_id)
    c.sendline(password)
    c.sendline(flag_rnd)
    c.recvline()
    c.close()
    return {"flag_id": flag_id, "token": password}

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: %s <ip>" % os.path.basename(__file__))
        sys.exit(1)
    else:
        print invalidFlag(sys.argv[1], 20066, getFlagID("blackgold"))
