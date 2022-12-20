import test_class

tests = [
    test_class.Test("No Passthrough Basic Expression", "bsc.c", "no_pass_obsc.uir", "--dbg-mode ir --yacc meowcc/tests/no_pass_smpl_exp.yacc"),
    test_class.Test("Basic Expression", "bsc.c", "obsc.uir", "--dbg-mode ir --yacc meowcc/tests/smpl_exp.yacc"),
    test_class.Test("All Operators", "all_opers.c", "all_opers_out.uir", "--dbg-mode ir --yacc meowcc/tests/all_opers.yacc"),
    test_class.Test("All No Passthrough", "all_no_passthrough.c", "all_no_passthrough_out.uir", "--dbg-mode ir --yacc meowcc/tests/all_no_passthrough.yacc"),
]