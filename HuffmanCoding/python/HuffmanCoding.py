#!/usr/bin/env python
from io import StringIO
import os
import sys
import queue
import time
from typing import Any


class Node:
    def __init__(self, ch: chr, freq: int, left, right) -> None:
        self.freq = freq
        self.ch = ch
        self.left = left
        self.right = right

    def __lt__(self, other):
        return (self.freq < other.freq)

    def __gt__(self, other):
        return (self.freq > other.freq)

    def isLeaf(self) -> bool:
        return (self.left == None) and (self.right == None)

    def __str__(self):
        return '<' + self.ch + ' ' + str(self.freq) + '>'


def buildTrie(freq) -> Node:
    pq = queue.PriorityQueue()

    for i in range(256):
        if freq[i] > 0:
            pq.put(Node(chr(i), freq[i], None, None))

    while pq.qsize() > 1:
        left = pq.get()
        right = pq.get()
        pq.put(Node('0', left.freq + right.freq, left, right))

    return pq.get()


def buildCode(st, x, s):
    if not x.isLeaf():
        buildCode(st, x.left, s + '0')
        buildCode(st, x.right, s + '1')
    else:
        st[ord(x.ch)] = s


def compress(chars):
    compressed = []

    freq = [0] * 256

    for ch in chars:
        freq[ord(ch)] += 1

    root = buildTrie(freq)

    st = [""] * 256

    buildCode(st, root, "")

    for ch in chars:
        s = list(st[ord(ch)])
        for c in s:
            if c == '0':
                compressed.append(False)
            elif c == '1':
                compressed.append(True)

    return compressed, root


def expand(compressed, root, inputLength):

    #file_wtr = StringIO()

    output = ""

    j = 0
    for i in range(inputLength):
        x = root
        while not x.isLeaf():
            bit = compressed[j]
            j += 1
            if bit:
                x = x.right
            else:
                x = x.left
        output += str(x.ch)
        # file_wtr.write(str(x.ch))

    # return file_wtr.getvalue()
    return output


def main():
    start = time.time()
    fileObject = open("tale.txt", "r")
    data = fileObject.read()
    chars = list(data)
    inputLength = chars.__len__()

    compressed, root = compress(chars)

    print("Input:" + str(inputLength) +
          " Compressed:" + str(compressed.__len__()/8))

    compressEnd = time.time()
    print(compressEnd - start)

    output = expand(compressed, root, inputLength)

    expandEnd = time.time()
    print(expandEnd - start)

    if data == output:
        print("Success")

    end = time.time()
    print(end - start)


if __name__ == '__main__':
    main()
