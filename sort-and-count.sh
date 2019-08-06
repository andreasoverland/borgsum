#!/bin/bash

sort level-6-all-part-a0.txt -o level-6-all-part-a0-sorted.txt
sort level-6-all-part-a1.txt -o level-6-all-part-a1-sorted.txt
sort level-6-all-part-a2.txt -o level-6-all-part-a2-sorted.txt
sort level-6-all-part-a3.txt -o level-6-all-part-a3-sorted.txt
sort level-6-all-part-a4.txt -o level-6-all-part-a4-sorted.txt

sort level-6-all-part-b0.txt -o level-6-all-part-b0-sorted.txt
sort level-6-all-part-b1.txt -o level-6-all-part-b1-sorted.txt
sort level-6-all-part-b2.txt -o level-6-all-part-b2-sorted.txt
sort level-6-all-part-b3.txt -o level-6-all-part-b3-sorted.txt
sort level-6-all-part-b4.txt -o level-6-all-part-b4-sorted.txt

sort level-6-all-part-c0.txt -o level-6-all-part-c0-sorted.txt
sort level-6-all-part-c1.txt -o level-6-all-part-c1-sorted.txt
sort level-6-all-part-c2.txt -o level-6-all-part-c2-sorted.txt
sort level-6-all-part-c3.txt -o level-6-all-part-c3-sorted.txt

sort level-6-all-part-d0.txt -o level-6-all-part-d0-sorted.txt
sort level-6-all-part-d1.txt -o level-6-all-part-d1-sorted.txt
sort level-6-all-part-d2.txt -o level-6-all-part-d2-sorted.txt
sort level-6-all-part-d3.txt -o level-6-all-part-d3-sorted.txt
sort level-6-all-part-d4.txt -o level-6-all-part-d4-sorted.txt
sort level-6-all-part-d5.txt -o level-6-all-part-d5-sorted.txt

sort -m level-6-all-part-a?-sorted.txt -o level-6-all-part-a-sorted.txt
sort -m level-6-all-part-b?-sorted.txt -o level-6-all-part-b-sorted.txt
sort -m level-6-all-part-c?-sorted.txt -o level-6-all-part-c-sorted.txt
sort -m level-6-all-part-d?-sorted.txt -o level-6-all-part-d-sorted.txt

sort -m level-6-all-part-a-sorted.txt level-6-all-part-b-sorted.txt level-6-all-part-c-sorted.txt level-6-all-part-d-sorted.txt -o level-6-merged.txt

