#!/bin/bash
echo Updating tests...

for filename in ./tests/*.out; do
        sed -i '/number of false dependencies for/d' $filename
done

echo Updating done...
echo Running tests...


for filename in tests/test*.command; do
    test_num=`echo $filename | cut -d'.' -f1`
    bash ${filename} > ${test_num}.YoursOut
done

echo Tests done...
echo The test succeed if there are no diffs printed.
echo Running diff...
echo

for filename in tests/test*.out; do
    test_num=`echo $filename | cut -d'.' -f1`
    diff_result=$(diff ${test_num}.out ${test_num}.YoursOut)
    if [ "$diff_result" != "" ]; then
        echo The test ${test_num} didnt pass
    fi
done

echo
echo Ran all tests.