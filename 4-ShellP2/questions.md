1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  Execvp immediately exits process when done, if that was called in the shell, the shell would just close. The fork makes a child process that then can exit while maintaing the shell.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If the fork() sys. call fails, you check the return value of fork to see if it is negative. If this happens, a child process is not made. 

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  execvp trys to follow the path to the exe, if it doesn't exist it looks at the environment variable path to find it.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  So the parent can clean when the child process exits. If we don't call wait we could get a zombie process, which PID1 has to clean.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS() reads the exit code of the child and makes it able to be read. It is important to know if the child succeeded or returned an error.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  My implementation to handle quoted arguments utilizes a flag variable to ignore spaces within a quote if 1. It is important because normally spaces are used as delimiters between arguments.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  The one change I made was to change how trailing white space is handled, since the last assignment's implementation deleted the trailing whitespace within a quote. One unexpected challenge was handling pointers to allocated memory, specifically in the argv array to indicate arguement start points.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  The purpose of a signal is to have an immediate communication form between processes. This is similar to a hardware interrupt, except software based, and immediately needs to be serviced by the process.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL is an immediate end to the receiving process. This can be utilized on a hung process. SIGTERM is the nicer version of SIGKILL, it can be handled so the process can clean up after itself before exiting. SIGINT interrupts a process and puts it into a stopped process state. 

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  SIGSTOP is not able to be ignored by the receiving process and puts the process in a stopped process state. This is due to the SIGSTOP being enforced by the kernel.