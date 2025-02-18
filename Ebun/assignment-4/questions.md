1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  _If we just called execvp from our shell process, our shell’s memory image would be overwritten by the new program, and the shell itself would cease to exist. By using fork, we create a child process that calls execvp_

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  _If fork() fails (it returns -1), it typically means the system cannot create a new process. In my implementation, we check if pid < 0 right after calling fork(). If true, we print an error message and skip executing the command. The shell continues rather than crashing._

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  _execvp() searches for the executable in each of the directories listed in the PATH environment variable._

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  _wait() allows the parent process to wait for the child to finish execution, preventing the creation of a “zombie” process. If we didn’t call wait(), the child might complete and remain in a zombie state until the shell terminates, because nobody collected its exit status._

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  _WEXITSTATUS(status) extracts the integer exit code of the child process from the status returned by wait(). This exit code indicates success or type of failure._

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  _My parser scans for double quotes. If it sees a ", it collects all characters until the matching " as part of a single argument, including any spaces._

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  _We removed pipe splitting and focused on splitting by whitespace unless inside quotes. We also added logic to auto-trim leading/trailing spaces. One challenge was ensuring that the new quoted-argument parsing fit well with the old code’s tokenization approach, and that we allocated enough memory to store the entire line plus null terminators._

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  _Signals provide a way to asynchronously notify a process that a particular event has occurred. Unlike other IPC (like pipes or sockets), signals contain minimal data and can forcibly interrupt or terminate processes unless caught or ignored._

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  _SIGKILL (9): Immediately kills a process; the process cannot catch or ignore it. Used when you must forcibly stop a runaway process.
SIGTERM (15): Politely asks a process to terminate; the process can catch or ignore it. Often used for graceful shutdown.
SIGINT (2): Triggered by Ctrl+C in a terminal; default action is to terminate the process, though processes can catch it to perform cleanup._

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  _SIGSTOP immediately suspends a process (like pressing Ctrl+Z) and cannot be caught, blocked, or ignored by the process. It’s a “special” signal used by the operating system to pause a process unconditionally_
