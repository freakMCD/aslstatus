#!/bin/bash

purple="^c#d3869b^"
grey="^c#7c6f64^"

if [[ $(mpc status %state%) == 'playing' ]]; then
    printf "${purple}  $(mpc current)^d^"
elif [[ $(mpc status %state%) == 'paused' ]]; then
    printf "${grey} $(mpc current)^d^" 
else 
    printf "fail"
fi
