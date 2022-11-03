import subprocess
from os import getcwd

def _call(args: list):
    """
    Call the multimedia processor with the given args

    :param list args: The list of arguments
    """

    args.insert(0, "processors/multimedia_processor")
    print(getcwd())
    popen = subprocess.Popen(args, cwd=getcwd(), stdout=subprocess.PIPE, encoding="utf_8")
    popen.wait()
    result = popen.stdout.read()
    print(result)
    return result 

def debug():
    return _call(["debug"])

def preprocess(filename: str):
    return _call(["preprocess", "query_upload", filename])

def extract(filename: str):
    return _call(["extract", "query_upload", filename])

def query_top_k_models(filename: str):
    return _call(["query-model", "../../frontend/model-viewer/public/PSBDatabase", filename])
