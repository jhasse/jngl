#!/usr/bin/env python3
# Copyright 2025 Jan Niklas Hasse <jhasse@bixense.com>
# For conditions of distribution and use, see copyright notice in LICENSE.txt
import subprocess
import sys

subdir = sys.argv[1]


def run(test, args, expected_exitcode=0):
    cmd = f"{subdir}/jngl-test-{test} {args}"
    print(f"{cmd}")
    result = subprocess.Popen(
        [f"{subdir}/jngl-test-{test}"] + args.split(" "),
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )
    output, _ = result.communicate()
    if expected_exitcode != result.returncode:
        print(
            f"\x1b[31;1mExpected exit code {expected_exitcode}, got {result.returncode}. Output:\x1b[0m"
        )
        print(output.decode())
        exit(1)


run("forceQuit", "0")
run("forceQuit", "42", expected_exitcode=42)
