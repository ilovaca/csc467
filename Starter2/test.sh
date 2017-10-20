#!/bin/bash
declare -a passInputs=(
    # only scope
    "{}"
    "{;;}"
    # nested scopes
    "{{}}"
    # declarations
    "{int foo;}"
    "{int foo = 5;}"
    "{int foo = lit();}"
    "{const bvec3 fuck = dp3(333);}"
    # expressions
    "{
        int (400);
        int (3.383737);
        vec4 (374673);
        lit();
    }"
    # multiple declarations
    "{
        int foo = 5;
        int foo = lit();
        const bvec3 fuck = dp3(1234);
    }"
    "{vec2 id1; vec2 id2; int foo = int(300); vec3 id3; vec4 id4; ivec3 id;}"
    # statements
    "{
        foo = int (400);
        foo = lit();;
        arr[12376] = int(400);
    }"
    # while statements
    "{
        while(true) int k = 9;
    }"
    "{
        while (false) {
            int k = 9;
            if ( foo == true) ; else a = 5;
            foo = rsq();
        }
    }"
    # if-else tests
    "{
        if (abc);
    }"
    "{
        if (1.234) ; else ;
    }"
    "{
        if ( foo == true) ; else a = 5;
    }"
    "{
        if (1234) 
            if (1.234) ; else a=1.2;
    }"
    "{
        if (1234)
            if (1.234) ; else a=1.2;
        else
            b = 1234;
    }"
    ############### expressions ##############
    # constructor
    "{
        a = bvec3();
    }"
    # function
    "{}"
    # int and float literals
    "{}"
    # unary
    "{
        a = -400;
        a = ! b;
        a = a + !b;
    }"
    # binary
    "{
        a = foo && foo;
        a = foo || foo;
        a = foo == foo;
        a = foo != foo;
        a = foo < foo;
        a = foo <= foo;
        a = foo > foo;
        a = foo >= foo;
        a = foo + foo;
        a = foo - foo;
        a = foo * foo;
        a = foo / foo;
        a = foo ^ foo;
    }"
    # precedence
    "{
        a = -b;
    }"
    "{
        a = !b * 2.2;
    }"
    "{
        a = b * !b;
    }"
    "{
        a = b * -b;
    }"
    "{
        a = 1 + 2.2 * 3.2;
    }"
    "{
        a = 1.2 * 2.2 + 3;
    }"
    "{
        a = 1 * 2 / 3;
    }"
    "{
        a = 1 && 2 + 3;
    }"
    "{
        a = 1 - 2 + 3;
    }"
    "{
        a = 1 + 2 / 3;
        a = 1 - 2 * 3;
        a = 1 - 2 / 3;
        a = 1 * 2 * 3;
        a = 1 * 2 * 3;
        a = 1 / 2 / 3;
        a = 1 / 2 / 3;
        a = 1 ^ 2 + 3;
        a = 1 ^ 2 - 3;
    }"
    #add more test cases here, enclose the test program in double quotes
)

arraylength=${#passInputs[@]}
echo "****************** Start *******************"             | tee ./passOut

for (( i=0; i<${arraylength}; i++ ));
do
    echo "TEST $i"                                              | tee -a ./passOut
    echo "=================== INPUT PROGRAM ================"   | tee -a ./passOut
    echo "${passInputs[i]}"                                     | tee -a ./passOut
    echo "=================== RULES ========================"   | tee -a ./passOut
    echo "${passInputs[i]}" | ./compiler467 -Tp                 | tee -a ./passOut
    echo -e "\n\n"                                              | tee -a ./passOut
done

echo "******************* END *******************"              | tee -a ./passOut


# Add cases that are expected to fail
declare -a failInputs=(
    # empty program
    ""
    "{"


)
arraylength2=${#failInputs[@]}
for (( j=0; j<${arraylength2}; j++ ));
do
    echo "TEST $j"                                              | tee -a ./failOut
    echo "=================== INPUT PROGRAM ================"   | tee -a ./failOut
    echo "${failInputs[j]}"                                     | tee -a ./failOut
    echo "=================== RULES ========================"   | tee -a ./failOut
    echo "${failInputs[j]}" | ./compiler467 -Tp                 | tee -a ./failOut
    echo -e "\n\n"                                              | tee -a ./failOut
done

echo "******************* END *******************"              | tee -a ./failOut