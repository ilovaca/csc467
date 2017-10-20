fout = open('testcases.txt', 'w')

scope = ["{", "}"]
# types = ("int", "ivec2", "ivec3", "ivec4",
#             "bool", "bvec2", "bvec3", "vec3")
types = ("int")
semi_colon = (';')
empty = ("")
qualifier = ("const", "")
unary_op = ["!", ""]
IF      = "if"
ELSE    = "else"
WHILE   = "while"

assignment = ("=", "");

ID = ("abc, jd32")
int_literal = ("12345","4373")
float_literal = ("123.456", "0.09876")
funcname = ("dp3","rsq","lit")

testOut = []

expressions = [lit for lit in int_literal]

declaration1 = [t+" "+id+';' for t in types for id in ID]
declaration2 = [t+" "+id+'='+ expr +';' for t in types for id in ID for expr in expressions]
declarations = [q+" "+t +" "+id+";"]
# for i in range(10000):
#   prog = "\"{"
#   # expressions
#   expr = []
#   # declarations
#   decls2 = [t+id+ "="+expr for t in types for id in ID for expr in EXPR]




#   prog += "}\""

#   fout.write(prog)

