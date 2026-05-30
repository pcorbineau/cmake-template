# Called by the coverage target to convert llvm-cov export JSON to Cobertura XML.
# Arguments passed via -D:
#   LLVM_COV      - path to llvm-cov executable
#   TARGET_FILE   - path to the instrumented test binary
#   PROFDATA      - path to the .profdata file
#   IGNORE_REGEX  - filename regex to ignore
#   OUTPUT        - path to write the Cobertura XML

execute_process(
    COMMAND ${LLVM_COV} export
        -object ${TARGET_FILE}
        -instr-profile=${PROFDATA}
        -ignore-filename-regex=${IGNORE_REGEX}
        -format=text
    OUTPUT_FILE "${OUTPUT}.json"
    COMMAND_ERROR_IS_FATAL ANY
)

find_program(PYTHON3 NAMES python3 python)
if(NOT PYTHON3)
    message(FATAL_ERROR "Python 3 is required to generate Cobertura XML from llvm-cov data.")
endif()

execute_process(
    COMMAND ${PYTHON3} -c [=[
import json, sys
from xml.etree.ElementTree import Element, SubElement, ElementTree, indent
from time import time

data = json.load(open(sys.argv[1]))['data'][0]
totals = data['totals']

def rate(obj):
    c = obj.get('count', 0)
    return obj.get('covered', 0) / c if c else 1.0

root = Element('coverage')
root.set('line-rate',   f"{rate(totals['lines']):.4f}")
root.set('branch-rate', f"{rate(totals['branches']):.4f}")
root.set('timestamp', str(int(time())))
root.set('version', '')

packages  = SubElement(root, 'packages')
pkg       = SubElement(packages, 'package')
pkg.set('name', '.')
pkg.set('line-rate',   root.get('line-rate'))
pkg.set('branch-rate', root.get('branch-rate'))
pkg.set('complexity', '0')
classes_el = SubElement(pkg, 'classes')

for f in data['files']:
    cls = SubElement(classes_el, 'class')
    cls.set('name',        f['filename'].split('/')[-1])
    cls.set('filename',    f['filename'])
    cls.set('line-rate',   f"{rate(f['summary']['lines']):.4f}")
    cls.set('branch-rate', f"{rate(f['summary']['branches']):.4f}")
    cls.set('complexity',  '0')
    SubElement(cls, 'methods')
    lines_el = SubElement(cls, 'lines')
    # Segments: [line, col, count, has_count, is_region, is_gap]
    line_hits = {}
    for seg in f.get('segments', []):
        if seg[3] and not seg[5]:   # has_count and not a gap
            ln = seg[0]
            line_hits[ln] = max(line_hits.get(ln, 0), seg[2])
    for ln in sorted(line_hits):
        line_el = SubElement(lines_el, 'line')
        line_el.set('number', str(ln))
        line_el.set('hits',   str(line_hits[ln]))

tree = ElementTree(root)
indent(tree)
tree.write(sys.argv[2], xml_declaration=True, encoding='unicode')
]=]
        "${OUTPUT}.json"
        "${OUTPUT}"
    COMMAND_ERROR_IS_FATAL ANY
)
