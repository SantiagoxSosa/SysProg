#!/usr/bin/env bats

########################################################################################
# File: student_tests.sh
#
# This is the student-authored test suite for the dsh shell.
#
# The tests below verify:
#  - That the shell prints its prompt ("dsh3>") on startup.
#  - Basic command execution (e.g., ls) runs without error.
#  - Piped command execution works as expected (e.g., echo with tr and rev).
#  - Built-in commands (cd, exit) function correctly.
#  - The shell handles empty command input gracefully.
#
# Detailed debugging output is provided to assist in pinpointing any issues.
########################################################################################

setup() {
  # Path to your shell binary
  DSH_BIN="./dsh"
}

@test "Shell displays prompt on startup" {
  # Verify that the shell prints its prompt when launched.
  run bash -c "printf 'exit\n' | $DSH_BIN"
  echo "Captured stdout:" 
  echo "$output"
  echo "Exit Status: $status"
  # Check that at least one instance of "dsh3>" appears
  echo "$output" | grep -q "dsh3>"
  [ "$status" -eq 0 ]
}

@test "Basic command execution: ls runs without errors" {
  # Check that a simple command (ls) executes without errors.
  run bash -c "printf 'ls\nexit\n' | $DSH_BIN"
  echo "Captured stdout:" 
  echo "$output"
  echo "Exit Status: $status"
  [ "$status" -eq 0 ]
}

@test "Piped command execution: echo and tr for uppercase conversion" {
  # This test verifies piped execution by converting output to uppercase.
  # The command 'echo hello world | tr a-z A-Z' should output "HELLO WORLD".
  run bash -c "printf 'echo hello world | tr a-z A-Z\nexit\n' | $DSH_BIN"
  echo "Captured stdout:" 
  echo "$output"
  echo "Exit Status: $status"
  # Remove all whitespace for consistent comparison.
  stripped_output=$(echo "$output" | tr -d '[:space:]')
  expected_output="HELLOWORLD"
  echo "Stripped output: ${stripped_output}"
  echo "Expected (whitespace removed): ${expected_output}"
  echo "$stripped_output" | grep -q "$expected_output"
  [ "$status" -eq 0 ]
}

@test "Multiple piped commands: echo, tr, and rev" {
  # This test chains multiple pipes. The input "echo hello world" is converted 
  # to uppercase then reversed.
  # Expected transformation: "hello world" -> "HELLO WORLD" -> "DLROW OLLEH".
  run bash -c "printf 'echo hello world | tr a-z A-Z | rev\nexit\n' | $DSH_BIN"
  echo "Captured stdout:" 
  echo "$output"
  echo "Exit Status: $status"
  stripped_output=$(echo "$output" | tr -d '[:space:]')
  expected_output="DLROWOLLEH"
  echo "Stripped output: ${stripped_output}"
  echo "Expected (whitespace removed): ${expected_output}"
  echo "$stripped_output" | grep -q "$expected_output"
  [ "$status" -eq 0 ]
}

@test "Built-in cd command changes directory correctly" {
  # This test verifies the built-in cd command.
  # The shell changes to the root directory and then outputs the working directory.
  run bash -c "printf 'cd /\npwd\nexit\n' | $DSH_BIN"
  echo "Captured stdout:" 
  echo "$output"
  echo "Exit Status: $status"
  # Check that the output of pwd starts with "/" indicating the root directory.
  echo "$output" | grep -q "^/"
  [ "$status" -eq 0 ]
}

@test "Handling empty command input gracefully" {
  # When an empty command is entered, the shell should warn the user.
  run bash -c "printf '\nexit\n' | $DSH_BIN"
  echo "Captured stdout:" 
  echo "$output"
  echo "Exit Status: $status"
  # Look for the warning string defined by CMD_WARN_NO_CMD (case-insensitive).
  echo "$output" | grep -qi "warning: no commands provided"
  [ "$status" -eq 0 ]
}

@test "Exit command terminates shell and returns exit status" {
  # Verify that issuing the exit command terminates the shell gracefully.
  run bash -c "printf 'exit\n' | $DSH_BIN"
  echo "Captured stdout:" 
  echo "$output"
  echo "Exit Status: $status"
  # Check for an output line indicating that the command loop returned a value.
  echo "$output" | grep -q "cmd loop returned"
  [ "$status" -eq 0 ]
}
