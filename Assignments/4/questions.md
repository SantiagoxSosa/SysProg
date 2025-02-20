1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  Fork allows to create a new process, and then execvp is used to replace that process image. Therefore using them together is better so you can create a new process and replace its image, instead of the parent process.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  When fork is called, it checks to see the "pid" or ID of the child, if that is -1 (or we check for < 0), we output an error, free the memory at hand, and continue.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  It finds the command given the command in PATH. In short, it searched in PATH enviroment variable for the command to be executed.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**: This helps wait for the child process to complete its execution before continuing. If we do not "wait", it can become a zombie process, and can cause synchronization issues with parent and child.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS() is used to see the exit status of a child process, which is valuable if we want to check if the child process successfully terminated.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  My code looks for the first set of double quotes ' " ', and then reads everything until the next set. Everything that was read is treated as a token, and is give memory using strdup.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  One issue I ran into was the pwd command. I would forget to end my ARGS with a '\0' byte, so if i would "cd /tmp", and then "pwd", it would still have /tmp as an arg, and this would cause errors. I didn't null terminate my last assignment (oops), so I had to do that this time. (Finding this was a pain lol)

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Linux signals are a way to alert a process about the system condition, usually communicating simple commands. They are less suited for complex commands when compared to other IPCs.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGINT is an interrupt that stops programs from running any longer. SIGTERM is used as a 'polite' way to terminate a process. And SIGKILL force quits a process immediately. For example if a process is unresponsive.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  When a process receives SIGSTOP, it immediately stops the process. Unlike SIGINT, SIGSTOP cannot be intercepted or caught by the process. It will remain stopped until a SIGCONT. It acts as a safe bet to be able to stop a process.
