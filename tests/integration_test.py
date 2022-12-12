import subprocess
import test_class
import test_list

COMPILER_BIN_PATH = "Debug/meowcc.exe"
DBG_OUTPUT_BLOCK_START = ";DBG_OUTPUT_START"
DBG_OUTPUT_BLOCK_END = ";DBG_OUTPUT_END"

class TermCols:
    BLACK   = '\033[30m'
    RED     = '\033[31m'
    GREEN   = '\033[32m'
    YELLOW  = '\033[33m'
    BLUE    = '\033[34m'
    MAGENTA = '\033[35m'
    CYAN    = '\033[36m'
    WHITE   = '\033[37m'
    RESET   = '\033[39m'


PASS = True
FAILED = False

def runTest(test):
    outCodeCorrect = open(test.outFile, "r").read()
    outCode = ""

    try:
        outCode = subprocess.run('%(compiler_bin_path)s %(in_file)s %(flags)s' % {
            'compiler_bin_path': COMPILER_BIN_PATH,
            'in_file': test.inFile,
            'flags': test.flags
        }, stdout=subprocess.PIPE,
        stderr=subprocess.PIPE).stdout
    except:
        return (FAILED, "Failed to execute compiler.")

    try:
        outCode = outCode.decode('utf-8').split(DBG_OUTPUT_BLOCK_START)[1].split(DBG_OUTPUT_BLOCK_END)[0]
    except:
        return (FAILED, "Failed to extract debug output from compiler output. Got \"%(out_code)s\"." % {
            'out_code': outCode
        })

    if outCodeCorrect != outCode:
        return (FAILED, "Output does not match expected output. Got:\n%(out_code)s Expected:\n%(out_code_correct)s" % {
            'out_code': outCode,
            'out_code_correct': outCodeCorrect
        })

def status_string(status):
    return (TermCols.GREEN + "PASS" if status == PASS else TermCols.MAGENTA + "FAIL") + TermCols.RESET

def bool_to_int(bool):
    return 0 if bool else 1

def run_tests():
    passes = 0
    fails = 0
    print("Running %d tests.\n\n" % len(test_list.tests))
    for test in test_list.tests:
        (result, message) = runTest(test)
        print("%(result)s %(label)s" % {
            'label': test.label,
            'result': status_string(result)
        })
        if result == FAILED:
            fails += 1
            print("---------- Output ----------")
            print(message)
            print("----------------------------")
        else:
            passes += 1
            
    allPassed = fails == 0;
    print("\n\nTest result: %s. %d passed; %d failed." % (status_string(allPassed), passes, fails))
    print("bye !!")
    return allPassed

exit(bool_to_int(run_tests()))