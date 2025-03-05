1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

To ensure that all child processes complete before the continues accepting use input, the program loops though all childpids and calls waitpid(). If you didn't call waitpid() zombie processes would be created.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

It is important to close unused pipe ends after calling dup2() because failing to close them can lead to broken pipes, where processes are hung and stuck waiting for the pipe to either fill or empty.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

If CD were considered an external process, and we had to fork/exec, then the child that executes the CD command will be the one to change directories, not the parent. 

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

The command array would have to be dynamically allocated, and freed before the next shell iteration. The trade off is that you have to allocate memory each iteration.