#!/usr/bin/env bats
# File: student_tests.sh

TEST_PORT=1234

clean_output() {
  echo "$1" | sed '/^socket client mode:/d' | sed '/^socket server mode:/d' | sed '/^->/d'
}

setup_remote_server() {
    ./dsh -s -p "$TEST_PORT" &
    SERVER_PID=$!
    sleep 1
}

teardown_remote_server() {
    if kill -0 "$SERVER_PID" 2>/dev/null; then
        kill "$SERVER_PID"
        wait "$SERVER_PID" 2>/dev/null
    fi
}

@test "Local mode: check ls runs without errors" {
    run ./dsh <<EOF
ls
EOF
    [ "$status" -eq 0 ]
}

@test "Local mode: piped command execution (echo and tr)" {
    run ./dsh <<EOF
echo hello world | tr a-z A-Z
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="HELLOWORLD"
    echo "$stripped_output" | grep -q "$expected_output"
    [ "$status" -eq 0 ]
}

@test "Local mode: multiple piped commands (echo, tr, rev)" {
    run ./dsh <<EOF
echo hello world | tr a-z A-Z | rev
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="DLROWOLLEH"
    echo "$stripped_output" | grep -q "$expected_output"
    [ "$status" -eq 0 ]
}

@test "Local mode: built-in cd command changes directory" {
    run ./dsh <<EOF
cd /
pwd
EOF
    echo "$output" | grep -q "^/"
    [ "$status" -eq 0 ]
}

@test "Local mode: exit command terminates shell" {
    run ./dsh <<EOF
exit
EOF
    echo "$output" | grep -q "cmd loop returned"
    [ "$status" -eq 0 ]
}

@test "Remote mode: echo command with EOF termination" {
    rdsh_eof=$'\x04'
    message="Hello, remote shell!"
    run ./dsh -c -p "$TEST_PORT" <<EOF
echo $message
EOF
    cleaned=$(clean_output "$output")
    cleaned=$(echo "$cleaned" | tr -d "$rdsh_eof")
    echo "$cleaned" | grep -q "$message"
    [ "$status" -eq 0 ]
    teardown_remote_server
}

@test "Remote mode: piped command execution (echo and tr)" {
    run ./dsh -c -p "$TEST_PORT" <<EOF
echo hello world | tr a-z A-Z
EOF
    cleaned=$(clean_output "$output")
    stripped=$(echo "$cleaned" | tr -d '[:space:]' | tr -d $'\x04')
    expected_output="HELLOWORLDdsh4>"
    echo "$stripped" | grep -q "$expected_output"
    [ "$status" -eq 0 ]
    teardown_remote_server
}

@test "Remote mode: multiple piped commands (echo, tr, rev)" {
    setup_remote_server
    run ./dsh -c -p "$TEST_PORT" <<EOF
echo hello world | tr a-z A-Z | rev
EOF
    cleaned=$(clean_output "$output")
    stripped=$(echo "$cleaned" | tr -d '[:space:]' | tr -d $'\x04')
    expected_output="DLROWOLLEH"
    echo "$stripped" | grep -q "$expected_output"
    [ "$status" -eq 0 ]
    teardown_remote_server
}

@test "Remote mode: exit command terminates remote shell" {
    setup_remote_server
    run ./dsh -c -p "$TEST_PORT" <<EOF
exit
EOF
    cleaned=$(clean_output "$output")
    echo "$cleaned" | grep -q "cmd loop returned"
    [ "$status" -eq 0 ]
    teardown_remote_server
}

@test "Remote mode: multiple client connections reconnect" {
    setup_remote_server
    run ./dsh -c -p "$TEST_PORT" <<EOF
exit
EOF
    [ "$status" -eq 0 ]
    run ./dsh -c -p "$TEST_PORT" <<EOF
ls
EOF
    [ "$status" -eq 0 ]
    teardown_remote_server
}

@test "Remote mode: fork/exec execution (uname -a)" {
    setup_remote_server
    run ./dsh -c -p "$TEST_PORT" <<EOF
uname -a
EOF
    cleaned=$(clean_output "$output")
    echo "$cleaned" | grep -q "Linux"
    [ "$status" -eq 0 ]
    teardown_remote_server
}

@test "Remote mode: unknown command handling returns error" {
    setup_remote_server
    run ./dsh -c -p "$TEST_PORT" <<EOF
foobar123
EOF
    cleaned=$(clean_output "$output")
    echo "$cleaned" | grep -q "execvp: No such file or directory"
    [ "$status" -eq 0 ]
    teardown_remote_server
}

@test "Remote mode: stop-server command shuts down server gracefully" {
    setup_remote_server
    run ./dsh -c -p "$TEST_PORT" <<EOF
stop-server
exit
EOF
    cleaned=$(clean_output "$output")
    echo "$cleaned" | grep -q "cmd loop returned 0"
    [ "$status" -eq 0 ]
    run ./dsh -c -p "$TEST_PORT" <<EOF
ls
EOF
    [ "$status" -eq 0 ]
}