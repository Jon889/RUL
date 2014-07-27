RUL
===

General format:  
./RUL \<fake ruls\> \<command\>  

eg:  
./RUL hsr2.txt ruls  

Commands
========

ruls
====

no parameters: generates all the RULs  
eg: ./RUL hsr2.txt ruls  

one parameter: generate all RULs with the specified piece on the left.  
eg: ./RUL hsr2.txt ruls 0x5dd31500  

two parameters: generate all RULs with the first specified piece on the left and the second on the right.  
eg: ./RUL hsr2.txt ruls 0x5dd31500 0x0d031500  

print
=====
Prints ascii art of pieces or combinations  

one parameter: print that piece in all rotations and flips or just the rotation/flip specified  
eg: ./RUL hsr2.txt print 0x5dd31500  
or: ./RUL hsr2.txt print 0x5dd31500,1,0  

two parameters: print all the combinations (or the just the ones specified by rotation/flip) of the two pieces.  
eg: ./RUL hsr2.txt print 0x5dd31500 0x0d031500  
or: ./RUL hsr2.txt print 0x5dd31500,1,0 0x0d031500,1,0  
or: ./RUL hsr2.txt print 0x5dd31500,1,0 0x0d031500  
or: ./RUL hsr2.txt print 0x5dd31500 0x0d031500,1,0  

find
====
Finds a piece based on the specified flags  

eg:./RUL hsr2.txt find HSR 0 2 0 2  
or:./RUL hsr2.txt find HSR 0 2 0 2 Road 2 0 2 0  

(Flag order is always W N E S)  

list
====
just lists every piece the program knows about  

eg:./RUL hsr2.txt list  


