
2/11/24

 uart4_pthread.c
 - faster polling ( this may be enough to capture device data and send to server)
 - you can pick up here and start creating the server
 - no pthread implementation here


 uart5_pthread.c
 -same as uart4_pthread.c but uses mutex to hold and run a serial transaction to the server
