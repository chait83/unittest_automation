#!/bin/bash

JOBS=3
#BROWSER="${LOCALAPPDATA}/Mozilla Firefox/firefox.exe"
BROWSER="C:\Program Files (x86)\Google\Chrome\Application\chrome.exe"

cd $(dirname ${0})  ||  exit
cd ..  ||  exit

if [[ ${#} == 1  &&  (${1} == "clean"  ||  ${1} == "c")  ]]; then
  mingw32-make.exe --no-builtin-rules --no-builtin-variables -f MakefileCmocka clean  ||  exit
  exit
fi

#mingw32-make.exe -j ${JOBS} -f MakefileCmocka build/tests/cmocka/src/common/FCan_test.exe  ||  exit
mingw32-make.exe -j ${JOBS} --no-builtin-rules --no-builtin-variables -f MakefileCmocka all  ||  exit
mingw32-make.exe --no-builtin-rules --no-builtin-variables -f MakefileCmocka report  ||  exit

## at least Chrome does not open files on relative paths, we need to make an absolute one ... in Windows-style (D:\ ...)
PREFIX="file://"$(cygpath --windows "${PWD}")

"${BROWSER}" "${PREFIX}/build/tests/covreport/index.html" &
#"${BROWSER}" "${PREFIX}/build/tests/testreport/report.html" &

