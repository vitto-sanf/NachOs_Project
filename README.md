# NachOS Project 

### Special Project for the Programmazione di Sistema course at the Politecnico di Torino.

The project was divided into two main phases: the first was a structural and functional comparison of NachOS and OS161 operating systems. 

- OS161 is an educational operating system primarily used in teaching operating systems concepts. It is designed to be a simple and modular operating system developed for educational and academic research purposes. OS161 provides students with the opportunity to explore fundamental operating system concepts through practical implementation of components such as memory management, process scheduling, and device handling. It is written in the C language and provides a complete development environment with emulators and debugging tools.

- NachOS (Not Another Completely Heuristic Operating System) 3.4 is an instructional operating system developed primarily for educational purposes. It is designed to provide students with hands-on experience in understanding operating system concepts by implementing various functionalities such as process management, memory management, file systems, and networking. NachOS is written in C++ and is commonly used in operating systems courses to teach concepts related to kernel design and implementation.

For more details regarding the comparison of the two OSs, go look at the pdf "Presentation OS161 vs NachOs".

The second phase consisted of adding some missing features to NachOS. Following the assignments proposed by Berkeley University, we decided to make the following changes:
-  We decided to modify the thread structure so that it would keep track of the active threads in the system and have a direct method of accessing the structure. We also added a priority to the processes by going to modify the scheduling algorithm so that threads with different priorities could be created.

- We added the Round Robin algorithm as an option for scheduling threads so that we would have a fairer algorithm.

- We implemented the missing synchronization algorithms, such as Lock and Condition Variable, by creating a test function to prove that they work properly.

To see the examples and get more clarification see the pdf "Presentation OS161 vs NachOs".


