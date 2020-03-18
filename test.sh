#!/bin/bash
for x in $(find build/ -executable -name "*_test")
do
    `$x`
    if [[ $? == 0 ]]
    then
        echo -e "\033[1;92msuccess\033[0m: $x"
    else
        echo -e "\033[1;91mfailure\033[0m: $x"
    fi
done
