#!/bin/env python3

import sys

def main(langs, dict_file):
    keys = []
    keyval = 1
    for l in langs:
        filename = l + '.txt'
        f = open(filename, 'r')
        words = f.read().split('\n')
        keys.extend([(word, keyval) for word in words])
        keyval *= 2

    dictionary = {}
    for (w,k) in keys:
        if w in dictionary:
            dictionary[w] |= k
        else:
            dictionary[w] = k

    dictionary_file = open(dict_file, 'w')
    dictionary_file.write('\n'.join([word + ' ' + str(num) for (word,num) in dictionary.items()]))

if __name__ == '__main__':
    main(sys.argv[1:-1], sys.argv[-1])
