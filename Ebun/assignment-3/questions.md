1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  _fgets() is a good choice because it safely reads a line of input (up to a specified maximum length) including the trailing newline character (if present), and it stops reading on either a newline or EOF._

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  _Using a dynamically allocated buffer (malloc()) gives us more flexibility and control over the size of cmd_buff at runtime_


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  _Trimming leading and trailing spaces is necessary because commands and arguments in a shell are typically tokenized by whitespace.If extra spaces remain it leads to incorrect parsing_

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  _i. Output Redirection (> or >>) - Challenges: We must open or create the file with the correct flags, then duplicate the file descriptor (usually via dup2) into STDOUT before exec(). Handling appending vs overwriting requires different file open modes (O_APPEND vs O_TRUNC).
    
    ii. Input Redirection (<) - Challenges: We must open the file, check for errors (file not found, permission denied, etc.), and then redirect STDIN via dup2. Error handling is crucial since failing to open the file invalidates the command.
    
    iii. Redirecting STDERR (2>, 2>>) - Challenges: The shell must distinguish between STDOUT (file descriptor 1) and STDERR (file descriptor 2). We have to parse the command line carefully to detect something like 2> and handle it just like STDOUT redirection except for the file descriptor._

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  _redirection usually deals with replacing STDIN/STDOUT/STDERR with files or devices i.e. changes where a command reads its input or writes its output, while pipes connect two processes together so they can communicate in memory without intermediate files._

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  _Keeping STDERR separate from STDOUT allows users and scripts to distinguish normal, expected output from errors or warnings._

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  _ Some users want to combine STDOUT and STDERR using syntax like 2>&1 (meaning “redirect file descriptor 2 to file descriptor 1”). The shell can support that syntax if we parse it.
To merge, you use dup2 or a similar approach to map the error descriptor to standard output (or vice versa).
We do not merge them by default. Instead, we allow the user to decide by specifying redirection commands_