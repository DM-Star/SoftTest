start wc.exe wc.exe char.c
start wc.exe wc.exe -c charwithspace.c
start wc.exe wc.exe -w -l wordtest.c
start wc.exe wc.exe -a atest.c
start wc.exe wc.exe -w stoptest.c -e stoplist.txt
start wc.exe wc.exe -s -w -a C:\Users\Star\Desktop\SoftTest\*.c
start wc.exe wc.exe -s -a -w -c -l C:\Users\Star\Desktop\SoftTest\*.c -o output.txt
start wc.exe wc.exe -s -a -w -c -l C:\Users\Star\Desktop\SoftTest\*.c -o output.txt -e stoplist.txt
start wc.exe wc.exe -c -d char.c charwithspace.c
start wc.exe wc.exe -e -c char.c