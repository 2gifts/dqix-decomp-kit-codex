# @runtime Jython
from ghidra.app.decompiler import DecompInterface
from ghidra.app.cmd.function import CreateFunctionCmd
from ghidra.util.task import ConsoleTaskMonitor
import os

args = getScriptArgs()
outdir = args[0]
if not os.path.isdir(outdir):
    os.makedirs(outdir)
ifc = DecompInterface()
ifc.openProgram(currentProgram)
fm = currentProgram.getFunctionManager()
for a in args[1:]:
    addr = toAddr(int(a, 16))
    func = fm.getFunctionContaining(addr)
    if func is None:
        CreateFunctionCmd(addr).applyTo(currentProgram)
        func = fm.getFunctionContaining(addr)
    if func is None:
        print("NOFUNC %s" % a)
        continue
    res = ifc.decompileFunction(func, 120, ConsoleTaskMonitor())
    text = res.getDecompiledFunction().getC() if res.decompileCompleted() else "FAILED " + res.getErrorMessage()
    name = "%s_%s.c" % (func.getEntryPoint().toString(), a)
    f = open(os.path.join(outdir, name), "w")
    f.write("// %s entry %s body %s\n" % (func.getName(), func.getEntryPoint(), func.getBody().getNumAddresses()))
    f.write(text)
    f.close()
    print("OK %s -> %s (%s)" % (a, name, func.getName()))
