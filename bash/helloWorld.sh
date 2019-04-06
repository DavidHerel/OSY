#!/bin/bash

a="test uvozovek" #do a je prirazeno 24 // edit string
echo x${a}x #vypis x a promenne a a pak x na standartni vystup

declare -a POLE

prazdne_pole=() #vytvoreni pole
barvy=(cervena zelena modra) #vytvori pole se tremi cervena zelena modra

echo ${barvy[0]} #vrati prvni polozku pole

echo ${#barvy[*]} #vrati pocet polozek pole barvy
echo ${barvy[*]}  #vrati vsechny polozky pole BARVY jako jeden retezec, ve kterem jsou polozky oddelene mezerou
#echo ${barvy[@]}  #vrati vsechny polozky pole barvy samostatne ()

barvy+=(cerna bila)
echo hodnota_parametru_${1} #zpracovavam parametr a tisknu jeho hodnotu
echo ${barvy[*]} 

#ctu input
while read input

do

    if [ "$input" == "abcd" ]; then
        echo "podminka splnena -- ukonceni programu"
        echo "delka inputu" ${#input} #delka inputu
        exit
    else
        echo "podminka nesplenna -- zadejte abcd a program se vypne"
        echo $input
    fi

done


// "${a:0:5}" == "PATH "


