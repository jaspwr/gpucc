class Test:
    def __init__(self, label, inFile, outFile, flags):
        self.label = label
        self.inFile = "meowcc/tests/" + inFile
        self.outFile = "meowcc/tests/" + outFile
        self.flags = flags