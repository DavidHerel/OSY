#!/bin/bash
# David Herel
# Started at 15.10.218

if [ $# -eq 0 ]; then
    echo "ERROR You have to enter atleast 1 argument" >&2
    exit 3
elif [ $# -gt "2" ]; then
    echo "ERROR You can enter only 1 argument" >&2
    exit 3
fi


while getopts ":uih" opt;
    do
    case $opt in
    h) echo "This script reads an HTML page and looks for PDF files"
       echo "Argument -i - loads page rom stdin"
       echo "Argument -u - loads URL from next argument"
       echo "Argument h will show you this manual"
       exit 0
      ;;
    i) input=1 #loading from input
        if [[ "$0" != "-i" && "$#" -ne 1 ]]; then
            echo "ERROR You can enter only 1 argument" >&2
            exit 3
        fi
      ;;
    u) input=0 #loading from web
        if [[ "$0" != "-u" && "$#" -ne 2 ]]; then
            echo "ERROR You have to enter olny one 2 arguments" >&2
            exit 3
        fi
      ;;
    ?)  echo "ERROR You entered invalid argument" >&2
        exit 2
      ;;
    esac
done
shift $(($OPTIND - 1))

if [ "$input" = "1" ]; then
    input=$(cat)
elif [ "$input" = 0 ];
    then
    wget -q --spider $1
    if [ "$?" -ne 0 ]; then
        exit 1;
    fi
    input=`wget -q -O- $1`
fi


 
echo "$input" |  # deleting ends of lines
 sed 's/.pdfx//' | sed ':a;N;$!ba;s/\n/ /g'  | sed ':a;N;$!ba;s/[[:space:]]/ /g'  |sed 's/	/ /g' |
 grep -i -o "<a[^>]\+href[[:blank:]]*[[:space:]]*=[[:blank:]]*[[:space:]]*['\"]*[^\"]\+\.pdf['\"]*" | #filtering <a...pdf#
  sed 's/href *= */href=/g' | # deleting spaces around =
 sed 's/<a[[:blank:]][[:space:]]*href=\"*[^\"]\*\.pdf//' | # deleting cases "file
 sed 's/\/a>/\n/' | grep -o -i "<a.*href=['\"]*.*\.pdf" | sed "s/<a	/<a /g" |sed "s/<[aA][[:space:]]*[[:blank:]]*.*href='*//" | sed 's/<[aA] HREF=//' | sed "s/ href=//"|tr -d '"' 
#| sed 's/<a href=//'
