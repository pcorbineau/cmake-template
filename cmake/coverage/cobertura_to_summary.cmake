# Called by the coverage target to convert a Cobertura XML report to the
# summary.json format consumed by merge_coverage_summary.py.
# Arguments passed via -D:
#   COBERTURA_XML - path to the Cobertura XML file produced by OpenCppCoverage
#   OUTPUT        - path to write the JSON summary

find_program(PYTHON3 NAMES python3 python)
if(NOT PYTHON3)
    message(FATAL_ERROR "Python 3 not found. It is required to convert the \
Cobertura XML report to summary.json.")
endif()

execute_process(
    COMMAND ${PYTHON3} -c
        "import json, xml.etree.ElementTree as ET, sys\n\
tree = ET.parse(sys.argv[1])\n\
root = tree.getroot()\n\
line_rate   = float(root.get('line-rate',   0)) * 100\n\
branch_rate = float(root.get('branch-rate', 0)) * 100\n\
methods = root.findall('.//method')\n\
covered_methods = sum(1 for m in methods if float(m.get('line-rate', 0)) > 0)\n\
func_pct = round(covered_methods / len(methods) * 100, 1) if methods else 0.0\n\
result = {'lines': round(line_rate, 1), 'functions': func_pct, 'branches': round(branch_rate, 1)}\n\
open(sys.argv[2], 'w').write(json.dumps(result))\n"
        ${COBERTURA_XML}
        ${OUTPUT}
    COMMAND_ERROR_IS_FATAL ANY
)
