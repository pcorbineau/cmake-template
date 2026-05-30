import json
import sys


def parse_gcovr(path):
    with open(path) as f:
        d = json.load(f)
    return {
        "lines":     round(d["line_percent"], 1),
        "functions": round(d["function_percent"], 1),
        "branches":  round(d["branch_percent"], 1),
    }


def parse_llvm(path):
    with open(path) as f:
        d = json.load(f)
    t = d["data"][0]["totals"]
    return {
        "lines":     round(t["lines"]["percent"], 1),
        "functions": round(t["functions"]["percent"], 1),
        "branches":  round(t["branches"]["percent"], 1),
    }


def parse_msvc(path):
    # summary.json produced by cobertura_to_summary.cmake from OpenCppCoverage
    # output: {"lines": <pct>, "functions": <pct>, "branches": <pct>}
    with open(path) as f:
        d = json.load(f)
    return {
        "lines":     round(d["lines"], 1),
        "functions": round(d["functions"], 1),
        "branches":  round(d["branches"], 1),
    }


summary = {
    "gcc":   parse_gcovr("pages/gcc/summary.json"),
    "clang": parse_llvm("pages/clang/summary.json"),
    "msvc":  parse_msvc("pages/msvc/summary.json"),
}

with open("pages/summary.json", "w") as f:
    json.dump(summary, f)
