#!/usr/bin/python3
# coding=utf-8

import os
import struct

def main():
    '''
    计算 kernel.bin 占用的扇区数，并输出到 .ksize 文件中
    '''
    sectors = 0

    try:
        ksize = os.path.getsize('kernel.bin')
        with open('.ksize', 'wb+') as f:
            sectors = ksize // 512
            if ksize % 512 > 0:
                sectors = sectors + 1

            sectorsBytes = struct.pack('<i', sectors)
            f.write(sectorsBytes)

    except Exception as e:
        print('Kernel size error: %s' % e)

    print(sectors)


if __name__ == '__main__':
    main()
