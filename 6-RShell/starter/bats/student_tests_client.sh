#!/usr/bin/env bats

# File: student_tests_client.sh
# 
# Create your unit tests suit in this file

#Now need to test client side:

setup(){
    ./dsh -s -i 0.0.0.0 -p 7890 &
    SERVER_PID=$!
    sleep 1;
}


@test "CLIENT: basic command over client-server" {

run ./dsh -c -i 0.0.0.0 -p 7890 <<EOF
echo "hello"
EOF

stripped_output=$(echo "$output" | tr -d '[:space:]')
expected_output="socketclientmode:addr:0.0.0.0:7890dsh4>hellodsh4>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]


}

@test "CLIENT: test ls over client-server" {

run ./dsh -c -i 0.0.0.0 -p 7890 <<EOF
ls
EOF

stripped_output=$(echo "$output" | tr -d '[:space:]')
expected_output="socketclientmode:addr:0.0.0.0:7890dsh4>batsdebugdshdsh_cli.cdshlib.cdshlib.hdsh-testmakefilersh_cli.crshlib.hrsh_server.cdsh4>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]

}


@test "CLIENT: test pipe over client-server" {

run ./dsh -c -i 0.0.0.0 -p 7890 <<EOF
ls -l | wc -l
EOF

stripped_output=$(echo "$output" | tr -d '[:space:]')
expected_output="socketclientmode:addr:0.0.0.0:7890dsh4>12dsh4>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]


}

@test "CLIENT: basic built in (cd) command over client-server" {

    current=$(pwd)
    mkdir -p dsh-test

run ./dsh -c -i 0.0.0.0 -p 7890 <<EOF
cd dsh-test
pwd
EOF

stripped_output=$(echo "$output" | tr -d '[:space:]')
expected_output="socketclientmode:addr:0.0.0.0:7890dsh4>dsh4>${current}/dsh-testdsh4>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]


}


@test "CLIENT: Test Multiple Non-Builtin Pipes" {
    run ./dsh -c -i 0.0.0.0 -p 7890 <<EOF
echo "test" | tr 't' 'T' | rev
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="socketclientmode:addr:0.0.0.0:7890dsh4>TseTdsh4>cmdloopreturned0"

   # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}

@test "CLIENT: Test Executable Is Too long" {


    run ./dsh -c -i 0.0.0.0 -p 7890 <<EOF               
ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="socketclientmode:addr:0.0.0.0:7890dsh4>error:Executabletoolongdsh4>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}

@test "CLIENT: Test stop-server" {


    run ./dsh -c -i 0.0.0.0 -p 7890 <<EOF               
stop-server
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="socketclientmode:addr:0.0.0.0:7890dsh4>serverappearedtoterminate-exitingcmdloopreturned-7"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}


@test "CLIENT: exit client" {


    run ./dsh -c -i 0.0.0.0 -p 7890 <<EOF               
exit
#
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="socketclientmode:addr:0.0.0.0:7890dsh4>Exitingtheclient...cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}

teardown() {
  kill $SERVER_PID 2>/dev/null || true
  sleep 1
}
