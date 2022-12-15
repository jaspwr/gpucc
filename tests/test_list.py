import test_class

tests = [
    test_class.Test("No Passthrough Basic Expression", "bsc.c", "no_pass_obsc.uir", "--dbg-mode ir --yacc meowcc/tests/no_pass_smpl_exp.yacc"),
    test_class.Test("Basic Expression", "bsc.c", "obsc.uir", "--dbg-mode ir --yacc meowcc/tests/smpl_exp.yacc"),
]