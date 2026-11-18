

**Commands to pass on exec**
gdb -q --interpreter=mi3 \
    -iex "set style emoji off" \
    -iex "set confirm off" \
    -iex "set pagination off" \
    -iex "set mi-async on" \
    --args ./prog arg1 arg2

-enable-pretty-printing



**Stack variables workflow**

-stack-list-locals      --no-values 
                        --simple-values
                        --all-values


-stack-list-locals      --no-values <-
>> Make a VarObject for each with
-var-create var_x * <var_name_from_stack_list_locals>

>> If a variable has children(1, 2 or 3 w.r.t public, private, protected) - view children with 
1-var-list-children --all-values var_x
2-var-list-children --all-values var_x.public   # default case
>> If a variable has more than 1 children (at the "class" layer)
3-var-list-children --all-values var_x.private
4-var-list-children --all-values var_x.protected

    # Accumulate all children of the 3 adjacent protection layers
    and flatted into a single list of children

-var-update --all-values *                      # every step, to detect changes to a variable


## REPEAT



