from pwn import *

def test_elf():
	f = open('test1', 'rb')
	buf = f.read()
	p.sendline(str(len(buf)))
	pause()
	p.send(buf)
	f = open('test2', 'rb')
	buf = f.read()
	p.sendline(str(len(buf)))
	pause()
	p.send(buf)


if __name__ == "__main__":
	p = remote('0.0.0.0', 31337)
	test_elf()
	p.interactive()
