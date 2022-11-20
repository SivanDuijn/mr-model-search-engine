import subprocess
import os
import json
import sys

def _call(args: list):
    """
    Call the multimedia processor with the given args

    :param list args: The list of arguments
    """

    args.insert(0, "processors/multimedia_processor")
    s = "";
    for arg in args:
        s += arg + " ";
    print(s)
    popen = subprocess.Popen(args, cwd=os.getcwd(), stdout=subprocess.PIPE, encoding="utf_8")
    # popen.wait()
    result = popen.communicate()[0]
    return result 

def debug():
    return _call(["debug"])

def preprocess(filename: str):
    return _call(["preprocess", "query_upload", filename])

def extract(filename: str):
    return _call(["extract", "query_upload", filename])

def query_top_k_models(filename: str):
    database_dir = "../../frontend/model-viewer/public/PSBDatabase/"
    logs = _call(["query-model", database_dir, filename])
    print(logs)
    try:
        result_path = f"{filename}_result.json"
        result_f = open(result_path)
        result = json.load(result_f)
        os.remove(result_path)
        return result
    except:
        return "{\"error\": \"Couldn't load result! File does not exist!\"}"

