from fsbuild import FastsortBuild
tester_files = ["errtests", "gentests", "duptest"]

all = []
for f in tester_files:
  module = __import__(f)
  all.extend(module.test_list)

import toolspath
from testing.runtests import main
main(FastsortBuild, all)
