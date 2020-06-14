#! /usr/bin/python3
#
#===- run-clang-tidy.py - Parallel clang-tidy runner ---------*- python -*--===#
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
#===------------------------------------------------------------------------===#
# FIXME: Integrate with clang-tidy-diff.py

"""
Parallel clang-tidy runner
==========================

Runs clang-tidy over all files in a compilation database. Requires clang-tidy
and clang-apply-replacements in $PATH.

Example invocations.
- Run clang-tidy on all files in the current working directory with a default
  set of checks and show warnings in the cpp files and all project headers.
    run-clang-tidy.py $PWD

- Fix all header guards.
    run-clang-tidy.py -fix -checks=-*,llvm-header-guard

- Fix all header guards included from clang-tidy and header guards
  for clang-tidy headers.
    run-clang-tidy.py -fix -checks=-*,llvm-header-guard extra/clang-tidy \
                      -header-filter=extra/clang-tidy

Compilation database setup:
http://clang.llvm.org/docs/HowToSetupToolingForLLVM.html
"""

from __future__ import print_function

import argparse
import glob
import json
import multiprocessing
import os
import re
import shutil
import subprocess
import sys
import tempfile
import threading
import traceback
import yaml

import queue as queue


def make_absolute(f, directory):
    if os.path.isabs(f):
        return f
    return os.path.normpath(os.path.join(directory, f))


def get_tidy_invocation(f, clang_tidy_binary, checks, build_path,
                        header_filter, quiet, config):
    """Gets a command line for clang-tidy."""
    start = [clang_tidy_binary]
    if header_filter is not None:
        start.append('-header-filter=' + header_filter)
    else:
        # Show warnings in all in-project headers by default.
        start.append('-header-filter=src/')
    if checks:
        start.append('-checks=' + checks)
    start.append('-p=' + build_path)
    if quiet:
        start.append('-quiet')
    if config:
        start.append('-config=' + config)
    start.append(f)
    return start


def run_tidy(args, build_path, queue, failed_files):
    """Takes filenames out of queue and runs clang-tidy on them."""
    while True:
        name = queue.get()
        invocation = get_tidy_invocation(name, 'clang-tidy', None,
                                         build_path, None,
                                         None, None)
        return_code = subprocess.call(invocation, stderr=subprocess.PIPE)
        if return_code != 0:
            failed_files.append(name)
        queue.task_done()


def main():
    # files to be processed (regex on path)
    args_files = sys.argv[1:] if len(sys.argv) > 1 else ['.*']
    print(args_files)

    # Build up a big regexy filter from all command line arguments.
    file_name_re = re.compile('|'.join(args_files))

    db_path = 'compile_commands.json'

    build_path = '.'

    # Load the database and extract all files.
    database = json.load(open(os.path.join(build_path, db_path)))
    files = [entry['file'] for entry in database]

    max_task = multiprocessing.cpu_count()

    return_code = 0
    try:
        # Spin up a bunch of tidy-launching threads.
        task_queue = queue.Queue(max_task)
        # List of files with a non-zero return code.
        failed_files = []
        for _ in range(max_task):
            t = threading.Thread(target=run_tidy,
                                 args=([], build_path, task_queue, failed_files))
            t.daemon = True
            t.start()

        # Fill the queue with files.
        for name in files:
            if file_name_re.search(name):
                task_queue.put(name)

        # Wait for all threads to be done.
        task_queue.join()
        if len(failed_files):
            return_code = 1

    except KeyboardInterrupt:
        # This is a sad hack. Unfortunately subprocess goes
        # bonkers with ctrl-c and we start forking merrily.
        print('\nCtrl-C detected, goodbye.')
        os.kill(0, 9)

    sys.exit(return_code)


if __name__ == '__main__':
    main()
