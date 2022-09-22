from encodings import utf_8
import subprocess

def call(args):
    """
    Call the multimedia processor with the given args

    :param list args: The list of arguments
    """

    args.insert(0, "processors/multimedia_processor")
    popen = subprocess.Popen(args, stdout=subprocess.PIPE, encoding="utf_8")
    popen.wait()
    result = popen.stdout.read()
    print(result)
    return result 
