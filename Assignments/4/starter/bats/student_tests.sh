@test "External cmd: ls runs without errors" {
  current=$(pwd)
  run "${current}/dsh" <<EOF
ls
exit
EOF

  stripped_output=$(echo "$output" | tr -d '[:space:]')
  expected_substring="dsh2>cmdloopreturned0"

  echo "Captured stdout:"
  echo "Output: $output"
  echo "Exit Status: $status"
  echo "Stripped: '${stripped_output}'"

  [ "$status" -eq 0 ]
  [[ "$stripped_output" =~ "$expected_substring" ]]
}

@test "Built-in cmd: exit" {
  current=$(pwd)
  run "${current}/dsh" <<EOF
exit
EOF

  stripped_output=$(echo "$output" | tr -d '[:space:]')
  expected_output="dsh2>cmdloopreturned0"

  echo "Captured stdout:"
  echo "Output: $output"
  echo "Exit Status: $status"
  echo "Stripped: '${stripped_output}' -> Expected: '${expected_output}'"

  [ "$stripped_output" = "$expected_output" ]
  [ "$status" -eq 0 ]
}

@test "cd with NO arguments" {
  current=$(pwd)
  cd /tmp
  mkdir -p dsh-test-no-args

  run "${current}/dsh" <<EOF
pwd
cd
pwd
exit
EOF

  stripped_output=$(echo "$output" | tr -d '[:space:]')
  expected_output="/tmp/tmpdsh2>dsh2>dsh2>dsh2>cmdloopreturned0"

  echo "Captured stdout:"
  echo "Output: $output"
  echo "Exit Status: $status"
  echo "Stripped: '${stripped_output}' -> Expected: '${expected_output}'"

  [ "$stripped_output" = "$expected_output" ]
  [ "$status" -eq 0 ]
}

@test "cd with one argument" {
  current=$(pwd)
  cd /tmp
  mkdir -p dsh-test-arg

  run "${current}/dsh" <<EOF
cd dsh-test-arg
pwd
exit
EOF

  stripped_output=$(echo "$output" | tr -d '[:space:]')
  expected_output="/tmp/dsh-test-argdsh2>dsh2>dsh2>cmdloopreturned0"

  echo "Captured stdout:"
  echo "Output: $output"
  echo "Exit Status: $status"
  echo "Stripped: '${stripped_output}' -> Expected: '${expected_output}'"

  [ "$stripped_output" = "$expected_output" ]
  [ "$status" -eq 0 ]
}

@test "External cmd: pwd" {
  current=$(pwd)
  run "${current}/dsh" <<EOF
pwd
exit
EOF

  stripped_output=$(echo "$output" | tr -d '[:space:]')
  unwanted_substring="ignoringnon-optionarguments"

  echo "Captured stdout:"
  echo "Output: $output"
  echo "Exit Status: $status"
  echo "Stripped: '${stripped_output}'"

  [[ "$stripped_output" =~ "$unwanted_substring" ]] && false
  [ "$status" -eq 0 ]
}

@test "Empty command" {
  current=$(pwd)
  run "${current}/dsh" <<EOF

exit
EOF

  stripped_output=$(echo "$output" | tr -d '[:space:]')

  echo "Captured stdout:"
  echo "Output: $output"
  echo "Exit Status: $status"
  echo "Stripped: '${stripped_output}'"

  [ "$status" -eq 0 ]
}