import json


def load_data(path):
    with open(path) as f:
        d = json.load(f)
    return {
        "lines":     round(d["line_percent"], 1),
        "functions": round(d["function_percent"], 1),
        "branches":  round(d["branch_percent"], 1),
    }
