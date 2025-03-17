#!/usr/bin/env bats

# File: student_tests.sh

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF
ls
EOF
    [ "$status" -eq 0 ]
}

@test "Builtin: cd with valid directory" {
    run ./dsh <<EOF
cd /tmp
pwd
exit
EOF
    # Expect status 0 and that output includes "/tmp"
    [ "$status" -eq 0 ]
    [[ "$output" =~ "/tmp" ]]
}

@test "Builtin: cd with invalid directory" {
    run ./dsh <<EOF
cd /no_such_dir
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "cd failed" ]]
}

@test "External command: echo Hello" {
    run ./dsh <<EOF
echo Hello
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "Hello" ]]
}

@test "Invalid command: not_exists123" {
    run ./dsh <<EOF
not_exists123
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "Command not found" || "$output" =~ "could not execute command" ]]
}
