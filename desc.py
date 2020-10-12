#!/usr/bin/python3
# coding=utf-8

import sys


def main():
    try:
        if 'gdt' == sys.argv[1]:
            gdt(sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5],
                sys.argv[6], sys.argv[7], sys.argv[8], sys.argv[9],
                sys.argv[10], sys.argv[11])

    except Exception as e:
        print(e)
        usage()


def usage():
    print('Usage:\t./desc.py gdt base limit G D/B L AVL P DPL S Type')
    print('      \t./desc.py gdt 0x00000000 0xFFFFF 1 1 0 0 1 00 1 1000')
    print('      \t./desc.py gdt 0x00000000 0xFFFFF 1 1 0 0 1 00 1 0010')


def gdt(base, limit, G, DB, L, AVL, P, DPL, S, Type):
    gdBytes = []

    # limit 15 ~ 0
    limit = int(limit, 16)
    gdBytes.append(limit & 0x000000FF)
    gdBytes.append((limit & 0x0000FF00) >> 8)

    # Base 23 ~ 0
    base = int(base, 16)
    gdBytes.append(base & 0x000000FF)
    gdBytes.append((base & 0x0000FF00) >> 8)
    gdBytes.append((base & 0x00FF0000) >> 16)

    # P DPL S Type
    P = int(P, 2)
    DPL = int(DPL, 2)
    S = int(S, 2)
    Type = int(Type, 2)
    gdBytes.append((P << 7) | (DPL << 5) | (S << 4) | Type)

    # G D/B L AVL Limit 15 ~ 0
    G = int(G, 2)
    DB = int(DB, 2)
    L = int(L, 2)
    AVL = int(AVL, 2)
    gdBytes.append((G << 7) | (DB << 6) | (L << 5) | (AVL << 4)
                   | ((limit & 0x000F0000) >> 16))

    # Base 31 ~ 24
    gdBytes.append((base & 0xFF000000) >> 24)

    gdBytes = ["{:02X}h".format(b) for b in gdBytes]

    print(', '.join(gdBytes))

if __name__ == '__main__':
    main()
