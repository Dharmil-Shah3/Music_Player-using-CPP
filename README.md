# Multi Threaded Music Player & Logger using C++
Simple CLI based music player to demonstrate use of multi-threading, mutexes, locks and condition variables in C++. 

This is <b>not a project</b>, but it is a small programing task to learn the C++ concepts like 
<ul>
  <li>Multithreading</li>
  <li>Mutex Locks</li>
  <li>Condition variables</li>
  <li>Logger/Logs etc...</li>
</ul>

Definition of the task was like...
There should be a common resource (esepcially a queue), which is accessed by more than one threads at a time.
- One thread contenously displays the data on the screen.
- Second thread should remove the objects from the queue once it's work id done.
- Third thread should monitor for any kind of exception into program, and handle it if occurs any.

To see the doxygen Documentation of the project, clone this project and open <b>doxygen/index.html</b>.

This project implements the basic C++ <b>logger</b>, similar to the <b><boost></b> trivial logger.
Logger logs into console as well as can write logs into files also.
