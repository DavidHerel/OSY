#!/bin/bash

#declaring functions
write_file(){ 
    if  [ -L "${input:5}" ]; #lookin for a file
    then
        rdl=$(readlink ${input:5})
        echo "LINK '${input:5}' '${rdl}'"
        
    elif [ -f "${input:5}" ]; #lookin for a symbolic link
    then
        
        number_lines=$(wc -l < "${input:5}")
        first_line=$(head -n 1 "${input:5}")
        file_array+=("${input:5}")        
        echo "FILE '${input:5}' ${number_lines} '${first_line}'"

        
    elif [ -d "${input:5}" ]; #lookin for a dir
    then 
        echo "DIR '${input:5}'"
        
    else   #error
        error=1
        echo "ERROR '${input:5}'" >&2
        
       
    fi
}

declare -a file_array
make_file=0 #false
error=0 #false
#waiting for arguments

while getopts ":hz" opt;
    do
    case $opt in
    h) echo "This script reads an input"
       echo "If your line starts with PATH /home/.., script will take words after PATH as a path to a file/dir/symlink"
       echo "You can write two arguments h and z"
       echo "Argument h will show you this manual"
       echo "Argument z will zip files you written after PATH"
       exit 0
      ;;
    z) make_file=1
      ;;
    ?) exit 2
      ;;
    esac
done
shift $(($OPTIND - 1))

#reading an input
while read input
    do
        if [ "${input:0:5}" = "PATH " ];
        then
            #print things that needs to be print
            write_file
        fi  
    done
    
#finishing the job
if [ "$make_file" = 1 ];
    then
        tar czf output.tgz "${file_array[@]}"
fi

if [ "$error" = 1 ];
    then
        exit 1
elif [ "$error" = 0 ];
    then 
        exit 0
    fi

    

