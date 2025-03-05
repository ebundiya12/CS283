1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

_The parent process calls waitpid() for each child PID after creating all processes in the pipeline. This blocks until all children exit. If waitpid() is omitted, zombie processes accumulate, and the shell might proceed to the next prompt before pipeline commands finish, creating race conditions._

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

_dup2() redirects I/O but leaves original pipe file descriptorss open. Failing to close unused ends prevents EOF detection, causing processes to hang. Closing them ensures proper resource cleanup and avoids leaks._

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

_cd must modify the shell’s own working directory. If run as an external process, it would only change the child’s directory, not the parent shell’s. Built-ins execute in the shell’s process to directly alter its state._

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

_Replace the static commands[CMD_MAX] array with a dynamically resized array (using realloc()). Trade-offs include increased risk of leaks and potential overhead from frequent allocations. A linked list could also be used but with slower traversal._
