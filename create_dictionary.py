#!/bin/env python3

from random import shuffle

english_file = open('English.txt', 'r')
esperanto_file = open('Esperanto.txt', 'r')
latin_file = open('Latin.txt', 'r')
german_file = open('German.txt', 'r')

english_words = english_file.read().split('\n')
esperanto_words = esperanto_file.read().split('\n')
latin_words = latin_file.read().split('\n')
german_words = german_file.read().split('\n')

latin_words = []
german_words = []

dictionary = {}

for word in english_words:
    dictionary[word] = 1

for word in esperanto_words:
    if word in dictionary:
        dictionary[word] |= 2
    else:
        dictionary[word] = 2

for word in latin_words:
    if word in dictionary:
        dictionary[word] |= 4
    else:
        dictionary[word] = 4

for word in german_words:
    if word in dictionary:
        dictionary[word] |= 8
    else:
        dictionary[word] = 8

dictionary_file = open('dictionary.txt', 'w')
dictionary_file.write("\n".join([word + ' ' + str(num) for (word,num) in dictionary.items()]))

min_size = min(len(english_words),len(esperanto_words),len(latin_words),len(german_words))

shuffle(english_words)
shuffle(esperanto_words)
shuffle(latin_words)
shuffle(german_words)

short_dictionary = {}

for i in range(min_size):
    dictionary[english_words[i]] = 1

for i in range(min_size):
    word = esperanto_words[i]
    if word in dictionary:
        dictionary[word] |= 2
    else:
        dictionary[word] = 2

for i in range(min_size):
    word = latin_words[i]
    if word in dictionary:
        dictionary[word] |= 4
    else:
        dictionary[word] = 4

for i in range(min_size):
    word = german_words[i]
    if word in dictionary:
        dictionary[word] |= 8
    else:
        dictionary[word] = 8

short_dictionary_file = open('short_dict.txt', 'w')
short_dictionary_file.write("\n".join([word + ' ' + str(num) for (word,num) in dictionary.items()]))
