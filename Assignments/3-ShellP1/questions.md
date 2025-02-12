1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  fgets is good because it reads safely until EOF or newline, since we are writing a shell, reading until newline is the easiest way to parse our input.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  I think this is because it puts memory on the heap? I am not sure to be honest.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  This is important as you might end up with unintended characters in names of commands or arguments, which when writing to a shell is key.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  1: pwd > output.txt 2: echo "Hello World!" >> output.txt 3: ls non_existent_dir 2> error.txt. This redirection ability is key to change the direction of our input or output. One challenge may be if we intend to write to another file, this can cause many issues of corrupt files, or access to write to a file.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  The big difference is that redirection just changes the file descriptor. A pipe deals with memory in house making the output of one command, the immediate input of the next command.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  Well for one its logical so that programs can properly deal with error code, and by doing so, debugging is far easier. No interference between output and errors.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  Display the error message to alert of the error using STDERR, to merge, we could use 2>&1 to redirect all output including STDERR output to STDOUT. I think.