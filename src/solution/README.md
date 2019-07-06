# Solution

## test1.cpp
  - fork() and send message to logger_daemon. 
    - The message will contain newline, making mounthide parse each side as separate line. 
  - Mounthide daemon will stop at this time. 
  - Then parent process will be closed, making orphan child process.   

## test2.cpp
  - Read flag file. 