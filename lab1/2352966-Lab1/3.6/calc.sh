#!/bin/bash

ANS_CACHE="ans.cache"
HIST_CACHE="hist.cache"

if ! [[ -f "$ANS_CACHE" ]]; then
    echo "0" > "$ANS_CACHE"
fi

if ! [[ -f "$HIST_CACHE" ]]; then
    touch "$HIST_CACHE"
fi

update()
{
    echo "$1" >> "$HIST_CACHE"
    hist=$(tail -n 5 "$HIST_CACHE")
    echo "$hist" > "$HIST_CACHE"
}

calculate()
{
    op1=$1
    operator=$2
    op2=$3
    
    if [[ "$op1" == "ANS" ]]; then
        op1=$(cat "$ANS_CACHE")
    fi
    if [[ "$op2" == "ANS" ]]; then
        op2=$(cat "$ANS_CACHE")
    fi
    
    if ! [[ "$op1" =~ ^-?[0-9]+(\.[0-9]+)?$ && "$op2" =~ ^-?[0-9]+(\.[0-9]+)?$ ]]; then
        echo "SYNTAX ERROR"
        return
    fi
    
    if [[("$operator" == "/" || "$operator" == "%") && $(echo "$op2 == 0" | bc -l) -eq 1 ]]; then
        echo "MATH ERROR"
        return
    fi
    
    result=""
    case $operator in
        +) result=$(echo "$op1 + $op2" | bc -l) ;;
        -) result=$(echo "$op1 - $op2" | bc -l) ;;
        x) result=$(echo "$op1 * $op2" | bc -l) ;;
        /) result=$(echo "$op1 / $op2" | bc -l) ;;
        %)
            if ! [[ "$op1" =~ ^-?[0-9]+(\.0+)?$ && "$op2" =~ ^-?[1-9]+(\.0+)?$ ]]; then
                echo "MATH ERROR"
                return
            fi
        result=$(echo "$op1 % $op2" | bc) ;;
        *)
            echo "SYNTAX ERROR"
            return
    esac
    
    if [[ $result =~ ^-?[0-9]+(\.0+)?$ ]]; then
        result=$(printf "%.0f" "$result")
    else
        result=$(printf "%.2f" "$result")
    fi
    echo "$result" > "$ANS_CACHE"
    update "$op1 $operator $op2 = $result"
    echo "$result"
    return 0
}

while true; do
    read -p ">> " input_line
    if [[ -z "$input_line" ]]; then
        continue
    fi
    
    if [[ "$input_line" == "EXIT" ]]; then
        exit 0
    elif [[ "$input_line" == "HIST" ]]; then
        cat "$HIST_CACHE"
        continue
    fi
    
    inputs=($input_line)
    if [[ ${#inputs[@]} -ne 3 ]]; then
        echo "SYNTAX ERROR"
        continue
    fi
    
    calc_res=$(calculate "${inputs[0]}" "${inputs[1]}" "${inputs[2]}")
    if [[ $? -eq 0 ]]; then
        echo "$calc_res"
    fi
done