#!/bin/bash

liczba=$#
prog=$(readlink -e $1)
dir=$(readlink -e $2)

for i in t ę c z a; do 
    echo -n -e  "\e[41m \e[101m \e[103m \e[102m \e[104m \e[44m \e[45m \e[0m"
done

echo ""

if [ ! -x $prog ]
then
    echo -e "\e[91mZły program\e[97m"
    exit
fi

if [ ! -d $dir ]
then
    echo -e "\e[91mZła ścieżka\e[97m"
    exit
fi

if [ ! ${liczba} -eq 2 ]
then
    echo -e "\e[91mZła liczba argumentów\e[97m"
    exit
fi

mkdir output/

zle=0
for f in ${dir}/*.in
do
    f2="$(basename -- ${f})"
    test_name="${f2%.in}"
    valgrind --error-exitcode=10 --errors-for-leak-kinds=all --leak-check=full --log-fd=9 9>/dev/null\
    $prog <"${f}" >"output/$test_name.exit" 2>"output/$test_name.exerr"
    pamiec=$?

    if diff "output/${test_name}.exit" "$dir/${test_name}.out" >/dev/null 2>&1
    then
        if diff "output/${test_name}.exerr" "$dir/${test_name}.err" >/dev/null 2>&1
        then
            echo -e "${test_name}:  wynik \e[92mOK\e[97m"
            if [ ${pamiec} -eq 10 ]
            then
                echo -e "${test_name}: pamięć \e[91mNieOK\e[97m"
                ((zle=zle+1))
            else
                echo -e "${test_name}: pamięć \e[92mOK\e[97m"
            fi
        else
            echo -e "${test_name}: \e[93mInny kod błędu\e[97m"
            if [ ${pamiec} -eq 10 ]
            then
                echo -e "${test_name}: pamięć \e[91mNieOK\e[97m"
                ((zle=zle+1))
            else
                echo -e "${test_name}: pamięć \e[92mOK\e[97m"
            fi
        fi
    else
        echo -e "${test_name}: \e[91mNieOK\e[97m" 
        ((zle=zle+1))
    fi

    echo ""
done

rm -r output

if [ $zle -eq 0 ]
then echo -e "\e[92mWszystko działa!\e[97m"
else echo -e "\e[91mLiczba błędnych testów: $zle\e[97m"
fi