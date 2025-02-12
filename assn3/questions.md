1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**: fgets reads the entire line into the buffer until the newline character is hit.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  You need to use malloc because you are using a lot of memory, and because of that you want to dynamically allocate it. Also because it is used throughout the entire program you don't want to keep it on the stack.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  It is necessary to trim the leading and trailing spaces before each command because when you go to execute the different commands, if they're implemented in for example an if statement, the program wouldn't be able to idenitify what command you are talking about because it wouldn't match the parameter. 

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  For redirection, we want to be able to redirect to an output, redirect an input into a command, and redirect the errors to a file. Some challenges might include managing the file descriptors. 

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**: Pipes are meant to send the output to another command, where as redirects redirect input or output to/from a file.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  If your STDOUT fails, your STDERR will still work. Also, STDERR is bufferless and goes directly to output.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  No, it is important to keep them seperate. If we merge them and it fails, we wouldn't have a way to get the error code, which is an important factor in a shell.