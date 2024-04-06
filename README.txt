
 uart6_pthread_server.c (will only commit to this file. Test was done on Rasp. pi-3+)
 TODO:
 - test on jetson nano setup
 - attemp to send data to a DB using mysql.c connector


 uart4_pthread.c ( will only cleanup)
 - faster polling ( this may be enough to capture device data and send to server)
 - you can pick up here and start creating the server
 - no pthread implementation here


 uart5_pthread.c ( will only cleanup)
 -same as uart4_pthread.c but uses mutex to hold and run a serial transaction to the server
