1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

The childs execute their respective commands using execvp, and then by using the waitpid() function, we can ensure that each child completes their given process.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

If you don't close your pipes, you end up using an unneccesary amount of resources. More so, the processes may end up waiting forever on input that doesn't exist, so it may never arrive, and this creates zombie code (That might be wrong terminology)

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

If used as an external command, it would end up being used on a child shell, and this would not affect the parent shell which is where we want the command to take effect.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

I think you could use a dynamic array for storing commands, that could fix the issue that would arise with CMD_MAX. This would bring the issue of more and more complex memory management. This could also slow down run time (I think). 