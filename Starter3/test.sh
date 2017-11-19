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
    "{
        {
            int a = 1 + 2;
        }
        {
            bvec3 b;
            {
                bool c;
                {
                    bool d;
                    bvec3 a = bvec3(c, d, c&&d);
                    b = a;
                }
            }
            if (false) {
                int k = 9;
            }
        }
    }"
    "{
        vec4 v4; int x; int y; int z;
        {
            float xx; float yy; float zz; bool b;
            x = 1; y = 2; z = 3;
            xx = 1.1; yy = 2.2; zz = 3.3; x=1.234; xx=222; b = 123;
            b = true;
        }
    }"
    "{int a; int b; int c; a = b + c;}"
    "{ivec2 a; ivec2 b; ivec2 c; a = b + c;}"
    "{float a; float b; float c; c = b - a;}"
    "{vec3 a; vec3 b; vec3 c; c = b - a;}"

    "{vec2 a; vec2 b; vec2 c; a = b * c;}"
    "{vec2 a; float b; vec2 c; a = b * c;}"
    "{vec2 a; vec2 b; float c; a = b * c;}"
    "{float a; float b; float c; a = b * c;}"

    "{float a; float b; float c; c = a / b;}"
    "{float a; float b; float c; c = a ^ b;}"

    "{bool a = false; bool b = !a;}"
    "{bvec2 a; bvec2 b = !a;}"
    "{bvec3 a; bvec3 b = !a;}"
    "{bvec4 a; bvec4 b = !a;}"
    "{bool a; bool b; bool c; a = b && c;}"
    "{bool a; bool b; bool c; a = b || c;}"
    "{bvec2 a; bvec2 b; bvec2 c; a = b && c;}"
    "{bvec2 a; bvec2 b; bvec2 c; a = b || c;}"

    "{int a; int b; bool c; c = a < b;}"
    "{int a; int b; bool c; c = a <= b;}"
    "{int a; int b; bool c; c = a > b;}"
    "{int a; int b; bool c; c = (a >= b);}"

    "{int a; int b; bool c; c = (a == b);}"
    "{int a; int b; bool c; c = (a != b);}"
    "{ivec2 a; ivec2 b; bool c; c = (a == b);}"
    "{ivec2 a; ivec2 b; bool c; c = (a != b);}"

    #condition
    "{bool b; int a; if(b){int c;}}"
    "{bool b = true; int a; if(b){int c = 10;}}"

    #fucntion calls
    "{float ret; float f; ret = rsq (f);}"
    "{int a; float f; f = rsq (a);}"
    "{vec4 a; vec4 b; float f; f = dp3 (a,b);}"
    "{vec3 a; vec3 b; float f; f = dp3 (a,b);}"
    "{ivec4 a; ivec4 b; int i; i = dp3 (a,b);}"
    "{ivec3 a; ivec3 b; int i; i = dp3 (a,b);}"
    "{vec4 a; vec4 ret; ret = lit(a);}"

    #constructor calls
    "{bvec2 bv; bv = bvec2();}"
    "{bvec2 bv; bv = bvec2(true);}"
    "{bvec2 bv; bv = bvec2(true,false);}"
    "{bvec3 bv; bv = bvec3(true,false,true);}"
    "{bvec4 bv; bv = bvec4(false,false,false,true);}"
    "{bvec4 bv; bv = bvec4(false,false,false,true,false,false,false,true,false,false,false,true);}"

    "{ivec2 iv; iv = ivec2();}"
    "{ivec2 iv; iv = ivec2(3);}"
    "{ivec2 iv; iv = ivec2(3,7);}"
    "{ivec3 iv; iv = ivec3(2,4,6);}"
    "{ivec2 iv; iv = ivec2(3,1111.2334);}"
    "{ivec2 iv; iv = ivec2(3,7, true);}"
    "{ivec4 iv; iv = ivec4(0,0,0,-1);}"

    "{vec2 v; v = vec2(3.0,7.12);}"
    "{vec3 v; v = vec3(1.2342,8.11,9.9);}"
    "{vec4 v; v = vec4(0.0,0.0,0.0,0.1);}"

    #vector indexing
    "{bvec2 bv; bool b; bv = bvec2(true,false); b = bv[1];}"
    "{bvec3 bv; bool b; bv = bvec3(true,false,true); b = bv[2];}"
    "{bvec4 bv; bool b; bv = bvec4(true,false,true,true); b = bv[3];}"

    "{ivec2 iv; int i; iv = ivec2(2, 2); i = iv[1];}"
    "{ivec3 iv; int i; iv = ivec3(2, 3, 4); i = iv[2];}"
    "{ivec4 iv; int i; iv = ivec4(2, 4, 6, 8); i = iv[3];}"

    "{vec2 v; float f; v = vec2(1.0, 2.0); f = v[1];}"
    "{vec3 v; float f; v = vec3(3.0, 4.0, 5.0); f = v[2];}"
    "{vec4 v; float f; v = vec4(0.0,0.0,0.0,0.1); f = v[3];}"

    #initialization
    "{const int a = 10;}"
    "{const bool a = true;}"
    "{const float a = 1.0;}"

    #assignment
    "{int a; a=10;}"
    "{int a = 9; a = 10;}"
    "{bool a; a = true;}"
    "{bool a = false; a = true;}"
    "{float a; a = 1.0;}"
    "{float a = 9.0; a = 10.0;}"

    #variables
    "{int a; {int a;}}"
    "{int a; {int a = 10;{int a;}}}"
    "{bool a; {bool a;} {bool a;}}"
    "{int a; {float a = 1.0;{bool a;}}}"
    "{int a; {float a = 1.0;{bool a = true;}}}"
    "{int a = 2; { int b = 2; {{a = 3;}}}}"

    #pre-defined variables
    "{vec4 a = vec4(1.0,2.0,3.0,4.0);  gl_FragColor = a;}"
    "{gl_FragDepth = true;}"
    "{vec4 a = vec4(1.0,2.0,3.0,4.0);  gl_FragCoord = a;}"
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
    echo "${passInputs[i]}" | ./compiler467 -Da                 | tee -a ./passOut
    echo -e "\n\n"                                              | tee -a ./passOut
done

echo "******************* END *******************"              | tee -a ./passOut


# Add cases that are expected to fail
declare -a failInputs=(
    # empty program
    ""
        "{int foo = lit();}"
    "{const bvec3 fuck = dp3(333);}"
    # expressions
    "{
        a= int (400);
        b = int (3.383737);
        c = vec4 (374673);
        d = lit();
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
        if (false) {
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
    "{"
    "{int a; int b; ivec2 c; a = b + c;}"
    "{ivec2 a; int b; ivec2 c; a = b + c;}"
    "{float a; float b; vec3 c; c = b - a;}"
    "{vec3 a; float b; vec3 c; c = b - a;}"

    "{float a; vec2 b; vec2 c; a = b * c;}"
    "{float a; float b; vec2 c; a = b * c;}"
    "{float a; vec2 b; float c; a = b * c;}"
    "{vec2 a; float b; float c; a = b * c;}"

    "{float a; vec2 b; float c; c = a / b;}"
    "{float a; float b; vec4 c; c = a ^ b;}"

    "{int a; bool b = !a;}"
    "{ivec2 a; bvec2 b = !a;}"

    "{bool a; bvec2 b; bool c; a = b && c;}"
    "{bvec2 a; bool b; bvec2 c; a = b || c;}"
    "{bvec2 a; bool b; bvec2 c; a = b && c;}"
    "{bool a; bvec2 b; bool c; a = b || c;}"

    "{ivec2 a; int b; bool c; c = a < b;}"
    "{int a; ivec2 b; bool c; c = a <= b;}"
    "{int a; ivec3 b; bool c; c = a > b;}"
    "{int a; ivec4 b; bool c; c = (a >= b);}"

    "{int a; ivec2 b; bool c; c = (a == b);}"
    "{ivec3 a; int b; bool c; c = (a != b);}"
    "{ivec2 a; ivec2 b; bool c; c = (a == b);}"
    "{ivec2 a; ivec2 b; bool c; c = (a != b);}"

    "{float a; vec2 b; vec2 c; a = b * c;}"
    "{int a; ivec3 b; ivec3 c; a = b + c;}"
    "{bool a; bvec4 b; bvec4 c; a = b || c;}"
    "{bvec2 a; bool b; bool c; a = b && c;}"

    #condition
    "{bool b; int a; if(a){int c;}}"

    #fucntion calls
    "{int ret; float f; ret = rsq (f);}"
    "{float ret; bool f; ret = rsq (f);}"
    "{float ret; float f; float a; ret = rsq (f,a);}"
    "{float ret; float f; float a; ret = rsq ();}"

    "{vec4 a; vec3 b; float f; f = dp3 (a,b);}"
    "{vec2 a; vec2 b; float f; f = dp3 (a,b);}"
    "{vec4 a; vec4 b; vec4 c; float f; f = dp3 (a,b,c);}"
    "{vec4 a; vec4 b; vec4 c; float f; f = dp3 ();}"

    "{ivec4 a; ivec3 b; int f; f = dp3 (a,b);}"
    "{ivec2 a; ivec2 b; int f; f = dp3 (a,b);}"
    "{ivec4 a; ivec4 b; ivec4 c; float f; f = dp3 (a,b,c);}"
    "{ivec4 a; ivec4 b; ivec4 c; float f; f = dp3 ();}"

    "{vec3 a; vec4 ret; ret = lit(a);}"
    "{vec4 a; vec4 ret; ret = lit();}"
    "{vec4 a; vec4 b;vec4 ret; ret = lit(a,b);}"

    #constructor calls
    "{ivec2 iv; iv = ivec2(8);}"
    "{ivec2 iv; iv = ivec2(3,4,5);}"
    "{ivec2 iv; iv = ivec2();}"
    "{ivec2 iv; iv = ivec2(1,0.0);}"

    "{ivec3 iv; iv = ivec3(3,4);}"
    "{ivec3 iv; iv = ivec3(8,8,8,8);}"
    "{ivec3 iv; iv = ivec3();}"
    "{ivec2 iv; iv = ivec2(3,4,false);}"

    "{ivec4 iv; iv = ivec4(1,2,3);}"
    "{ivec4 iv; iv = ivec4(1,1,2,2,3);}"
    "{ivec4 iv; iv = ivec4(1,1,1,true);}"
    "{ivec4 iv; iv = ivec4(0.0,0.0,0.0,0.0);}"

    "{bvec2 bv; bv = bvec2(true);}"
    "{bvec3 bv; bv = bvec3(true,false,false,false);}"
    "{bvec4 bv; bv = bvec4();}"
    "{bvec2 bv; bv = bvec2(true,1);}"
    "{bvec4 bv; bv = bvec4();}"

    "{vec2 v; v = vec2(1.0);}"
    "{vec2 v; v = vec3(2.0,2.0,2.0,2.0);}"
    "{vec2 v; v = vec4();}"
    "{vec2 v; v = vec2(true,1);}"
    "{vec2 v; v = vec2(1.0,1);}"

    "{ivec3 bv; bv = bvec3(true,false,true);}"

    #vector indexing
    "{bvec2 bv; bool b; bv = bvec2(true,false); b = bv[2];}"
    "{bvec3 bv; bool b; bv = bvec3(true,false,true); b = bv[3];}"
    "{bvec4 bv; bool b; bv = bvec4(true,false,true,true); b = bv[5];}"

    "{ivec2 iv; int i; iv = ivec2(2,2); i = iv[2];}"
    "{ivec3 iv; int i; iv = ivec3(2,3,4); i = iv[3];}"
    "{ivec4 iv; int i; iv = ivec4(2,4,6,8); i = iv[5];}"

    "{vec2 v; float f; v = vec2(1.0,2.0); f = v[1];}"
    "{vec3 v; float f; v = vec3(3.0,4.0,5,0); f = v[2];}"
    "{vec4 v; float f; v = vec4(0.0,0.0,0.0,0.1); f = v[3];}"

    #initialization
    "{int a; int b; const int c = a + b;}"
    "{const bool a = true; const bool b = false; const bool c = a || b;}"
    "{const float a = 1.0; const float b = 2.0; const float c = a * b;}"
    "{const int a = 10 + 2;}"

    #assignment
    "{int a; a = 1.0;}"
    "{int a = 9; a = false;}"
    "{bool a; a = 1;}"
    "{bool a = false; a = 1.2;}"
    "{float a; a = false;}"
    "{float a = 9.0; a = 10;}"

    #variables
    "{a = 1;}"
    "{{a = true;}}"
    "{{{a = 1.0;}}}"
    "{int a; int a;}"
    "{int a; int b; int a;}"
    "{int a; float b; bool c; bool d; int a = 10;}"
    "{bool a; {bool a; int b; bool a;}}"
    "{const float a = 1.0; a = 2.0;}"
    "{const bool a = true; {{a = false;}}}"
    "{const int a = 1; {int b = 2; {a = 2;}}}"

    #pre-defined varibles

    "{vec4 a = vec4(1.0,2.0,3.0,4.0); env1 = a;}"
    "{vec4 a = vec4(1.0,2.0,3.0,4.0); env2 = a;}"
    "{vec4 a = vec4(1.0,2.0,3.0,4.0); env3 = a;}"
    "{vec4 a = vec4(1.0,2.0,3.0,4.0); gl_Light_Half = a;}"
    "{vec4 a = vec4(1.0,2.0,3.0,4.0); gl_Light_Ambient = a;}"
    "{vec4 a = vec4(1.0,2.0,3.0,4.0); gl_Material_Shininess = a;}"
    "{vec4 a = vec4(1.0,2.0,3.0,4.0); gl_TexCoord  = a;}"
    "{vec4 a = vec4(1.0,2.0,3.0,4.0); gl_Color = a;}"
    "{vec4 a = vec4(1.0,2.0,3.0,4.0);  gl_Secondary = a;}"
    "{vec4 a = vec4(1.0,2.0,3.0,4.0); gl_FogFragCoord = a;}"
    "{vec3 a = vec3(1.0,2.0,3.0);  gl_FragCoord = a;}"


)
arraylength2=${#failInputs[@]}
for (( j=0; j<${arraylength2}; j++ ));
do
    echo "TEST $j"                                              | tee -a ./failOut
    echo "=================== INPUT PROGRAM ================"   | tee -a ./failOut
    echo "${failInputs[j]}"                                     | tee -a ./failOut
    echo "=================== RULES ========================"   | tee -a ./failOut
    echo "${failInputs[j]}" | ./compiler467 -Da                 | tee -a ./failOut
    echo -e "\n\n"                                              | tee -a ./failOut
done

echo "******************* END *******************"              | tee -a ./failOut