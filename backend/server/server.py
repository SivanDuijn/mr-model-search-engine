from flask import Flask, request
from uuid import uuid4
from os import getcwd
import multimedia_processor
import file_validation

app = Flask(__name__)
app.config["UPLOAD_FOLDER"] = "query_upload"
app.config["MAX_CONTENT_LENGTH"] = 16 * 1000 * 1000

@app.route("/api/debug", methods=['GET'])
def debug():
    result = multimedia_processor.debug
    response = {"message": result}
    return response, 200

@app.route("/api/query", methods=['GET', 'POST'])
def query():
    if request.method == 'POST':
        # check file validity
        if 'file' not in request.files:
            response = {"message": "No file was found in request"}
            return response, 400
        file = request.files['file']
        if not file.filename:
            response = {"message": "No file was uploaded"}
            return response, 400
        if not file_validation.validate_file(file):
            response = {"message": "Invalid file"}
            return response, 400

        # save file on disk
        filename = uuid4().hex
        extension = file.filename.split('.')[-1].lower()
        print(f"User uploaded file {file.filename}, saving as {filename}.{extension}")
        model_location = f"{getcwd()}/{app.config['UPLOAD_FOLDER']}/{filename}.{extension}"
        file.save(model_location)

        # preprocess the file
        result = multimedia_processor.query_top_k_models(model_location)

        response = result;
        return response, 200

    # give a small upload form for debugging
    return '''
    <!doctype html>
    <h1>Upload a model</h1>
    <form method=post enctype=multipart/form-data>
      <input type=file name=file>
      <input type=submit value=Upload>
    </form>
    '''
