#!/usr/bin/env python
import os
import sys
import time

def main():
	start = time.time()
	print(jump(30))
	end = time.time()
	print(end - start)


def jump(steps):
	if steps > 0:
		return jump(steps - 1) + jump(steps - 2) + jump(steps - 3)
	else: 
		return 1;

if __name__ == '__main__':
    main()

