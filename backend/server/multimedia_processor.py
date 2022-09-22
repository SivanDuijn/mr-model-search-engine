import subprocess

def call(args):
    """
    Call the multimedia processor with the given args

    :param list args: The list of arguments
    """

    args.insert(0, "multimedia_processor")
    popen = subprocess.Popen(args, stdout=subprocess.PIPE)
    popen.wait()
    print(popen.stdout.read())
