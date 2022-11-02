from flask import Flask

UPLOAD_EXTENSIONS = {"off"}

def validate_file(file):
    """
    Validate whether a given file is valid input for the multimedia processor
    e.g. it is a valid .off file

    :param FileStorage file: The file to be validated
    """

    file_name = file.filename
    if not '.' in file_name: return False
    file_extension = file_name.split('.')[-1].lower()
    if not file_extension in UPLOAD_EXTENSIONS: return False

    if file_extension == "off":
        # TODO file verification
        # print(file.stream.read())
        # file.seek(0)

        return True

    return False
